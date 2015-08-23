/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

import com.fin.httrader.utils.HtDateTimeUtils;
import htrangebreaker.HtInternalLogWindow;


/**
 *
 * @author DanilinS
 */
public class HtFrameLogger {
	public static void log(String msg)
	{
		StringBuilder entry = new StringBuilder();
		entry.append(HtDateTimeUtils.time2SimpleString_Local(HtDateTimeUtils.getCurGmtTime()));
		entry.append(" - [ ");
		entry.append(Thread.currentThread().getId());
		entry.append(" ] - ");
		entry.append(msg);

		HtInternalLogWindow.instance().addEntry(entry.toString());
		
	}

	public static void logError(String msg)
	{
		StringBuilder entry = new StringBuilder();
		
		entry.append(HtDateTimeUtils.time2SimpleString_Local(HtDateTimeUtils.getCurGmtTime()));
		entry.append(" - [ ");
		entry.append(Thread.currentThread().getId());
		entry.append(" ] - ERROR: ");
		entry.append(msg);

		HtInternalLogWindow.instance().addErrorEntry(entry.toString());

	}

	public static void logError(Throwable e, String msg)
	{
		StringBuilder entry = new StringBuilder();

		entry.append(HtDateTimeUtils.time2SimpleString_Local(HtDateTimeUtils.getCurGmtTime()));
		entry.append(" - [ ");
		entry.append(Thread.currentThread().getId());
		entry.append(" ] - ERROR: ");
		entry.append(msg);
		entry.append("\nStack trace:\n");

		StackTraceElement[] stack = e.getStackTrace();
		if (stack != null) {
			for (int i = 0; i < stack.length; i++) {

				String fname = stack[i].getFileName();
				entry.append(fname == null? "<NO FILE NAME>":fname);
				entry.append("#");
				entry.append(stack[i].getLineNumber());
				entry.append(" - \"");
				entry.append(stack[i].getClassName());
				entry.append("@");
				entry.append(stack[i].getMethodName());
				entry.append("\"");

				
				if (stack[i].isNativeMethod()) {
					entry.append(" native");
				}
				else {
					entry.append(" non-native");
				}

				entry.append("\n");
			}
		} else {
			entry.append("Stack trace is not available\n");
		}

		HtInternalLogWindow.instance().addErrorEntry(entry.toString());

	}

	public static void logError(Throwable e)
	{
		logError(e, e.getMessage());
	}

}
