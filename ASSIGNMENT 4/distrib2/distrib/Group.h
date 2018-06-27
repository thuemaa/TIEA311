#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

//kooksi 0 jos kutsutaan konstruktoria ilman argumenttia
  Group(){
	  this->objM = 0;
  }
	
  Group( int num_objects ){
	  this->objM = num_objects;
  }

  ~Group(){
  }

  virtual bool intersect( const Ray& r , Hit& h , float tmin ) {
	  for (unsigned i = 0; i < objM; i++) {
		  objects[i]->intersect;
	  }
   }
	
  void addObject( int index , Object3D* obj ){
	  //virhe jos indeksi menee yli
	  if (index > objM) {
		  cout << "objekti index yli" << endl;
	  }
	  else this->objects.push_back(obj);
  }

  int getGroupSize(){ 
	  return this->objM;
  }

 private:
	 vector <Object3D*> objects;
	 int objM;
};

#endif
	
