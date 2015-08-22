/*
 * HtDDEServer.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.win32;

import com.fin.httrader.utils.win32.HtSysPowerEventListener;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtSystem;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtDDEServer {

	private HtSysPowerEventListener listener_m = null;

	/** Creates a new instance of HtPowerEventDetector */
	public HtDDEServer(HtSysPowerEventListener listener) throws Exception {
		listener_m = listener;
		//System.load(libPathName);
		boolean result = init();

		if (!result) {
			throw new HtException("HtDDEServer", "HtDDEServer", "Can't initialize");
		}

		if (listener_m == null) {
			throw new HtException("HtDDEServer", "HtDDEServer", "Listener is invalid");
		}
	}

	

	// --------------------------
	private native boolean init();

	public native void destroy();

	// -----------------------------------
	public native boolean initialize(String application, Vector<DDEItem> itemList);

	public native void  uninitialize2();
	// -----------------------------------

	public void onDDEXLTableDataArrived(String topic, String item, Vector<Vector<String>> data) {
		DDEItem ddeitem = new DDEItem(topic, item);
		listener_m.onDDEXLTableDataArrived(ddeitem, data);

	}

	public void onDDEDataArrived(String topic, String item, String value) {
		DDEItem ddeitem = new DDEItem(topic, item);
		listener_m.DDEDataArrived(ddeitem, value);
	}
}
