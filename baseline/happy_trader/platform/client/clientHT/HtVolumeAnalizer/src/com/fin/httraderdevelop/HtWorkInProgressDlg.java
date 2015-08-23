/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * HtWorkInProgressDlg.java
 *
 * Created on 23.07.2010, 13:24:45
 */
package com.fin.httraderdevelop;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.Point;
import org.jdesktop.application.Action;

/**
 *
 * @author DanilinS
 */
public class HtWorkInProgressDlg extends javax.swing.JDialog {

	private HtVolumeAnalizerView view_m =  null;

	/** Creates new form HtWorkInProgressDlg */
	public HtWorkInProgressDlg(java.awt.Frame parent, boolean modal, HtVolumeAnalizerView view) {
		super(parent, modal);
		initComponents();

		view_m = view;
	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jPanel1 = new javax.swing.JPanel();
    jCancel = new javax.swing.JButton();
    jLabel1 = new javax.swing.JLabel();

    setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
    org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(com.fin.httraderdevelop.HtVolumeAnalizerApp.class).getContext().getResourceMap(HtWorkInProgressDlg.class);
    setBackground(resourceMap.getColor("Form.background")); // NOI18N
    setForeground(resourceMap.getColor("Form.foreground")); // NOI18N
    setModalityType(java.awt.Dialog.ModalityType.APPLICATION_MODAL);
    setName("Form"); // NOI18N
    setResizable(false);
    setUndecorated(true);

    jPanel1.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
    jPanel1.setName("jPanel1"); // NOI18N

    javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(com.fin.httraderdevelop.HtVolumeAnalizerApp.class).getContext().getActionMap(HtWorkInProgressDlg.class, this);
    jCancel.setAction(actionMap.get("onCancel")); // NOI18N
    jCancel.setText(resourceMap.getString("jCancel.text")); // NOI18N
    jCancel.setName("jCancel"); // NOI18N

    jLabel1.setFont(resourceMap.getFont("jLabel1.font")); // NOI18N
    jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
    jLabel1.setName("jLabel1"); // NOI18N

    javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGap(78, 78, 78)
            .addComponent(jCancel))
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGap(41, 41, 41)
            .addComponent(jLabel1)))
        .addContainerGap(44, Short.MAX_VALUE))
    );
    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
        .addGap(28, 28, 28)
        .addComponent(jLabel1)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 34, Short.MAX_VALUE)
        .addComponent(jCancel)
        .addContainerGap())
    );

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

	

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton jCancel;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JPanel jPanel1;
  // End of variables declaration//GEN-END:variables
	// centers the dialog within the parent container [1.1]
//  (put that in the Dialog class)
	public void centerParent() {
		int x;
		int y;

		// Find out our parent
		Container myParent = getParent();
		Point topLeft = myParent.getLocationOnScreen();
		Dimension parentSize = myParent.getSize();

		Dimension mySize = getSize();

		if (parentSize.width > mySize.width) {
			x = ((parentSize.width - mySize.width) / 2) + topLeft.x;
		} else {
			x = topLeft.x;
		}

		if (parentSize.height > mySize.height) {
			y = ((parentSize.height - mySize.height) / 2) + topLeft.y;
		} else {
			y = topLeft.y;
		}

		setLocation(x, y);
		super.setVisible(true);
		requestFocus();
	}

	@Action
	public void onCancel() {
		if (view_m != null) {
			
			view_m.onClose();

			this.setVisible( false );
		}
	}
}