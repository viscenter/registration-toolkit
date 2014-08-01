#ifndef _EDUCE_SELECTION
#define _EDUCE_SELECTION

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

class SELECTION
{
	public:
		SELECTION();
		~SELECTION();
		void SetCallback( Callback *handle );
		
		void SetSelection( float *data, int dataWidth, int dataHeight, int dataDepth );
		void SetIntensity( float minimum, float maximum );
		
		int LoadInformation( FILE *loadFile );
		int SaveCurrentInformation( FILE *saveFile ); // Appends to file
		
		void ResetCamera();
		void RotateCameraX( float angleAmount );
		void RotateCameraY( float angleAmount );
		void TranslateCamera( float xAmount, int yAmount, int zAmount );
		
		void ResetPlane( int sliceIndex );
		void RotatePlaneAroundX( float angleAmount );
		void RotatePlaneAroundY( float angleAmount );
		void RotatePlaneAroundZ( float angleAmount );
		void TranslatePlane( int xAmount, int yAmount, int zAmount );
		void ScalePlane( float amount );
		void CalculatePlane();
		
		// Opengl functions
		void DrawMinitureSlice( int canvasWidth, int canvasHeight );
		void DrawMiniture3D( int canvasWidth, int canvasHeight );
	
	private:
		Callback *callback;
		float *selectionData;
		int width, height, depth, openglWidth;
		unsigned char *slice;
		
		float minimumIntensity, maximumIntensity;
		float cameraX, cameraY, cameraZ, cameraAngleX, cameraAngleY, cameraDistance;
		float lowerLeft[3], upperLeft[3], lowerRight[3], upperRight[3];
};

#endif // ENDOF - _EDUCE_SELECTION
