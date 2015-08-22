/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;



/**
 *
 * @author DanilinS
 */
public class HtFrameException extends Exception {
	public HtFrameException(String message)
	{
		super(message);
		HtFrameLogger.logError(message);
	}
}
