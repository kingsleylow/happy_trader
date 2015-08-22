/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

/**
 *
 * @author DanilinS
 */
public class SingleReaderQueueExceedLimit extends Exception {
	public SingleReaderQueueExceedLimit(String message) {
		super(message);
	}

	private SingleReaderQueueExceedLimit() {
		super();
	}

	private SingleReaderQueueExceedLimit(String message, Throwable cause) {
		super(message, cause);
	}

	private SingleReaderQueueExceedLimit(Throwable cause) {
		super(cause);
	}
}
