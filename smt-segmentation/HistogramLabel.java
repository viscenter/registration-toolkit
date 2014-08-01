
import java.awt.*;
import java.awt.geom.*;
import java.util.*;
import javax.swing.*;



public class HistogramLabel extends JLabel
{
	private boolean pointStructure;
	private EduceProject educeHandle;
	private float minimum, maximum;
	private float minimumIntensity, maximumIntensity;
	private float minimumPointStructureIntensity, maximumPointStructureIntensity;
	private int intensityData[], maximumIntensityCount;
	
	public HistogramLabel( EduceProject handle )
	{
		educeHandle = handle;
		setText( " " );
		
		pointStructure = false;
		intensityData = new int[256];
		maximumIntensityCount = 0;
		
		
		minimum = 0.0f;
		maximum = 255.0f;
	}
	
	
	public void UpdatePanel( float minimumValue, float maximumValue, int data[] )
	{
		minimum = minimumValue;
		maximum = maximumValue;
		
		maximumIntensityCount = 0;
		for( int i = 0; i < 256; i++ )
		{
			intensityData[i] = data[i];
			if( intensityData[i] > maximumIntensityCount ) maximumIntensityCount = intensityData[i];
		}
		
		repaint();
	}
	
	public void SetIntensity( float minimumValue, float maximumValue )
	{
		minimumIntensity = minimumValue;
		maximumIntensity = maximumValue;
		
		repaint();
	}
	
	public void SetPointStructure( boolean value )
	{
		pointStructure = value;
	}
	
	public void SetPointStructureIntensity( float minimumValue, float maximumValue )
	{
		minimumPointStructureIntensity = minimumValue;
		maximumPointStructureIntensity = maximumValue;
		pointStructure = true;
		
		repaint();
	}
	
	public void paint( Graphics graphics )
	{
		Graphics2D temporaryGraphics = (Graphics2D)graphics;
		
		float componentHeight = (float)getHeight() - 1.0f;
		float componentWidth = (float)getWidth() - 1.0f;
		float scaleFactorHeight = componentHeight / maximumIntensityCount;
		float scaleFactorWidth = componentWidth / 256.0f;
		
		GeneralPath path = new GeneralPath();
		path.moveTo( 0.0f, componentHeight );
		path.lineTo( componentWidth, componentHeight );
		path.lineTo( componentWidth, 0.0f );
		path.lineTo( 0.0f, 0.0f );
		path.lineTo( 0.0f, componentHeight );
		
		for( int i = 0; i < 256; i++ )
		{
			path.lineTo( scaleFactorWidth * (float)i, componentHeight - ( scaleFactorHeight * (float)intensityData[i] ) );
		}
		temporaryGraphics.draw( path );
		
		float scaleFactor = ( maximum - minimum ) / componentWidth;
		float minimumLocation = minimumIntensity * scaleFactor;
		float maximumLocation = maximumIntensity * scaleFactor;
		if( minimumLocation < 0.0f ) minimumLocation = 0.0f;
		if( minimumLocation > componentWidth - 1.0f ) minimumLocation = componentWidth;
		if( maximumLocation < 0.0f ) maximumLocation = 0.0f;
		if( maximumLocation > componentWidth - 1.0f ) maximumLocation = componentWidth;
		
		temporaryGraphics.setColor( Color.RED );
		path = new GeneralPath();
		path.moveTo( minimumLocation, 1.0f );
		path.lineTo( minimumLocation, componentHeight - 1.0f );
		path.moveTo( maximumLocation, 1.0f );
		path.lineTo( maximumLocation, componentHeight - 1.0f );
		temporaryGraphics.draw( path );
		
		if( pointStructure && minimumPointStructureIntensity != maximumPointStructureIntensity )
		{
			minimumLocation = minimumPointStructureIntensity * scaleFactor;
			maximumLocation = maximumPointStructureIntensity * scaleFactor;
			if( minimumLocation < 0.0f ) minimumLocation = 0.0f;
			if( minimumLocation > componentWidth - 1.0f ) minimumLocation = componentWidth;
			if( maximumLocation < 0.0f ) maximumLocation = 0.0f;
			if( maximumLocation > componentWidth - 1.0f ) maximumLocation = componentWidth;
			
			temporaryGraphics.setColor( Color.BLUE );
			path = new GeneralPath();
			path.moveTo( minimumLocation, 1.0f );
			path.lineTo( minimumLocation, componentHeight - 1.0f );
			path.moveTo( maximumLocation, 1.0f );
			path.lineTo( maximumLocation, componentHeight - 1.0f );
			temporaryGraphics.draw( path );
		}
		
		super.paint( graphics );
	}
};
