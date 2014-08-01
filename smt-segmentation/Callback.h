#ifndef _EDUCE_Callback
#define _EDUCE_Callback

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

#include "JavaCInterface.h"

class Callback
{
	public:
		Callback();
		~Callback();
		
		// Callback functions
		void Redraw();
		void SetJavaVolumeIntensity( float minimum, float maximum );
		void SetJavaCropRegion( int minx, int maxx, int miny, int maxy, int minz, int maxz );
		void SetJavaPointStructureIntensity( float minimum, float maximum );
		void SetJavaIntensityLabel( float minimum, float maximum, int data[] );
		
		// Progress bar functions
		void ZeroProgress();
		void ZeroTotalProgress();
		void SetProgressTitle( const char title[] );
		void SetTotalProgressTitle( const char title[] );
		void Add( float value );
		void AddToTotal( float value );
		void Complete();
		void CompleteTotal();
		// Opengl
		void OutputString( char *string, GLfloat x, GLfloat y );
		void DrawProgressBars( int canvasWidth, int canvasHeight );
	
	private:
		char progressTitle[128], totalTitle[128];
		float progressValue, totalValue;
};

#endif // ENDOF - _EDUCE_Callback
