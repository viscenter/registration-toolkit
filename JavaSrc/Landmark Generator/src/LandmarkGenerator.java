import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.*;


public class LandmarkGenerator extends JFrame implements ActionListener{

	static PicturePanel FixedPicture;
	static PicturePanel MovingPicture;
	static JButton open,open2,makeLandmarks;
	static JScrollPane scroll1,scroll2;
	static Boolean Scroll1Listen,Scroll2Listen;
	
	int counter;
	static int cposition,rposition;		
	static int [][] PointsforLandmarks= new int [15][4];
	static JEditorPane DisplayedLandmarks = new JEditorPane();


	//static LandmarkGenerator JP = 
	
	
	public LandmarkGenerator(int r, int c){
		super("Landmark Generator");
		
		JPanel holdall=new JPanel();//make panel
		GroupLayout layout = new GroupLayout(holdall);
		holdall.setLayout(layout);
		JPanel Landmarks = new JPanel();
		Landmarks.setLayout(new GridLayout(2,1));
		JEditorPane DisplayedLandmarks = new JEditorPane();
		int [][] PointsforLandmarks= new int [100][100];
		makeLandmarks = new JButton("Create Landmarks");
		open = new JButton("Open 1st Image");
		open2 = new JButton("Open 2nd Image");
		open.addActionListener(this);
		open2.addActionListener(this);
		makeLandmarks.addActionListener(this);
		Scroll1Listen=false;
		Scroll2Listen=false;
		FixedPicture = new PicturePanel();
		MovingPicture = new PicturePanel();
		final JScrollPane scroll1 = new JScrollPane(FixedPicture);//declares it
		final JScrollPane scroll2 = new JScrollPane(MovingPicture);//declares it
		
		//Landmarks.add(makeLandmarks);
		//Landmarks.add(DisplayedLandmarks);
		
		layout.setHorizontalGroup(
				   layout.createSequentialGroup()
				      .addComponent(scroll1)
				      .addComponent(scroll2)
				      .addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
				           .addComponent(makeLandmarks)
				           .addComponent(open)
				           .addComponent(open2)
				           .addComponent(DisplayedLandmarks))
				);
				layout.setVerticalGroup(
				   layout.createSequentialGroup()
				      .addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
				           .addComponent(scroll1)
				           .addComponent(scroll2)
				           .addComponent(DisplayedLandmarks, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 800, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
				           .addComponent(open)
				           .addComponent(open2)
				           .addComponent(makeLandmarks)
				);

		//holdall.add(scroll1);
		//holdall.add(scroll2);
		//holdall.add(Landmarks);
		this.add(holdall);
		
		
		
		//makeMenu();
		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setVisible(true);
		this.setSize(1600,1000);

	}

	public static void main(String[] args) throws IOException {
		//JFrame JP = new JFrame();
		//JP.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		new LandmarkGenerator(1,2);
		
		if(Scroll1Listen){
			scroll1.addMouseListener(new MouseListener(){

				@Override
				public void mouseClicked(MouseEvent arg0) {
					System.out.println(SwingUtilities.convertPoint(scroll1, arg0.getPoint(), FixedPicture));
				}

				@Override
				public void mouseEntered(MouseEvent arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void mouseExited(MouseEvent arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void mousePressed(MouseEvent arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void mouseReleased(MouseEvent arg0) {
					// TODO Auto-generated method stub
					
				}
				
			});
		}
		
}

	/*private void makeMenu(){
		JMenuBar bar = new JMenuBar();
		JMenu file = new JMenu("File");
		//open = new JMenuItem("Open 1st Image");
		//open2 = new JMenuItem("Open 2nd Image");
		
		file.add(open);
		file.add(open2);
		bar.add(file);
		this.setJMenuBar(bar);
	//this.add(bar);


	}
*/
	public void actionPerformed(ActionEvent e) {
		System.out.println("HELLO");
		if(e.getSource()==open){
			JFileChooser jfc = new JFileChooser();
			int result = jfc.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc.getSelectedFile();
			FixedPicture.setImage(f);
			this.repaint();
		}
		if(e.getSource()==open2){
			JFileChooser jfc = new JFileChooser();
			int result = jfc.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc.getSelectedFile();
			MovingPicture.setImage(f);
			this.repaint();
		}
		if(e.getSource()==makeLandmarks){
			Scroll1Listen = true;
			
		}
	}
}
