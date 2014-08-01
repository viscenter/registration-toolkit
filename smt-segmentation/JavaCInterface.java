
public class JavaCInterface
{
	private boolean valid;
	private EduceProject educeHandle;
	
	/*** Connect java side to c side ***/
	static
	{
		String name = "\\JavaCInterface.dll";
		String os = System.getProperty( "os.name" );
		
		// If operating system is not windows - assume linux
		if( os.charAt(0) != 'W' && os.charAt(0) != 'w' )
		{
			name = "/JavaCInterface.so";
		}
		
		// This can be changed to reflect the location of the shared library
		System.load( System.getProperty( "user.dir" ) + name );
	} // END OF - static
	
	
	public JavaCInterface( EduceProject handle )
	{
		valid = true;
		educeHandle = handle;
		SetupCSide();
	}
	
	public void FreeData()
	{
		valid = false;
		FreeCSide();
	}
	
	public void LoadVolume( String filename )
	{
		if( LoadVolumeFile( filename ) == 0 )
		{
			valid = true;
		}
	}
	
	public void Redraw()
	{
		educeHandle.RedrawCanvases();
	}
	
	public void SetVolumeIntensityFields( float minimum, float maximum )
	{
		educeHandle.GetControlPanel().SetVolumeIntensityFields( minimum, maximum );
	}
	
	public void SetCropRegionFields( int minx, int maxx, int miny, int maxy, int minz, int maxz )
	{
		educeHandle.GetControlPanel().SetCropRegionFields( minx, maxx, miny, maxy, minz, maxz );
	}
	
	public void SetPointStructureIntensityFields( float minimum, float maximum )
	{
		educeHandle.GetControlPanel().SetJavaPointStructureIntensityFields( minimum, maximum );
	}
	
	public void SetIntensityLabel( float minimum, float maximum, int data[] )
	{
		educeHandle.GetControlPanel().SetIntensityLabel( minimum, maximum, data );
	}
	
	private native void SetupCSide();
	private native void FreeCSide();
	
	public native void DrawVolumeSlice( int canvasWidth, int canvasHeight );
	public native void DrawVolumeCrop( int canvasWidth, int canvasHeight );
	public native void DrawMiniture3D( int canvasWidth, int canvasHeight );
	public native void DrawRegionSlice( int canvasWidth, int canvasHeight );
	public native void DrawRegion3D( int canvasWidth, int canvasHeight );
	public native void Draw3DModel( int canvasWidth, int canvasHeight );
	public native void DrawProgressBar( int canvasWidth, int canvasHeight );
	
	public native void MousePressed( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
	public native void MouseReleased( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
	public native void MouseDragged( int drawFunction, int x, int y, int button, int modifiers, int w, int h );
	
	public native int LoadVolumeFile( String filename );
	public native int LoadFloatVolumeFile( String filename, int width, int height, int depth );
	
	public native int LoadMinitureFile( String filename, int width, int height, int depth );
	public native int LoadMiniturePointStructureFile( String filename );
	public native int LoadMeshFile( String filename );
	public native int LoadTextureFile( String filename );
	
	public native int[] GetVolumeDimensions();
	
	public native void LoadVolumeSlice( int sliceNumber );
	
	public native void AutoIntensity( int sliceNumber );
	public native void SetVolumeIntensity( float minimum, float maximum );
	public native void SetPointStructureIntensity( float minimum, float maximum );
	public native void SetPointStructure( int value );
	
	public native void ResetVolumeCropRegion();
	
	public native int SaveNewProject( String volumeFilename, int start, int end, String minitureFilename, float scale, String miniturePointFilename, String meshFilename, String textureFilename );
}