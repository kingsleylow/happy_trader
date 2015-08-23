/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.win32;

import com.fin.httrader.utils.HtException;

/**
 *
 * @author DanilinS
 */
public class HtQuikListener {

	ListenerInterface listener_m = null;

	public static interface ListenerInterface
	{
		public void onQuikTickArrived(int hour, int minute, int second, String symbol, String operation, double price, double volume);

		public void onErrorArrived(String error);

	}

	public HtQuikListener(ListenerInterface listener) throws Exception
	{
		listener_m = listener;
		boolean result = init();

		if (!result) {
			throw new HtException("HtQuikListener", "HtQuikListener", "Can't initialize");
		}
	}

	// ---------------------------

	private native boolean init();

	public native void destroy();

	// ---------------------------

	public void onQuikTickArrived(int hour, int minute, int second, String symbol, String operation, double price, double volume)
	{
		listener_m.onQuikTickArrived(hour, minute, second, symbol, operation, price, volume);
	}

	public void onErrorArrived(String error)
	{
		listener_m.onErrorArrived(error);
	}

};
