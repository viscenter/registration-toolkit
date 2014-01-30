import javax.imageio.ImageIO;
import javax.swing.*;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;



public class PicturePanel extends JPanel{
	public int w, h,maskcount; 
	private int panelwidth, panelheight;
	private int mask1,mask2,mask3,mask4,mask5;
	public BufferedImage img, original;//create the buffered image
	private static final int TYPE = BufferedImage.TYPE_INT_ARGB;//set the type for later
	
	public PicturePanel (){
		super();//make the outline
		panelwidth=800;
		panelheight=800;
		this.setPreferredSize(new Dimension(panelwidth,panelheight));//set the dimensions of the gui
		img = new BufferedImage(300,300,TYPE);//make the image a certain size
		w = img.getWidth();//store width in a variable
		h = img.getHeight();//store the height in a variable
		this.setOpaque(false);
	}
	public BufferedImage getImage(){return img;}//return the image
	public void setImage(File input){//set the image
		try{ 
			img = ImageIO.read((input));
			original=img;
			MediaTracker mt = new MediaTracker(new Component(){});
			mt.addImage(img, 0);
			mt.waitForAll();
		}
		catch(Exception ex){ex.printStackTrace();}
		w = img.getWidth();
		h = img.getHeight();
		
		maskcount = 1;
		
		//pix = imgToArray();
		this.setPreferredSize(new Dimension(img.getWidth(), img.getHeight()));
	}
	//override the paint function
	public void paint(Graphics g){
		super.paint(g);
		g.drawImage(img,0,0,null);
	}
	
	/*public void applyMaskToAlpha()
	{
	    int width = img.getWidth();
	    int height = img.getHeight();

	    int[] imagePixels = img.getRGB(0, 0, width, height, null, 0, width);
	    int[] maskPixels = mask1.getRGB(0, 0, width, height, null, 0, width);

	    for (int i = 0; i < imagePixels.length; i++)
	    {
	        int color = imagePixels[i] & 0x00ffffff; // Mask preexisting alpha
	        int alpha = maskPixels[i] << 24; // Shift green to alpha
	        imagePixels[i] = color | alpha;
	    }

	    img.setRGB(0, 0, width, height, imagePixels, 0, width);
	}
*/
	public void resize (double scale){
		double newwidth = original.getWidth()*scale;
		double newheight = original.getHeight()*scale;
		BufferedImage resizedImage = new BufferedImage((int)newwidth, (int)newheight, original.getType());
		Graphics2D g = resizedImage.createGraphics();
		g.drawImage(original, 0, 0, (int)newwidth, (int)newheight, null);
		img=resizedImage;
		this.setPreferredSize(new Dimension(img.getWidth(), img.getHeight()));
		this.repaint();
		
	}
	
}
