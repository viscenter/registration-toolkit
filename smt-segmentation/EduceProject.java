
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class EduceProject extends JFrame implements ActionListener, WindowListener
{
	private boolean introductionPanelVisible;
	private Container frameContainer;
	private ControlPanel sidePanel;
	private JavaCInterface javaCInterface;
	private educe educeHandle;
	private EduceMenuBar menuBar;
	private int windowWidth, windowHeight;
	private JButton createNewProjectButton, loadProjectButton;
	private JMenu fileMenu, helpMenu;
	private JPanel minitureCanvasPanel;
 	private CanvasWindow mainWindow, minitureWindow;
	
	public EduceProject( int width, int height, educe handle )
	{
		super( "EDUCE: Enhanced Digital Unwrapping for Conservation and Exploration" );
		
		windowWidth = width;
		windowHeight = height;
		educeHandle = handle;
		
		javaCInterface = new JavaCInterface( this );
		mainWindow = new CanvasWindow( javaCInterface, this, true );
		minitureWindow = new CanvasWindow( javaCInterface, this, false );
		
		// Get the JFrame's contant pane
		frameContainer = getContentPane();
		frameContainer.setLayout( new BorderLayout() );
		
		Dimension sidePanelSize = new Dimension( width - height, width - height );
		minitureCanvasPanel = new JPanel( new BorderLayout() );
		minitureCanvasPanel.setBorder( BorderFactory.createEtchedBorder() );
		minitureCanvasPanel.setMinimumSize( sidePanelSize );
		minitureCanvasPanel.setMaximumSize( sidePanelSize );
		minitureCanvasPanel.setPreferredSize( sidePanelSize );
		minitureCanvasPanel.add( minitureWindow );
		sidePanelSize = new Dimension( width - height, height );
		sidePanel = new ControlPanel( minitureCanvasPanel, javaCInterface, this );
		sidePanel.setMinimumSize( sidePanelSize );
		sidePanel.setMaximumSize( sidePanelSize );
		sidePanel.setPreferredSize( sidePanelSize );
		
		// Create and load the menu bar
		menuBar = new EduceMenuBar( this );
		setJMenuBar( menuBar );
		
		LoadIntroPanel();
		
		addWindowListener( this );
		setSize( windowWidth, windowHeight );
		setVisible( true );
	}
	
	public void println( String message, boolean error )
	{
		educeHandle.println( message, error );
	}
	
	public void Quit()
	{
		// Warning message if saving is needed
/***********************************************************/
/***********************************************************/
		
		educeHandle.println( "Stopping Canvas Windows.", false );
		mainWindow.StopDrawing();
		while( mainWindow.isDrawing() );
		
		javaCInterface.FreeData();
		educeHandle.Quit();
	}
	
	private void LoadIntroPanel()
	{
		frameContainer.removeAll();
		introductionPanelVisible = true;
		
		JPanel introPanel = new JPanel( new BorderLayout() );
		
		introPanel.add( new JLabel( new ImageIcon( "images/educeLogo.jpg" ) ), BorderLayout.CENTER );
/***********************************************************/
/***********************************************************/
/***********************************************************/
/***********************************************************/
		
		JPanel buttonPanel = new JPanel( new GridLayout( 1, 3 ) );
		createNewProjectButton = new JButton( "Create Project" );
		createNewProjectButton.addActionListener(this);
		buttonPanel.add( createNewProjectButton );
		buttonPanel.add( new JLabel( "" ) );
		loadProjectButton = new JButton( "Load Project" );
		loadProjectButton.addActionListener(this);
		buttonPanel.add( loadProjectButton );
		introPanel.add( buttonPanel, BorderLayout.SOUTH );
		
		JPanel temporary = new JPanel();
		temporary.add( introPanel );
		frameContainer.add( temporary );
		validate();
		repaint();
	}
	
	
	public void CreateNewProject()
	{
		if( introductionPanelVisible )
		{
			frameContainer.removeAll();
			
			frameContainer.add( mainWindow, BorderLayout.CENTER );
			frameContainer.add( sidePanel, BorderLayout.WEST );
			
			introductionPanelVisible = false;
		}
		
		mainWindow.Draw( mainWindow.CANVAS_WINDOW_DRAW_VOLUME_SLICE );
		minitureWindow.Draw( mainWindow.CANVAS_WINDOW_DRAW_VOLUME_CROP );
		
		sidePanel.LoadPanel( sidePanel.CONTROL_PANEL_NEW_PROJECT );
		
		validate();
		repaint();
	}
	
	public void LoadCreatedProject()
	{
		mainWindow.Draw( mainWindow.CANVAS_WINDOW_DRAW_VOLUME_SLICE );
		minitureWindow.Draw( mainWindow.CANVAS_WINDOW_DRAW_MINITURE_3D );
		
		sidePanel.LoadPanel( sidePanel.CONTROL_PANEL_PROJECT );
		
		validate();
		repaint();
		
		mainWindow.display();
		minitureWindow.display();
	}
	
	
	public ControlPanel GetControlPanel()
	{
		return sidePanel;
	}
	
 	public CanvasWindow GetMainWindow()
 	{
	 	return mainWindow;
 	}
 	
 	public CanvasWindow GetMinitureWindow()
 	{
	 	return minitureWindow;
 	}
	
	
	public void HideMinitureCanvas()
	{
		try
		{
			minitureWindow.setVisible( false );
		}
		catch( NullPointerException exception ){}
	}
	
	public void ShowMinitureCanvas()
	{
		try
		{
			minitureWindow.setVisible( true );
			minitureWindow.display();
		}
		catch( NullPointerException exception ){}
	}
	
	public void RedrawCanvases()
	{
		mainWindow.display();
		minitureWindow.display();
	}
	
	public void UpdateVolumeSlice()
	{
		DrawCanvases( false );
		javaCInterface.LoadVolumeSlice( sidePanel.GetCurrentSlice() );
		DrawCanvases( true );
		RedrawCanvases();
	}
	
	public void DrawCanvases( boolean value )
	{
		mainWindow.SetDrawing( value );
		minitureWindow.SetDrawing( value );
	}
	
	public void SwapWindows()
	{
		int temporary = mainWindow.GetDrawFunction();
		mainWindow.Draw( minitureWindow.GetDrawFunction() );
		minitureWindow.Draw( temporary );
		
		mainWindow.display();
		minitureWindow.display();
	}
	
	public void LoadProject()
	{
		JFileChooser projectFileChooser = new JFileChooser(); 
		projectFileChooser.setCurrentDirectory( new File( System.getProperty( "user.dir" ) ) );
		projectFileChooser.setDialogTitle( "Open Project File" );
		SingleFileFilter filter = new SingleFileFilter( ".xml", "EDUCE Project File (.xml)" );
		projectFileChooser.addChoosableFileFilter(filter);
		
		int returnValue = projectFileChooser.showOpenDialog(this);
		if( returnValue == JFileChooser.APPROVE_OPTION )
		{
			boolean validFile = true;
			File file = projectFileChooser.getSelectedFile();
			String filename = file.getAbsolutePath();
			if( !file.isFile() ) validFile = false;
			String currentExtension = SingleFileFilter.getExtension( file );
			if( currentExtension == null || !currentExtension.toLowerCase().equals( ".xml" ) ) validFile = false;
			
			if( validFile )
			{
				try
				{ // huge hack
					BufferedReader reader = new BufferedReader( new FileReader( file ) );
					String line = "", temporary, temporaryFilename;
					int index1, index2;
					int w, h, d;
					float min, max;
					
					while( line.indexOf( "<volume>" ) < 0 ) line = reader.readLine();
					line = reader.readLine(); // "\t\t<file>...</file>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporaryFilename = line.substring( index1 + 1, index2 );
					
					line = reader.readLine(); // "\t\t<dimensions>\n"
					line = reader.readLine(); // "\t\t\t<width>...</width>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					w = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t\t<height>...</height>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					h = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t\t<depth>...</depth>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					d = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t</dimensions>\n"
					line = reader.readLine(); // "\t\t<intensity>\n"
					line = reader.readLine(); // "\t\t\t<minimum>...</minimum>\n\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					min = Float.parseFloat( temporary );
					
					line = reader.readLine(); // "\t\t\t<maximum>...</maximum>\n\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					max = Float.parseFloat( temporary );
					
					line = reader.readLine(); // "\t\t</intensity>\n"
					line = reader.readLine(); // "\t</volume>\n"
					
					javaCInterface.SetVolumeIntensity( min, max );
					javaCInterface.LoadFloatVolumeFile( temporaryFilename, w, h, d );
					
					line = reader.readLine(); // "\t<miniture>\n"
					line = reader.readLine(); // "\t\t<file>...</file>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporaryFilename = line.substring( index1 + 1, index2 );
					
					line = reader.readLine(); // "\t\t<dimensions>\n"
					line = reader.readLine(); // "\t\t\t<width>...</width>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					w = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t\t<height>...</height>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					h = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t\t<depth>...</depth>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					d = Integer.parseInt( temporary );
					
					line = reader.readLine(); // "\t\t</dimensions>\n"
					line = reader.readLine(); // "\t\t<meshfile>...</meshfile>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					
					line = reader.readLine(); // "\t</miniture>\n"
					
					javaCInterface.LoadMinitureFile( temporaryFilename, w, h, d );
					javaCInterface.LoadMiniturePointStructureFile( temporary );
					
					
					line = reader.readLine(); // "\t<meshfile>...</meshfile>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					
					javaCInterface.LoadMeshFile( temporary );
					
					line = reader.readLine(); // "\t<texturefile>...</texturefile>\n"
					index1 = line.indexOf( ">" );
					index2 = line.lastIndexOf( "<" );
					temporary = line.substring( index1 + 1, index2 );
					
					javaCInterface.LoadTextureFile( temporary );
						
					reader.close();
				}
				catch( FileNotFoundException exception ){}
				catch( IOException exception ){}
			}
			LoadCreatedProject();
		}
	}
	
	public void SaveProject()
	{
		
System.out.println( "Save Project" );
	}
	
	public void DisplayHelp()
	{
		
System.out.println( "Help" );
	}
	
	public void DisplayAbout()
	{
		
System.out.println( "About" );
	}
	
	public void actionPerformed( ActionEvent event )
	{
		Object source = event.getSource();
		
		if( source == createNewProjectButton ){ CreateNewProject(); }
		else if( source == loadProjectButton ){ LoadProject(); }
	}
	
	public void windowActivated( WindowEvent event ){}
	public void windowClosed( WindowEvent event ){ Quit(); }
	public void windowDeactivated( WindowEvent event ){}
	public void windowDeiconified( WindowEvent event ){}
	public void windowIconified( WindowEvent event ){}
	public void windowOpened( WindowEvent event ){}
	public void windowClosing( WindowEvent event ){ Quit(); }
	
}
/*** END OF CLASS ***/
