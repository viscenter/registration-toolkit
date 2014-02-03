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
import java.util.Iterator;

import javax.imageio.stream.ImageInputStream;
import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.metadata.IIOMetadata;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import javax.swing.*;

// Dylan's UI

public class LandmarkGenerator extends JFrame implements ActionListener, MouseListener{

	static PicturePanel FixedPicture;
	static PicturePanel MovingPicture;
	static JButton open,open2,makeLandmarks,CreateFile, Bigger1, Bigger2, Smaller1, Smaller2;
	static JScrollPane scroll1,scroll2;
	int counter;
	static int cposition,rposition;		
	static double fscale, mscale;
	static int [][] PointsforLandmarks=new int[5][4];
	static JEditorPane DisplayedLandmarks = new JEditorPane();
	JPanel holdall=new JPanel();//make panel
	static LandmarkGenerator Generator = new LandmarkGenerator();
	int fpx1,fpy1,fpx2,fpy2,fpx3,fpy3,fpx4,fpy4,fpx5,fpy5,mpx1,mpy1,mpx2,mpy2,mpx3,mpy3,mpx4,mpy4,mpx5,mpy5;
	int fixedDPIx, fixedDPIy, movingDPIx, movingDPIy;




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
						.addComponent(Bigger1)//zoom in on FixedPicture
						.addComponent(Bigger2)//zoom in on MovingPicture
						.addComponent(Smaller1)//zoom out on FixedPicture
						.addComponent(Smaller2)//zoom out on MovingPicture
						.addComponent(DisplayedLandmarks,org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 200, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
				);
		layout.setVerticalGroup(
				layout.createSequentialGroup()
				.addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
						.addComponent(scroll1)
						.addComponent(scroll2)
						.addComponent(DisplayedLandmarks, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 600, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
						.addComponent(open)
						.addComponent(open2)
						.addComponent(Bigger1)
						.addComponent(Bigger2)
						.addComponent(Smaller1)
						.addComponent(Smaller2)
						.addComponent(makeLandmarks)
						.addComponent(CreateFile)
				);
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

	public static final double INCH_PER_MM = 25.45d;
	
	public double[] getDPI(File imageFile) throws IOException {

        double[] dpi = new double[]{72, 72};

        ImageInputStream iis = null;
        try {
            iis = ImageIO.createImageInputStream(imageFile);
            Iterator<ImageReader> readers = ImageIO.getImageReaders(iis);
            if (!readers.hasNext()) {
                throw new IOException("Bad format, no readers");
            }
            ImageReader reader = readers.next();
            reader.setInput(iis);
            IIOMetadata meta = reader.getImageMetadata(0);

            Node root = meta.getAsTree("javax_imageio_1.0");
            NodeList nl = root.getChildNodes();
            float horizontalPixelSize = 0;
            float verticalPixelSize = 0;
            for (int index = 0; index < nl.getLength(); index++) {
                Node child = nl.item(index);
                if ("Dimension".equals(child.getNodeName())) {
                    NodeList dnl = child.getChildNodes();
                    for (int inner = 0; inner < dnl.getLength(); inner++) {
                        child = dnl.item(inner);
                        if ("HorizontalPixelSize".equals(child.getNodeName())) {
                            horizontalPixelSize = Float.parseFloat(child.getAttributes().getNamedItem("value").getNodeValue());
                        } else if ("VerticalPixelSize".equals(child.getNodeName())) {
                            verticalPixelSize = Float.parseFloat(child.getAttributes().getNamedItem("value").getNodeValue());
                        }
                    }
                }
            }

            dpi = new double[]{(INCH_PER_MM / horizontalPixelSize), (INCH_PER_MM / verticalPixelSize)};
        } finally {
            try {
                iis.close();
            } catch (Exception e) {
            }
        }

        return dpi;
    }

	public void actionPerformed(ActionEvent e) {
		if(e.getSource()==open){
			JFileChooser jfc = new JFileChooser();
			int result = jfc.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc.getSelectedFile();
			FixedPicture.setImage(f);
			// fixedDPIx = f.getDPIx();
			this.repaint();
		}
		if(e.getSource()==open2){
			JFileChooser jfc2 = new JFileChooser();
			int result = jfc2.showOpenDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File f = jfc2.getSelectedFile();
			MovingPicture.setImage(f);
			this.repaint();
		}
		if(e.getSource()==CreateFile){
			JFileChooser jfc3 = new JFileChooser();
			int result = jfc3.showSaveDialog(this);
			if(result == JFileChooser.CANCEL_OPTION)
				return;
			File newfile = jfc3.getSelectedFile();
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
			try {
			    // retrieve image
			    MaskImage f1 = new MaskImage(FixedPicture.img.getWidth(),FixedPicture.img.getHeight());
			    f1.addCrosshair(fpx1, fpy1);
			    File outputfile = new File(newfile.getParent() + "/FMask1.png");
			    ImageIO.write(f1, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage f2 = new MaskImage(FixedPicture.img.getWidth(),FixedPicture.img.getHeight());
			    f2.addCrosshair(fpx2, fpy2);
			    File outputfile = new File(newfile.getParent()+"/FMask2.png");
			    ImageIO.write(f2, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage f3 = new MaskImage(FixedPicture.img.getWidth(),FixedPicture.img.getHeight());
			    f3.addCrosshair(fpx3, fpy3);
			    File outputfile = new File(newfile.getParent()+"/FMask3.png");
			    ImageIO.write(f3, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage f4 = new MaskImage(FixedPicture.img.getWidth(),FixedPicture.img.getHeight());
			    f4.addCrosshair(fpx4, fpy4);
			    File outputfile = new File(newfile.getParent()+"/FMask4.png");
			    ImageIO.write(f4, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage f5 = new MaskImage(FixedPicture.img.getWidth(),FixedPicture.img.getHeight());
			    f5.addCrosshair(fpx5, fpy5);
			    File outputfile = new File(newfile.getParent()+"/FMask5.png");
			    ImageIO.write(f5, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage m1 = new MaskImage(MovingPicture.img.getWidth(),MovingPicture.img.getHeight());
			    m1.addCrosshair(mpx1, mpy1);
			    File outputfile = new File(newfile.getParent()+"/MMask1.png");
			    ImageIO.write(m1, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage m2 = new MaskImage(MovingPicture.img.getWidth(),MovingPicture.img.getHeight());
				m2.addCrosshair(mpx2, mpy2);
				File outputfile = new File(newfile.getParent()+"/MMask2.png");
			    ImageIO.write(m2, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage m3 = new MaskImage(MovingPicture.img.getWidth(),MovingPicture.img.getHeight());
				m3.addCrosshair(mpx3, mpy3);
				File outputfile = new File(newfile.getParent()+"/MMask3.png");
			    ImageIO.write(m3, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage m4 = new MaskImage(MovingPicture.img.getWidth(),MovingPicture.img.getHeight());
				m4.addCrosshair(mpx4, mpy4);
				File outputfile = new File(newfile.getParent()+"/MMask4.png");
			    ImageIO.write(m4, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
				MaskImage m5 = new MaskImage(MovingPicture.img.getWidth(),MovingPicture.img.getHeight());
				m5.addCrosshair(mpx5, mpy5);
				File outputfile = new File(newfile.getParent()+"/MMask5.png");
			    ImageIO.write(m5, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
			try {
			    // retrieve image
			    BufferedImage b = FixedPicture.img;
			    File outputfile = new File("DisplayImage.png");
			    ImageIO.write(b, "png", outputfile);
			} catch(IOException e1){e1.printStackTrace();}
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
		if(e.getSource()==Bigger1){
			fscale*=1.1;
			FixedPicture.resize(fscale);
			
		}
			
		if(e.getSource()==Bigger2){
			mscale*=1.1;
			MovingPicture.resize(mscale);}
		if(e.getSource()==Smaller1){
			fscale/=1.1;
			FixedPicture.resize(fscale);
		}
			
		if(e.getSource()==Smaller2){
			mscale/=1.1;
			MovingPicture.resize(mscale);
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
			if(counter==0){
				fpx1=(int) (P1.x/fscale);
				fpy1=(int) (P1.y/fscale);
			}
			if(counter==2){
				fpx2=(int) (P1.x/fscale);
				fpy2=(int) (P1.y/fscale);
			}
			if(counter==4){
				fpx3=(int) (P1.x/fscale);
				fpy3=(int) (P1.y/fscale);
			}
			if(counter==6){
				fpx4=(int) (P1.x/fscale);
				fpy4=(int) (P1.y/fscale);
			}
			if(counter==8){
				fpx5=(int) (P1.x/fscale);
				fpy5=(int) (P1.y/fscale);
			}
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
					if(counter==1){
						mpx1=(int) (P1.x/fscale);
						mpy1=(int) (P1.y/fscale);
					}
					if(counter==3){
						mpx2=(int) (P1.x/fscale);
						mpy2=(int) (P1.y/fscale);
					}
					if(counter==5){
						mpx3=(int) (P1.x/fscale);
						mpy3=(int) (P1.y/fscale);
					}
					if(counter==7){
						mpx4=(int) (P1.x/fscale);
						mpy4=(int) (P1.y/fscale);
					}
					if(counter==9){
						mpx5=(int) (P1.x/fscale);
						mpy5=(int) (P1.y/fscale);
					}

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
