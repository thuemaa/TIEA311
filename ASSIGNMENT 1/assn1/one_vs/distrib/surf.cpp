#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;

    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	//käännöksen transformmatriisi = rotateM (( pitää kääntää Y akselin suuntaan))
	//*Surface Vertex saa kun kertoo käännös transform matriisilla curvepointin verteksin ((rotateM * profile[i].V =  Surface.VV))
	//Surface normalin saa kun kertoo transformM inverse transposella curvepoint normaalin ((rotateM.inverseTranspose * Profile[i].N == Surface.VN))
	
	
	//tehdään operaatiot silmukassa steps määrän verran
	
	for (int i = 0; i < steps; i++)
	{

		//täys kierros radiaaneina = 2 * PI

		//käännösmatriisi, viimeisessä steps = 360 astetta, joten kyseisen iteroinin radin saa jakamalla i stepsillä
		Matrix4f rotateM = Matrix4f::rotateY(2.0*M_PI*i/steps);
		//inverse3x3 submatriisi rotateM matriisista
		Matrix3f inversesubM = rotateM.getSubmatrix3x3(0, 0).inverse().transposed();

		//iteroidaan kaikki profiilin curvepointit läpi
		for (unsigned n = 0; n < profile.size(); n++) 
		{
			//lasketaan surfacen vertexi kertomalla transformaatiomatriisilla curvepointin vertex
			//pitää muuttaa curvepointin vertex vector4f jotta saa matrix4f kertolaskulla laskettua ja lopuksi tuloksesta otetaan
			//vector3f xyz metodilla
			surface.VV.push_back( (rotateM * Vector4f(profile[n].V, 1)).xyz() );
			//normaalit, kerrotaan normaali -1 jotta sen suunnan saa vastakkaiseksi
			surface.VN.push_back( (inversesubM * profile[n].N*-1) );


			//käydään kaksi curvepointtia kerrallaan, eli viimeistä iterointia ei tehdä
			if (n < (profile.size() - 1))
			{
				//ensimmäiseksi indeksiksi curvepoint n sijainti + se curve mitä iteroidaan
				unsigned ind1 = n + i * profile.size();

				//toinen indeksi seuraavan curven ind1 kohdalla oleva curvepoint
				unsigned ind2;

				//ind2 sijoitus muuten kuin viimeisen curven kohdalla
				if (i != steps - 1)
				{
					ind2 = (ind1 + profile.size());
				}
				//jos ollaan viimeisessä curvessa tulee seuraavan curven cp:n indeksiksi
				//ensimmäisen curven vastaava cp
				else
				{
					ind2 = n;
				}

				//sijoitus
				Tup3u t1(ind1, ind1+1, ind2);
				Tup3u t2(ind2, ind1 + 1, ind2 + 1);
				surface.VF.push_back(t1);
				surface.VF.push_back(t2);

				//cout << "i: " << i << " t1: " << t1 << " t2: " << t2 <<endl;
			}

			
		}
		
	}
	

    cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	//iteroidaan sweepin ympäri
	for (unsigned i = 0; i < sweep.size(); i++)
	{

		//käännösmatriisit EI TOIMI (kerro matriisilla) ?????
		Matrix4f rotateM = Matrix4f::translation(sweep[i].V);
		Matrix3f inversesubM = rotateM.getSubmatrix3x3(0, 0).inverse().transposed();

		Matrix4f kanta(
			sweep[i].N[0], sweep[i].B[0], sweep[i].T[0], sweep[i].V[0],
			sweep[i].N[1], sweep[i].B[1], sweep[i].T[1], sweep[i].V[1],
			sweep[i].N[2], sweep[i].B[2], sweep[i].T[2], sweep[i].V[2],
			0, 0, 0, 1);
	
		Matrix3f normaalikanta = kanta.getSubmatrix3x3(0, 0).inverse().transposed();


		//curven käsittely
		for (unsigned n = 0; n < profile.size(); n++)
		{
			
			//VÄÄÄÄRIN
			surface.VV.push_back( (kanta * Vector4f(profile[n].V, 1)).xyz() );
			surface.VN.push_back( normaalikanta * profile[n].N*-1);

			//surface.VV.push_back( (rotateM * Vector4f(profile[n].V, 1)).xyz() );
			//surface.VN.push_back((inversesubM * profile[n].N*-1));



			//FACES TEKO sama kuin sweep-----------------------------------------------------------------------
			//käydään kaksi curvepointtia kerrallaan, eli viimeistä iterointia ei tehdä
			if (n < (profile.size() - 1) && i < (sweep.size() - 1 ) )
			{
				//ensimmäiseksi indeksiksi curvepoint n sijainti + se curve mitä iteroidaan
				unsigned ind1 = n + i * profile.size();

				//toinen indeksi seuraavan curven ind1 kohdalla oleva curvepoint
				unsigned ind2;

				//ind2 sijoitus muuten kuin viimeisen curven kohdalla
				if (i != sweep.size() - 1)
				{
					ind2 = (ind1 + profile.size());
				}
				//jos ollaan viimeisessä curvessa tulee seuraavan curven cp:n indeksiksi
				//ensimmäisen curven vastaava cp
				else
				{
					ind2 = n;
				}

				//sijoitus
				Tup3u t1(ind1, ind1 + 1, ind2);
				Tup3u t2(ind2, ind1 + 1, ind2 + 1);
				surface.VF.push_back(t1);
				surface.VF.push_back(t2);

				//cout << "i: " << i << " t1: " << t1 << " t2: " << t2 <<endl;
			}
			//.------------------------------------------------------------------------------------------------


		}


	}

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
