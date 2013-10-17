package Layout;

import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JScrollPane;

import java.awt.BorderLayout;

import javax.swing.JSplitPane;

import java.awt.GridLayout;

import javax.swing.JEditorPane;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import java.awt.Component;

import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;

public class LandmarkGenerator extends JFrame implements ActionListener, MouseListener{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JFrame MainWindow;
	private JScrollPane scroll1;
	static PicturePanel FixedPicture;
    static PicturePanel MovingPicture;

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
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		MainWindow = new JFrame();
		MainWindow.setResizable(false);
		MainWindow.setBounds(100, 100, 1069, 776);
		MainWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		JPanel UpperPanel = new JPanel();
		
		JSplitPane splitPane = new JSplitPane();
		splitPane.setAlignmentY(Component.CENTER_ALIGNMENT);
		
		scroll1 = new JScrollPane(FixedPicture);
		scroll1.setPreferredSize(new Dimension(450, 550));
		splitPane.setLeftComponent(scroll1);
		
		JScrollPane scroll2 = new JScrollPane(MovingPicture);
		scroll2.setPreferredSize(new Dimension(450, 550));
		splitPane.setRightComponent(scroll2);
	
		
		JEditorPane DisplayedLandmarks = new JEditorPane();
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
		
		JButton CreateFile = new JButton("Save");
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
		open2.setAlignmentX(0.5f);
		
		JButton Smaller2 = new JButton("-");
		Smaller2.setAlignmentX(0.5f);
		
		JButton Bigger2 = new JButton("+");
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
		Smaller1.setAlignmentX(Component.CENTER_ALIGNMENT);
		
		JButton Bigger1 = new JButton("+");
		Bigger1.setAlignmentX(Component.CENTER_ALIGNMENT);
		
		JButton open = new JButton("Open Image...");
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
					.addComponent(splitPane, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
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
						.addComponent(splitPane, Alignment.LEADING))
					.addContainerGap())
		);
		UpperPanel.setLayout(gl_UpperPanel);
		MainWindow.getContentPane().setLayout(groupLayout);
		
	}
	
	public Dimension getScroll1PreferredSize() {
		return scroll1.getPreferredSize();
	}
	public void setScroll1PreferredSize(Dimension preferredSize) {
		scroll1.setPreferredSize(preferredSize);
	}

	@Override
	public void mouseClicked(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
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

	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		
	}
}
