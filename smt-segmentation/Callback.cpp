
#include "Callback.h"

Callback::Callback()
{
	strcpy( progressTitle, "" );
	strcpy( totalTitle, "" );
	
	progressValue = 0.0f;
	totalValue = 0.0f;
}

Callback::~Callback(){}


void Callback::Redraw()
{
	RedrawJavaSide();
}

void Callback::SetJavaVolumeIntensity( float minimum, float maximum )
{
	JavaSetVolumeIntensity( minimum, maximum );
}

void Callback::SetJavaCropRegion( int minx, int maxx, int miny, int maxy, int minz, int maxz )
{
	JavaSetCropRegion( minx, maxx, miny, maxy, minz, maxz );
}

void Callback::SetJavaPointStructureIntensity( float minimum, float maximum )
{
	JavaSetPointStructureIntensity( minimum, maximum );
}

void Callback::SetJavaIntensityLabel( float minimum, float maximum, int data[] )
{
	JavaSetIntensityLabel( minimum, maximum, data );
}

void Callback::ZeroProgress()
{
	progressValue = 0.0f;
	Redraw();
}

void Callback::ZeroTotalProgress()
{
	progressValue = 0.0f;
	Redraw();
}

void Callback::SetProgressTitle( const char title[] )
{
	int i = 0;
	while( title[i] != '\0' && i < 127 )
	{
		progressTitle[i] = title[i];
		i++;
	}
	progressTitle[i] = '\0';
}

void Callback::SetTotalProgressTitle( const char title[] )
{
	int i = 0;
	while( title[i] != '\0' && i < 127 )
	{
		totalTitle[i] = title[i];
		i++;
	}
	totalTitle[i] = '\0';
}

void Callback::Add( float value )
{
	progressValue += value;
	Redraw();
}

void Callback::AddToTotal( float value )
{
	totalValue += value;
	Redraw();
}

void Callback::Complete()
{
	progressValue = 100.0f;
}

void Callback::CompleteTotal()
{
	totalValue = 100.0f;
}

void Callback::OutputString( char *string, GLfloat x, GLfloat y )
{
	glRasterPos2f( x, y );
	char *character = string;
	while( *character != 0 )
	{
		glutBitmapCharacter( GLUT_BITMAP_8_BY_13, *character );
		character++;
	}
}

void Callback::DrawProgressBars( int canvasWidth, int canvasHeight )
{
	glViewport( 0, 0, canvasWidth, canvasHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight );
	// GLKMatrix4MakeOrtho( 0.0, (float)canvasWidth, 0.0, (float)canvasHeight, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	float minimum = canvasWidth * 0.25f;
	float middle = canvasWidth * 0.5f;
	float maximum = canvasWidth * 0.75f;
	
	float barOneBottom = canvasHeight * 0.40f;
	float barOneMiddle = canvasHeight * 0.425f;
	float barOneTop = canvasHeight * 0.45f;
	float barOneText = canvasHeight * 0.46f;
	float barTwoBottom = canvasHeight * 0.5f;
	float barTwoMiddle = canvasHeight * 0.525f;
	float barTwoTop = canvasHeight * 0.55f;
	float barTwoText = canvasHeight * 0.56f;
	
	float barOneX = ( ( maximum - minimum ) * ( progressValue / 100.0f ) ) + minimum;
	float barTwoX = ( ( maximum - minimum ) * ( totalValue / 100.0f ) ) + minimum;
	
	glColor3f( 0.0f, 0.0f, 1.0f );
	glBegin( GL_LINE_LOOP );
		glVertex2f( minimum, barOneBottom );
		glVertex2f( minimum, barOneTop );
		glVertex2f( maximum, barOneTop );
		glVertex2f( maximum, barOneBottom );
	glEnd();
	
	glBegin( GL_LINE_LOOP );
		glVertex2f( minimum, barTwoBottom );
		glVertex2f( minimum, barTwoTop );
		glVertex2f( maximum, barTwoTop );
		glVertex2f( maximum, barTwoBottom );
	glEnd();
	
	glBegin( GL_POLYGON );
		glVertex2f( minimum, barOneBottom );
		glVertex2f( minimum, barOneTop );
		glVertex2f( barOneX, barOneTop );
		glVertex2f( barOneX, barOneBottom );
	glEnd();
	
	glBegin( GL_POLYGON );
		glVertex2f( minimum, barTwoBottom );
		glVertex2f( minimum, barTwoTop );
		glVertex2f( barTwoX, barTwoTop );
		glVertex2f( barTwoX, barTwoBottom );
	glEnd();
	
	char output[10];
	glColor3f( 1.0f, 0.0f, 0.0f );
	OutputString( totalTitle, minimum, barTwoText );
	sprintf( output, "%d%%", (int)progressValue );
	OutputString( output, middle, barOneMiddle );
	
	OutputString( progressTitle, minimum, barOneText );
	sprintf( output, "%d%%", (int)totalValue );
	OutputString( output, middle, barTwoMiddle );
}
