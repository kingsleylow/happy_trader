/*
 * HtExtException.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtException extends Exception {

	public HtExtException(String message) {
		super(message);
	}

	public HtExtException() {
		super();
	}

	public HtExtException(String message, Throwable cause) {
		super(message, cause);
	}

	public HtExtException(Throwable cause) {
		super(cause);
	}
}
