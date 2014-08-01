
#include "Control.h"

CONTROL::CONTROL()
{
	callback = NULL;
	volume = new VOLUME();
	miniture = new MINITURE();
	selection = new SELECTION();
	
	volumeFilename = NULL;
	minitureFilename = NULL;
	miniturePointFilename = NULL;
	meshFilename = NULL;
	textureFilename = NULL;
}

CONTROL::~CONTROL()
{
	if( volume != NULL ) delete volume;
	if( miniture != NULL ) delete miniture;
	if( selection != NULL ) delete selection;
	if( callback != NULL ) delete callback;
	
	if( volumeFilename != NULL ) free( volumeFilename );
	if( minitureFilename != NULL ) free( minitureFilename );
	if( miniturePointFilename != NULL ) free( miniturePointFilename );
	if( meshFilename != NULL ) free( meshFilename );
	if( textureFilename != NULL ) free( textureFilename );
}

void CONTROL::SetCallback( Callback *handle )
{
	callback = handle;
	if( volume != NULL ) volume->SetCallback( callback );
	if( miniture != NULL ) miniture->SetCallback( callback );
	if( selection != NULL ) selection->SetCallback( callback );
}

void CONTROL::DrawVolumeSlice( int canvasWidth, int canvasHeight )
{
	if( volume != NULL ) volume->DrawVolumeSlice( canvasWidth, canvasHeight );
}

void CONTROL::DrawVolumeCrop( int canvasWidth, int canvasHeight )
{
	if( volume != NULL ) volume->DrawVolumeCrop( canvasWidth, canvasHeight );
}

void CONTROL::DrawMiniture3D( int canvasWidth, int canvasHeight )
{
	if( miniture != NULL ) miniture->DrawMiniture3D( canvasWidth, canvasHeight );
}

void CONTROL::DrawSelectionSlice( int canvasWidth, int canvasHeight )
{
	//*************************************************************************************************************
}

void CONTROL::DrawSelection3D( int canvasWidth, int canvasHeight )
{
	//*************************************************************************************************************
}

void CONTROL::Draw3DModel( int canvasWidth, int canvasHeight )
{
	//*************************************************************************************************************
}

void CONTROL::DrawProgressBars( int canvasWidth, int canvasHeight )
{
	if( callback != NULL ) callback->DrawProgressBars( canvasWidth, canvasHeight );
}

void CONTROL::MousedPressed( int drawFunction, int x, int y, int button, int modifiers, int w, int h )
{
	if( volume != NULL )
	{
		//**********************************************************************************************
		if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_SLICE ) volume->SetCropPoint( x, y, w, h );
	}
	
	/*
	public static int CANVAS_WINDOW_DRAW_VOLUME_SLICE   = 10;
	public static int CANVAS_WINDOW_DRAW_VOLUME_CROP = 11;
	public static int CANVAS_WINDOW_DRAW_MINITURE_3D    = 21;
	public static int CANVAS_WINDOW_DRAW_REGION_SLICE   = 30;
	public static int CANVAS_WINDOW_DRAW_REGION_3D      = 31;
	public static int CANVAS_WINDOW_DRAW_3D_MODEL       = 50;
	
		if( event.getButton() == MouseEvent.BUTTON1 ) button = 1;
		if( event.getButton() == MouseEvent.BUTTON2 ) button = 2;
		if( event.getButton() == MouseEvent.BUTTON3 ) button = 3;
		
		if( event.isAltDown() ) modifiers += 1;
		if( event.isControlDown() ) modifiers += 2;
		if( event.isShiftDown() ) modifiers += 4;
	//*/
}

void CONTROL::MousedReleased( int drawFunction, int x, int y, int button, int modifiers, int w, int h )
{
	if( volume != NULL )
	{
		//**********************************************************************************************
		if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_SLICE ) volume->MoveCropPoint( x, y, w, h );
		else if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_CROP ) volume->GetPointStructureIntensity( x, y, w, h );
	}
}

void CONTROL::MousedDragged( int drawFunction, int x, int y, int button, int modifiers, int w, int h )
{
	if( volume != NULL )
	{
		//**********************************************************************************************
		if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_SLICE ) volume->MoveCropPoint( x, y, w, h );
	}
}

int CONTROL::LoadVolume( char filename[] )
{
	volumeFilename = filename;
	
	if( volume == NULL ) return -1;
	return volume->LoadVolume( filename );
}

int CONTROL::LoadFloatVolume( char filename[], int width, int height, int depth )
{
	volumeFilename = filename;
	
	if( volume == NULL ) return -1;
	return volume->LoadFloatVolume( filename, width, height, depth );
}

int CONTROL::LoadMinitureVolume( char filename[], int width, int height, int depth )
{
	minitureFilename = filename;
	
	if( miniture == NULL ) return -1;
	return miniture->LoadMinitureVolume( filename, width, height, depth );
}

int CONTROL::LoadMiniturePointStructureFile( char filename[] )
{
	miniturePointFilename = filename;
	
	if( miniture == NULL ) return -1;
	return miniture->LoadMiniturePointStructureFile( filename );
}

int CONTROL::LoadMeshFile( char filename[] )
{
	meshFilename = filename;
	
	/************************************************************/
	/************************************************************/
	/************************************************************/
	/************************************************************/
	return 0;
}

int CONTROL::LoadTextureFile( char filename[] )
{
	textureFilename = filename;
	
	/************************************************************/
	/************************************************************/
	/************************************************************/
	/************************************************************/
	return 0;
}

int CONTROL::GetVolumeWidth()
{
	if( volume == NULL ) return -1;
	return volume->GetVolumeWidth();
}

int CONTROL::GetVolumeHeight()
{
	if( volume == NULL ) return -1;
	return volume->GetVolumeHeight();
}

int CONTROL::GetVolumeDepth()
{
	if( volume == NULL ) return -1;
	return volume->GetVolumeDepth();
}

void CONTROL::LoadVolumeSlice( int sliceIndex )
{
	if( volume != NULL ) volume->LoadSlice( sliceIndex );
}

void CONTROL::VolumeAutoIntensity( int sliceIndex )
{
	if( volume != NULL ) volume->AutoIntensity( sliceIndex );
}

void CONTROL::SetVolumeIntensity( float minimum, float maximum )
{
	if( volume != NULL ) volume->SetIntensity( minimum, maximum );
}

void CONTROL::SetPointStructureIntensity( float minimum, float maximum )
{
	if( volume != NULL ) volume->SetPointStructureIntensity( minimum, maximum );
}

void CONTROL::SetPointStructure( bool value )
{
	if( volume != NULL ) volume->SetPointStructure( value );
}

void CONTROL::ResetVolumeCroppedRegion()
{
	if( volume != NULL ) volume->ResetCroppedRegion();
}

int CONTROL::SaveNewProject( char *volumeFile, int start, int end, char *minitureFile, float scale, char *miniturePointFile, char *meshFile, char *textureFile )
{
	if( volumeFilename != NULL ) free( volumeFilename );
	if( minitureFilename != NULL ) free( minitureFilename );
	if( miniturePointFilename != NULL ) free( miniturePointFilename );
	if( meshFilename != NULL ) free( meshFilename );
	if( textureFilename != NULL ) free( textureFilename );
	
	volumeFilename = volumeFile;
	minitureFilename = minitureFile;
	miniturePointFilename = miniturePointFile;
	meshFilename = meshFile;
	textureFilename = textureFile;
	
	if( volume == NULL || miniture == NULL ) return -1;
	
	if( callback != NULL )
	{
		callback->SetTotalProgressTitle( "Saving New Project" );
		callback->ZeroTotalProgress();
	}
	
	int result = volume->SaveFloatVolume( volumeFilename, start, end );
	if( result != 0 ) return result + 100;
	
	if( callback != NULL ) callback->AddToTotal( 70.0f ); // 70% done
	
	// Create Miniture Volume
	result = volume->CreateMiniture( minitureFile, scale );
	if( result != 0 ) return result + 200;
	
	if( callback != NULL ) callback->AddToTotal( 25.0f ); // 95% done
	
	// Create Miniture Point Structure
//*********************************************************************************************************************
	
	return 0;
}
