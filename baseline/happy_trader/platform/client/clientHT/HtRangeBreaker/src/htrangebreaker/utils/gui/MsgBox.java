/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils.gui;

import htrangebreaker.*;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Label;
import java.awt.Panel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * @author DanilinS
 */
public class MsgBox extends Dialog implements ActionListener {
	private Button ok,  can;
		public boolean isOk = false;

		/*
		 * @param frame   parent frame
		 * @param msg     message to be displayed
		 * @param okcan   true : ok cancel buttons, false : ok button only
		 */
		public MsgBox(Frame frame, String msg, boolean okcan, boolean modal) {
			super(frame, "Message", true);
			setLayout(new BorderLayout());
			add("Center", new Label(msg));
			addOKCancelPanel(okcan);
			createFrame();
			pack();
			setVisible(true);
		}

		public MsgBox(Frame frame, String msg) {
			this(frame, msg, false, true);
		}

		public MsgBox(String msg)
		{
			this(HtRangeBreakerApp.getApplication().getMainFrame(), msg );
		}

		public static void showMessageBox(String msg)
		{
			MsgBox mb = new MsgBox(msg);
			mb.dispose();
			mb = null;
		}

		public static void showModelessMessageBox(String msg)
		{
			MsgBox mb = new MsgBox(HtRangeBreakerApp.getApplication().getMainFrame(), msg, false, false);
			
		}

		public void addOKCancelPanel(boolean okcan) {
			Panel p = new Panel();
			p.setLayout(new FlowLayout());
			createOKButton(p);
			if (okcan == true) {
				createCancelButton(p);
			}
			add("South", p);
		}

		public void createOKButton(Panel p) {
			p.add(ok = new Button("OK"));
			ok.addActionListener(this);
		}

		public void createCancelButton(Panel p) {
			p.add(can = new Button("Cancel"));
			can.addActionListener(this);
		}

		public void createFrame() {
			Dimension d = getToolkit().getScreenSize();
			setLocation(d.width / 3, d.height / 3);
		}

		public void actionPerformed(ActionEvent ae) {
			if (ae.getSource() == ok) {
				isOk = true;
				setVisible(false);
			} else if (ae.getSource() == can) {
				setVisible(false);
			}
		}
}
