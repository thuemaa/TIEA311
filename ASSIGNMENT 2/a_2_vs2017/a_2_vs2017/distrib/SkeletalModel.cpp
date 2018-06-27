#include "SkeletalModel.h"

#include <FL/Fl.H>
#include <fstream>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.

	//streami filun lukemiselle
	std::ifstream filu(filename);
	std::string rivi;

	//luetaan streamia pitkälti samaan tapaan kuin assignment nollassa
	while (std::getline(filu, rivi))
	{
		istringstream streami(rivi);
		std::vector<string> pilkottu;

		do
		{
			string pala;
			streami >> pala;

			pilkottu.push_back(pala);
		} while (streami);

		cout << pilkottu[0] << ":" << pilkottu[1] << ":" << pilkottu[2] << ":" << pilkottu[3] << endl;
		
		//muutetaan floatit koordinaateista ja int indeksistä
		float x = std::stof(pilkottu[0]);
		float y = std::stof(pilkottu[1]);
		float z = std::stof(pilkottu[2]);
		int ind = std::stof(pilkottu[3]);
		
		Joint *joint = new Joint;
		//Transform nivelen vanhemman suhteen
		joint->transform = Matrix4f::translation(x, y, z);

		//laitetaan joint listaan
		m_joints.push_back(joint);

		//parent joint käsittely
		if (ind == -1) 
		{
			m_rootJoint = joint;
			//child nodeksi kaikki muut nodet (jaha ei näköjään)
			//joint->children = m_joints;
		}
		else 
		{
			//lisätään tämä nivel sen vanhemman lapseksi
			m_joints[ind]->children.push_back(joint);
		}
	}

}

//drawjoint funktion apufunktio
void drawJoints_apu(MatrixStack stack, Joint *joint) 
{
	//työnnetään joint transformation stackkiin
	stack.push(joint->transform);
	//ladataan glloadmatrixf transformaatio
	glLoadMatrixf(stack.top());
	//pallo
	glutSolidSphere(0.025f, 12, 12);
	//kutsutaan nivelen lapsinivelille samaa funktiota
	for (int i = 0; i < joint->children.size(); i++)
	{
		drawJoints_apu(stack, joint->children[i]);
	}
	//työnnetään pois matriisipinosta
	stack.pop();
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.

	//nivelten teko
	drawJoints_apu(m_matrixStack, m_rootJoint);

}

//TODO: Luiden teko
void drawSkeleton_apu(MatrixStack stack, Joint *joint)
{
	
	//Jointin transform stäkkiin
	stack.push(joint->transform);


	//lapsisolmujen käsittely
	for (int i = 0; i < joint->children.size(); i++)
	{

		
		//TÄHÄN TRANSFORMOINNIT ENNEN SOLIDCUBE PIIRTOA JA TUNGE NE STÄKKKIIN---------------------------
		
		

		//z koordinaatin suuntavektori lapsesta vanhempaa
		Vector3f suunta = joint->children[i]->transform.getCol(3).xyz();
		//x y z vektorit ohjeiden mukaan
		Vector3f z(suunta.normalized());
		Vector3f y(Vector3f::cross(z, Vector3f(0, 0, 1)).normalized());
		Vector3f x(Vector3f::cross(y, z).normalized());
		
		//matriisit:
		//kääntämisen matriisi
		Matrix4f kaanto(Vector4f(x, 0.), Vector4f(y, 0.), Vector4f(z, 0.), Vector4f(0,0,0,1), 1);
		//skaalaus kokoon nähden, skaalaa 
		Matrix4f skaalaus(Matrix4f::scaling(0.02, 0.02, suunta.abs()));
		Matrix4f translate(Matrix4f::translation(0, 0, 0.5));
		

		//työnnetään stäkkiin kääntö, skaalaus ja translate
		stack.push(kaanto * skaalaus * translate);
		//----------------------------------------------------------------------------------------------
		glLoadMatrixf(stack.top());
		glutSolidCube(1.0f);
		stack.pop();

		//kutstutaan rekursiivisesti lapsijointeille
		drawSkeleton_apu(stack, joint->children[i]);
	}

	stack.pop();


}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	drawSkeleton_apu(m_matrixStack, m_rootJoint);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
}

