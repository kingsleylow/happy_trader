/*
 * BrokerConnect.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.broker;

import com.fin.httrader.model.HtServerProxy;
import com.fin.httrader.utils.Uid;

/**
 *
 * @author Victor_Zoubok
 * This class serves as a proxy to work with CPP server broker library
 */
public class BrokerConnect {

	// ctor
	public BrokerConnect(
					HtServerProxy srvproxy,
					Uid connectionUid,
					String connectionString,
					boolean isConnected,
					long state,
					String runName,
					String runComment) {
		serverProxy_m = srvproxy;
		uid_m.fromUid(connectionUid);
		connectString_m.append(connectionString);
		isConnected_m = isConnected;
		stateMachineFlag_m = (int) state;
		runName_m.append(runName);
	}

	public boolean isConnected() {
		return isConnected_m;
	}

	public String getConnection() {
		return connectString_m.toString();
	}

	public Uid getUid() {
		return uid_m;
	}

	public int getState() {
		return stateMachineFlag_m;
	}

	public String getRunName() {
		return runName_m.toString();
	}

	public String getRunComment() {
		return runComment_m.toString();
	}

	// -------------------------

	private Uid uid_m = new Uid();
	private StringBuilder connectString_m = new StringBuilder();
	private StringBuilder runName_m = new StringBuilder();
	private StringBuilder runComment_m = new StringBuilder();
	private boolean isConnected_m = false;
	private HtServerProxy serverProxy_m = null;
	private int stateMachineFlag_m = -1;
}
