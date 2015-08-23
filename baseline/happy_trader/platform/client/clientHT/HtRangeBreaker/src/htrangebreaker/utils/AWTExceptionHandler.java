/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class AWTExceptionHandler implements Thread.UncaughtExceptionHandler {

	public void uncaughtException(Thread t, Throwable e) {
    handle(e);
  }

  public void handle(Throwable throwable) {
    try {
      // insert your e-mail code here
			HtFrameLogger.logError(throwable, "AWT thread exception: " + throwable.toString());
    } catch (Throwable t) {
      // don't let the exception get thrown out, will cause infinite looping!
    }
  }

  public static void registerExceptionHandler() {
    Thread.setDefaultUncaughtExceptionHandler(new AWTExceptionHandler());
    System.setProperty("sun.awt.exception.handler", AWTExceptionHandler.class.getName());
  }


}
