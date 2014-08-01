import java.lang.*;
import java.awt.event.*;

import javax.media.opengl.GL2;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLContext;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLProfile;
import javax.media.opengl.awt.GLCanvas;
import javax.media.opengl.glu.GLU;
import static javax.media.opengl.GL.*;  // GL constants
import static javax.media.opengl.GL2.*; // GL2 constants

public class CanvasWindow extends GLCanvas implements MouseListener, MouseMotionListener, GLEventListener
{
    public static int CANVAS_WINDOW_DRAW_VOLUME_SLICE   = 10;
    public static int CANVAS_WINDOW_DRAW_VOLUME_CROP    = 11;
    public static int CANVAS_WINDOW_DRAW_MINITURE_3D    = 21;
    public static int CANVAS_WINDOW_DRAW_REGION_SLICE   = 30;
    public static int CANVAS_WINDOW_DRAW_REGION_3D      = 31;
    public static int CANVAS_WINDOW_DRAW_3D_MODEL       = 50;
    public static int CANVAS_WINDOW_DRAW_PROGRESS_BAR   = 98;
    public static int CANVAS_WINDOW_DRAW_NOTHING        = 99;
	
    private boolean isMain, doNotDraw, drawing, firstMousePress;
    private JavaCInterface javaCInterface;
    private EduceProject educeHandle;
    private int canvasWidth, canvasHeight, drawFunction;
    private GLU glu;
    private GLContext glc;
    private boolean debug = true;

    private void debugInfo(String input)
    {
	if(debug)
	    {
		System.out.println(input);
	    }
    }
	
    private static GLCapabilities makeCapabilities()
    {
	GLProfile.initSingleton();
	GLCapabilities capabilities = new GLCapabilities(GLProfile.getDefault());
	capabilities.setDoubleBuffered(true);
	capabilities.setStereo(false);
	return capabilities;
    }

    public CanvasWindow( JavaCInterface cInterface, EduceProject handle, boolean main )
    {
	super(makeCapabilities());
	
	javaCInterface = cInterface;
	educeHandle = handle;
	isMain = main;
	doNotDraw = true;
	drawing = false;
	firstMousePress = true;
		
	addMouseListener(this);
	addMouseMotionListener(this);
	this.addGLEventListener(this);
    }

    public void init(GLAutoDrawable drawable)
    {
	GL2 gl = drawable.getGL().getGL2();
	// glu = new GLU();
	// glc = drawable.getContext();
	
	gl.glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
		
	gl.glShadeModel( GL_SMOOTH );
	gl.glClearDepth( 1.0f );
	gl.glDepthFunc( GL_LEQUAL );
	gl.glEnable( GL_DEPTH_TEST );
		
	gl.glEnable( GL_TEXTURE_2D );
	gl.glPolygonMode( GL_FRONT, GL_FILL );
	gl.glPolygonMode( GL_BACK, GL_FILL );
		
	canvasWidth = super.getWidth();
	canvasHeight = super.getHeight();
    }

    public void display(GLAutoDrawable drawable)
    {
	GL2 gl = drawable.getGL().getGL2();
	glc = drawable.getContext();
	if( doNotDraw ) {
	    debugInfo("Did not draw.");
	    return;
	}
	// glc.makeCurrent();
	drawing = true;
	gl.glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl.glLoadIdentity();
		
	if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_SLICE )
	    {
		debugInfo("draw volume slice.");
		javaCInterface.DrawVolumeSlice( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_CROP )
	    {
		debugInfo("draw volume crop.");
		javaCInterface.DrawVolumeCrop( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_MINITURE_3D )
	    {
		debugInfo("draw miniture 3D.");
		javaCInterface.DrawMiniture3D( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_REGION_SLICE )
	    {
		debugInfo("draw region slice.");
		javaCInterface.DrawRegionSlice( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_REGION_3D )
	    {
		debugInfo("draw region 3D.");
		javaCInterface.DrawRegion3D( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_3D_MODEL )
	    {
		debugInfo("draw 3D model.");
		javaCInterface.Draw3DModel( canvasWidth, canvasHeight );
	    }
	else if( drawFunction == CANVAS_WINDOW_DRAW_PROGRESS_BAR )
	    {
		debugInfo("draw progress bar.");
		javaCInterface.DrawProgressBar( canvasWidth, canvasHeight );
	    }
	
	// gl.glFlush();		
	// drawable.swapBuffers();

	gl.glEnd();
	// glj.gljCheckGL();
	// glc.release();
	drawing = false;
    }

    public void dispose(GLAutoDrawable drawable)
    {
	// System.out.println("dispose.");
    }

    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height)
    {
	GL2 gl = drawable.getGL().getGL2();
	gl.glViewport(x, y, width, height);
	gl.glLoadIdentity();
	// System.out.println("reshape. " + x + " " + y + " " + width + " " + height);
	// super.reshape( x, y, width, height );
    	canvasWidth = width;
    	canvasHeight = height;
    }

    public void StopDrawing()
    {
	doNotDraw = true;
    }
	
    public void SetDrawing( boolean value )
    {
	doNotDraw = !value;
    }
	
    public boolean isDrawing()
    {
	return drawing;
    }
	
    public void Draw( int function )
    {
	drawFunction = function;
	doNotDraw = false;
    }
	
    public void preInit()
    {
	// doubleBuffer = true;
	// stereoView = false;
    } // END OF - public void preInit()
	
    // public void init(GLAutoDrawable drawable)
    // {
    // 	System.out.println("reached init.");
	
    // 	GL2 gl = drawable.getGL().getGL2();
    // 	glu = new GLU();
    // 	glc = drawable.getContext();
	
    // 	gl.glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
		
    // 	gl.glShadeModel( GL_SMOOTH );
    // 	gl.glClearDepth( 1.0 );
    // 	gl.glDepthFunc( GL_LEQUAL );
    // 	gl.glEnable( GL_DEPTH_TEST );
		
    // 	gl.glEnable( GL_TEXTURE_2D );
    // 	gl.glPolygonMode( GL_FRONT, GL_FILL );
    // 	gl.glPolygonMode( GL_BACK, GL_FILL );
		
    // 	canvasWidth = super.getWidth();
    // 	canvasHeight = super.getHeight();
    // }
	
    // public void reshape( int w, int h )
    // {
    // 	super.reshape( super.getX(), super.getY(), w, h );
    // 	canvasWidth = super.getWidth();
    // 	canvasHeight = super.getHeight();
    // }
	
    // public void display(GLAutoDrawable drawable)
    // {
    // 	System.out.println("display!");

    // 	GL2 gl = drawable.getGL().getGL2();
    // 	if( doNotDraw ) return;
    // 	// glc.makeCurrent();
    // 	drawing = true;
    // 	gl.glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
    // 	if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_SLICE )
    // 	    {
    // 		javaCInterface.DrawVolumeSlice( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_VOLUME_CROP )
    // 	    {
    // 		javaCInterface.DrawVolumeCrop( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_MINITURE_3D )
    // 	    {
    // 		javaCInterface.DrawMiniture3D( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_REGION_SLICE )
    // 	    {
    // 		javaCInterface.DrawRegionSlice( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_REGION_3D )
    // 	    {
    // 		javaCInterface.DrawRegion3D( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_3D_MODEL )
    // 	    {
    // 		javaCInterface.Draw3DModel( canvasWidth, canvasHeight );
    // 	    }
    // 	else if( drawFunction == CANVAS_WINDOW_DRAW_PROGRESS_BAR )
    // 	    {
    // 		javaCInterface.DrawProgressBar( canvasWidth, canvasHeight );
    // 	    }
		
    // 	this.swapBuffers();
    // 	gl.glEnd();
    // 	// glj.gljCheckGL();
    // 	glc.release();
    // 	drawing = false;
    // }
	
    public int GetDrawFunction()
    {
	return drawFunction;
    }
	
    public void mousePressed( MouseEvent event )
    {
	if( !firstMousePress )
	    {
		firstMousePress = true;
		return;
	    }
		
	if( !isMain )
	    {
		educeHandle.SwapWindows();
		firstMousePress = false;
		return;
	    }
		
	firstMousePress = false;
		
	int button = 0, modifiers = 0;
	if( event.getButton() == MouseEvent.BUTTON1 ) button = 1;
	if( event.getButton() == MouseEvent.BUTTON2 ) button = 2;
	if( event.getButton() == MouseEvent.BUTTON3 ) button = 3;
		
	if( event.isAltDown() ) modifiers += 1;
	if( event.isControlDown() ) modifiers += 2;
	if( event.isShiftDown() ) modifiers += 4;
		
	javaCInterface.MousePressed( drawFunction, event.getX(), canvasHeight - event.getY(), button, modifiers, canvasWidth, canvasHeight );
	display();
    }
	
    public void mouseReleased( MouseEvent event )
    {
	if( !isMain ) return;
	if( !firstMousePress )
	    {
		firstMousePress = true;
		return;
	    }
	firstMousePress = false;
		
	int button = 0, modifiers = 0;
	if( event.getButton() == MouseEvent.BUTTON1 ) button = 1;
	if( event.getButton() == MouseEvent.BUTTON2 ) button = 2;
	if( event.getButton() == MouseEvent.BUTTON3 ) button = 3;
		
	if( event.isAltDown() ) modifiers += 1;
	if( event.isControlDown() ) modifiers += 2;
	if( event.isShiftDown() ) modifiers += 4;
		
	javaCInterface.MouseReleased( drawFunction, event.getX(), canvasHeight - event.getY(), button, modifiers, canvasWidth, canvasHeight );
	educeHandle.UpdateVolumeSlice();
	display();
    }
	
    public void mouseMoved( MouseEvent event ){}
    public void mouseExited( MouseEvent event ){}
    public void mouseEntered( MouseEvent event ){}
    public void mouseClicked( MouseEvent event ){}
    public void mouseDragged( MouseEvent event )
    {
	if( !isMain ) return;
	if( !firstMousePress )
	    {
		firstMousePress = true;
		return;
	    }
	firstMousePress = false;
		
	int button = 0, modifiers = 0;
	if( event.getButton() == MouseEvent.BUTTON1 ) button = 1;
	if( event.getButton() == MouseEvent.BUTTON2 ) button = 2;
	if( event.getButton() == MouseEvent.BUTTON3 ) button = 3;
		
	if( event.isAltDown() ) modifiers += 1;
	if( event.isControlDown() ) modifiers += 2;
	if( event.isShiftDown() ) modifiers += 4;
		
	javaCInterface.MouseDragged( drawFunction, event.getX(), canvasHeight - event.getY(), button, modifiers, canvasWidth, canvasHeight );
	display();
    }
	
}
/*** END OF CLASS ***/
