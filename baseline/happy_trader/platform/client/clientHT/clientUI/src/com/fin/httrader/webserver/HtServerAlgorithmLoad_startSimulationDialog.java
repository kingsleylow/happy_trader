/*
 * HtServerAlgorithmLoad_startSimulationDialog.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.ServerThreadStatus;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerAlgorithmLoad_startSimulationDialog extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtServerAlgorithmLoad_startSimulationDialog */
	public HtServerAlgorithmLoad_startSimulationDialog() {
	}

	// ----------------------------
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// new UID
			this.generateUniquePageId();

			// init simulation dialog
			// here only we are initializing alg brk parameters
			setStringSessionValue(getUniquePageId(), req, "thread_id", "");
			setStringSessionValue(getUniquePageId(), req, "server_id", "");

			
			if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
				throw new HtException(getContext(), "readHistSimulParams", "Server is busy with lengthy operation: " +
								HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
			}

			String cur_server_id = getStringParameter(req, "server_id", false);
			int thread_id = (int) getIntParameter(req, "thread_id", false);

			int tStatus = HtCommandProcessor.instance().get_HtServerProxy().remote_requestThreadStatus(cur_server_id, thread_id);

			if (ServerThreadStatus.TH_Lazy != tStatus) {
				throw new HtException(getContext(), "readHistSimulParams", "Thread must be in " + ServerThreadStatus.getThreadStatus(ServerThreadStatus.TH_Lazy) + " mode");
			}


			setStringSessionValue(getUniquePageId(), req, "thread_id", String.valueOf(thread_id));
			setStringSessionValue(getUniquePageId(), req, "server_id", cur_server_id);

			//
			//
			// resolve the list of cached ticks
			Map<Uid, HtPair<String, Long>> uidIdCreatedMap = new HashMap<Uid, HtPair<String, Long>>();
			HtCommandProcessor.instance().get_HtServerProxy().remote_queryAvailableSimulationEntries(cur_server_id, uidIdCreatedMap);
			StringBuilder out = new StringBuilder();

			for(Iterator<Uid> it = uidIdCreatedMap.keySet().iterator(); it.hasNext(); ) {
				Uid key = it.next();
				HtPair<String, Long> pair_i = uidIdCreatedMap.get(key);

				out.append("<option value='").append(key.toString()).append("'>");
				out.append(key.toString()).append(" - ").append(pair_i.first).append(" - ").append(HtDateTimeUtils.time2SimpleString_Gmt(pair_i.second));
				out.append("</option>");
			}

			setStringSessionValue(getUniquePageId(), req, "simulation_uid_list", out.toString());


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}
	
}
