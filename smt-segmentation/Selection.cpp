
#include "Selection.h"

SELECTION::SELECTION()
{
	callback = NULL;
	selectionData = NULL;
	slice = NULL;
}

SELECTION::~SELECTION()
{
	if( selectionData != NULL ) free( selectionData );
	if( slice != NULL ) free( slice );
}

void SELECTION::SetCallback( Callback *handle )
{
	callback = handle;
}

void SELECTION::SetSelection( float *data, int dataWidth, int dataHeight, int dataDepth )
{
	if( selectionData != NULL ) free( selectionData );
	selectionData = data;
	
	width = dataWidth;
	height = dataHeight;
	depth = dataDepth;
	openglWidth = width - ( width % 4 );
	
	ResetCamera();
	ResetPlane( 0 );
}

void SELECTION::SetIntensity( float minimum, float maximum )
{
	minimumIntensity = minimum;
	maximumIntensity = maximum;
}

int SELECTION::LoadInformation( FILE *loadFile )
{
//********************************************************************************************************************
	return 0;
}

int SELECTION::SaveCurrentInformation( FILE *saveFile )
{
//********************************************************************************************************************
	return 0;
}

void SELECTION::ResetCamera()
{
	cameraX = (float)width / 2.0f;
	cameraY = (float)height / 2.0f;
	cameraZ = (float)depth / 2.0f;
	cameraAngleX = 0.0f;
	cameraAngleY = 0.0f;
	cameraDistance = (float)depth * 2.5f;
}

void SELECTION::RotateCameraX( float angleAmount )
{
	cameraAngleX += angleAmount;
}

void SELECTION::RotateCameraY( float angleAmount )
{
	cameraAngleY += angleAmount;
}

void SELECTION::TranslateCamera( float xAmount, int yAmount, int zAmount )
{
	cameraX += xAmount;
	cameraY += yAmount;
	cameraZ += zAmount;
}


void SELECTION::ResetPlane( int sliceIndex )
{
	float sliceDepth = (float)sliceIndex;
	
	lowerLeft[0] = 0.0f;
	lowerLeft[1] = 0.0f;
	lowerLeft[2] = sliceDepth;
	
	upperLeft[0] = 0.0f;
	upperLeft[1] = (float)height;
	upperLeft[2] = sliceDepth;
	
	lowerRight[0] = (float)width;
	lowerRight[1] = 0.0f;
	lowerRight[2] = sliceDepth;
	
	upperRight[0] = (float)width;
	upperRight[1] = (float)height;
	upperRight[2] = sliceDepth;
}

void SELECTION::RotatePlaneAroundX( float angleAmount )
{
	float sinValue = sinf( angleAmount );
	float cosValue = cosf( angleAmount );
	
	float centerY = ( lowerLeft[1] + upperLeft[1] + lowerRight[1] + upperRight[1] ) / 4.0f;
	float centerZ = ( lowerLeft[2] + upperLeft[2] + lowerRight[2] + upperRight[2] ) / 4.0f;
	
	float temporaryY, temporaryZ;
	
	temporaryY = lowerLeft[1] - centerY;
	temporaryZ = lowerLeft[2] - centerZ;
	lowerLeft[1] = ( ( temporaryY * cosValue ) - ( temporaryZ * sinValue ) ) + centerY;
	lowerLeft[2] = ( ( temporaryY * sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryY = upperLeft[1] - centerY;
	temporaryZ = upperLeft[2] - centerZ;
	upperLeft[1] = ( ( temporaryY * cosValue ) - ( temporaryZ * sinValue ) ) + centerY;
	upperLeft[2] = ( ( temporaryY * sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryY = lowerRight[1] - centerY;
	temporaryZ = lowerRight[2] - centerZ;
	lowerRight[1] = ( ( temporaryY * cosValue ) - ( temporaryZ * sinValue ) ) + centerY;
	lowerRight[2] = ( ( temporaryY * sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryY = upperRight[1] - centerY;
	temporaryZ = upperRight[2] - centerZ;
	upperRight[1] = ( ( temporaryY * cosValue ) - ( temporaryZ * sinValue ) ) + centerY;
	upperRight[2] = ( ( temporaryY * sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
}

void SELECTION::RotatePlaneAroundY( float angleAmount )
{
	float sinValue = sinf( angleAmount );
	float cosValue = cosf( angleAmount );
	
	float centerX = ( lowerLeft[0] + upperLeft[0] + lowerRight[0] + upperRight[0] ) / 4.0f;
	float centerZ = ( lowerLeft[2] + upperLeft[2] + lowerRight[2] + upperRight[2] ) / 4.0f;
	
	float temporaryX, temporaryZ;
	
	temporaryX = lowerLeft[0] - centerX;
	temporaryZ = lowerLeft[2] - centerZ;
	lowerLeft[0] = ( ( temporaryX * cosValue ) + ( temporaryZ * sinValue ) ) + centerX;
	lowerLeft[2] = ( ( temporaryX * -sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryX = upperLeft[0] - centerX;
	temporaryZ = upperLeft[2] - centerZ;
	upperLeft[0] = ( ( temporaryX * cosValue ) + ( temporaryZ * sinValue ) ) + centerX;
	upperLeft[2] = ( ( temporaryX * -sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryX = lowerRight[0] - centerX;
	temporaryZ = lowerRight[2] - centerZ;
	lowerRight[0] = ( ( temporaryX * cosValue ) + ( temporaryZ * sinValue ) ) + centerX;
	lowerRight[2] = ( ( temporaryX * -sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
	
	temporaryX = upperRight[0] - centerX;
	temporaryZ = upperRight[2] - centerZ;
	upperRight[0] = ( ( temporaryX * cosValue ) + ( temporaryZ * sinValue ) ) + centerX;
	upperRight[2] = ( ( temporaryX * -sinValue ) + ( temporaryZ * cosValue ) ) + centerZ;
}

void SELECTION::RotatePlaneAroundZ( float angleAmount )
{
	float sinValue = sinf( angleAmount );
	float cosValue = cosf( angleAmount );
	
	float centerX = ( lowerLeft[0] + upperLeft[0] + lowerRight[0] + upperRight[0] ) / 4.0f;
	float centerY = ( lowerLeft[1] + upperLeft[1] + lowerRight[1] + upperRight[1] ) / 4.0f;
	
	float temporaryX, temporaryY;
	
	temporaryX = lowerLeft[0] - centerX;
	temporaryY = lowerLeft[1] - centerY;
	lowerLeft[0] = ( ( temporaryX * cosValue ) - ( temporaryY * sinValue ) ) + centerX;
	lowerLeft[1] = ( ( temporaryX * sinValue ) + ( temporaryY * cosValue ) ) + centerY;
	
	temporaryX = upperLeft[0] - centerX;
	temporaryY = upperLeft[1] - centerY;
	upperLeft[0] = ( ( temporaryX * cosValue ) - ( temporaryY * sinValue ) ) + centerX;
	upperLeft[1] = ( ( temporaryX * sinValue ) + ( temporaryY * cosValue ) ) + centerY;
	
	temporaryX = lowerRight[0] - centerX;
	temporaryY = lowerRight[1] - centerY;
	lowerRight[0] = ( ( temporaryX * cosValue ) - ( temporaryY * sinValue ) ) + centerX;
	lowerRight[1] = ( ( temporaryX * sinValue ) + ( temporaryY * cosValue ) ) + centerY;
	
	temporaryX = upperRight[0] - centerX;
	temporaryY = upperRight[1] - centerY;
	upperRight[0] = ( ( temporaryX * cosValue ) - ( temporaryY * sinValue ) ) + centerX;
	upperRight[1] = ( ( temporaryX * sinValue ) + ( temporaryY * cosValue ) ) + centerY;
}

void SELECTION::TranslatePlane( int xAmount, int yAmount, int zAmount )
{
	float x = (float)xAmount;
	float y = (float)yAmount;
	float z = (float)zAmount;
	
	float centerX = ( lowerLeft[0] + upperLeft[0] + lowerRight[0] + upperRight[0] ) / 4.0f;
	float centerY = ( lowerLeft[1] + upperLeft[1] + lowerRight[1] + upperRight[1] ) / 4.0f;
	float centerZ = ( lowerLeft[2] + upperLeft[2] + lowerRight[2] + upperRight[2] ) / 4.0f;
	
	if( ( centerX + x ) < 0.0f || ( centerX + x ) > (float)width ) x = 0.0f;
	if( ( centerY + y ) < 0.0f || ( centerY + y ) > (float)height ) y = 0.0f;
	if( ( centerZ + z ) < 0.0f || ( centerZ + z ) > (float)depth ) z = 0.0f;
	
	lowerLeft[0] += x;
	lowerLeft[1] += y;
	lowerLeft[2] += z;
	
	upperLeft[0] += x;
	upperLeft[1] += y;
	upperLeft[2] += z;
	
	lowerRight[0] += x;
	lowerRight[1] += y;
	lowerRight[2] += z;
	
	upperRight[0] += x;
	upperRight[1] += y;
	upperRight[2] += z;
}

void SELECTION::ScalePlane( float amount )
{
	if( amount <= 0.5f || amount > 1.5f ) return;
	
	float centerX = ( lowerLeft[0] + upperLeft[0] + lowerRight[0] + upperRight[0] ) / 4.0f;
	float centerY = ( lowerLeft[1] + upperLeft[1] + lowerRight[1] + upperRight[1] ) / 4.0f;
	float centerZ = ( lowerLeft[2] + upperLeft[2] + lowerRight[2] + upperRight[2] ) / 4.0f;
	
	float temporaryX, temporaryY, temporaryZ;
	
	temporaryX = lowerLeft[0] - centerX;
	temporaryY = lowerLeft[1] - centerY;
	temporaryZ = lowerLeft[2] - centerZ;
	lowerLeft[0] = ( temporaryX * amount ) + centerX;
	lowerLeft[1] = ( temporaryY * amount ) + centerY;
	lowerLeft[2] = ( temporaryZ * amount ) + centerZ;
	
	temporaryX = upperLeft[0] - centerX;
	temporaryY = upperLeft[1] - centerY;
	temporaryZ = upperLeft[2] - centerZ;
	upperLeft[0] = ( temporaryX * amount ) + centerX;
	upperLeft[1] = ( temporaryY * amount ) + centerY;
	upperLeft[2] = ( temporaryZ * amount ) + centerZ;
	
	temporaryX = lowerRight[0] - centerX;
	temporaryY = lowerRight[1] - centerY;
	temporaryZ = lowerRight[2] - centerZ;
	lowerRight[0] = ( temporaryX * amount ) + centerX;
	lowerRight[1] = ( temporaryY * amount ) + centerY;
	lowerRight[2] = ( temporaryZ * amount ) + centerZ;
	
	temporaryX = upperRight[0] - centerX;
	temporaryY = upperRight[1] - centerY;
	temporaryZ = upperRight[2] - centerZ;
	upperRight[0] = ( temporaryX * amount ) + centerX;
	upperRight[1] = ( temporaryY * amount ) + centerY;
	upperRight[2] = ( temporaryZ * amount ) + centerZ;
}

void SELECTION::CalculatePlane()
{
	if( selectionData == NULL ) return;
	
	if( slice == NULL ) slice = (unsigned char *)malloc(sizeof(unsigned char)*openglWidth*height);
	
	float thresholdFactor = 255.0f / (float)( maximumIntensity - minimumIntensity );
	
	float leftVectorX = upperLeft[0] - lowerLeft[0];
	float leftVectorY = upperLeft[1] - lowerLeft[1];
	float leftVectorZ = upperLeft[2] - lowerLeft[2];
	
	float rightVectorX = upperRight[0] - lowerRight[0];
	float rightVectorY = upperRight[1] - lowerRight[1];
	float rightVectorZ = upperRight[2] - lowerRight[2];
	
	float t1 = 0.0f, t2;
	float t1Increament = 1.0f / ( (float)height - 1 );
	float t2Increament = 1.0f / ( (float)width - 1 );
	
	float p1x, p1y, p1z, p2x, p2y, p2z, vectorp1p2x, vectorp1p2y, vectorp1p2z, value;
	int x, y, z;
	
	int location = 0;
	for( int i = 0; i < height; i++ )
	{
		p1x = lowerLeft[0] + ( leftVectorX * t1 );
		p1y = lowerLeft[1] + ( leftVectorY * t1 );
		p1z = lowerLeft[2] + ( leftVectorZ * t1 );
		
		p2x = lowerRight[0] + ( rightVectorX * t1 );
		p2y = lowerRight[1] + ( rightVectorY * t1 );
		p2z = lowerRight[2] + ( rightVectorZ * t1 );
		
		vectorp1p2x = p2x - p1x;
		vectorp1p2y = p2y - p1y;
		vectorp1p2z = p2z - p1z;
		
		t2 = 0.0f;
		for( int j = 0; j < width; j++ )
		{
			x = (int)( p1x + ( vectorp1p2x * t2 ) );
			y = (int)( p1y + ( vectorp1p2y * t2 ) );
			z = (int)( p1z + ( vectorp1p2z * t2 ) );
			
			value = 0.0f;
			if( (x >= 0 && x < width && y >= 0) || (y < height && z >= 0 && z < depth) )
			{
				long offset = ( ( ( z * height ) + y ) * width ) + x;
				value = selectionData[offset];
			}
			
			if( value < minimumIntensity ) value = minimumIntensity;
			if( value > maximumIntensity ) value = maximumIntensity;
			value -= minimumIntensity;
			value = value * thresholdFactor;
			
			slice[location++] = (unsigned char)value;
			t2 += t2Increament;
		}
		t1 = t1Increament;
	}
}

// Opengl functions
void SELECTION::DrawMinitureSlice( int canvasWidth, int canvasHeight )
{
	glViewport( 0, 0, canvasWidth, canvasHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight );
	// GLKMatrix4MakeOrtho( 0.0,(float)canvasWidth, 0.0, (float)canvasHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	if( slice != NULL )
	{
		glRasterPos2f( 0.0f, 0.0f );
		float scaleFactorX = (float)canvasWidth / (float)openglWidth;
		float scaleFactorY = (float)canvasHeight / (float)height;
		glPixelZoom( scaleFactorX, scaleFactorY );
		glDrawPixels( openglWidth, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, slice );
	}
}

void SELECTION::DrawMiniture3D( int canvasWidth, int canvasHeight )
{
	
//********************************************************************************************************
	
}
