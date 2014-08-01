#ifndef _EDUCE_VOLUME
#define _EDUCE_VOLUME

#ifndef _EDUCE_STANDARD_INCLUDES
#define _EDUCE_STANDARD_INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif // ENDOF - _EDUCE_STANDARD_INCLUDES

#ifndef _EDUCE_OPENGL_INCLUDES
#define _EDUCE_ewOPENGL_INCLUDES
#include <GL/glew.h>
// #include <GL/glu.h>
#include <GLUT/glut.h>
#include <GLKit/GLKMath.h>
#endif // ENDOF - _EDUCE_OPENGL_INCLUDES

#ifndef _EDUCE_MATH_INCLUDES
#define _EDUCE_MATH_INCLUDES
#include <math.h>
#endif // ENDOF - _EDUCE_MATH_INCLUDES

#include "Miniture.h"
#include "Selection.h"
#include "Callback.h"

#ifndef _EDUCE_CROPPED_REGION
#define _EDUCE_CROPPED_REGION
struct CROPPED_REGION
{
	int llx, lly, llz, urx, ury, urz;
};
#endif // ENDOF - _EDUCE_CROPPED_REGION


class VOLUME
{
	public:
		VOLUME();
		~VOLUME();
		
		void SetCallback( Callback *handle );
		
		int LoadVolume( char filename[] ); // CT-Scan Volume
		int LoadFloatVolume( char filename[], int width, int height, int depth );
		int SaveFloatVolume( char filename[], int start, int end );
		
		int GetVolumeWidth();
		int GetVolumeHeight();
		int GetVolumeDepth();
		
		int LoadInformation( char filename[] );
		int SaveCurrentInformation( char filename[] ); // Appends to file
		
		void LoadSlice( int sliceIndex );
		
		void AutoIntensity( int sliceIndex );
		void SetIntensity( float minimum, float maximum ); // pass to selection
		float GetMinimumIntensity();
		float GetMaximumIntensity();
		
		void SetPointStructureIntensity( float minimum, float maximum );
		void SetPointStructure( bool value );
		
		void ResetCroppedRegion();
		void SetCropRegion( int llx, int lly, int llz, int urx, int ury, int urz ); // -value means ignore
		void GetCroppedRegion( int region[] ); // returns [llx, lly, llz, urx, ury, urz]
		
		int CreateMiniture( char filename[], float scale );
		
		// Opengl functions
		void DrawVolumeSlice( int canvasWidth, int canvasHeight );
		void DrawVolumeCrop( int canvasWidth, int canvasHeight );
		
		// Opengl Mouse Functions
		void SetCropPoint( int x, int y, int canvasWidth, int canvasHeight );
		void MoveCropPoint( int x, int y, int canvasWidth, int canvasHeight );
		void SetCurrentRegion();
		void GetPointStructureIntensity( int x, int y, int canvasWidth, int canvasHeight );
	
	private:
		Callback *callback;
		FILE *volumeDataFile;
		long headerOffset; // for CT-Scan Volume files
		int width, height, depth, openglWidth;
		float minimumIntensity, maximumIntensity;
		float absoluteMinimumIntensity, absoluteMaximumIntensity;
		float minimumPointStructureIntensity, maximumPointStructureIntensity;
		bool pointStructure;
		
		int selectedCropCorner;
		CROPPED_REGION currentRegion;
		CROPPED_REGION *processedRegions;
		int numberOfProcessedRegions;
		
		unsigned char *slice, *croppedSlice;
		int currentSlice, croppedWidth, croppedHeight; // for opengl
};

#endif // ENDOF - _EDUCE_VOLUME
