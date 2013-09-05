import javax.imageio.ImageIO;
import javax.swing.*;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;



public class PicturePanel extends JPanel{
	private int w;
	private int h;
	private BufferedImage img;//create the buffered image
	private static final int TYPE = BufferedImage.TYPE_INT_ARGB_PRE;//set the type for later
	
	public PicturePanel (){
		super();//make the outline
		this.setPreferredSize(new Dimension(800,800));//set the dimensions of the gui
		img = new BufferedImage(300,300,TYPE);//make the image a certain size
		w = img.getWidth();//store width in a variable
		h = img.getHeight();//store the height in a variable
	}
	public BufferedImage getImage(){return img;}//return the image
	public void setImage(File input){//set the image
		try{ 
			img = ImageIO.read((input));
			MediaTracker mt = new MediaTracker(new Component(){});
			mt.addImage(img, 0);
			mt.waitForAll();
		}
		catch(Exception ex){ex.printStackTrace();}
		w = img.getWidth();
		h = img.getHeight();
		//pix = imgToArray();
		this.setPreferredSize(new Dimension(img.getWidth(), img.getHeight()));
	}
	
	//override the paint function
	public void paint(Graphics g){
		super.paint(g);
		g.drawImage(img,0,0,null);
	}
	
}
