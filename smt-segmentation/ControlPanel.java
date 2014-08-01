
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.text.*;
import java.lang.Number;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeEvent;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;



public class ControlPanel extends JPanel implements ActionListener, PropertyChangeListener, ChangeListener
{
	public static int CONTROL_PANEL_DISPLAY_NOTHING = 0;
	public static int CONTROL_PANEL_NEW_PROJECT     = 1;
	public static int CONTROL_PANEL_PROJECT         = 2;
	
	private boolean performNoChanges;
	private JavaCInterface javaCInterface;
	private Dimension controlPanelSize;
	private EduceProject educeHandle;
	private int displayPanel, currentSlice;
	private JPanel newProjectPanel, projectPanel, minitureCanvasPanel;
	private JSlider currentSliceSlider;
	
	
	// New Project Panel - buttons, labels, etc
	private boolean intensityPointStructure;
	private File volumeFile;
	private HistogramLabel intensityLabel, pointStructurePanel;
	private int volumeWidth, volumeHeight, volumeDepth, cropStart, cropEnd, cropX, cropY, cropWidth, cropHeight, cropDepth;
	private JButton openVolumeFile, changeSaveLocation, autoIntensityButton, pointStructureButton;
	private JButton startSliceButton, endSliceButton, resetCropButton, createProjectButton;
	private JFileChooser volumeFileChooser, saveLocationChooser;
	private JFormattedTextField lowerLeftFieldX, lowerLeftFieldY, lowerLeftFieldZ, upperRightFieldX, upperRightFieldY;
	private JFormattedTextField upperRightFieldZ, voxelSizeFieldX, voxelSizeFieldY, voxelSizeFieldZ;
	private JFormattedTextField minimumIntensity, maximumIntensity, minimumIntensityPointStructure, maximumIntensityPointStructure;
	private JLabel volumeFilenameLabel, widthLabel, heightLabel, depthLabel, sliceLabel, cropLabel;
	private JPanel volumePane, cropPane;
	private JTabbedPane tabbedPanel;
	private JTextField projectNameField, volumeFilenameField, meshFilenameField, textureFilenameField;
	private JTextField minitureVolumeFilenameField, miniturePointStructureFilenameField;
	private String currentVolumeFile, saveDirectory;
	
	
	public ControlPanel( JPanel minitureCanvas, JavaCInterface cInterface, EduceProject handle )
	{
		setLayout( new BorderLayout() );
		javaCInterface = cInterface;
		
		volumeWidth = -1;
		volumeHeight = -1;
		volumeDepth = -1;
		saveDirectory = System.getProperty( "user.dir" );
		
		minitureCanvasPanel = minitureCanvas;
		add( minitureCanvasPanel, BorderLayout.NORTH );
		
		educeHandle = handle;
		LoadPanel( CONTROL_PANEL_DISPLAY_NOTHING );
	}
	
	public int GetCurrentSlice()
	{
		return currentSlice;
	}
	
	public void LoadPanel( int panelNumber )
	{
		displayPanel = panelNumber;
		
		// Remove control panels
		try{ remove( newProjectPanel ); }
		catch( NullPointerException exception ){}
		try{ remove( projectPanel ); }
		catch( NullPointerException exception ){}
		
		if( displayPanel == CONTROL_PANEL_NEW_PROJECT )
		{
			CreateNewProjectPanel();
			add( newProjectPanel );
		}
		else if( displayPanel == CONTROL_PANEL_PROJECT )
		{
			LoadProjectPanel();
			add( projectPanel );
		}
		else
		{
			displayPanel = CONTROL_PANEL_DISPLAY_NOTHING;
		}
		
		validate();
		repaint();
	}
	
	public void SetVolumeIntensityFields( float minimum, float maximum )
	{
		performNoChanges = true;
		minimumIntensity.setValue( new Float( minimum ) );
		maximumIntensity.setValue( new Float( maximum ) );
		performNoChanges = false;
	}
	
	public void SetCropRegionFields( int minx, int maxx, int miny, int maxy, int minz, int maxz )
	{
		cropStart = minz;
		cropEnd = maxz;
		cropX = minx;
		cropY = miny;
		cropWidth = maxx - minx;
		cropHeight = maxy - miny;
		cropDepth = maxz - minz;
		cropLabel.setText( " Crop (W,H,D):  " + cropWidth + " , " + cropHeight + " , " + cropDepth );
	}
	
	public void SetJavaPointStructureIntensityFields( float minimum, float maximum )
	{
		performNoChanges = true;
		minimumIntensityPointStructure.setValue( new Float( minimum ) );
		maximumIntensityPointStructure.setValue( new Float( maximum ) );
		intensityPointStructure = true;
		pointStructureButton.setText( "Turn Off" );
		performNoChanges = false;
	}
	
	public void SetIntensityLabel( float minimum, float maximum, int data[] )
	{
		if( intensityLabel != null ) intensityLabel.UpdatePanel( minimum, maximum, data );
	}
	
	public void actionPerformed( ActionEvent event )
	{
		Object source = event.getSource();
		
		if( displayPanel == CONTROL_PANEL_NEW_PROJECT ) NewProjectAction( source );
		else if( displayPanel == CONTROL_PANEL_PROJECT ) ProjectAction( source );
	}
	
	private void NewProjectAction( Object source )
	{
		if( source == openVolumeFile )
		{
			int returnValue = volumeFileChooser.showOpenDialog(educeHandle);
			if( returnValue == JFileChooser.APPROVE_OPTION )
			{
				boolean validFile = true;
				File file = volumeFileChooser.getSelectedFile();
				String filename = file.getAbsolutePath();
				if( !file.isFile() ) validFile = false;
				String currentExtension = SingleFileFilter.getExtension( file );
				if( currentExtension == null || !currentExtension.toLowerCase().equals( ".vol" ) ) validFile = false;
				
				if( validFile )
				{
					currentVolumeFile = file.getName();
					javaCInterface.LoadVolume( filename );
					int volumeDimensions[] = javaCInterface.GetVolumeDimensions();
					volumeWidth = volumeDimensions[0];
					volumeHeight = volumeDimensions[1];
					volumeDepth = volumeDimensions[2];
					currentSlice = 0;
					cropStart = 0;
					cropEnd = volumeDepth - 1;
					cropX = 0;
					cropY = 0;
					cropWidth = volumeWidth;
					cropHeight = volumeHeight;
					cropDepth = volumeDepth;
					
					volumeFilenameLabel.setText( file.getName() );
					widthLabel.setText( "Width: " + volumeWidth );
					heightLabel.setText( "Height: " + volumeHeight );
					depthLabel.setText( "Depth: " + volumeDepth );
					startSliceButton.setText( "Start: " + ( cropStart + 1 ) );
					endSliceButton.setText( "End: " + ( cropEnd + 1 ) );
					
					sliceLabel.setText( "Slice: " + ( currentSlice + 1 ) );
					cropLabel.setText( " Crop (W,H,D):  " + cropWidth + " , " + cropHeight + " , " + cropDepth );
					
					performNoChanges = true;
					currentSliceSlider.setMinimum( 1 );
					currentSliceSlider.setMaximum( volumeDepth );
					currentSliceSlider.setValue( 1 );
					currentSliceSlider.setMajorTickSpacing( 100 );
					currentSliceSlider.setMinorTickSpacing( 25 );
					currentSliceSlider.setPaintTicks( true );
					
					upperRightFieldX.setValue( new Float( (float)volumeWidth ) );
					upperRightFieldY.setValue( new Float( (float)volumeHeight ) );
					performNoChanges = false;
					upperRightFieldZ.setValue( new Float( (float)volumeDepth ) );
					
					CanvasWindow temporary = educeHandle.GetMinitureWindow();
					temporary.Draw( temporary.CANVAS_WINDOW_DRAW_VOLUME_CROP );
					temporary = educeHandle.GetMainWindow();
					temporary.Draw( temporary.CANVAS_WINDOW_DRAW_VOLUME_SLICE );
					
					educeHandle.RedrawCanvases();
					
					tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( volumePane ), true );
					tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( cropPane ), true );
					tabbedPanel.setSelectedComponent( cropPane );
				}
			}
		}
		else if( source == changeSaveLocation )
		{
			int returnValue = saveLocationChooser.showSaveDialog(educeHandle);
			if( returnValue == JFileChooser.APPROVE_OPTION )
			{
				try
				{
					File file = saveLocationChooser.getSelectedFile();
					String absolutePath = file.getAbsolutePath();
					String filename = file.getName();
					int index = absolutePath.lastIndexOf( filename );
					saveDirectory = absolutePath.substring( 0, index );
				}
				catch( NullPointerException exception ){}
				catch( IndexOutOfBoundsException exception ){}
			}
		}
		else if( source == autoIntensityButton )
		{
			javaCInterface.AutoIntensity( currentSlice );
			javaCInterface.LoadVolumeSlice( currentSlice );
			
			float minimum = ((Number)minimumIntensity.getValue()).floatValue();
			float maximum = ((Number)maximumIntensity.getValue()).floatValue();
			if( intensityLabel != null ) intensityLabel.SetIntensity( minimum, maximum );
			
			educeHandle.RedrawCanvases();
		}
		else if( source == pointStructureButton )
		{
			performNoChanges = true;
			intensityPointStructure = !intensityPointStructure;
			if( intensityPointStructure )
			{
				pointStructureButton.setText( "Turn Off" );
				javaCInterface.SetPointStructure( 1 );
			}
			else
			{
				pointStructureButton.setText( "Turn On" );
				javaCInterface.SetPointStructure( 0 );
			}
			if( intensityLabel != null ) intensityLabel.SetPointStructure( false );
			performNoChanges = false;
			
			educeHandle.RedrawCanvases();
		}
		else if( source == startSliceButton )
		{
			cropStart = currentSlice;
			
			if( cropEnd < cropStart ) cropEnd = cropStart + 1;
			
			if( cropStart == volumeDepth - 1 )
			{
				cropStart--;
				cropEnd = volumeDepth - 1;
			}
			
			startSliceButton.setText( "Start: " + ( cropStart + 1 ) );
			endSliceButton.setText( "End: " + ( cropEnd + 1 ) );
			
			cropDepth = cropEnd - cropStart;
			
			cropLabel.setText( " Crop (W,H,D):  " + cropWidth + " , " + cropHeight + " , " + cropDepth );
		}
		else if( source == endSliceButton )
		{
			cropEnd = currentSlice;
			
			if( cropStart > cropEnd ) cropStart = cropEnd - 1;
			
			if( cropEnd == 0 )
			{
				cropStart = 0;
				cropEnd = 1;
			}
			
			startSliceButton.setText( "Start: " + ( cropStart + 1 ) );
			endSliceButton.setText( "End: " + ( cropEnd + 1 ) );
			
			cropDepth = cropEnd - cropStart;
			
			cropLabel.setText( " Crop (W,H,D):  " + cropWidth + " , " + cropHeight + " , " + cropDepth );
		}
		else if( source == resetCropButton )
		{
			cropStart = 0;
			cropEnd = volumeDepth - 1;
			cropX = 0;
			cropY = 0;
			cropWidth = volumeWidth;
			cropHeight = volumeHeight;
			cropDepth = volumeDepth;
			
			startSliceButton.setText( "Start: " + ( cropStart + 1 ) );
			endSliceButton.setText( "End: " + ( cropEnd + 1 ) );
			cropLabel.setText( " Crop (W,H,D):  " + cropWidth + " , " + cropHeight + " , " + cropDepth );
			
			javaCInterface.ResetVolumeCropRegion();
			javaCInterface.LoadVolumeSlice( currentSlice );
			educeHandle.RedrawCanvases();
		}
		else if( source == createProjectButton )
		{
			CanvasWindow temporary = educeHandle.GetMinitureWindow();
			temporary.Draw( temporary.CANVAS_WINDOW_DRAW_VOLUME_SLICE );
			
			temporary = educeHandle.GetMainWindow();
			temporary.Draw( temporary.CANVAS_WINDOW_DRAW_PROGRESS_BAR );
			
			tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( volumePane ), false );
			tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( cropPane ), false );
			
			CreateProject();
		}
	}
	
	
	private void ProjectAction( Object source )
	{
		//if( source == openVolumeFile )
		{
		}
	}
	
	public void propertyChange( PropertyChangeEvent event )
	{
		if( performNoChanges ) return;
		
		if( "value".equals( event.getPropertyName() ) )
		{
			Object source = event.getSource();
			
			if( source == lowerLeftFieldX || source == lowerLeftFieldY || source == lowerLeftFieldZ || 
				source == upperRightFieldX || source == upperRightFieldY || source == upperRightFieldZ )
			{
				float llx, lly, llz, urx, ury, urz, vsx, vsy, vsz;
				
				llx = ((Number)lowerLeftFieldX.getValue()).floatValue();
				lly = ((Number)lowerLeftFieldY.getValue()).floatValue();
				llz = ((Number)lowerLeftFieldZ.getValue()).floatValue();
				urx = ((Number)upperRightFieldX.getValue()).floatValue();
				ury = ((Number)upperRightFieldY.getValue()).floatValue();
				urz = ((Number)upperRightFieldZ.getValue()).floatValue();
				
				vsx = ( urx - llx ) / (float)volumeWidth;
				vsy = ( ury - lly ) / (float)volumeHeight;
				vsz = ( urz - llz ) / (float)volumeDepth;
				
				voxelSizeFieldX.setValue( new Float( vsx ) );
				voxelSizeFieldY.setValue( new Float( vsy ) );
				voxelSizeFieldZ.setValue( new Float( vsz ) );
			}
			else if( source == voxelSizeFieldX || source == voxelSizeFieldY || source == voxelSizeFieldZ )
			{
				float llx, lly, llz, urx, ury, urz, vsx, vsy, vsz;
				
				llx = ((Number)lowerLeftFieldX.getValue()).floatValue();
				lly = ((Number)lowerLeftFieldY.getValue()).floatValue();
				llz = ((Number)lowerLeftFieldZ.getValue()).floatValue();
				vsx = ((Number)voxelSizeFieldX.getValue()).floatValue();
				vsy = ((Number)voxelSizeFieldY.getValue()).floatValue();
				vsz = ((Number)voxelSizeFieldZ.getValue()).floatValue();
				
				urx = ( vsx * (float)volumeWidth ) + llx;
				ury = ( vsy * (float)volumeHeight ) + lly;
				urz = ( vsz * (float)volumeDepth ) + llz;
				
				upperRightFieldX.setValue( new Float( urx ) );
				upperRightFieldY.setValue( new Float( ury ) );
				upperRightFieldZ.setValue( new Float( urz ) );
			}
			else if( source == minimumIntensity || source ==  maximumIntensity )
			{
				float minimum, maximum;
				
				minimum = ((Number)minimumIntensity.getValue()).floatValue();
				maximum = ((Number)maximumIntensity.getValue()).floatValue();
				
				javaCInterface.SetVolumeIntensity( minimum, maximum );
				if( intensityLabel != null ) intensityLabel.SetIntensity( minimum, maximum );
				
				javaCInterface.LoadVolumeSlice( currentSlice );
				educeHandle.RedrawCanvases();
			}
			else if( source == minimumIntensityPointStructure || source == maximumIntensityPointStructure )
			{
				float minimum, maximum;
				
				minimum = ((Number)minimumIntensityPointStructure.getValue()).floatValue();
				maximum = ((Number)maximumIntensityPointStructure.getValue()).floatValue();
				
				javaCInterface.SetPointStructureIntensity( minimum, maximum );
				if( intensityLabel != null ) intensityLabel.SetPointStructureIntensity( minimum, maximum );
				
				javaCInterface.LoadVolumeSlice( currentSlice );
				educeHandle.RedrawCanvases();
			}
		}
	}
	
	public void stateChanged( ChangeEvent event )
	{
		if( performNoChanges ) return;
		
		Object source = event.getSource();
		
		if( source == currentSliceSlider )
		{
			currentSlice = currentSliceSlider.getValue() - 1;
			sliceLabel.setText( "Slice: " + ( currentSlice + 1 ) );
			educeHandle.DrawCanvases( false );
			javaCInterface.LoadVolumeSlice( currentSlice );
//***************************************************
			
			educeHandle.DrawCanvases( true );
			educeHandle.RedrawCanvases();
		}
	}
	
	private void CreateProject()
	{
		// Create xml project directory
		
		String projectFilename = projectNameField.getText();
		String projectDirectory = saveDirectory + File.separator + projectFilename;
		
		File temporaryFile = new File( projectFilename );
		
		if( !temporaryFile.exists() ) temporaryFile.mkdir();
		
		projectDirectory = saveDirectory + File.separator + projectFilename + File.separator;
		
		File projectFile = new File( projectDirectory + projectNameField.getText() + ".xml" );
		File croppedVolumeFile = new File( projectDirectory + volumeFilenameField.getText() + ".vfl" );
		File meshFile = new File( projectDirectory + meshFilenameField.getText() + ".surf" );
		File textureFile = new File( projectDirectory + textureFilenameField.getText() + ".pgm" );
		File minitureVolumeFile = new File( projectDirectory + minitureVolumeFilenameField.getText() + ".vuc" );
		File miniturePSFile = new File( projectDirectory + miniturePointStructureFilenameField.getText() + ".vps" );
		
		if( projectFile.exists() || croppedVolumeFile.exists() || meshFile.exists() ||
			textureFile.exists() || minitureVolumeFile.exists() || miniturePSFile.exists() )
		{
			// Overwrite project warning
//****************************************************************************************************************
		}
		
		try
		{
			// First Step - write project information xml file
			FileWriter writer = new FileWriter( projectFile );
			
			writer.write( "<?xml version=\"1.0\" standalone='yes'?>\n<!-- EDUCE PROJECT -->\n" );
			writer.write( "<!DOCTYPE project [\n" );
			writer.write( "\t<!ELEMENT project (name, original, volume, miniture, meshfile, texturefile)>\n" );
			writer.write( "\t<!ELEMENT original (file, dimensions, boundingbox, point)>\n");
			writer.write( "\t<!ELEMENT volume (file, dimensions, intensity)>\n" );
			writer.write( "\t<!ELEMENT miniture (file, dimensions, meshfile)>\n" );
			writer.write( "\t<!ELEMENT dimensions (width, height, depth)>\n" );
			writer.write( "\t<!ELEMENT intensity (minimum, maximum)>\n" );
			writer.write( "\t<!ELEMENT boundingbox (point, dimensions)>\n" );
			writer.write( "\t<!ELEMENT point (x, y, z)>\n" );
			writer.write( "\t<!ELEMENT name (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT file (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT width (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT height (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT depth (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT x (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT y (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT z (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT minimum (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT maximum (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT meshfile (#PCDATA)>\n" );
			writer.write( "\t<!ELEMENT texturefile (#PCDATA)>\n]>\n" );
			
			// XML Body
			writer.write( "<project>\n" );
			writer.write( "\t<name>" + projectNameField.getText() + "</name>\n" );
			writer.write( "\t<original>\n" );
			writer.write( "\t\t<file>" + currentVolumeFile + "</file>\n" );
			writer.write( "\t\t<dimensions>\n" );
			writer.write( "\t\t\t<width>" + volumeWidth + "</width>\n" );
			writer.write( "\t\t\t<height>" + volumeHeight + "</height>\n" );
			writer.write( "\t\t\t<depth>" + volumeDepth + "</depth>\n" );
			writer.write( "\t\t</dimensions>\n" );
			writer.write( "\t\t<boundingbox>\n" );
			writer.write( "\t\t\t<point>\n" );
			
			float llx = ((Number)lowerLeftFieldX.getValue()).floatValue();
			float lly = ((Number)lowerLeftFieldY.getValue()).floatValue();
			float llz = ((Number)lowerLeftFieldZ.getValue()).floatValue();
			float bbw = ((Number)upperRightFieldX.getValue()).floatValue() - llx;
			float bbh = ((Number)upperRightFieldY.getValue()).floatValue() - lly;
			float bbd = ((Number)upperRightFieldZ.getValue()).floatValue() - llz;
			
			writer.write( "\t\t\t\t<x>" + llx + "</x>\n" );
			writer.write( "\t\t\t\t<y>" + lly + "</y>\n" );
			writer.write( "\t\t\t\t<z>" + llz + "</z>\n" );
			writer.write( "\t\t\t</point>\n" );
			writer.write( "\t\t\t<dimensions>\n" );
			writer.write( "\t\t\t\t<width>" + bbw + "</width>\n" );
			writer.write( "\t\t\t\t<height>" + bbh + "</height>\n" );
			writer.write( "\t\t\t\t<depth>" + bbd + "</depth>\n" );
			writer.write( "\t\t\t</dimensions>\n" );
			writer.write( "\t\t</boundingbox>\n" );
			writer.write( "\t\t<point>\n" );
			writer.write( "\t\t\t<x>" + cropStart + "</x>\n" );
			writer.write( "\t\t\t<y>" + cropX + "</y>\n" );
			writer.write( "\t\t\t<z>" + cropY + "</z>\n" );
			writer.write( "\t\t</point>\n" );
			writer.write( "\t</original>\n" );
			writer.write( "\t<volume>\n" );
			writer.write( "\t\t<file>" + volumeFilenameField.getText() + ".vfl</file>\n" );
			writer.write( "\t\t<dimensions>\n" );
			writer.write( "\t\t\t<width>" + cropWidth + "</width>\n" );
			writer.write( "\t\t\t<height>" + cropHeight + "</height>\n" );
			writer.write( "\t\t\t<depth>" + cropDepth + "</depth>\n" );
			writer.write( "\t\t</dimensions>\n" );
			writer.write( "\t\t<intensity>\n" );
			writer.write( "\t\t\t<minimum>" + ((Number)minimumIntensity.getValue()).floatValue() + "</minimum>\n" );
			writer.write( "\t\t\t<maximum>" + ((Number)maximumIntensity.getValue()).floatValue() + "</maximum>\n" );
			writer.write( "\t\t</intensity>\n" );
			writer.write( "\t</volume>\n" );
			writer.write( "\t<miniture>\n" );
			writer.write( "\t\t<file>" + minitureVolumeFilenameField.getText() + ".vuc</file>\n" );
			writer.write( "\t\t<dimensions>\n" );
			
//********************************************************************************************** Can Improve
			float cropArea = (float)cropWidth * (float)cropHeight * (float)cropDepth;
			float scale = 1.0f;
			if( cropWidth > cropHeight ){ if( cropWidth > 512 ) scale = 512.0f / (float)cropWidth; }
			else{ if( cropHeight > 512 ) scale = 512.0f / (float)cropHeight; }
			while( cropArea * scale * scale * scale > 10000000.0f ) scale -= 0.01f;
//********************************************************************************************** Can Improve
			
			int minitureWidth = (int)( (float)cropWidth * scale );
			int minitureHeight = (int)( (float)cropHeight * scale );
			int minitureDepth = (int)( (float)cropDepth * scale );
			
			writer.write( "\t\t\t<width>" + minitureWidth + "</width>\n" );
			writer.write( "\t\t\t<height>" + minitureHeight + "</height>\n" );
			writer.write( "\t\t\t<depth>" + minitureDepth + "</depth>\n" );
			writer.write( "\t\t</dimensions>\n" );
			writer.write( "\t\t<meshfile>" + miniturePointStructureFilenameField.getText() + ".sps</meshfile>\n" );
			writer.write( "\t</miniture>\n" );
			writer.write( "\t<meshfile>" + meshFilenameField.getText() + ".surf</meshfile>\n" );
			writer.write( "\t<texturefile>" + textureFilenameField.getText() + ".pgm</texturefile>\n" );
			writer.write( "</project>\n" );
			writer.close();
			
			// Secnd Step - Save actual data
			javaCInterface.SaveNewProject( projectDirectory + volumeFilenameField.getText() + ".vfl",
						cropStart, cropEnd,
						projectDirectory + minitureVolumeFilenameField.getText() + ".vuc", scale,
						projectDirectory + miniturePointStructureFilenameField.getText() + ".sps",
						projectDirectory + meshFilenameField.getText() + ".surf",
						projectDirectory + textureFilenameField.getText() + ".pgm" );
		}
		catch( IOException exception ){}
		
		educeHandle.LoadCreatedProject();
	}
	
	private void CreateNewProjectPanel()
	{
		volumeFileChooser = new JFileChooser(); 
		volumeFileChooser.setCurrentDirectory( new File( System.getProperty( "user.dir" ) + "/Volumes" ) );
		volumeFileChooser.setDialogTitle( "Open Volume File" );
		SingleFileFilter filter = new SingleFileFilter( ".vol", "Volumetric Data (.vol)" );
		volumeFileChooser.addChoosableFileFilter(filter);
		
		saveLocationChooser = new JFileChooser(); 
		saveLocationChooser.setCurrentDirectory( new File( saveDirectory ) );
		saveLocationChooser.setDialogTitle( "Change Save Location" );
		
		NumberFormat sevenPlaces = NumberFormat.getNumberInstance();
		sevenPlaces.setMinimumFractionDigits(7);
		sevenPlaces.setMaximumFractionDigits(7);
		
		newProjectPanel = new JPanel( new BorderLayout() );
		
		tabbedPanel = new JTabbedPane();
		
		//  Project Pane
		JLabel firstPaneLabel_1 = new JLabel( " Select Volume:" );
		JLabel firstPaneLabel_2 = new JLabel( " Project Name:" );
		JLabel firstPaneLabel_3 = new JLabel( " Save Location:" );
		JLabel firstPaneLabel_4 = new JLabel( " Volume Filename:" );
		JLabel firstPaneLabel_5 = new JLabel( " Mesh Filename:" );
		JLabel firstPaneLabel_6 = new JLabel( " Texture Filename:" );
		JLabel firstPaneLabel_7 = new JLabel( " Miniture" );
		JLabel firstPaneLabel_8 = new JLabel( "Volume Filename:" );
		JLabel firstPaneLabel_9 = new JLabel( "Point Structure:" );
		
		firstPaneLabel_1.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_2.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_3.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_4.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_5.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_6.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_7.setHorizontalAlignment( JLabel.LEFT );
		firstPaneLabel_8.setHorizontalAlignment( JLabel.CENTER );
		firstPaneLabel_9.setHorizontalAlignment( JLabel.CENTER );
		
		projectNameField = new JTextField( "project" );
		volumeFilenameField = new JTextField( "volume" );
		meshFilenameField = new JTextField( "surface" );
		textureFilenameField = new JTextField( "surface" );
		minitureVolumeFilenameField = new JTextField( "miniture" );
		miniturePointStructureFilenameField = new JTextField( "miniture" );
		
		projectNameField.setHorizontalAlignment( JLabel.LEFT );
		volumeFilenameField.setHorizontalAlignment( JLabel.LEFT );
		meshFilenameField.setHorizontalAlignment( JLabel.LEFT );
		textureFilenameField.setHorizontalAlignment( JLabel.LEFT );
		minitureVolumeFilenameField.setHorizontalAlignment( JLabel.LEFT );
		miniturePointStructureFilenameField.setHorizontalAlignment( JLabel.LEFT );
		
		openVolumeFile = new JButton( "open" );
		openVolumeFile.addActionListener(this);
		changeSaveLocation = new JButton( "Change" );
		changeSaveLocation.addActionListener(this);
		createProjectButton = new JButton( "Proceed" );
		createProjectButton.addActionListener(this);
		
		JPanel firstOuterPane = new JPanel( new BorderLayout() );
		JPanel firstPane = new JPanel( new GridLayout( 12, 2 ) );
		
		firstPane.add( firstPaneLabel_1 );
		firstPane.add( openVolumeFile );
		firstPane.add( new JLabel( "" ) );
		firstPane.add( new JLabel( "" ) );
		firstPane.add( firstPaneLabel_2 );
		firstPane.add( projectNameField );
		firstPane.add( firstPaneLabel_3 );
		firstPane.add( changeSaveLocation );
		firstPane.add( firstPaneLabel_4 );
		firstPane.add( volumeFilenameField );
		firstPane.add( firstPaneLabel_5 );
		firstPane.add( meshFilenameField );
		firstPane.add( firstPaneLabel_6 );
		firstPane.add( textureFilenameField );
		firstPane.add( firstPaneLabel_7 );
		firstPane.add( new JLabel( "" ) );
		firstPane.add( firstPaneLabel_8 );
		firstPane.add( minitureVolumeFilenameField );
		firstPane.add( firstPaneLabel_9 );
		firstPane.add( miniturePointStructureFilenameField );
		firstPane.add( new JLabel( "" ) );
		firstPane.add( new JLabel( "" ) );
		firstPane.add( new JLabel( "Create Project" ) );
		firstPane.add( createProjectButton );
		
		firstOuterPane.add( firstPane, BorderLayout.NORTH );
		tabbedPanel.add( "Project", firstOuterPane );
		
		
		// Volume Pane
		JLabel secondPaneLabel_1 = new JLabel( " Filename:" );
		
		volumeFilenameLabel = new JLabel( "" );
		widthLabel = new JLabel( "Width: " + volumeWidth );
		heightLabel = new JLabel( "Height: " + volumeHeight );
		depthLabel = new JLabel( "Depth: " + volumeDepth );
		
		secondPaneLabel_1.setHorizontalAlignment( JTextField.LEFT );
		volumeFilenameLabel.setHorizontalAlignment( JTextField.LEFT );
		widthLabel.setHorizontalAlignment( JTextField.CENTER );
		heightLabel.setHorizontalAlignment( JTextField.CENTER );
		depthLabel.setHorizontalAlignment( JTextField.CENTER );
		
		lowerLeftFieldX = new JFormattedTextField( sevenPlaces );
		lowerLeftFieldY = new JFormattedTextField( sevenPlaces );
		lowerLeftFieldZ = new JFormattedTextField( sevenPlaces );
		upperRightFieldX = new JFormattedTextField( sevenPlaces );
		upperRightFieldY = new JFormattedTextField( sevenPlaces );
		upperRightFieldZ = new JFormattedTextField( sevenPlaces );
		voxelSizeFieldX = new JFormattedTextField( sevenPlaces );
		voxelSizeFieldY = new JFormattedTextField( sevenPlaces );
		voxelSizeFieldZ = new JFormattedTextField( sevenPlaces );
		
		Float zeroFloat = new Float( 0.0f );
		lowerLeftFieldX.setValue( zeroFloat );
		lowerLeftFieldY.setValue( zeroFloat );
		lowerLeftFieldZ.setValue( zeroFloat );
		upperRightFieldX.setValue( zeroFloat );
		upperRightFieldY.setValue( zeroFloat );
		upperRightFieldZ.setValue( zeroFloat );
		voxelSizeFieldX.setValue( zeroFloat );
		voxelSizeFieldY.setValue( zeroFloat );
		voxelSizeFieldZ.setValue( zeroFloat );
		
		lowerLeftFieldX.setHorizontalAlignment( JTextField.RIGHT );
		lowerLeftFieldY.setHorizontalAlignment( JTextField.RIGHT );
		lowerLeftFieldZ.setHorizontalAlignment( JTextField.RIGHT );
		upperRightFieldX.setHorizontalAlignment( JTextField.RIGHT );
		upperRightFieldY.setHorizontalAlignment( JTextField.RIGHT );
		upperRightFieldZ.setHorizontalAlignment( JTextField.RIGHT );
		voxelSizeFieldX.setHorizontalAlignment( JTextField.RIGHT );
		voxelSizeFieldY.setHorizontalAlignment( JTextField.RIGHT );
		voxelSizeFieldZ.setHorizontalAlignment( JTextField.RIGHT );
		
		lowerLeftFieldX.addPropertyChangeListener(this);
		lowerLeftFieldY.addPropertyChangeListener(this);
		lowerLeftFieldZ.addPropertyChangeListener(this);
		upperRightFieldX.addPropertyChangeListener(this);
		upperRightFieldY.addPropertyChangeListener(this);
		upperRightFieldZ.addPropertyChangeListener(this);
		voxelSizeFieldX.addPropertyChangeListener(this);
		voxelSizeFieldY.addPropertyChangeListener(this);
		voxelSizeFieldZ.addPropertyChangeListener(this);
		
		volumePane = new JPanel( new BorderLayout() );
		JPanel secondPane = new JPanel( new GridLayout( 4, 1 ) );
		secondPane.setBorder( BorderFactory.createTitledBorder( "Volume Information" ) );
		
		JPanel secondSubPanel = new JPanel( new GridLayout( 4, 2 ) );
		secondSubPanel.add( secondPaneLabel_1 );
		secondSubPanel.add( volumeFilenameLabel );
		secondSubPanel.add( widthLabel );
		secondSubPanel.add( heightLabel );
		secondSubPanel.add( depthLabel );
		secondSubPanel.add( new JLabel( "" ) );
		secondSubPanel.add( new JLabel( "" ) );
		secondSubPanel.add( new JLabel( "" ) );
		secondPane.add( secondSubPanel );
		
		JPanel boundingBoxSubPanel_1 = new JPanel( new BorderLayout() );
		boundingBoxSubPanel_1.setBorder( BorderFactory.createTitledBorder( "Bounding Box - Lower Left" ) );
		JPanel boundingBoxSubPanel_1_WEST = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_1_WEST.add( new JLabel( "X: " ) );
		boundingBoxSubPanel_1_WEST.add( new JLabel( "Y: " ) );
		boundingBoxSubPanel_1_WEST.add( new JLabel( "Z: " ) );
		boundingBoxSubPanel_1.add( boundingBoxSubPanel_1_WEST, BorderLayout.WEST );
		JPanel boundingBoxSubPanel_1_CENTER = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_1_CENTER.add( lowerLeftFieldX );
		boundingBoxSubPanel_1_CENTER.add( lowerLeftFieldY );
		boundingBoxSubPanel_1_CENTER.add( lowerLeftFieldZ );
		boundingBoxSubPanel_1.add( boundingBoxSubPanel_1_CENTER );
		secondPane.add( boundingBoxSubPanel_1 );
		
		JPanel boundingBoxSubPanel_2 = new JPanel( new BorderLayout() );
		boundingBoxSubPanel_2.setBorder( BorderFactory.createTitledBorder( "Bounding Box - Upper Right" ) );
		JPanel boundingBoxSubPanel_2_WEST = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_2_WEST.add( new JLabel( "X: " ) );
		boundingBoxSubPanel_2_WEST.add( new JLabel( "Y: " ) );
		boundingBoxSubPanel_2_WEST.add( new JLabel( "Z: " ) );
		boundingBoxSubPanel_2.add( boundingBoxSubPanel_2_WEST, BorderLayout.WEST );
		JPanel boundingBoxSubPanel_2_CENTER = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_2_CENTER.add( upperRightFieldX );
		boundingBoxSubPanel_2_CENTER.add( upperRightFieldY );
		boundingBoxSubPanel_2_CENTER.add( upperRightFieldZ );
		boundingBoxSubPanel_2.add( boundingBoxSubPanel_2_CENTER );
		secondPane.add( boundingBoxSubPanel_2 );
		
		JPanel boundingBoxSubPanel_3 = new JPanel( new BorderLayout() );
		boundingBoxSubPanel_3.setBorder( BorderFactory.createTitledBorder( "Bounding Box - Voxel Size" ) );
		JPanel boundingBoxSubPanel_3_WEST = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_3_WEST.add( new JLabel( "W: " ) );
		boundingBoxSubPanel_3_WEST.add( new JLabel( "H: " ) );
		boundingBoxSubPanel_3_WEST.add( new JLabel( "D: " ) );
		boundingBoxSubPanel_3.add( boundingBoxSubPanel_3_WEST, BorderLayout.WEST );
		JPanel boundingBoxSubPanel_3_CENTER = new JPanel( new GridLayout( 3, 1 ) );
		boundingBoxSubPanel_3_CENTER.add( voxelSizeFieldX );
		boundingBoxSubPanel_3_CENTER.add( voxelSizeFieldY );
		boundingBoxSubPanel_3_CENTER.add( voxelSizeFieldZ );
		boundingBoxSubPanel_3.add( boundingBoxSubPanel_3_CENTER );
		secondPane.add( boundingBoxSubPanel_3 );
		
		volumePane.add( secondPane, BorderLayout.NORTH );
		tabbedPanel.add( "Volume", volumePane );
		
		
		// Crop Pane
		JLabel thirdPaneLabel_2 = new JLabel( " Intensity:" );
		JLabel thirdPaneLabel_3 = new JLabel( "Minimum:" );
		JLabel thirdPaneLabel_4 = new JLabel( "Maximum:" );
		JLabel thirdPaneLabel_5 = new JLabel( " Point Structure:" );
		JLabel thirdPaneLabel_6 = new JLabel( "Minimum:" );
		JLabel thirdPaneLabel_7 = new JLabel( "Maximum:" );
		
		sliceLabel = new JLabel( "Slice: -1" );
		cropLabel = new JLabel( " Crop (W,H,D):  -1, -1, -1" );
		
		thirdPaneLabel_2.setHorizontalAlignment( JTextField.LEFT );
		thirdPaneLabel_3.setHorizontalAlignment( JTextField.CENTER );
		thirdPaneLabel_4.setHorizontalAlignment( JTextField.CENTER );
		thirdPaneLabel_5.setHorizontalAlignment( JTextField.LEFT );
		thirdPaneLabel_6.setHorizontalAlignment( JTextField.CENTER );
		thirdPaneLabel_7.setHorizontalAlignment( JTextField.CENTER );
		
		thirdPaneLabel_2.setForeground( Color.RED );
		thirdPaneLabel_5.setForeground( Color.BLUE );
		
		sliceLabel.setHorizontalAlignment( JTextField.CENTER );
		cropLabel.setHorizontalAlignment( JTextField.LEFT );
		
		autoIntensityButton = new JButton( "Auto" );
		autoIntensityButton.addActionListener(this);
		pointStructureButton = new JButton( "Turn On" );
		pointStructureButton.addActionListener(this);
		intensityPointStructure = false;
		startSliceButton = new JButton( "Start: -1" );
		startSliceButton.addActionListener(this);
		endSliceButton = new JButton( "End: -1" );
		endSliceButton.addActionListener(this);
		resetCropButton = new JButton( "Reset Crop Area" );
		resetCropButton.addActionListener(this);
		
		minimumIntensity = new JFormattedTextField( sevenPlaces );
		maximumIntensity = new JFormattedTextField( sevenPlaces );
		minimumIntensityPointStructure = new JFormattedTextField( sevenPlaces );
		maximumIntensityPointStructure = new JFormattedTextField( sevenPlaces );
		
		minimumIntensity.setValue( zeroFloat );
		maximumIntensity.setValue( zeroFloat );
		minimumIntensityPointStructure.setValue( zeroFloat );
		maximumIntensityPointStructure.setValue( zeroFloat );
		
		minimumIntensity.setHorizontalAlignment( JTextField.RIGHT );
		maximumIntensity.setHorizontalAlignment( JTextField.RIGHT );
		minimumIntensityPointStructure.setHorizontalAlignment( JTextField.RIGHT );
		maximumIntensityPointStructure.setHorizontalAlignment( JTextField.RIGHT );
		
		minimumIntensity.addPropertyChangeListener(this);
		maximumIntensity.addPropertyChangeListener(this);
		minimumIntensityPointStructure.addPropertyChangeListener(this);
		maximumIntensityPointStructure.addPropertyChangeListener(this);
		
		currentSliceSlider = new JSlider( JSlider.HORIZONTAL );
		currentSliceSlider.addChangeListener(this);
		
		intensityLabel = new HistogramLabel( educeHandle );
		
		cropPane = new JPanel( new BorderLayout() );
		JPanel thirdPane = new JPanel( new BorderLayout() );
		
		JPanel thirdPaneNorth = new JPanel( new GridLayout( 6, 1 ) );
		thirdPaneNorth.add( sliceLabel );
		thirdPaneNorth.add( currentSliceSlider );
		thirdPaneNorth.add( cropLabel );
		JPanel thirdPaneNorthSub = new JPanel( new GridLayout( 1, 2 ) );
		thirdPaneNorthSub.add( startSliceButton );
		thirdPaneNorthSub.add( endSliceButton );
		thirdPaneNorth.add( thirdPaneNorthSub );
		thirdPaneNorth.add( resetCropButton );
		thirdPaneNorth.add( new JLabel( "" ) );
		thirdPane.add( thirdPaneNorth, BorderLayout.NORTH );
		
		JPanel thirdPaneCenter = new JPanel( new BorderLayout() );
		
		JPanel thirdPaneCenterNorth = new JPanel( new GridLayout( 3, 2 ) );
		thirdPaneCenterNorth.add( thirdPaneLabel_2 );
		thirdPaneCenterNorth.add( autoIntensityButton );
		thirdPaneCenterNorth.add( thirdPaneLabel_3 );
		thirdPaneCenterNorth.add( minimumIntensity );
		thirdPaneCenterNorth.add( thirdPaneLabel_4 );
		thirdPaneCenterNorth.add( maximumIntensity );
		thirdPaneCenter.add( thirdPaneCenterNorth, BorderLayout.NORTH );
		thirdPaneCenter.add( intensityLabel );
		JPanel thirdPaneCenterSouth = new JPanel( new GridLayout( 3, 2 ) );
		thirdPaneCenterSouth.add( thirdPaneLabel_5 );
		thirdPaneCenterSouth.add( pointStructureButton );
		thirdPaneCenterSouth.add( thirdPaneLabel_6 );
		thirdPaneCenterSouth.add( minimumIntensityPointStructure );
		thirdPaneCenterSouth.add( thirdPaneLabel_7 );
		thirdPaneCenterSouth.add( maximumIntensityPointStructure );
		thirdPaneCenter.add( thirdPaneCenterSouth, BorderLayout.SOUTH );
		thirdPane.add( thirdPaneCenter );
		
		cropPane.add( thirdPane, BorderLayout.CENTER );//BorderLayout.NORTH );
		tabbedPanel.add( "Crop", cropPane );
		
		tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( volumePane ), false );
		tabbedPanel.setEnabledAt( tabbedPanel.indexOfComponent( cropPane ), false );
		
		newProjectPanel.add( tabbedPanel );
	}
	
	void LoadProjectPanel()
	{
		projectPanel = new JPanel( new BorderLayout() );
		
		tabbedPanel = new JTabbedPane();
		
		JPanel firstOuterPane = new JPanel( new BorderLayout() );
		
		sliceLabel = new JLabel( "Slice: -1" );
		cropLabel = new JLabel( "Crop: -1 -1 -1" );
		
		sliceLabel.setHorizontalAlignment( JTextField.CENTER );
		cropLabel.setHorizontalAlignment( JTextField.LEFT );
		
		startSliceButton = new JButton( "Start: -1" );
		startSliceButton.addActionListener(this);
		endSliceButton = new JButton( "End: -1" );
		endSliceButton.addActionListener(this);
		resetCropButton = new JButton( "Reset Crop Area" );
		resetCropButton.addActionListener(this);
		
		currentSliceSlider = new JSlider( JSlider.HORIZONTAL );
		currentSliceSlider.addChangeListener(this);
		
		JPanel thirdPaneNorth = new JPanel( new GridLayout( 5, 1 ) );
		thirdPaneNorth.add( sliceLabel );
		thirdPaneNorth.add( currentSliceSlider );
		JPanel thirdPaneNorthSub = new JPanel( new GridLayout( 1, 2 ) );
		thirdPaneNorthSub.add( startSliceButton );
		thirdPaneNorthSub.add( endSliceButton );
		thirdPaneNorth.add( thirdPaneNorthSub );
		thirdPaneNorth.add( resetCropButton );
		thirdPaneNorth.add( new JLabel( "" ) );
		firstOuterPane.add( thirdPaneNorth, BorderLayout.NORTH );
		
		tabbedPanel.add( "Select Region", firstOuterPane );
		
		
		projectPanel.add( tabbedPanel );
	}
}
/*** END OF CLASS ***/
