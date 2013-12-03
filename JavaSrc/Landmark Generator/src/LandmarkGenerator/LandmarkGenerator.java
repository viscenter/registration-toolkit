package LandmarkGenerator;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;
import javax.swing.plaf.SplitPaneUI;

import java.awt.event.MouseAdapter;


public class LandmarkGenerator extends JFrame implements ActionListener, MouseListener{
  
		/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
		static PicturePanel FixedPicture;
        static PicturePanel MovingPicture;
        static JButton open,open2,makeLandmarks,CreateFile, Bigger1, Bigger2, Smaller1, Smaller2;
        static JScrollPane scroll1,scroll2;
        int counter;
        static int cposition,rposition;                
        static double fscale, mscale;
        static int [][] PointsforLandmarks=new int[5][4];
        static JEditorPane DisplayedLandmarks = new JEditorPane();
        private JFrame MainWindow;
        static LandmarkGenerator Generator = new LandmarkGenerator();

    	/**
    	 * Launch the application.
    	 */
    	public static void main(String[] args) {
    		EventQueue.invokeLater(new Runnable() {
    			public void run() {
    				try {
    					LandmarkGenerator window = new LandmarkGenerator();
    					window.MainWindow.setVisible(true);
    				} catch (Exception e) {
    					e.printStackTrace();
    				}
    			}
    		});
    	}
    	
    	/**
    	 * Create the application.
    	 */
    	public LandmarkGenerator() {
    		Initializer();
    		CreateLayout();
    	}

        public void Initializer(){
            makeLandmarks = new JButton("Create Landmarks");
            open = new JButton("Open 1st Image");
            open2 = new JButton("Open 2nd Image");
            CreateFile = new JButton("Write to File");
            Bigger1 = new JButton("Zoom in 1st picture");
            Bigger2 = new JButton("Zoom in 2nd picture");
            Smaller1 = new JButton("Zoom out 1st picture");
            Smaller2 = new JButton("Zoom out 2nd picture");
            counter=0;
            fscale = 1;
            mscale = 1;
            FixedPicture = new PicturePanel();
            MovingPicture = new PicturePanel();
            scroll1 = new JScrollPane(FixedPicture);//declares it
            scroll2 = new JScrollPane(MovingPicture);//declares it
            
    }
    MouseListener landmarkReturn = new MouseAdapter() {
            public void mouseClicked(MouseEvent arg0) {
                    JScrollPane s = (JScrollPane)(arg0.getSource());
                    System.out.println(s);
                    cposition=counter%2;
                    if(counter%2!=0){
                            cposition=2;
                    }
                    rposition=counter/2;
                    Point P1=new Point();
                    
                    
                    if(counter%2==0&&arg0.getSource()==scroll1){
                            P1 = SwingUtilities.convertPoint(s, arg0.getPoint(), FixedPicture);
                            PointsforLandmarks[rposition][cposition] = (int) (P1.x/fscale);
                            PointsforLandmarks[rposition][++cposition] = (int) (P1.y/fscale);
                            counter++;

                    }
                    else
                            if(counter%2==0&&s==scroll2){                        
                                    JOptionPane.showMessageDialog(Generator, "Please choose a point from the first picture before choosing one from the second picture.", "Selection Error", JOptionPane.PLAIN_MESSAGE);
                                    return;
                            }
                            else
                                    if(counter%2!=0&&arg0.getSource()==scroll2){
                                            P1 = SwingUtilities.convertPoint(s, arg0.getPoint(), MovingPicture);
                                            PointsforLandmarks[rposition][cposition] = (int) (P1.x/mscale);
                                            PointsforLandmarks[rposition][++cposition] = (int) (P1.y/mscale);
                                            counter++;
                                            scroll1.removeMouseListener(landmarkReturn);
                                            scroll2.removeMouseListener(landmarkReturn);
                                    }
                                    else
                                            if(counter%2!=0&&s==scroll1){
                                                    JOptionPane.showMessageDialog(Generator, "Please choose a point from the Second picture now instead of choosing one from the first picture.", "Selection Error", JOptionPane.PLAIN_MESSAGE);
                                                    return;
                                            }

                    
                    DisplayedLandmarks.setText(null);
                    for(int i=0;i<PointsforLandmarks.length;i++){
                            for(int j=0;j<PointsforLandmarks[i].length;j++){
                                    DisplayedLandmarks.setText(DisplayedLandmarks.getText() + PointsforLandmarks[i][j] +" ");
                            }
                            DisplayedLandmarks.setText(DisplayedLandmarks.getText()+"\n");
                    }
                    return;                                         
            }  
    };    
        public void CreateLayout(){
        	MainWindow = new JFrame();
    		MainWindow.setResizable(false);
    		MainWindow.setBounds(100, 100, 1069, 776);
    		MainWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    		
    		JPanel UpperPanel = new JPanel();
    		
    		JSplitPane viewersSplitPane = new JSplitPane();
    		viewersSplitPane.setEnabled(false);
    		viewersSplitPane.setAlignmentY(Component.CENTER_ALIGNMENT);
    		
    		scroll1 = new JScrollPane(FixedPicture);
    		scroll1.setPreferredSize(new Dimension(450, 550));
    		viewersSplitPane.setLeftComponent(scroll1);
    		
    		final JScrollPane scroll2 = new JScrollPane(MovingPicture);
    		scroll2.setPreferredSize(new Dimension(450, 550));
    		viewersSplitPane.setRightComponent(scroll2);
    	
    		
    		final JEditorPane DisplayedLandmarks = new JEditorPane();
    		DisplayedLandmarks.setAlignmentX(Component.LEFT_ALIGNMENT);
    		
    		JPanel LowerPanel = new JPanel();
    		GroupLayout groupLayout = new GroupLayout(MainWindow.getContentPane());
    		groupLayout.setHorizontalGroup(
    			groupLayout.createParallelGroup(Alignment.TRAILING)
    				.addComponent(UpperPanel, GroupLayout.PREFERRED_SIZE, 1024, Short.MAX_VALUE)
    				.addGroup(groupLayout.createSequentialGroup()
    					.addComponent(LowerPanel, GroupLayout.DEFAULT_SIZE, 1068, Short.MAX_VALUE)
    					.addGap(1))
    		);
    		groupLayout.setVerticalGroup(
    			groupLayout.createParallelGroup(Alignment.LEADING)
    				.addGroup(groupLayout.createSequentialGroup()
    					.addComponent(UpperPanel, GroupLayout.PREFERRED_SIZE, 565, GroupLayout.PREFERRED_SIZE)
    					.addPreferredGap(ComponentPlacement.RELATED)
    					.addComponent(LowerPanel, GroupLayout.PREFERRED_SIZE, 176, Short.MAX_VALUE)
    					.addContainerGap())
    		);
    		
    		JPanel LeftPanelControls = new JPanel();
    		LeftPanelControls.setPreferredSize(new Dimension(450, 10));
    		
    		JPanel RightPanelControls = new JPanel();
    		RightPanelControls.setPreferredSize(new Dimension(450, 10));
    		
    		JButton makeLandmarks = new JButton("Add Landmark");
    		makeLandmarks.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
                    //String input = JOptionPane.showInputDialog(this  ,"Landmark number to edit:");
                    //int Input = Integer.parseInt(input);
                    //if(Input%2==0){Selector(scroll2,Input);}
                    //else
                  
                    scroll1.addMouseListener(landmarkReturn);
                    scroll2.addMouseListener(landmarkReturn);
    			}});
    		
    		JButton CreateFile = new JButton("Save");
    		CreateFile.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
                    String newfile = JOptionPane.showInputDialog("Enter a filename..."  ,"File Name")+".txt";
                    PrintWriter writer = null;
                    try {
                            writer = new PrintWriter(newfile, "UTF-8");
                    } catch (FileNotFoundException e1) {
                            // TODO Auto-generated catch block
                            e1.printStackTrace();
                    } catch (UnsupportedEncodingException e1) {
                            // TODO Auto-generated catch block
                            e1.printStackTrace();
                    }
                    writer.print(Arrays.toString(PointsforLandmarks));
                    writer.close();
    			}
    		});
    		GroupLayout gl_LowerPanel = new GroupLayout(LowerPanel);
    		gl_LowerPanel.setHorizontalGroup(
    			gl_LowerPanel.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_LowerPanel.createSequentialGroup()
    					.addContainerGap()
    					.addComponent(LeftPanelControls, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
    					.addPreferredGap(ComponentPlacement.RELATED)
    					.addComponent(RightPanelControls, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
    					.addPreferredGap(ComponentPlacement.RELATED, 15, Short.MAX_VALUE)
    					.addGroup(gl_LowerPanel.createParallelGroup(Alignment.TRAILING)
    						.addGroup(gl_LowerPanel.createSequentialGroup()
    							.addComponent(makeLandmarks)
    							.addContainerGap())
    						.addComponent(CreateFile)))
    		);
    		gl_LowerPanel.setVerticalGroup(
    			gl_LowerPanel.createParallelGroup(Alignment.TRAILING)
    				.addGroup(Alignment.LEADING, gl_LowerPanel.createSequentialGroup()
    					.addContainerGap()
    					.addGroup(gl_LowerPanel.createParallelGroup(Alignment.LEADING)
    						.addGroup(gl_LowerPanel.createSequentialGroup()
    							.addComponent(makeLandmarks)
    							.addPreferredGap(ComponentPlacement.RELATED, 112, Short.MAX_VALUE)
    							.addComponent(CreateFile))
    						.addComponent(RightPanelControls, GroupLayout.DEFAULT_SIZE, 170, Short.MAX_VALUE)
    						.addComponent(LeftPanelControls, GroupLayout.DEFAULT_SIZE, 170, Short.MAX_VALUE))
    					.addContainerGap())
    		);
    		
    		JButton open2 = new JButton("Open Image...");
    		open2.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
    				JFileChooser jfc = new JFileChooser();
                    int result = jfc.showOpenDialog(MainWindow);
                    if(result == JFileChooser.CANCEL_OPTION)
                            return;
                    File f = jfc.getSelectedFile();
                    MovingPicture.setImage(f);
                    MainWindow.repaint();
    			}
    		});
    		open2.setAlignmentX(0.5f);
    		
    		JButton Smaller2 = new JButton("-");
    		Smaller2.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
    				mscale/=1.1;
                    MovingPicture.resize(mscale);
    			}
    		});
    		Smaller2.setAlignmentX(0.5f);
    		
    		JButton Bigger2 = new JButton("+");
    		Bigger2.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
    				 mscale*=1.1;
                     MovingPicture.resize(mscale);
    			}
    		});
    		Bigger2.setAlignmentX(0.5f);
    		GroupLayout gl_RightPanelControls = new GroupLayout(RightPanelControls);
    		gl_RightPanelControls.setHorizontalGroup(
    			gl_RightPanelControls.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_RightPanelControls.createSequentialGroup()
    					.addGap(150)
    					.addGroup(gl_RightPanelControls.createParallelGroup(Alignment.LEADING)
    						.addGroup(gl_RightPanelControls.createSequentialGroup()
    							.addComponent(Smaller2, GroupLayout.PREFERRED_SIZE, 75, GroupLayout.PREFERRED_SIZE)
    							.addComponent(Bigger2, GroupLayout.PREFERRED_SIZE, 75, GroupLayout.PREFERRED_SIZE))
    						.addGroup(gl_RightPanelControls.createSequentialGroup()
    							.addGap(10)
    							.addComponent(open2, GroupLayout.PREFERRED_SIZE, 131, GroupLayout.PREFERRED_SIZE)))
    					.addContainerGap(150, Short.MAX_VALUE))
    		);
    		gl_RightPanelControls.setVerticalGroup(
    			gl_RightPanelControls.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_RightPanelControls.createSequentialGroup()
    					.addGroup(gl_RightPanelControls.createParallelGroup(Alignment.LEADING)
    						.addComponent(Smaller2)
    						.addComponent(Bigger2))
    					.addGap(6)
    					.addComponent(open2)
    					.addContainerGap(106, Short.MAX_VALUE))
    		);
    		RightPanelControls.setLayout(gl_RightPanelControls);
    		
    		JButton Smaller1 = new JButton("-");
    		Smaller1.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent arg0) {
    				fscale/=1.1;
                    FixedPicture.resize(fscale);
    			}
    		});
    		Smaller1.setAlignmentX(Component.CENTER_ALIGNMENT);
    		
    		JButton Bigger1 = new JButton("+");
    		Bigger1.addActionListener(new ActionListener() {
    			public void actionPerformed(ActionEvent e) {
    				fscale*=1.1;
                    FixedPicture.resize(fscale);
    			}
    		});
    		Bigger1.setAlignmentX(Component.CENTER_ALIGNMENT);
    		
    		JButton open = new JButton("Open Image...");
    		open.addMouseListener(new MouseAdapter() {
    			@Override
    			public void mouseClicked(MouseEvent e) {
    				JFileChooser jfc = new JFileChooser();
                    int result = jfc.showOpenDialog(MainWindow);
                    if(result == JFileChooser.CANCEL_OPTION)
                            return;
                    File f = jfc.getSelectedFile();
                    FixedPicture.setImage(f);
                    MainWindow.repaint();
    			}
    		});
    		open.setAlignmentX(Component.CENTER_ALIGNMENT);
    		GroupLayout gl_LeftPanelControls = new GroupLayout(LeftPanelControls);
    		gl_LeftPanelControls.setHorizontalGroup(
    			gl_LeftPanelControls.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_LeftPanelControls.createSequentialGroup()
    					.addGroup(gl_LeftPanelControls.createParallelGroup(Alignment.LEADING)
    						.addGroup(gl_LeftPanelControls.createSequentialGroup()
    							.addGap(160)
    							.addComponent(open))
    						.addGroup(gl_LeftPanelControls.createSequentialGroup()
    							.addGap(150)
    							.addComponent(Smaller1)
    							.addComponent(Bigger1)))
    					.addContainerGap(140, Short.MAX_VALUE))
    		);
    		gl_LeftPanelControls.setVerticalGroup(
    			gl_LeftPanelControls.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_LeftPanelControls.createSequentialGroup()
    					.addGroup(gl_LeftPanelControls.createParallelGroup(Alignment.LEADING)
    						.addComponent(Smaller1)
    						.addComponent(Bigger1))
    					.addPreferredGap(ComponentPlacement.RELATED)
    					.addComponent(open)
    					.addGap(106))
    		);
    		LeftPanelControls.setLayout(gl_LeftPanelControls);
    		LowerPanel.setLayout(gl_LowerPanel);
    		GroupLayout gl_UpperPanel = new GroupLayout(UpperPanel);
    		gl_UpperPanel.setHorizontalGroup(
    			gl_UpperPanel.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_UpperPanel.createSequentialGroup()
    					.addGap(3)
    					.addComponent(viewersSplitPane, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
    					.addGap(2)
    					.addComponent(DisplayedLandmarks, GroupLayout.DEFAULT_SIZE, 145, Short.MAX_VALUE)
    					.addContainerGap())
    		);
    		gl_UpperPanel.setVerticalGroup(
    			gl_UpperPanel.createParallelGroup(Alignment.LEADING)
    				.addGroup(gl_UpperPanel.createSequentialGroup()
    					.addGap(5)
    					.addGroup(gl_UpperPanel.createParallelGroup(Alignment.TRAILING, false)
    						.addComponent(DisplayedLandmarks, Alignment.LEADING)
    						.addComponent(viewersSplitPane, Alignment.LEADING))
    					.addContainerGap())
    		);
    		UpperPanel.setLayout(gl_UpperPanel);
    		MainWindow.getContentPane().setLayout(groupLayout);
    		
        };


	@Override
	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub
		
	}
}