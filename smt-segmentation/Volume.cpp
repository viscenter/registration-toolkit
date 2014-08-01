
#include "Volume.h"
#include "Callback.h"

VOLUME::VOLUME()
{
	callback = NULL;
	
	volumeDataFile = NULL;
	processedRegions = NULL;
	numberOfProcessedRegions = 0;
	slice = NULL;
	croppedSlice = NULL;
	
	minimumIntensity = 0.0f;
	maximumIntensity = 0.0f;
	
	minimumPointStructureIntensity = 0.0f;
	maximumPointStructureIntensity = 0.0f;
	pointStructure = false;
	
	selectedCropCorner = -1;
}

VOLUME::~VOLUME()
{
	if( volumeDataFile != NULL ) fclose( volumeDataFile );
	if( processedRegions != NULL ) free( processedRegions );
	if( slice != NULL ) free( slice );
	if( croppedSlice != NULL ) free( croppedSlice );
}

void VOLUME::SetCallback( Callback *handle )
{
	callback = handle;
}

int VOLUME::LoadVolume( char filename[] )
{
	if( volumeDataFile != NULL )
	{
		fclose( volumeDataFile );
		if( slice != NULL ) free( slice );
		slice = NULL;
	}
	
	volumeDataFile = fopen( filename, "rb" );
	if( volumeDataFile <= 0 )
	{
		volumeDataFile = NULL;
		return -1;
	}
	
	// CT-Scan Volume
	int headerLength;
	fread( &headerLength, sizeof(int), 1, volumeDataFile );
	headerOffset = sizeof(int);
		
	// Read Header
	char *header = (char*)malloc(sizeof(char)*headerLength );
	fread( header, sizeof(char), headerLength, volumeDataFile );
	headerOffset += ( sizeof(char) * headerLength );
	
	// Read Width, Height, Depth
	fread( &width, sizeof(int), 1, volumeDataFile );
	fread( &height, sizeof(int), 1, volumeDataFile );
	fread( &depth, sizeof(int), 1, volumeDataFile );
	headerOffset += ( sizeof(int) * 3 );
	
	fread( (float*)header, sizeof(float), 1, volumeDataFile );
	fread( (float*)header, sizeof(float), 1, volumeDataFile );
	fread( (float*)header, sizeof(float), 1, volumeDataFile );
	fread( (float*)header, sizeof(float), 1, volumeDataFile );
	fread( (float*)header, sizeof(float), 1, volumeDataFile );
	headerOffset += ( sizeof(float) * 5 );
	
	fread( (float*)header, sizeof(int), 1, volumeDataFile );
	fread( (float*)header, sizeof(int), 1, volumeDataFile );
	fread( (float*)header, sizeof(int), 1, volumeDataFile );
	headerOffset += ( sizeof(int) * 3 );
	
	// Needed For Opengl Raster/DrawPixels
	openglWidth = width - ( width % 4 );
	
	ResetCroppedRegion();
	
	// Get first slice intensities
	if( minimumIntensity == maximumIntensity ) AutoIntensity( 0 );
	LoadSlice( 0 );
	
	return 0;
}

int VOLUME::LoadFloatVolume( char filename[], int w, int h, int d )
{
	if( volumeDataFile != NULL )
	{
		fclose( volumeDataFile );
		if( slice != NULL ) free( slice );
		slice = NULL;
	}
	
	volumeDataFile = fopen( filename, "rb" );
	if( volumeDataFile <= 0 )
	{
		volumeDataFile = NULL;
		return -1;
	}
	headerOffset = 0;
	
	width = w;
	height = h;
	depth = d;
	
	openglWidth = width - ( width % 4 );
	
	ResetCroppedRegion();
	
	// Get first slice intensities
	if( minimumIntensity == maximumIntensity ) AutoIntensity( 0 );
	LoadSlice( 0 );
	
	return 0;
}

int VOLUME::SaveFloatVolume( char filename[], int start, int end )
{
	if( volumeDataFile == NULL ) return -2;
	
	FILE *saveFile = fopen( filename, "wb" );
	if( saveFile <= 0 ) return -1;
	
	int minX = currentRegion.llx;
	int maxX = currentRegion.urx;
	int minY = currentRegion.lly;
	int maxY = currentRegion.ury;
	int minZ = start;
	int maxZ = end;
	
	int numberOfVoxelsInRow = maxX - minX;
	float *row = (float*)malloc(sizeof(float)*numberOfVoxelsInRow);
	
	if( callback != NULL )
	{
		callback->SetProgressTitle( "Saving Float Volume" );
		callback->ZeroProgress();
	}
	float percentCompletePerRow = 100.0f / (float)( maxZ - minZ );
	
	fseek( volumeDataFile, headerOffset, SEEK_SET );
	
	long offset = minZ * height * width; // Start of Minimum Slice
	for( int i = minZ; i < maxZ; i++ )
	{
		offset += minY * width; // Start Of Minimum Row
		for( int j = minY; j < maxY; j++ )
		{
			offset += minX; // Start Of Data
			fseek( volumeDataFile, offset * sizeof(float), SEEK_CUR );
			fread( row, sizeof(float), numberOfVoxelsInRow, volumeDataFile );
			fwrite( row, sizeof(float), numberOfVoxelsInRow, saveFile );
			offset = width - maxX; // End Of Row
		}
		offset += ( height - maxY ) * width;
		
		// Progress Bar
		if( callback != NULL ) callback->Add( percentCompletePerRow );
	}
	fclose( saveFile );
	
	if( callback != NULL ) callback->Complete();
	
	// Volume is file just created
	LoadFloatVolume( filename, maxX - minX, maxY - minY, maxZ - minZ );
	
	return 0;
}

int VOLUME::GetVolumeWidth()
{
	return width;
}

int VOLUME::GetVolumeHeight()
{
	return height;
}

int VOLUME::GetVolumeDepth()
{
	return depth;
}

int VOLUME::LoadInformation( char filename[] )
{
//*********************************************************************************************************
	return 0;
}

int VOLUME::SaveCurrentInformation( char filename[] )
{
//*********************************************************************************************************
	return 0;
}

void VOLUME::LoadSlice( int sliceIndex )
{
	if( volumeDataFile == NULL || sliceIndex < 0 || sliceIndex >= depth ) return;
	currentSlice = sliceIndex;
	
	long offset = headerOffset + ( sliceIndex * width * height * sizeof(float) );
	fseek( volumeDataFile, offset, SEEK_SET );
	if( slice == NULL ) slice = (unsigned char *)malloc(sizeof(unsigned char)*openglWidth*height*3);
	
	float *row = (float*)malloc(sizeof(float)*width);
	
	float scaleFactor = ( maximumIntensity - minimumIntensity ) / 255.0f;
	
	int data[256];
	for( int i = 0; i < 256; i++ ) data[i] = 0;
	float dataScaleFactor = ( absoluteMaximumIntensity - absoluteMinimumIntensity ) / 255.0f;
	
	int location = 0;
	for( int i = 0; i < height; i++ )
	{
		fread( row, sizeof(float), width, volumeDataFile );
		
		for( int j = 0; j < openglWidth; j++ )
		{
			float value = ( row[j] - minimumIntensity ) * scaleFactor;
			if( value < 0 ) value = 0;
			if( value > 255 ) value = 255;
			slice[location++] = (unsigned char)value;
			slice[location++] = (unsigned char)value;
			slice[location++] = (unsigned char)value;
		}
		
		if( callback != NULL )
		{
			for( int j = 0; j < width; j++ ) // for histogram
			{
				float value = ( row[j] - absoluteMinimumIntensity ) * dataScaleFactor;
				if( value < 0 ) value = 0;
				if( value > 255 ) value = 255;
				data[(int)value]++;
			}
		}
	}
	free( row );
	
	if( callback != NULL ) callback->SetJavaIntensityLabel( absoluteMinimumIntensity, absoluteMaximumIntensity, data );
	
	// If point cloud
	if( pointStructure && minimumPointStructureIntensity != maximumPointStructureIntensity )
	{
		int minimumPointStructure = (int)(( minimumPointStructureIntensity - minimumIntensity ) * scaleFactor);
		int maximumPointStructure = (int)(( maximumPointStructureIntensity - minimumIntensity ) * scaleFactor);
		for( int i = 0; i < openglWidth*height*3; i += 3 )
		{
			if( slice[i] >= minimumPointStructure && slice[i] <= maximumPointStructure )
			{
				slice[i+1] = (unsigned char)( ( (int)slice[i+1] + 255 ) / 2 );
			}
		}
	}
	
	// If order
	int temporary;
	if( currentRegion.urx < currentRegion.llx )
	{
		temporary = currentRegion.urx;
		currentRegion.urx = currentRegion.llx;
		currentRegion.llx = temporary;
	}
	if( currentRegion.ury < currentRegion.lly )
	{
		temporary = currentRegion.ury;
		currentRegion.ury = currentRegion.lly;
		currentRegion.lly = temporary;
	}
	
	int originalCroppedWidth = currentRegion.urx - currentRegion.llx;
	croppedWidth = originalCroppedWidth - ( originalCroppedWidth % 4 );
	croppedHeight = currentRegion.ury - currentRegion.lly;
	
	int location2 = 0;
	if( croppedWidth != width || croppedHeight != height )
	{
		if( croppedSlice != NULL ) free( croppedSlice );
		croppedSlice = (unsigned char*)malloc(sizeof(unsigned char)*croppedWidth*croppedHeight*3);
		
		for( int i = 0; i < croppedHeight; i++ )
		{
			location = ( currentRegion.lly + i ) * openglWidth * 3 + currentRegion.llx * 3;
			for( int j = 0; j < croppedWidth; j++ )
			{
				croppedSlice[location2++] = slice[location++];
				croppedSlice[location2++] = slice[location++];
				croppedSlice[location2++] = slice[location++];
			}
		}
	}
}


void VOLUME::AutoIntensity( int sliceIndex )
{
//*********************************************************************************************************	 Can improve
	int histogramSize = 2000;
	
	if( volumeDataFile == NULL ) return;
	
	long offset = headerOffset + ( sliceIndex * width * height * sizeof(float) );
	fseek( volumeDataFile, offset, SEEK_SET );
	
	float *temporary = (float*)malloc(sizeof(float)*width*height);
	fread( (float*)temporary, sizeof(float), width * height, volumeDataFile );
	
	minimumIntensity = temporary[0];
	maximumIntensity = temporary[0];
	
	for( int i = 0; i < width * height; i++ )
	{
		if( temporary[i] < minimumIntensity ) minimumIntensity = temporary[i];
		if( temporary[i] > maximumIntensity ) maximumIntensity = temporary[i];
	}
	absoluteMinimumIntensity = minimumIntensity;
	absoluteMaximumIntensity = maximumIntensity;
	
	int *array = (int*)malloc(sizeof(int)*histogramSize);
	for( int i = 0; i < histogramSize; i++ ) array[i] = 0;
	
	float scaleFactor = (float)histogramSize / ( maximumIntensity - minimumIntensity );
	
	for( int i = 0; i <  width * height; i++ )
	{
		int index = (int)(( temporary[i] - minimumIntensity ) * scaleFactor );
		if( index >= 0 && index < histogramSize )
		{
			array[index]++;
		}
	}
	free( temporary );
		
	int sum = 0, maximum = 0;
	for( int i = 0; i < histogramSize; i++ )
	{
		sum += array[i];
		if( array[i] > array[maximum] ) maximum = i;
	}
	
	int average = (int)( (float)sum / (float)histogramSize );
	for( int i = 0; i < histogramSize; i++ )
	{
		array[i] = array[i] - average;
		if( array[i] < 0 ) array[i] = -array[i];
	}
	
	int left = maximum - 1;
	for( int i = maximum - 1; i >= 0; i-- )
	{
		if( array[i] < array[left] ) left = i;
	}
	
	int right = maximum + 1;
	for( int i = maximum + 1; i < histogramSize; i++ )
	{
		if( array[i] < array[right] ) right = i;
	}
	free( array );
	
	float temp = ( left / scaleFactor ) + minimumIntensity;
	maximumIntensity = ( right / scaleFactor ) + minimumIntensity;
	minimumIntensity = temp;
	
	if( callback != NULL ) callback->SetJavaVolumeIntensity( minimumIntensity, maximumIntensity );
}

void VOLUME::SetIntensity( float minimum, float maximum )
{
	minimumIntensity = minimum;
	maximumIntensity = maximum;
}

float VOLUME::GetMinimumIntensity()
{
	return minimumIntensity;
}

float VOLUME::GetMaximumIntensity()
{
	return maximumIntensity;
}

void VOLUME::SetPointStructureIntensity( float minimum, float maximum )
{
	minimumPointStructureIntensity = minimum;
	maximumPointStructureIntensity = maximum;
	pointStructure = true;
}

void VOLUME::SetPointStructure( bool value )
{
	pointStructure = value;
	
	LoadSlice( currentSlice );
}

void VOLUME::ResetCroppedRegion()
{
	selectedCropCorner = -1;
	
	currentRegion.llx = 0;
	currentRegion.lly = 0;
	currentRegion.llz = 0;
	currentRegion.urx = width;
	currentRegion.ury = height;
	currentRegion.urz = depth;
	
	if( croppedSlice != NULL )
	{
		free( croppedSlice );
		croppedSlice = NULL;
	}
}

void VOLUME::SetCropRegion( int llx, int lly, int llz, int urx, int ury, int urz )
{
	selectedCropCorner = 0;
	
	// -value means ignore
	if( llx >= 0 ) currentRegion.llx = llx;
	if( lly >= 0 ) currentRegion.lly = lly;
	if( llz >= 0 ) currentRegion.llz = llz;
	if( urx >= 0 ) currentRegion.urx = urx;
	if( ury >= 0 ) currentRegion.ury = ury;
	if( urz >= 0 ) currentRegion.urz = urz;
}

void VOLUME::GetCroppedRegion( int region[] )
{
	region[0] = currentRegion.llx;
	region[1] = currentRegion.lly;
	region[2] = currentRegion.llz;
	region[3] = currentRegion.urx;
	region[4] = currentRegion.ury;
	region[5] = currentRegion.urz;
}

int VOLUME::CreateMiniture( char filename[], float scale )
{
	FILE *minitureOutput = fopen( filename, "wb" );
	if( minitureOutput <= 0 ) return -1;
	
	int minitureWidth = (int)( (float)width * scale );
	int minitureHeight = (int)( (float)height * scale );
	int minitureDepth = (int)( (float)depth * scale );
	
	int volumeImageSize = width * height;
	int minitureImageSize = minitureWidth * minitureHeight;
	float *volumeImage = (float*)malloc(sizeof(float)*volumeImageSize);
	unsigned char *minitureImage = (unsigned char*)malloc(sizeof(unsigned char)*minitureImageSize);
	
	float inverseScale = 1.0f / scale;
	float thresholdFactor = 255.0f / ( maximumIntensity - minimumIntensity );
	
	if( callback != NULL )
	{
		callback->SetProgressTitle( "Saving Miniture Volume" );
		callback->ZeroProgress();
	}
	float percentCompletePerRow = 100.0f / (float)depth;
	
	for( int i = 0; i < depth; i++ )
	{
		// Get desired slice
		long fileOffset = (long)( (int)( (float)i * inverseScale ) * volumeImageSize * sizeof( float ) );
		fseek( volumeDataFile, fileOffset, SEEK_SET );
		long numread = fread( volumeImage, sizeof(float), volumeImageSize, volumeDataFile );
		// Error check here if later desired
		
		// Have desired slice
		int offset = 0;
		for( int j = 0; j < minitureHeight; j++ )
		{
			for( int k = 0; k < minitureWidth; k++ )
			{
				float sum = 0.0f;
				int counter = 0;
				for( int jj = (int)( (float)j * inverseScale ); jj < (int)( (float)(j+1) * inverseScale ); jj++ )
				{
					for( int kk = (int)( (float)k * inverseScale ); kk < (int)( (float)(k+1) * inverseScale ); kk++ )
					{
						sum += volumeImage[jj*width + kk];
						counter++;
					}
				}
				
				float value = sum / counter;
				if( value < minimumIntensity ) value = minimumIntensity;
				if( value > maximumIntensity ) value = maximumIntensity;
				value = ( value - minimumIntensity ) * thresholdFactor;
				
				minitureImage[offset++] = (unsigned char)value;
			}
		}
		fwrite( minitureImage, sizeof(unsigned char), minitureImageSize, minitureOutput );
		
		if( callback != NULL ) callback->Add( percentCompletePerRow );
	}
	fclose( minitureOutput );
	if( volumeImage != NULL ) free( volumeImage );
	
	if( callback != NULL ) callback->Complete();
	
	return 0;
}

// Opengl functions
void VOLUME::DrawVolumeSlice( int canvasWidth, int canvasHeight )
{
	glViewport( 0, 0, canvasWidth, canvasHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight );
	// GLKMatrix4MakeOrtho( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	if( slice != NULL )
	{
		glRasterPos2f( 0.0f, 0.0f );
		float scaleFactorX = (float)canvasWidth / (float)openglWidth;
		float scaleFactorY = (float)canvasHeight / (float)height;
		glPixelZoom( scaleFactorX, scaleFactorY );
		glDrawPixels( openglWidth, height, GL_RGB, GL_UNSIGNED_BYTE, slice );
		
		if( selectedCropCorner > -1 )
		{
			float scaleFactorX = (float)canvasWidth / (float)width;
			float scaleFactorY = (float)canvasHeight / (float)height;
			float llfx = scaleFactorX * (float)currentRegion.llx;
			float llfy = scaleFactorY * (float)currentRegion.lly;
			float urfx = scaleFactorX * (float)currentRegion.urx;
			float urfy = scaleFactorY * (float)currentRegion.ury;
			
			glColor3f( 1.0f, 0.0f, 0.0f );
			glBegin( GL_LINE_LOOP );
				glVertex2f( llfx, llfy );
				glVertex2f( urfx, llfy );
				glVertex2f( urfx, urfy );
				glVertex2f( llfx, urfy );
			glEnd();
		}
	}
}

void VOLUME::DrawVolumeCrop( int canvasWidth, int canvasHeight )
{
	glViewport( 0, 0, canvasWidth, canvasHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight );
	// GLKMatrix4MakeOrtho( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	if( croppedSlice != NULL )
	{
		glRasterPos2f( 0.0f, 0.0f );
		float scaleFactorX = (float)canvasWidth / (float)croppedWidth;
		float scaleFactorY = (float)canvasHeight / (float)croppedHeight;
		glPixelZoom( scaleFactorX, scaleFactorY );
		glDrawPixels( croppedWidth, croppedHeight, GL_RGB, GL_UNSIGNED_BYTE, croppedSlice );
	}
	else if( slice != NULL )
	{
		glRasterPos2f( 0.0f, 0.0f );
		float scaleFactorX = (float)canvasWidth / (float)openglWidth;
		float scaleFactorY = (float)canvasHeight / (float)height;
		glPixelZoom( scaleFactorX, scaleFactorY );
		glDrawPixels( openglWidth, height, GL_RGB, GL_UNSIGNED_BYTE, slice );
	}
}

void VOLUME::SetCropPoint( int x, int y, int canvasWidth, int canvasHeight )
{
	x = (int)( (float)x * ( (float)width / (float)canvasWidth ) );
	y = (int)( (float)y * ( (float)height / (float)canvasHeight ) );
	
	// Get Selected Corner
	selectedCropCorner = -1;
	
	int d1x = currentRegion.llx - x;
	int d1y = currentRegion.lly - y;
	int d2x = currentRegion.urx - x;
	int d2y = currentRegion.ury - y;
	
	if( d1x > -6 && d1x < 6 )
	{
		if( d1y > -6 && d1y < 6 ) selectedCropCorner = 0; // lower left
		else if( d2y > -6 && d2y < 6 ) selectedCropCorner = 1; // upper left
	}
	else if( d2x > -6 && d2x < 6 )
	{
		if( d1y > -6 && d1y < 6 ) selectedCropCorner = 2; // lower right
		else if( d2y > -6 && d2y < 6 ) selectedCropCorner = 3; // upper right
	}
	
	switch( selectedCropCorner )
	{
		case 0:
		{
			currentRegion.llx = x;
			currentRegion.lly = y;
		} break;
		
		case 1:
		{
			currentRegion.llx = x;
			currentRegion.ury = y;
		} break;
		
		case 2:
		{
			currentRegion.urx = x;
			currentRegion.lly = y;
		} break;
		
		case 3:
		{
			currentRegion.urx = x;
			currentRegion.ury = y;
		} break;
		
		default:
		{
			currentRegion.llx = x;
			currentRegion.lly = y;
			currentRegion.urx = x;
			currentRegion.ury = y;
			
			selectedCropCorner = 3; // upper right
		}
	}
	
	SetCurrentRegion();
}

void VOLUME::MoveCropPoint( int x, int y, int canvasWidth, int canvasHeight )
{
	x = (int)( (float)x * ( (float)width / (float)canvasWidth ) );
	y = (int)( (float)y * ( (float)height / (float)canvasHeight ) );
	
	switch( selectedCropCorner )
	{
		case 0:
		{
			currentRegion.llx = x;
			currentRegion.lly = y;
		} break;
		
		case 1:
		{
			currentRegion.llx = x;
			currentRegion.ury = y;
		} break;
		
		case 2:
		{
			currentRegion.urx = x;
			currentRegion.lly = y;
		} break;
		
		case 3:
		{
			currentRegion.urx = x;
			currentRegion.ury = y;
		} break;
	}
	
	SetCurrentRegion();
}

void VOLUME::SetCurrentRegion()
{
	int minx = currentRegion.llx;
	int maxx = currentRegion.urx;
	int miny = currentRegion.lly;
	int maxy = currentRegion.ury;
	int minz = currentRegion.llz;
	int maxz = currentRegion.urz;
	
	int temporary;
	if( minx > maxx )
	{
		temporary = minx;
		minx = maxx;
		maxx = temporary;
	}
	
	if( miny > maxy )
	{
		temporary = miny;
		miny = maxy;
		maxy = temporary;
	}
	
	if( minz > maxz )
	{
		temporary = minz;
		minz = maxz;
		maxz = temporary;
	}
	
	if( callback != NULL ) callback->SetJavaCropRegion( minx, maxx, miny, maxy, minz, maxz );
}

void VOLUME::GetPointStructureIntensity( int x, int y, int canvasWidth, int canvasHeight )
{
	if( volumeDataFile == NULL ) return;
	
	x = (int)( (float)x * ( (float)croppedWidth / (float)canvasWidth ) );
	y = (int)( (float)y * ( (float)croppedHeight / (float)canvasHeight ) );
	
	int minx = currentRegion.llx + x;
	int miny = currentRegion.lly + y;
	
	long offset = headerOffset + ( currentSlice * width * height * sizeof(float) );
	offset += ( miny * width + minx ) * sizeof(float);
	fseek( volumeDataFile, offset, SEEK_SET );
	
	float intensity[1];
	fread( intensity, sizeof(float), 1, volumeDataFile );
	
	float scaleFactor = 255.0f / ( maximumIntensity - minimumIntensity );
	int minimum = (int)(( intensity[0] - minimumIntensity ) * scaleFactor);
	
	minimumPointStructureIntensity = (float)minimum / scaleFactor + minimumIntensity;
	maximumPointStructureIntensity = minimumPointStructureIntensity + scaleFactor * 0.99;
	pointStructure = true;
	
	LoadSlice( currentSlice );
	
	if( callback != NULL ) callback->SetJavaPointStructureIntensity( minimumPointStructureIntensity, maximumPointStructureIntensity );
}
