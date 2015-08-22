/*
 * HtException.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.lang.Exception;
import java.lang.Exception;
import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 * Exception clas for this project. Logs any exception that take place
 */
public class HtException extends Exception {

	/** Creates a new instance of HtException */
	// TODO print stack trace
	public HtException(String classname, String methodname, String message) {
		super(message);

		HtLog.log(Level.WARNING, classname, methodname, "Exception happened: " + getMessage());
	}

	public HtException(String classname, String methodname, String message, Object[] params) {
		super(message);

		HtLog.log(Level.WARNING, classname, methodname, "Exception happened: " + getMessage(), params);
	}

	public HtException(String classname, String methodname, Throwable e) {
		super(e);

		HtLog.log(Level.WARNING, classname, methodname, "Exception happened: " + e.getMessage());
		HtLog.log(Level.WARNING, classname, methodname, "Stack trace as follows: " + getErrorStack(e));
	}

	private HtException() {
		super();
	}

	private HtException(String message, Throwable cause) {
		super(message, cause);
	}

	private HtException(Throwable cause) {
		super(cause);
	}

	public static String getFullErrorString(Throwable e) {
		StringBuilder out = new StringBuilder();
		out.append(e.getMessage());

		out.append("\nError type: ");
		out.append(e.getClass().getSimpleName());
		out.append("\n");

		out.append("\nStacktrace: \n");
		out.append(getErrorStack(e));
		return out.toString();
	}

	public static String getErrorStack(Throwable e) {
		StringBuilder out = new StringBuilder();
		StackTraceElement[] stack = e.getStackTrace();
		if (stack != null) {
			out.append("\n");
			for (int i = 0; i < stack.length; i++) {

				String fname = stack[i].getFileName();
				out.append(fname == null? "<NO FILE NAME>":fname);
				out.append("#");
				out.append(stack[i].getLineNumber());
				out.append(" - \"");
				out.append(stack[i].getClassName());
				out.append("@");
				out.append(stack[i].getMethodName());
				out.append("\"");


				if (stack[i].isNativeMethod())
					out.append(" native\n");
				else
					out.append(" non-native\n");

			}
		} else {
			out.append("Stack trace is not available\n");
		}

		return out.toString();
	}
}
