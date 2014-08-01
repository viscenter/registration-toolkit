
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class EduceMenuBar extends JMenuBar implements ActionListener, PopupMenuListener
{
	private EduceProject educeHandle;
	private JMenu fileMenu, helpMenu;
	private JMenuItem quit, newProject, load, save;
	private JMenuItem help, about;
	
	public EduceMenuBar( EduceProject handle )
	{
		educeHandle = handle;
		
		fileMenu = new JMenu( "File", false );
		helpMenu = new JMenu( "Help", false );
		
		newProject = new JMenuItem( "New Project", KeyEvent.VK_CONTROL + KeyEvent.VK_N );
		newProject.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_N, ActionEvent.CTRL_MASK ));
		newProject.addActionListener(this);
		fileMenu.add( newProject );
		load = new JMenuItem( "Open Project" );
		load.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_O, ActionEvent.CTRL_MASK ));
		load.addActionListener(this);
		fileMenu.add( load );
		save = new JMenuItem( "Save Project" );
		save.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_S, ActionEvent.CTRL_MASK ));
		save.addActionListener(this);
		save.setEnabled(false);
		fileMenu.add( save );
		fileMenu.addSeparator();
		quit = new JMenuItem( "Quit" );
		quit.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_F4, ActionEvent.ALT_MASK ));
		quit.addActionListener(this);
		fileMenu.add( quit );
		
		help = new JMenuItem( "Help" );
		help.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_F1, 0 ));
		help.addActionListener(this);
		helpMenu.add( help );
		about = new JMenuItem( "About" );
		about.addActionListener(this);
		helpMenu.add( about );
		
		add( fileMenu );
		add( helpMenu );
		
		fileMenu.getPopupMenu().addPopupMenuListener(this);
		helpMenu.getPopupMenu().addPopupMenuListener(this);
	}
	
	
	public void actionPerformed( ActionEvent event )
	{
		Object source = event.getSource();
		
		if( source == quit )
		{
			educeHandle.Quit();
		}
		else if( source == newProject )
		{
			educeHandle.CreateNewProject();
		}
		else if( source == load )
		{
			educeHandle.LoadProject();
		}
		else if( source == save )
		{
			educeHandle.SaveProject();
		}
		else if( source == help )
		{
			educeHandle.DisplayHelp();
		}
		else if( source == about )
		{
			educeHandle.DisplayAbout();
		}
	}
	
	public void popupMenuCanceled( PopupMenuEvent event ){}
	public void popupMenuWillBecomeVisible( PopupMenuEvent event )
	{
		educeHandle.HideMinitureCanvas();
	}
	
	public void popupMenuWillBecomeInvisible( PopupMenuEvent event )
	{
		educeHandle.ShowMinitureCanvas();
	}
	
}
/*** END OF CLASS ***/
