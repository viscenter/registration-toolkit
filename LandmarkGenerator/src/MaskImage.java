import java.awt.Component;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.MediaTracker;
import java.awt.image.BufferedImage;
import java.io.File;

import javax.imageio.ImageIO;
import javax.swing.JPanel;


public class MaskImage extends BufferedImage{
	public double scale;
	public BufferedImage img;//create the buffered image
	public MaskImage (double w, double h){
		super((int)w,(int)h,BufferedImage.TYPE_INT_ARGB);
		scale = w/4000;
		FillMask();
	}
public void FillMask(){			
	Graphics g = this.getGraphics();
	g.setColor(Color.BLACK);
	g.fillRect(0, 0, getWidth(), getHeight());
}
public void addCrosshair(int x,int y){
		/*Crosshair cross = new Crosshair(scale);
		int[] ARGBArray = cross.GetARGBArray();
		int startx = x - (cross.getWidth()/2);
		int starty = y - (cross.getHeight()/2);
		this.setRGB(startx, starty, cross.getWidth(), cross.getHeight(), ARGBArray, 0, cross.getWidth());
		*/
	Color myWhite = new Color(255, 255, 255); // Color white
	int rgb=myWhite.getRGB();
	this.setRGB(x, y, rgb);
	}
}


	