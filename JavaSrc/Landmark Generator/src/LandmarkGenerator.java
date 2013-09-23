import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.util.Arrays;

import javax.imageio.ImageIO;
import javax.swing.*;


public class LandmarkGenerator extends JFrame implements ActionListener, MouseListener{

	static PicturePanel FixedPicture;
	static PicturePanel MovingPicture;
	static JButton open,open2,makeLandmarks,CreateFile;
	static JScrollPane scroll1,scroll2;
	int counter;
	static int cposition,rposition;		
	static int [][] PointsforLandmarks=new int[5][4];
	static JEditorPane DisplayedLandmarks = new JEditorPane();
	JPanel holdall=new JPanel();//make panel
	static LandmarkGenerator Generator = new LandmarkGenerator();




	public LandmarkGenerator(){
		super("Landmark Generator");
		this.add(holdall);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setVisible(true);
		this.setSize(1600,1000);

	}

	public static void main(String[] args) throws IOException {



		Generator.Initializer();
		Generator.CreateLayout();


	}

	public void CreateLayout(){
		GroupLayout layout = new GroupLayout(holdall);
		holdall.setLayout(layout);
		layout.setHorizontalGroup(
				layout.createSequentialGroup()
				.addComponent(scroll1,org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 700, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
				.addComponent(scroll2,org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 700, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
						.addComponent(makeLandmarks)
						.addComponent(open)
						.addComponent(open2)
						.addComponent(CreateFile)
						.addComponent(DisplayedLandmarks,org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 200, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
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
						.addComponent(CreateFile)
				);
	}

	public void Initializer(){
		makeLandmarks = new JButton("Create Landmarks");
		open = new JButton("Open 1st Image");
		open2 = new JButton("Open 2nd Image");
		CreateFile = new JButton("Write to File");
		CreateFile.addActionListener(this);
		open.addActionListener(this);
		open2.addActionListener(this);
		makeLandmarks.addActionListener(this);
		counter=0;
		FixedPicture = new PicturePanel();
		MovingPicture = new PicturePanel();
		scroll1 = new JScrollPane(FixedPicture);//declares it
		scroll2 = new JScrollPane(MovingPicture);//declares it
	}

	

	public void actionPerformed(ActionEvent e) {
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
		if(e.getSource()==CreateFile){
			String newfile = JOptionPane.showInputDialog(this  ,"File Name")+".txt";
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
			writer.print(DisplayedLandmarks.getText());
			writer.close();
		
		}
		if(e.getSource()==makeLandmarks){
			//String input = JOptionPane.showInputDialog(this  ,"Landmark number to edit:");
			//int Input = Integer.parseInt(input);
			//if(Input%2==0){Selector(scroll2,Input);}
			//else
			
			scroll1.addMouseListener(this);
			scroll2.addMouseListener(this);

		}
	}

	@Override
	public void mouseClicked(MouseEvent arg0) {
		JScrollPane s = (JScrollPane)(arg0.getSource());

		
		cposition=counter%2;
		if(counter%2!=0){
			cposition=2;
		}
		rposition=counter/2;
		Point P1=new Point();
		
		
		if(counter%2==0&&arg0.getSource()==scroll1){
			P1 = SwingUtilities.convertPoint(s, arg0.getPoint(), FixedPicture);
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
					counter++;
					scroll1.removeMouseListener(this);
					scroll2.removeMouseListener(this);
				}
				else
					if(counter%2!=0&&s==scroll1){
						JOptionPane.showMessageDialog(Generator, "Please choose a point from the Second picture now instead of choosing one from the first picture.", "Selection Error", JOptionPane.PLAIN_MESSAGE);
						return;
					}

		PointsforLandmarks[rposition][cposition] = P1.x;
		PointsforLandmarks[rposition][++cposition] = P1.y;
		DisplayedLandmarks.setText(null);
		for(int i=0;i<PointsforLandmarks.length;i++){
			for(int j=0;j<PointsforLandmarks[i].length;j++){
				DisplayedLandmarks.setText(DisplayedLandmarks.getText() + PointsforLandmarks[i][j] +" ");
			}
			DisplayedLandmarks.setText(DisplayedLandmarks.getText()+"\n");
		}
		return;


			
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
}
