#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  for( int argNum = 1; argNum < argc; ++argNum )
    {
      std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }
	
    
  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene.  Write the color at the intersection to that
  // pixel in your output image.



 
  ///TODO: below demonstrates how to use the provided Image class

  ///Should be removed when you start
  Vector3f pixelColor(0.0f, 0, 0);
  //width and height
  Image image(1000, 1000);
 
  /// OMAAAA ----------------------------------
  for (size_t x = 0; x < image.Width(); x++) {
	  for (size_t y = 0; y < image.Height(); y++) {
		 
		  float imx = -1.f + (2*float(x) / image.Width());
		  float imy = -1.f + (2*float(y) / image.Width());
		  //pixelColor = Vector3f (-1.f + 2 * float(x)  / image.Width(), -1.f + 2* float(y)  / image.Width(), 0);
		  pixelColor = Vector3f(imx, imy, 0);
		  image.SetPixel(x, y, pixelColor);
		  
	  }

  }


  image.SaveImage("demo.bmp");

  return 0;
}

