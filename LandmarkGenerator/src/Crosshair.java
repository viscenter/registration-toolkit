import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;

import javax.imageio.ImageIO;


public class Crosshair extends BufferedImage{
	int width, height;
	
	public  Crosshair(double scale){
		super((int)(scale*200),(int)(scale*200),BufferedImage.TYPE_INT_ARGB);
		initialize();
		try{ 
			BufferedImage temp = ImageIO.read(new File("crosshair.png"));
			Graphics g = this.getGraphics();
			g.drawImage(temp, 0, 0, (int)(scale*200), (int)(scale*200), null, null);
			g.dispose();
		}
		catch(Exception ex){ex.printStackTrace();}
		
		
	}
	public void initialize(){
	width= getWidth();
	height = getHeight();}

	public int[] GetARGBArray(){
		int [] rgbArray = this.getRGB(0, 0, width, height, null, 0, width);
		return rgbArray;
			
	}
}
