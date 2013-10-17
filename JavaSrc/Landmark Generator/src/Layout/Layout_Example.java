package Layout;

import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.JPanel;
import java.awt.Dimension;
import javax.swing.JScrollPane;
import javax.swing.JButton;
import javax.swing.LayoutStyle.ComponentPlacement;
import java.awt.Component;
import javax.swing.JEditorPane;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.border.LineBorder;
import java.awt.Color;
import javax.swing.border.CompoundBorder;
import javax.swing.UIManager;

public class Layout_Example {

	private JFrame MainWindow;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Layout_Example window = new Layout_Example();
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
	public Layout_Example() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		MainWindow = new JFrame();
		MainWindow.setBounds(100, 100, 1210, 768);
		MainWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		JPanel LeftPanel = new JPanel();
		LeftPanel.setPreferredSize(new Dimension(450, 500));
		
		JPanel RightPanel = new JPanel();
		RightPanel.setPreferredSize(new Dimension(450, 500));
		
		JScrollPane scroll2 = new JScrollPane();
		scroll2.setPreferredSize(new Dimension(500, 600));
		
		JPanel RightControls = new JPanel();
		RightControls.setPreferredSize(new Dimension(450, 10));
		
		JButton open2 = new JButton("Open Image...");
		open2.setAlignmentX(0.5f);
		
		JButton Smaller2 = new JButton("-");
		Smaller2.setAlignmentX(0.5f);
		
		JButton Bigger2 = new JButton("+");
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
		GroupLayout groupLayout = new GroupLayout(MainWindow.getContentPane());
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
		
		JButton CreateFile = new JButton("Save Landmarks File...");
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
		
		JEditorPane DisplayedLandmarks = new JEditorPane();
		DisplayedLandmarks.setBorder(UIManager.getBorder("ScrollPane.border"));
		DisplayedLandmarks.setEditable(false);
		DisplayedLandmarks.setAlignmentY(Component.TOP_ALIGNMENT);
		DisplayedLandmarks.setAlignmentX(Component.LEFT_ALIGNMENT);
		
		JButton makeLandmarks = new JButton("Add Landmark");
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
		
		JScrollPane scroll1 = new JScrollPane();
		scroll1.setPreferredSize(new Dimension(500, 600));
		
		JPanel LeftControls = new JPanel();
		
		JButton open = new JButton("Open Image...");
		open.setAlignmentX(Component.CENTER_ALIGNMENT);
		
		JButton Smaller1 = new JButton("-");
		Smaller1.setAlignmentX(Component.CENTER_ALIGNMENT);
		
		JButton Bigger1 = new JButton("+");
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
		MainWindow.getContentPane().setLayout(groupLayout);
	}
}
