/*
 * HtVolumeAnalizerApp.java
 */

package com.fin.httraderdevelop;

import com.fin.httrader.eventplugins.volanalizer.MsgBox;
import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;

/**
 * The main class of the application.
 */
public class HtVolumeAnalizerApp extends SingleFrameApplication {

    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {
        show(new HtVolumeAnalizerView(this));
    }

    /**
     * This method is to initialize the specified window by injecting resources.
     * Windows shown in our application come fully initialized from the GUI
     * builder, so this additional configuration is not needed.
     */
    @Override protected void configureWindow(java.awt.Window root) {
    }

    /**
     * A convenient static getter for the application instance.
     * @return the instance of HtVolumeAnalizerApp
     */
    public static HtVolumeAnalizerApp getApplication() {
        return Application.getInstance(HtVolumeAnalizerApp.class);
    }

    /**
     * Main method launching the application.
     */
    public static void main(String[] args) {
			// some data returned from jnlp

			  //new MsgBox(HtVolumeAnalizerApp.getApplication().getMainFrame(), "arg0="+args[0]+" arg1="+args[1]);


        launch(HtVolumeAnalizerApp.class, args);
    }
}
