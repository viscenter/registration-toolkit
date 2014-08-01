#ifndef _EDUCE_CONTROL
#define _EDUCE_CONTROL

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
#endif // ENDOF - _EDUCE_OPENGL_INCLUDES

#include "Callback.h"
#include "Volume.h"
#include "Miniture.h"
#include "Selection.h"

// Same as in CavasWindow.java
#define CANVAS_WINDOW_DRAW_VOLUME_SLICE     10
#define CANVAS_WINDOW_DRAW_VOLUME_CROP      11

class CONTROL
{
	public:
		CONTROL();
		~CONTROL();
		void SetCallback( Callback *handle );
		void DrawVolumeSlice( int canvasWidth, int canvasHeight );
		void DrawVolumeCrop( int canvasWidth, int canvasHeight );
		void DrawMiniture3D( int canvasWidth, int canvasHeight );
		void DrawSelectionSlice( int canvasWidth, int canvasHeight );
		void DrawSelection3D( int canvasWidth, int canvasHeight );
		void Draw3DModel( int canvasWidth, int canvasHeight );
		void DrawProgressBars( int canvasWidth, int canvasHeight );
		void MousedPressed( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
		void MousedReleased( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
		void MousedDragged( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
		int LoadVolume( char filename[] );
		int LoadFloatVolume( char filename[], int width, int height, int depth );
		int LoadMinitureVolume( char filename[], int width, int height, int depth );
		int LoadMiniturePointStructureFile( char filename[] );
		int LoadMeshFile( char filename[] );
		int LoadTextureFile( char filename[] );
		int GetVolumeWidth();
		int GetVolumeHeight();
		int GetVolumeDepth();
		void LoadVolumeSlice( int sliceIndex );
		void VolumeAutoIntensity( int sliceIndex );
		void SetVolumeIntensity( float minimum, float maximum );
		void SetPointStructureIntensity( float minimum, float maximum );
		void SetPointStructure( bool value );
		void ResetVolumeCroppedRegion();
		
		int SaveNewProject( char *volumeFile, int start, int end, char *minitureFile, float scale, char *miniturePointFile, char *meshFile, char *textureFile );
	
	private:
		Callback *callback;
		VOLUME *volume;
		MINITURE *miniture;
		SELECTION *selection;
		
		char *volumeFilename, *minitureFilename, *miniturePointFilename, *meshFilename, *textureFilename;
};

#endif // ENDOF - _EDUCE_CONTROL
