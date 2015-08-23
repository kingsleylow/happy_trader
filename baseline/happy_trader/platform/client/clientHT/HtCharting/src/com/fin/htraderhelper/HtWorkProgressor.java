/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import javax.swing.JProgressBar;
import javax.swing.SwingWorker;

/**
 *
 * @author DanilinS
 */
public abstract class HtWorkProgressor extends SwingWorker<Void, Void> implements PropertyChangeListener {


	private JProgressBar bar_m = null;

	HtWorkProgressor(JProgressBar bar)
	{
		super();
		bar_m = bar;
	}

	public void setPctValue(int pct) {
		this.setProgress(pct);
	}

	public void propertyChange(PropertyChangeEvent evt) {
		if ("progress" == evt.getPropertyName()) {

			if (bar_m != null) {
				int progress = (Integer) evt.getNewValue();
				bar_m.setValue(progress);
			}
		}
	}
}
