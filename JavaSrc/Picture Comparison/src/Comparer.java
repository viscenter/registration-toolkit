import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;


public class Comparer {
	BufferedImage Fixed, Warped;
	public Comparer(String fixedname, String warpedname)
	{
		BufferedImage Fixed = new BufferedImage(0, 0, BufferedImage.TYPE_INT_ARGB);
		BufferedImage Warped = new BufferedImage(0, 0, BufferedImage.TYPE_INT_ARGB);
		try {
			Fixed = ImageIO.read(new File(fixedname));
			Warped = ImageIO.read(new File(warpedname));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}	
	public int compare(){
		double score = 0;
		int fixedx=-1,fixedy=-1,warpedx=-1,warpedy=-1;
		while(fixedx==-1&warpedx==-1){
			for(int r = 0; r<Fixed.getHeight();r++){
				for(int c = 0; c<Fixed.getWidth();c++){
					Color FixedColor = new Color(Fixed.getRGB(c, r), true);
					Color WarpedColor = new Color(Warped.getRGB(c, r), true);
				
					if(FixedColor.getRed()!=0){
						fixedx= c;
						fixedy= r;
						}
						
					if(WarpedColor.getRed()!=0){
						warpedx= c;
						warpedy= r;
						}
				
				}
			
			}
		
		}
		score= (warpedx-fixedx)-(warpedy-fixedy);
		score = Math.sqrt(score);
		return (int) score;}
}
