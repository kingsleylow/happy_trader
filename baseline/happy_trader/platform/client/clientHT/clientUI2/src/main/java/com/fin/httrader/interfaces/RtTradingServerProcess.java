/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 * This class encapsulate process management of trading server
 */
public interface RtTradingServerProcess {

		// launches server process
		public void startProcess(
				String serverExecutable,
				int serverPort, // communication port with server
				int servicePort, // callback port as a parameter
				String serverId,
				Map<String, String> newEntries // additional enviroment
		) throws Exception;

		// wait politely for finish
		public void waitForFinish();

		// forcibly destroy and clean resources
		public void destroyProcess();
}
