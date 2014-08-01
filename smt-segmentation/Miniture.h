#ifndef _EDUCE_MINITURE
#define _EDUCE_MINITURE

#ifndef _EDUCE_STANDARD_INCLUDES
#define _EDUCE_STANDARD_INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif // ENDOF - _EDUCE_STANDARD_INCLUDES

#ifndef _EDUCE_OPENGL_INCLUDES
#define _EDUCE_OPENGL_INCLUDES
#include <GL/glew.h>
// #include <GL/glu.h>
#include <GLUT/glut.h>
#include <GLKit/GLKMath.h>
#endif // ENDOF - _EDUCE_OPENGL_INCLUDES

#ifndef _EDUCE_MATH_INCLUDES
#define _EDUCE_MATH_INCLUDES
#include <math.h>
#endif // ENDOF - _EDUCE_MATH_INCLUDES

#include "Callback.h"

#ifndef _EDUCE_CROPPED_REGION
#define _EDUCE_CROPPED_REGION
struct CROPPED_REGION
{
	int llx, lly, llz, urx, ury, urz;
};
#endif // ENDOF - _EDUCE_CROPPED_REGION

class MINITURE
{
	public:
		MINITURE();
		~MINITURE();
		void SetCallback( Callback *handle );
		
		int LoadMinitureVolume( char *filename, int width, int height, int depth );
		int LoadMiniturePointStructureFile( char *filename );
		void LoadSlice( int sliceIndex );
		
		// Opengl functions
		void DrawMiniture3D( int canvasWidth, int canvasHeight );
	
	private:
		Callback *callback;
		FILE *minitureDataFile, *miniturePointStructureFile;
		int width, height, depth, openglWidth;
		
		unsigned char *slice;
};

#endif // ENDOF - _EDUCE_MINITURE
