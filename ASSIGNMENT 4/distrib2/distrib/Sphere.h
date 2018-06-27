#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		this->center = Vector3f(0, 0, 0);
		this->radius = 1.f;
		//this->material = Material(Vector3f(1, 0, 0), Vector3f(0, 0, 0), 0.5);
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){

	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){

		//pallo origossa

		//muuttuja alustuksia
		//säteen origin ja suunta
		Vector3f Rd = r.getDirection();
		Vector3f Ro = r.getOrigin();
		float r2 = this->radius * this->radius;
		//aina 1, koska suuntavektorin pituus on aina 1 (normalisoitu)
		float a = 1;
		float b = 2 * (Vector3f::dot(Rd, Ro));
		float c = Vector3f::dot(Ro, Rd) - r2;
		
		//jos 2 asteen yhtälöllä ei ratkaisua palautetaan false
		if (b * b - 4 * a * c < 0) return false;

		float d = sqrt(b*b - 4 * a*c);

		float t1 = (-b + d) / 2 * a;
		float t2 = (-b - d) / 2 * a,

		//muuttuja lähemmän leikkauspisteen tallentamiseksi
		float t;
		
		//otetaan lähempi pisteen leikkauksen etäisyys
		if (t1 < t2) t = t1;
		else t = t2;
		
		//palautetaan false jos on lähempänä kuin tmin
		if (t <= tmin) return false;

		//Katsotaan onko leikkauspiste lähempänä kuin nykyinen hit
		if (t < h.getT()) {
			//säteen leikkauspiste pallon pinnalla, normalisoituna
			Vector3f normaali = r.pointAtParameter(t).normalized();
			h.set(t, this->material, normaali);
			return true;
		}

	}

protected:
	
	Vector3f center;
	float radius;

};


#endif
