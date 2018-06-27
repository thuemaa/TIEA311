/*
Tekijä: Tuomas Kaappa
Itsearvio: 3 (perusosio, pyörimisanimaatio ja värivaihtoanimaatio tehty)
Miltä tehtävä tuntui: Mukava tehtävä koska heti alusta asti sai näkyvää jälkeä aikaiseksi. Sopivan haasteellinen ensimmäiseksi
tehtäväksi, varsinkin kun ensimmäistä kertaa mitään c++ tein. Haastavinta oli glutTimerFunc käyttäminen alkuun, kun toimintatavan keksi
niin se muuttui selkeäksi. Merkkijonojen pilkkomista sai jonkin aikaa myös järkeillä. Valonlähteen siirron maksimiarvot eivät ole aivan
kohdallaan: toteutin sen ennen animaation tekemistä. maksimiarvot on nyt 30 ja -30 mutta objektin pyöriessä ne eivät riitä osoittamaan
valoa objektin jokaiselle puolelle.
*/

#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include "main.h"
//tarvitaan obj filun lukemiseen
#include <fstream>
//tarvitaan itseisarvoon
#include <stdlib.h>
using namespace std;

// Globals
// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
//varien vaihto

//Valittavat värit. Viimeinen alkio on muuttuva väri.
GLfloat diffColors[5][4] = { { 0.2, 0.3, 0.4, 1.0 },
{ 0.9, 0.0, 0.4, 1.0 },
{ 0.0, 0.9, 0.9, 1.0 },
{ 0.3, 1.0, 0.0, 1.0 },
{ 0.2, 0.3, 0.4, 1.0 } };

//muuttuja jos väri haetaan. Viittaa diffcolors viimeiseen alkioon.
GLfloat* vari = diffColors[4];

//Taulukkoindeksi valitulle värille
int valittuVari = 0;
//alkuarvo kameran käännölle
double angle = 1.0;
//muuttuja joka määrää onko animaatiot käynnissä vai ei.
int timerFlag = 0;
int variFlag = 1;
//tarkkuusarvo väriarvojen vertailulle
float delta = 0.015;
//Lightning pos
GLfloat Lt0pos[] = { 0.5f, -0.5f, 5.0f, 1.0f };

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{
	glVertex3fv(a);
}

inline void glNormal(const Vector3f &a)
{
	glNormal3fv(a);
}

/*ajastinfunktio joka lisää kameran angle muuttujan arvoa ja kutsuu itseään uudestaan
glutTimerFunc käyttämällä. Jos flag = 1 animaatio pyörii, jos 0 niin ei pyöri*/
static void timer(int arvo) {
	if (timerFlag == 1)
	{
		angle = angle + 0.25;
		glutPostRedisplay();
		glutTimerFunc(25, timer, 0);
	} 
}

/*Ajastinfunktio värinvaihdolle*/
static void variTimer(int valinta) {
	//Nykyinen väriarvo
	GLfloat* vari1 = vari;
	//Väri johon vaihdetaan
	GLfloat* vari2 = diffColors[valittuVari];

	//Tarkistetaan silmukassa onko nykyisen ja halutun värin erot sallituissa rajoissa
	bool jatkuuko = false;
	for (int x = 0; x < 4; x++) {
		//jos ollaan yli sallitussa rajassa muutetaan jatkuuko true
		if (fabs(vari[x] - diffColors[valittuVari][x]) > delta)
		{
			jatkuuko = true;
		}
	}
	//jos ollaan sallituissa rajoissa laitetaan variFlag nollaksi
	if (jatkuuko == false)
	{
		variFlag = 0;
	}
	else variFlag = 1;

	/*testitulostuksia*/
	/*
	for (int i = 0; i < 4; i++) {
		cout << vari1[i];
	}
	cout << ":";
	for (int i = 0; i < 4; i++) {
		cout << vari2[i];
	}
	cout << endl;
	cout << variFlag << endl;
	*/
	//----------
	//jos väri ei ole sallittujen rajojen sisällä
	//Muutetaan jokaista väriarvoa oikeaan suuntaan 0.005 kerrallaan
	if (variFlag == 1)
	{
		for (int i = 0; i < 4; i++) {
			if (vari1[i] - vari2[i] < 0) {
				vari1[i] += 0.005;
			}
			else if (vari1[i] - vari2[i] > 0) {
				vari1[i] -= 0.005;
			}
		}

		//asetetaan objektin väriksi uusi väri
		vari = vari1;
	}
	
	glutPostRedisplay();
	glutTimerFunc(100, variTimer, 0);
	
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc(unsigned char key, int x, int y)
{	
	switch (key)
	{
	case 27: // Escape key
		exit(0);
		break;
	case 'c':
		//palataan ekaan indeksiin jos ollaan toiseksi viimeisessä väritaulukon indeksissä
		if (valittuVari >= 3) 
		{
			valittuVari = 0;
		}
		else
		{
			valittuVari++;
		}
		//värinvaihto timer päälle
		variTimer(0);
		break;
	case 'r':
		//kutsutaan ajastinfunktio käyntiin
		if (timerFlag == 0)
		{	
			timerFlag = 1;
			timer(0);
			cout << "Rotation animation started." << endl;
		}
		else {
			timerFlag = 0;
			cout << "Rotation animation stopped." << endl;
		} break;
	default:
		cout << "Unhandled key press " << key << "." << endl;
	}

	// this will refresh the screen so that the user sees the color change
	glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		// Muutetaan lp0s arvoa, jos menee tietyn pisteen yli aloitetaan pohjasta
		if (Lt0pos[1] < 20.0) {
			Lt0pos[1] += 0.5;
		}
		else Lt0pos[1] = -20.0;
		break;
	case GLUT_KEY_DOWN:
		if (Lt0pos[1] > -20.0) {
			Lt0pos[1] -= 0.5;
		}
		else Lt0pos[1] = 20.0;
		break;
	case GLUT_KEY_LEFT:
		if (Lt0pos[0] > -20.0) {
			Lt0pos[0] -= 0.5;
		}
		else Lt0pos[0] = 20.0;
		break;
	case GLUT_KEY_RIGHT:
		if (Lt0pos[0] < 20.0) {
			Lt0pos[0] += 0.5;
		}
		else Lt0pos[0] = -20.0;
		break;
	}

	// this will refresh the screen so that the user sees the light position
	glutPostRedisplay();
}

/*Objektin piirtofunktio*/
void drawObject() {
	for (int x = 0; x < vecf.size(); x++) {

		int a = vecf[x][0];
		int c = vecf[x][2];
		int d = vecf[x][3];
		int f = vecf[x][5];
		int g = vecf[x][6];
		int i = vecf[x][8];

		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[c - 1][0], vecn[c - 1][1], vecn[c - 1][2]);
		glVertex3d(vecv[a - 1][0], vecv[a - 1][1], vecv[a - 1][2]);
		glNormal3d(vecn[f - 1][0], vecn[f - 1][1], vecn[f - 1][2]);
		glVertex3d(vecv[d - 1][0], vecv[d - 1][1], vecv[d - 1][2]);
		glNormal3d(vecn[i - 1][0], vecn[i - 1][1], vecn[i - 1][2]);
		glVertex3d(vecv[g - 1][0], vecv[g - 1][1], vecv[g - 1][2]);
		glEnd();
	}
}

// This function is responsible for displaying the object.
void drawScene(void)
{
	int i;

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rotate the image
	glMatrixMode(GL_MODELVIEW);  // Current matrix affects objects positions
	glLoadIdentity();              // Initialize to the identity

								   // Position the camera at [0,0,5], looking at [0,0,0],
								   // with [0,1,0] as the up direction.
	gluLookAt(0.0, 0.0, 5.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	/*kameran kääntö*/
	glRotatef(angle, 0.0, 1.0, 0.0);
	// Set material properties of object


	// Here are some colors you might use - feel free to add more
	/* Siirretty globaaliksi
	GLfloat diffColors[4][4] = { { 0.2, 0.3, 0.4, 1.0 },
	{ 0.9, 0.0, 0.4, 1.0 },
	{ 0.0, 0.9, 0.9, 1.0 },
	{ 0.3, 1.0, 0.0, 1.0 } };
	*/ 
	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, vari);

	// Define specular color and shininess
	GLfloat specColor[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat shininess[] = { 100.0 };

	// Note that the specular color and shininess can stay constant
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Set light properties

	// Light color (RGBA)
	GLfloat Lt0diff[] = { 1.0,1.0,1.0,1.0 };
	// Light position
	//GLfloat Lt0pos[] = {0.5f, -0.5f, 5.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);

	/*Objektin piirto*/
	drawObject();
	

	// Dump the image to the screen.
	glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
	glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
	glEnable(GL_LIGHTING);     // Enable lighting calculations
	glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
	// Always use the largest square viewport possible
	if (w > h) {
		glViewport((w - h) / 2, 0, h, h);
	}
	else {
		glViewport(0, (h - w) / 2, w, w);
	}

	// Set up a perspective view, with square aspect ratio
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 50 degree fov, uniform aspect ratio, near = 1, far = 100
	gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	//Haetaan haluttu obj streamiin (pois käytöstä)
	//std::ifstream filu("garg.obj");
	//std::string rivi;
	
	const int MAX_BUFFER_SIZE = 100;
	char buffer[MAX_BUFFER_SIZE];

	//luetaan tiedostoa rivi kerrallaan niin kauan kuin rivejä riittää
	while (cin.getline(buffer, MAX_BUFFER_SIZE))
	{
		//stringstream rivistä
		istringstream striimi(buffer);
		//uus stringarray substringejä varten
		std::vector<string> pilkottu;
		//pilkotaan string stream sana kerrallaan
		do
		{
			//luetaan sana kerralla streamia
			string pala;
			striimi >> pala;
			//pusketaan pala taulukkoon
			pilkottu.push_back(pala);
		} while (striimi);

		if (pilkottu[0] == "v" || pilkottu[0] == "vn")
		{
			//otetaan floatit stringin luvuista
			float n1 = std::stof(pilkottu[1]);
			float n2 = std::stof(pilkottu[2]);
			float n3 = std::stof(pilkottu[3]);
			//testitulostus
			//cout << pilkottu[0] << ":" << pilkottu[1] << ":" << pilkottu[2] << ":" << pilkottu[3] << endl;
			//cout << "---------------------" << endl;
			//laitetaan numerot oikeisiin taulukoihin
			if (pilkottu[0] == "v")
			{
				vecv.push_back(Vector3f(n1, n2, n3));
			}
			else vecn.push_back(Vector3f(n1, n2, n3));

		}

		//faces käsittely
		if (pilkottu[0] == "f") {
			//erotinmerkki
			std::string erotin = "/";
			//taulukko johon tallennetaan yhden facen luvut
			vector<unsigned> naama;
			//splitataan loput stringit ja laitetaan ne naama taulukkoon
			for (int i = 1; i <= 3; i++) {
				//indeksisijainti
				size_t pos = 0;
				//alustetaan muuttuja erotettavalle palalle
				std::string pala;
				//käydään niin kauan läpi kuin erottimia löytyy
				while ((pos = pilkottu[i].find(erotin)) != std::string::npos)
				{
					//pilkotaan pala erottimen kohdalta
					pala = pilkottu[i].substr(0, pos);
					//muutetaan pala int ja laitetaan taulukkoo
					naama.push_back(std::stoi(pala));
					//poistetaan erotettu osa stringistä
					pilkottu[i].erase(0, pos + erotin.length());
				}
				//laitetaan vielä viimeinen pala taulukkoon
				naama.push_back(std::stoi(pilkottu[i]));
			};
			//lopuksi taulukko laitetaan vecf taulukkoon
			vecf.push_back(naama);
		}
	}

}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char** argv)
{
	loadInput();

	glutInit(&argc, argv);

	// We're going to animate it, so double buffer 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Initial parameters for window position and size
	glutInitWindowPosition(60, 60);
	glutInitWindowSize(360, 360);
	glutCreateWindow("Assignment 0");

	// Initialize OpenGL parameters.
	initRendering();

	// Set up callback functions for key presses
	glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
	glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

									// Set up the callback function for resizing windows
	glutReshapeFunc(reshapeFunc);

	// Call this whenever window needs redrawing
	glutDisplayFunc(drawScene);

	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop();

	return 0;	// This line is never reached.
}
