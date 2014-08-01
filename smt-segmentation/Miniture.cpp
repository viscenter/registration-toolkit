
#include "Miniture.h"

MINITURE::MINITURE()
{
	callback = NULL;
	minitureDataFile = NULL;
	miniturePointStructureFile = NULL;
	slice = NULL;
}

MINITURE::~MINITURE()
{
	if( minitureDataFile != NULL ) fclose( minitureDataFile );
	if( miniturePointStructureFile != NULL ) fclose( miniturePointStructureFile );
	if( slice != NULL ) free( slice );
}

void MINITURE::SetCallback( Callback *handle )
{
	callback = handle;
}

int MINITURE::LoadMinitureVolume( char *filename, int width, int height, int depth )
{
	
	return 0;
}

int MINITURE::LoadMiniturePointStructureFile( char *filename )
{
	
	return 0;	
}

void MINITURE::LoadSlice( int sliceIndex )
{
//********************************************************************************************************
}


// Opengl functions
void MINITURE::DrawMiniture3D( int canvasWidth, int canvasHeight )
{
	glViewport( 0, 0, canvasWidth, canvasHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight );
	// GLKMatrix4MakeOrtho( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
//********************************************************************************************************
}
