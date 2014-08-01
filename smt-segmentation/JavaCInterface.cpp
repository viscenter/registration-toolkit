/**********************************
<***
need to add to JavaCInterface.h -> void RedrawJavaSide();
***>

<***
if this is altered in CanvasWindow.java   alter in Mouse... Functions

	public static int CANVAS_WINDOW_DRAW_VOLUME_SLICE   = 10;
	public static int CANVAS_WINDOW_DRAW_MINITURE_SLICE = 20;
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
***>

//*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "JavaCInterface.h"
#include "Control.h"
#include "Callback.h"

CONTROL *control = NULL;
Callback *callback = NULL;

// Java callback
JNIEnv *javaEnv;
jobject javaObj;
jmethodID javaRedrawId = 0, javaSetVolumeIntensity = 0;
jmethodID javaSetCropRegion = 0, javaSetPointStructureIntensity = 0;
jmethodID javaSetIntensityLabel = 0;



void RedrawJavaSide()
{
	if( javaRedrawId == 0 ) return;
	javaEnv->CallVoidMethod( javaObj, javaRedrawId );
}

void JavaSetVolumeIntensity( float minimum, float maximum )
{
	if( javaSetVolumeIntensity == 0 ) return;
	javaEnv->CallVoidMethod( javaObj, javaSetVolumeIntensity, (jfloat)minimum, (jfloat)maximum );
}

void JavaSetCropRegion( int minx, int maxx, int miny, int maxy, int minz, int maxz )
{
	if( javaSetCropRegion == 0 ) return;
	javaEnv->CallVoidMethod( javaObj, javaSetCropRegion, minx, maxx, miny, maxy, minz, maxz );
}

void JavaSetPointStructureIntensity( float minimum, float maximum )
{
	if( javaSetPointStructureIntensity == 0 ) return;
	javaEnv->CallVoidMethod( javaObj, javaSetPointStructureIntensity, (jfloat)minimum, (jfloat)maximum );
}

void JavaSetIntensityLabel( float minimum, float maximum, int data[] )
{
	if( javaSetIntensityLabel == 0 ) return;
	
	jintArray array = (jintArray)javaEnv->NewIntArray(256);
	javaEnv->SetIntArrayRegion( array, 0, 255, (jint *)data );
	
	javaEnv->CallVoidMethod( javaObj, javaSetIntensityLabel, (jfloat)minimum, (jfloat)maximum, array );
}

JNIEXPORT void JNICALL Java_JavaCInterface_SetupCSide( JNIEnv *env, jobject obj )
{
	control = new CONTROL();
	
	jclass cls = env->GetObjectClass( obj );
	javaEnv = env;
	javaObj = obj;
	javaRedrawId = env->GetMethodID( cls, "Redraw", "()V" );
	javaSetVolumeIntensity = env->GetMethodID( cls, "SetVolumeIntensityFields", "(FF)V" );
	javaSetCropRegion = env->GetMethodID( cls, "SetCropRegionFields", "(IIIIII)V" );
	javaSetPointStructureIntensity = env->GetMethodID( cls, "SetPointStructureIntensityFields", "(FF)V" );
	javaSetIntensityLabel = env->GetMethodID( cls, "SetIntensityLabel", "(FF[I)V" );
	
	callback = new Callback();
	control->SetCallback( callback );
}

JNIEXPORT void JNICALL Java_JavaCInterface_FreeCSide( JNIEnv *env, jobject obj )
{
	if( control != NULL ) delete control;
	if( callback != NULL ) delete callback;
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawVolumeSlice( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->DrawVolumeSlice( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawVolumeCrop( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->DrawVolumeCrop( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawMiniture3D( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->DrawMiniture3D( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawRegionSlice( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->DrawSelectionSlice( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawRegion3D( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->DrawSelection3D( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_Draw3DModel( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( control != NULL ) control->Draw3DModel( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_DrawProgressBar( JNIEnv *env, jobject obj, jint canvasWidth, jint canvasHeight )
{
	if( callback != NULL ) callback->DrawProgressBars( canvasWidth, canvasHeight );
}

JNIEXPORT void JNICALL Java_JavaCInterface_MousePressed( JNIEnv *env, jobject obj, jint drawFunction, jint x, jint y, jint button, jint modifiers, jint w, jint h )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->MousedPressed( drawFunction, x, y, button, modifiers, w, h );
}

JNIEXPORT void JNICALL Java_JavaCInterface_MouseReleased( JNIEnv *env, jobject obj, jint drawFunction, jint x, jint y, jint button, jint modifiers, jint w, jint h )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->MousedReleased( drawFunction, x, y, button, modifiers, w, h );
}

JNIEXPORT void JNICALL Java_JavaCInterface_MouseDragged( JNIEnv *env, jobject obj, jint drawFunction, jint x, jint y, jint button, jint modifiers, jint w, jint h )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->MousedDragged( drawFunction, x, y, button, modifiers, w, h );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadVolumeFile( JNIEnv *env, jobject obj, jstring javaFilename )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadVolume( filename );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadFloatVolumeFile( JNIEnv *env, jobject obj, jstring javaFilename, jint width, jint height, jint depth )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadFloatVolume( filename, width, height, depth );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadMinitureFile( JNIEnv *env, jobject obj, jstring javaFilename, jint width, jint height, jint depth )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadMinitureVolume( filename, width, height, depth );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadMiniturePointStructureFile( JNIEnv *env, jobject obj, jstring javaFilename )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadMiniturePointStructureFile( filename );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadMeshFile( JNIEnv *env, jobject obj, jstring javaFilename )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadMeshFile( filename );
}

JNIEXPORT jint JNICALL Java_JavaCInterface_LoadTextureFile( JNIEnv *env, jobject obj, jstring javaFilename )
{
	javaEnv = env;
	javaObj = obj;
	if( control == NULL ) return -1;
	
	const char *str = env->GetStringUTFChars( javaFilename, 0 );
	char filename[256];
	strcpy( filename, str );
	env->ReleaseStringUTFChars( javaFilename, str );
	
	return (jint)control->LoadTextureFile( filename );
}

JNIEXPORT jintArray JNICALL Java_JavaCInterface_GetVolumeDimensions( JNIEnv *env, jobject obj )
{
	javaEnv = env;
	javaObj = obj;
	
	int dimensions[3];
	dimensions[0] = -1;
	dimensions[1] = -1;
	dimensions[2] = -1;
	
	if( control != NULL )
	{
		dimensions[0] = control->GetVolumeWidth();
		dimensions[1] = control->GetVolumeHeight();
		dimensions[2] = control->GetVolumeDepth();
	}
	
	jintArray array = (jintArray)env->NewIntArray(3);
	env->SetIntArrayRegion( array, 0, 3, (jint *)dimensions );
	return array;
}

JNIEXPORT void JNICALL Java_JavaCInterface_LoadVolumeSlice( JNIEnv *env, jobject obj, jint sliceIndex )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->LoadVolumeSlice( sliceIndex );
}

JNIEXPORT void JNICALL Java_JavaCInterface_AutoIntensity( JNIEnv *env, jobject obj, jint sliceIndex )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->VolumeAutoIntensity( sliceIndex );
}

JNIEXPORT void JNICALL Java_JavaCInterface_SetVolumeIntensity( JNIEnv *env, jobject obj, jfloat minimum, jfloat maximum )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->SetVolumeIntensity( minimum, maximum );
}

JNIEXPORT void JNICALL Java_JavaCInterface_SetPointStructureIntensity( JNIEnv *env, jobject obj, jfloat minimum, jfloat maximum )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->SetPointStructureIntensity( minimum, maximum );
}

JNIEXPORT void JNICALL Java_JavaCInterface_SetPointStructure( JNIEnv *env, jobject obj, jint value )
{
	javaEnv = env;
	javaObj = obj;
	bool booleanValue = true;
	if( (int)value == 0 ) booleanValue = false;
	
	if( control != NULL ) control->SetPointStructure( booleanValue );
}

JNIEXPORT void JNICALL Java_JavaCInterface_ResetVolumeCropRegion( JNIEnv *env, jobject obj )
{
	javaEnv = env;
	javaObj = obj;
	if( control != NULL ) control->ResetVolumeCroppedRegion();
}

JNIEXPORT jint JNICALL Java_JavaCInterface_SaveNewProject( JNIEnv *env, jobject obj, jstring volumeFilename, jint start, jint end, jstring minitureFilename, jfloat scale, jstring miniturePointFilename, jstring meshFilename, jstring textureFilename )
{
	javaEnv = env;
	javaObj = obj;
	
	char *filename1, *filename2, *filename3, *filename4, *filename5;
	
	const char *str = env->GetStringUTFChars( volumeFilename, 0 );
	int length = env->GetStringUTFLength( volumeFilename ) + 1;
	filename1 = (char *)malloc(sizeof(char)*length);
	strcpy( filename1, str );
	env->ReleaseStringUTFChars( volumeFilename, str );
	
	str = env->GetStringUTFChars( minitureFilename, 0 );
	length = env->GetStringUTFLength( minitureFilename ) + 1;
	filename2 = (char *)malloc(sizeof(char)*length);
	strcpy( filename2, str );
	env->ReleaseStringUTFChars( minitureFilename, str );
	
	str = env->GetStringUTFChars( miniturePointFilename, 0 );
	length = env->GetStringUTFLength( miniturePointFilename ) + 1;
	filename3 = (char *)malloc(sizeof(char)*length);
	strcpy( filename3, str );
	env->ReleaseStringUTFChars( miniturePointFilename, str );
	
	str = env->GetStringUTFChars( meshFilename, 0 );
	length = env->GetStringUTFLength( meshFilename ) + 1;
	filename4 = (char *)malloc(sizeof(char)*length);
	strcpy( filename4, str );
	env->ReleaseStringUTFChars( meshFilename, str );
	
	str = env->GetStringUTFChars( textureFilename, 0 );
	length = env->GetStringUTFLength( textureFilename) + 1;
	filename5 = (char *)malloc(sizeof(char)*length);
	strcpy( filename5, str );
	env->ReleaseStringUTFChars( textureFilename, str );
	
	return control->SaveNewProject( filename1, (int)start, (int)end, filename2, (float)scale, filename3, filename4, filename5 );
}
