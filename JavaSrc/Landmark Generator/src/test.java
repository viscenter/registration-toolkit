import java.awt.*;
import javax.swing.*;


public class test extends JFrame{

	public static void main(String[] args) {
	    JPanel jPanel1 = new javax.swing.JPanel();
	    JPanel jPanel2 = new javax.swing.JPanel();		
	    JScrollPane jScrollPane1 = new javax.swing.JScrollPane();
	    JScrollPane jScrollPane2 = new javax.swing.JScrollPane();
	    JScrollPane jScrollPane3 = new javax.swing.JScrollPane();
	    JEditorPane jEditorPane1 = new javax.swing.JEditorPane();
	    JButton jButton1 = new javax.swing.JButton();
	    JButton jButton2 = new javax.swing.JButton();
	    JButton jButton3 = new javax.swing.JButton();
		initComponents();
	}
	 void setDefaultCloseOperation(javax.swing.JFrame.EXIT_ON_CLOSE);

	 private static void initComponents() {
	

   

    org.jdesktop.layout.GroupLayout jPanel2Layout = new org.jdesktop.layout.GroupLayout(jPanel2);
    jPanel2.setLayout(jPanel2Layout);
    jPanel2Layout.setHorizontalGroup(
        jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(0, 137, Short.MAX_VALUE)
    );
    jPanel2Layout.setVerticalGroup(
        jPanel2Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(0, 311, Short.MAX_VALUE)
    );

    jScrollPane1.setViewportView(jPanel2);

    org.jdesktop.layout.GroupLayout jPanel1Layout = new org.jdesktop.layout.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
        jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(0, 151, Short.MAX_VALUE)
    );
    jPanel1Layout.setVerticalGroup(
        jPanel1Layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(0, 311, Short.MAX_VALUE)
    );


    jButton1.setText("jButton1");

    jButton2.setText("jButton2");

    jButton3.setText("jButton3");

    org.jdesktop.layout.GroupLayout layout = new org.jdesktop.layout.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
        layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(layout.createSequentialGroup()
            .add(jScrollPane1, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 141, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jScrollPane2, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 155, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
                .add(jScrollPane3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE)
                .add(jButton1)
                .add(jButton2)
                .add(jButton3))
            .addContainerGap())
    );
    layout.setVerticalGroup(
        layout.createParallelGroup(org.jdesktop.layout.GroupLayout.LEADING)
        .add(jScrollPane1)
        .add(jScrollPane2)
        .add(org.jdesktop.layout.GroupLayout.TRAILING, layout.createSequentialGroup()
            .addContainerGap()
            .add(jButton1)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED)
            .add(jButton2)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.RELATED, org.jdesktop.layout.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .add(jButton3)
            .addPreferredGap(org.jdesktop.layout.LayoutStyle.UNRELATED)
            .add(jScrollPane3, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE, 198, org.jdesktop.layout.GroupLayout.PREFERRED_SIZE))
    );

    pack();
}

}
