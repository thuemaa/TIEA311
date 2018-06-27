#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
       exit( 0 );
    }
	//OMA KOODI---------------------------------------------------------------------------

	Curve kurvi;

	//Bernsteinin kanta bÈzierille
	Matrix4f B(1.f, -3.f, 3.f, -1.f,
			 0.f, 3.f, -6.f, 3.f,
			 0.f, 0.f, 3.f, -3.f,
			 0.f, 0.f, 0.f, 1.f);

	//taulukko vektoreille
	vector < vector< Vector3f > > Allpoints;
	vector < Vector3f > Points;
	int i = 0;

	//silmukassa t‰ytet‰‰n allpoints Points vectoreilla. Seuraavan pisteen ensimm‰inen indeksi on edellisen viimeinen.
	while (i < P.size()) {
		if (Points.size() == 4)
		{
			Allpoints.push_back(Points);
			while (Points.size() > 0) {
				Points.pop_back();
			}
			i--;
		}
		else
		{
			Points.push_back(P[i]);
			i++;
		}
	}
	Allpoints.push_back(Points);
	
	/* testi
	vector <Vector3f> vikaPiste;
	vikaPiste.push_back(Points[1]);
	vikaPiste.push_back(Points[2]);
	vikaPiste.push_back(Points[3]);
	vikaPiste.push_back(Allpoints[0][3]);
	Allpoints.push_back(vikaPiste);
	*/

	//tallennetaan t‰h‰n edellinen cp jotta voi verrata seuraavan tangenttia ja vertexi‰
	CurvePoint edellinenCP;

	for (int y = 0; y < Allpoints.size(); y++)
	{
		//kontrollipisteet
		Matrix4f G(Allpoints[y][0][0], Allpoints[y][1][0], Allpoints[y][2][0], Allpoints[y][3][0],
				Allpoints[y][0][1], Allpoints[y][1][1], Allpoints[y][2][1], Allpoints[y][3][1],
				Allpoints[y][0][2], Allpoints[y][1][2], Allpoints[y][2][2], Allpoints[y][3][2],
				0.f, 0.f, 0.f, 0.f);

		float t = 0.;
		float stepT = 1. / steps;

		for (int z = 0; z < steps; z++) {
			Vector4f Tt(1, t, t*t, t*t*t);
			Vector4f dTt(0, 1, 2 * t, 3 * t*t);
			CurvePoint cp;
			cp.V = (G * B * Tt).xyz();
			cp.T = (G * B * dTt).xyz().normalized();
			//ekassa askeleessa valitaan 0,0,1 vektori binormaaliksi
			if (y == 0) {
				cp.N = Vector3f::cross(Vector3f(0, 0, 1), cp.T);
				cp.B = Vector3f::cross(cp.T, cp.N);
			}
			//muuten lasketaan binormaali edellisen askeleen binormaalin kautta
			else {
				cp.N = Vector3f::cross(kurvi[kurvi.size() - 1].B, cp.T);
				cp.B = Vector3f::cross(cp.T, cp.N);
			}
			kurvi.push_back(cp);
			t += stepT;
			
			//ep‰jatkuvuuden korjausoperaatio
			edellinenCP = cp;

			//cout << "Kurvipiste numero: " << z << ". " << cp.T << "  " << cp.V << "  " << cp.N << "  " << cp.B << endl;
		}
	}
	//-------------------------------------------------------------------------------------

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
	//cerr << "\t>>> Returning empty curve." << endl;

    // Right now this will just return this empty curve.
    //return Curve();
	return kurvi;
}

Curve evalBspline(const vector< Vector3f >& P, unsigned steps)
{
	// Check
	if (P.size() < 4)
	{
		cerr << "evalBspline must be called with 4 or more control points." << endl;
		exit(0);
	}

	///OMA KOOODI------------------------------------------------------------------------------------

	//alustetaan kurvi
	Curve kurvi;


	//pilkotaan kontrollipisteet taulukoiksi jossa jokaisessa 4 alkiota
	vector < vector< Vector3f > > Allpoints;
	vector < Vector3f > Points;
	int i = 0;
	int aindex = 0;

	while (i < P.size() - 3)
	{
		for (int n = 0; n < 4; n++)
		{
			Points.push_back(P[i]);
			i++;
		}
		aindex++;
		i = aindex;
		Allpoints.push_back(Points);

		while (Points.size() > 0)
		{
			Points.pop_back();
		}
	}

	//Bernsteinin kanta bÈzierille
	Matrix4f B1(1.f, -3.f, 3.f, -1.f,
		0.f, 3.f, -6.f, 3.f,
		0.f, 0.f, 3.f, -3.f,
		0.f, 0.f, 0.f, 1.f);

	//B-spline kanta
	Matrix4f B2(1.f / 6, -3.f / 6, 3.f / 6, -1.f / 6,
		4.f / 6, 0.f / 6, -6.f / 6, 3.f / 6,
		1.f / 6, 3.f / 6, 3.f / 6, -3.f / 6,
		0.f / 6, 0.f / 6, 0.f / 6, 1.f / 6);

	//Bezierin k‰‰nteismatriisi
	const Matrix4f B1inverse = B1.inverse(false, 0.001f);
	Vector3f temp;

	for (int y = 0; y < Allpoints.size(); y++)
	{
	
		Matrix4f G(Allpoints[y][0][0], Allpoints[y][1][0], Allpoints[y][2][0], Allpoints[y][3][0],
			Allpoints[y][0][1], Allpoints[y][1][1], Allpoints[y][2][1], Allpoints[y][3][1],
			Allpoints[y][0][2], Allpoints[y][1][2], Allpoints[y][2][2], Allpoints[y][3][2],
			0.f, 0.f, 0.f, 0.f);

		//Lasketaan uudet control pointit bezier k‰yr‰lle
		Matrix4f uudetCpoint = G * B2 * B1inverse;

		Vector3f v1 = uudetCpoint.getCol(0).xyz();
		Vector3f v2 = uudetCpoint.getCol(1).xyz();
		Vector3f v3 = uudetCpoint.getCol(2).xyz();
		Vector3f v4 = uudetCpoint.getCol(3).xyz();

		//laita silmukalla cps taulukkoon!!!
		vector< Vector3f > kontrollivektorit;

		kontrollivektorit.push_back(v1);
		kontrollivektorit.push_back(v2);
		kontrollivektorit.push_back(v3);
		kontrollivektorit.push_back(v4);

		//kutsutaan evalBezieri‰ uusilla controlpointeilla
		Curve tempcurve = evalBezier(kontrollivektorit, steps);


		//Laitetaan kurvin palaset p‰‰kurviin
		for (int z = 0; z < tempcurve.size(); z++)
		{
			kurvi.push_back(tempcurve[z]);
		}
	}
	//----------------------------------------------------------------------------------------------

	cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

	cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
	for (unsigned i = 0; i < P.size(); ++i)
	{
		cerr << "\t>>> " << P[i] << endl;
	}

	cerr << "\t>>> Steps (type steps): " << steps << endl;
	cerr << "\t>>> Returning empty curve." << endl;

	// Return an empty curve right now.
	return kurvi;

}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

