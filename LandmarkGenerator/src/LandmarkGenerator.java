import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
//import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

//import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;

// Dylan's UI


public class LandmarkGenerator extends JFrame implements ActionListener, MouseListener{

	static PicturePanel FixedPicture;
	static PicturePanel MovingPicture;
	static PicturePanel RightPanel, LeftPanel;
	static JButton open,open2,makeLandmarks,CreateFile, Bigger1, Bigger2, Smaller1, Smaller2;
	static JScrollPane scroll1,scroll2;
	JPanel RightControls, LeftControls;
	int counter;
	static int cposition,rposition;                
	static double fscale, mscale;
	static int [][] PointsforLandmarks=new int[5][4];
	static JEditorPane DisplayedLandmarks = new JEditorPane();
	static LandmarkGenerator Generator = new LandmarkGenerator();
	int fpx1,fpy1,fpx2,fpy2,fpx3,fpy3,fpx4,fpy4,fpx5,fpy5,mpx1,mpy1,mpx2,mpy2,mpx3,mpy3,mpx4,mpy4,mpx5,mpy5;
	JFileChooser jfc;




	public LandmarkGenerator(){
		super("Landmark Generator");
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setVisible(true);
		this.setSize(1600,1000);

	}

	public static void main(String[] args) throws IOException {



		Generator.Initializer();
		Generator.CreateLayout();


	}

	public void CreateLayout(){

		this.setBounds(100, 100, 1210, 768);

		LeftPanel.setPreferredSize(new Dimension(450, 500));

		RightPanel.setPreferredSize(new Dimension(450, 500));

		scroll2.setPreferredSize(new Dimension(500, 600));

		RightControls.setPreferredSize(new Dimension(450, 10));

		open2.setAlignmentX(0.5f);

		Smaller2.setAlignmentX(0.5f);

		Bigger2.setAlignmentX(0.5f);
		GroupLayout gl_RightControls = new GroupLayout(RightControls);
		gl_RightControls.setHorizontalGroup(
				gl_RightControls.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_RightControls.createSequentialGroup()
						.addGap(173)
						.addGroup(gl_RightControls.createParallelGroup(Alignment.LEADING)
								.addGroup(gl_RightControls.createSequentialGroup()
										.addGap(10)
										.addComponent(open2))
										.addGroup(gl_RightControls.createSequentialGroup()
												.addComponent(Smaller2)
												.addComponent(Bigger2)))
												.addContainerGap(177, Short.MAX_VALUE))
				);
		gl_RightControls.setVerticalGroup(
				gl_RightControls.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_RightControls.createSequentialGroup()
						.addGroup(gl_RightControls.createParallelGroup(Alignment.LEADING)
								.addComponent(Smaller2)
								.addComponent(Bigger2))
								.addPreferredGap(ComponentPlacement.RELATED)
								.addComponent(open2)
								.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
				);
		RightControls.setLayout(gl_RightControls);
		GroupLayout gl_RightPanel = new GroupLayout(RightPanel);
		gl_RightPanel.setHorizontalGroup(
				gl_RightPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_RightPanel.createSequentialGroup()
						.addGroup(gl_RightPanel.createParallelGroup(Alignment.TRAILING, false)
								.addComponent(RightControls, Alignment.LEADING, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
								.addComponent(scroll2, Alignment.LEADING, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
								.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
				);
		gl_RightPanel.setVerticalGroup(
				gl_RightPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_RightPanel.createSequentialGroup()
						.addComponent(scroll2, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(ComponentPlacement.RELATED, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
						.addComponent(RightControls, GroupLayout.PREFERRED_SIZE, 67, GroupLayout.PREFERRED_SIZE))
				);
		RightPanel.setLayout(gl_RightPanel);

		JPanel LandmarksPanel = new JPanel();
		LandmarksPanel.setAlignmentY(Component.TOP_ALIGNMENT);
		LandmarksPanel.setAlignmentX(Component.LEFT_ALIGNMENT);

		JPanel ProgramControlsPanel = new JPanel();
		GroupLayout groupLayout = new GroupLayout(this.getContentPane());
		groupLayout.setHorizontalGroup(
				groupLayout.createParallelGroup(Alignment.LEADING)
				.addGroup(groupLayout.createSequentialGroup()
						.addContainerGap()
						.addGroup(groupLayout.createParallelGroup(Alignment.LEADING)
								.addComponent(ProgramControlsPanel, GroupLayout.DEFAULT_SIZE, 1198, Short.MAX_VALUE)
								.addGroup(groupLayout.createSequentialGroup()
										.addComponent(LeftPanel, GroupLayout.PREFERRED_SIZE, 500, GroupLayout.PREFERRED_SIZE)
										.addPreferredGap(ComponentPlacement.RELATED)
										.addComponent(RightPanel, GroupLayout.PREFERRED_SIZE, 500, GroupLayout.PREFERRED_SIZE)
										.addPreferredGap(ComponentPlacement.RELATED)
										.addComponent(LandmarksPanel, GroupLayout.PREFERRED_SIZE, 186, GroupLayout.PREFERRED_SIZE)))
										.addContainerGap())
				);
		groupLayout.setVerticalGroup(
				groupLayout.createParallelGroup(Alignment.LEADING)
				.addGroup(groupLayout.createSequentialGroup()
						.addContainerGap()
						.addGroup(groupLayout.createParallelGroup(Alignment.LEADING, false)
								.addComponent(RightPanel, GroupLayout.DEFAULT_SIZE, 673, Short.MAX_VALUE)
								.addComponent(LeftPanel, GroupLayout.PREFERRED_SIZE, 673, Short.MAX_VALUE)
								.addComponent(LandmarksPanel, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
								.addPreferredGap(ComponentPlacement.RELATED)
								.addComponent(ProgramControlsPanel, GroupLayout.DEFAULT_SIZE, 55, Short.MAX_VALUE)
								.addContainerGap())
				);

		GroupLayout gl_ProgramControlsPanel = new GroupLayout(ProgramControlsPanel);
		gl_ProgramControlsPanel.setHorizontalGroup(
				gl_ProgramControlsPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(Alignment.TRAILING, gl_ProgramControlsPanel.createSequentialGroup()
						.addContainerGap(1015, Short.MAX_VALUE)
						.addComponent(CreateFile))
				);
		gl_ProgramControlsPanel.setVerticalGroup(
				gl_ProgramControlsPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_ProgramControlsPanel.createSequentialGroup()
						.addContainerGap(26, Short.MAX_VALUE)
						.addComponent(CreateFile))
				);
		ProgramControlsPanel.setLayout(gl_ProgramControlsPanel);

		DisplayedLandmarks = new JEditorPane();
		DisplayedLandmarks.setBorder(UIManager.getBorder("ScrollPane.border"));
		DisplayedLandmarks.setEditable(false);
		DisplayedLandmarks.setAlignmentY(Component.TOP_ALIGNMENT);
		DisplayedLandmarks.setAlignmentX(Component.LEFT_ALIGNMENT);

		GroupLayout gl_LandmarksPanel = new GroupLayout(LandmarksPanel);
		gl_LandmarksPanel.setHorizontalGroup(
				gl_LandmarksPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LandmarksPanel.createSequentialGroup()
						.addGroup(gl_LandmarksPanel.createParallelGroup(Alignment.LEADING)
								.addComponent(DisplayedLandmarks, GroupLayout.PREFERRED_SIZE, 186, GroupLayout.PREFERRED_SIZE)
								.addComponent(makeLandmarks, GroupLayout.PREFERRED_SIZE, 186, GroupLayout.PREFERRED_SIZE))
								.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
				);
		gl_LandmarksPanel.setVerticalGroup(
				gl_LandmarksPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LandmarksPanel.createSequentialGroup()
						.addComponent(DisplayedLandmarks, GroupLayout.PREFERRED_SIZE, 598, GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(ComponentPlacement.RELATED)
						.addComponent(makeLandmarks)
						.addContainerGap(40, Short.MAX_VALUE))
				);
		LandmarksPanel.setLayout(gl_LandmarksPanel);

		scroll1.setPreferredSize(new Dimension(500, 600));


		open.setAlignmentX(Component.CENTER_ALIGNMENT);

		Smaller1.setAlignmentX(Component.CENTER_ALIGNMENT);

		Bigger1.setAlignmentX(Component.CENTER_ALIGNMENT);
		GroupLayout gl_LeftControls = new GroupLayout(LeftControls);
		gl_LeftControls.setHorizontalGroup(
				gl_LeftControls.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LeftControls.createSequentialGroup()
						.addGap(172)
						.addGroup(gl_LeftControls.createParallelGroup(Alignment.LEADING)
								.addGroup(gl_LeftControls.createSequentialGroup()
										.addGap(10)
										.addComponent(open))
										.addGroup(gl_LeftControls.createSequentialGroup()
												.addComponent(Smaller1)
												.addComponent(Bigger1)))
												.addContainerGap(178, Short.MAX_VALUE))
				);
		gl_LeftControls.setVerticalGroup(
				gl_LeftControls.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LeftControls.createSequentialGroup()
						.addGroup(gl_LeftControls.createParallelGroup(Alignment.LEADING)
								.addComponent(Smaller1)
								.addComponent(Bigger1))
								.addPreferredGap(ComponentPlacement.RELATED)
								.addComponent(open)
								.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
				);
		LeftControls.setLayout(gl_LeftControls);
		GroupLayout gl_LeftPanel = new GroupLayout(LeftPanel);
		gl_LeftPanel.setHorizontalGroup(
				gl_LeftPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LeftPanel.createSequentialGroup()
						.addGroup(gl_LeftPanel.createParallelGroup(Alignment.TRAILING, false)
								.addComponent(LeftControls, Alignment.LEADING, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
								.addComponent(scroll1, Alignment.LEADING, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
								.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
				);
		gl_LeftPanel.setVerticalGroup(
				gl_LeftPanel.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_LeftPanel.createSequentialGroup()
						.addComponent(scroll1, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addPreferredGap(ComponentPlacement.RELATED, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
						.addComponent(LeftControls, GroupLayout.PREFERRED_SIZE, 67, GroupLayout.PREFERRED_SIZE)
						.addContainerGap())
				);
		LeftPanel.setLayout(gl_LeftPanel);
		this.getContentPane().setLayout(groupLayout);
		CreateFile.addActionListener(this);
		open.addActionListener(this);
		open2.addActionListener(this);
		makeLandmarks.addActionListener(this);
		Bigger1.addActionListener(this);
		Bigger2.addActionListener(this);
		Smaller1.addActionListener(this);
		Smaller2.addActionListener(this);
	}

	public void Initializer(){
		this.setResizable(false);
		makeLandmarks = new JButton("Create Landmarks");
		open = new JButton("Open 1st Image");
		open2 = new JButton("Open 2nd Image");
		RightControls = new JPanel();
		LeftControls = new JPanel();
		CreateFile = new JButton("Write to File");
		Bigger1 = new JButton("+");
		Bigger2 = new JButton("+");
		Smaller1 = new JButton("-");
		Smaller2 = new JButton("-");
		counter=0;
		fscale = 1;
		mscale = 1;
		FixedPicture = new PicturePanel();
		MovingPicture = new PicturePanel();
		LeftPanel = new PicturePanel();
		RightPanel = new PicturePanel();
		scroll1 = new JScrollPane(FixedPicture);//declares it
		scroll2 = new JScrollPane(MovingPicture);//declares it
		jfc = new JFileChooser();
	}

	public void setFixedZoom(){
boolean canzoom = true;
		if(FixedPicture.img.getWidth()<=scroll1.getWidth()&&FixedPicture.img.getHeight()<=scroll1.getHeight()){
			while(canzoom){

				fscale*=1.1;

				if(FixedPicture.isresizable(fscale,scroll1.getSize())){
					FixedPicture.resize(fscale);}
				else
				{fscale/=1.1;
				canzoom = false;
				Bigger1.setEnabled(false);
				}
			}
		}
		else{
			while(canzoom){
				fscale/=1.1;

				if(FixedPicture.isresizable(fscale,scroll1.getSize())){
					FixedPicture.resize(fscale);}
				else
				{fscale*=1.1;
				canzoom= false;
				Smaller1.setEnabled(false);
				}
			}
		}
	}
	public void setMovingZoom(){
boolean canzoom = true;
		if(MovingPicture.img.getWidth()<=scroll2.getWidth()&&MovingPicture.img.getHeight()<=scroll2.getHeight()){
			while(canzoom){
				mscale*=1.1;

				if(MovingPicture.isresizable(mscale, scroll2.getSize())){
					MovingPicture.resize(mscale);}
				else
				{mscale/=1.1;
				canzoom=false;
				Bigger2.setEnabled(false);
				}
			}
		}
		else{
			while(canzoom){
				mscale/=1.1;

				if(MovingPicture.isresizable(mscale, scroll2.getSize())){
					MovingPicture.resize(mscale);}
				else
				{mscale*=1.1;
				canzoom=false;
				Smaller2.setEnabled(false);
				}
			}
		}
	}

	public void actionPerformed(ActionEvent e) {
		if(e.getSource()==open){
			fscale=1;
			int result = jfc.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc.getSelectedFile();
			FixedPicture.setImage(f);
			this.setFixedZoom();
			this.repaint();
			Bigger1.setEnabled(true);
			Smaller1.setEnabled(true);
		}
		if(e.getSource()==open2){
			mscale=1;
			int result = jfc.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc.getSelectedFile();
			MovingPicture.setImage(f);
			this.setMovingZoom();
			this.repaint();
			Bigger2.setEnabled(true);
			Smaller2.setEnabled(true);
		}
		if(e.getSource()==CreateFile){
			int result = jfc.showSaveDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File newfile = jfc.getSelectedFile();
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

			scroll1.addMouseListener(this);
			scroll2.addMouseListener(this);

		}
		if(e.getSource()==Bigger1){

			fscale*=1.1;

			if(FixedPicture.isresizable(fscale,scroll1.getSize())){
				FixedPicture.resize(fscale);}
			else
			{fscale/=1.1;
			Bigger1.setEnabled(false);}
			
			if(!Smaller1.isEnabled()){Smaller1.setEnabled(true);}

		}

		if(e.getSource()==Bigger2){
			mscale*=1.1;

			if(MovingPicture.isresizable(mscale, scroll2.getSize())){
				MovingPicture.resize(mscale);}
			else
			{mscale/=1.1;
			Bigger2.setEnabled(false);}
			
			if(!Smaller2.isEnabled()){Smaller2.setEnabled(true);}

		}
		if(e.getSource()==Smaller1){
			fscale/=1.1;

			if(FixedPicture.isresizable(fscale,scroll1.getSize())){
				FixedPicture.resize(fscale);}
			else
				{fscale*=1.1;
				Smaller1.setEnabled(false);}
			
			if(!Bigger1.isEnabled()){Bigger1.setEnabled(true);}

		}

		if(e.getSource()==Smaller2){
			mscale/=1.1;

			if(MovingPicture.isresizable(mscale, scroll2.getSize())){
				MovingPicture.resize(mscale);}
			else
				{mscale*=1.1;
				Smaller2.setEnabled(false);}
			
			if(!Bigger2.isEnabled()){Bigger2.setEnabled(true);}

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
					scroll1.removeMouseListener(this);
					scroll2.removeMouseListener(this);
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
