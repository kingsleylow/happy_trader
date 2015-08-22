/*
 * HtChartingApp.java
 */
package com.fin.htraderhelper;

import com.fin.htraderhelper.utils.NonblockingBufferedReader;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;

/**
 * The main class of the application.
 */
public class HtChartingApp extends SingleFrameApplication {

	private static String[] args_m = null;

	/**
	 * At startup create and show the main frame of the application.
	 */
	@Override
	protected void startup() {
		show(new HtChartingView(this, args_m));
	}

	/**
	 * This method is to initialize the specified window by injecting resources.
	 * Windows shown in our application come fully initialized from the GUI
	 * builder, so this additional configuration is not needed.
	 */
	@Override
	protected void configureWindow(java.awt.Window root) {
	}

	/**
	 * A convenient static getter for the application instance.
	 * @return the instance of HtChartingApp
	 */
	public static HtChartingApp getApplication() {
		return Application.getInstance(HtChartingApp.class);
	}

	/**
	 * Main method launching the application.
	 */
	public static void main(String[] args) {
		args_m = args;
		launch(HtChartingApp.class, args_m);

	}
}
