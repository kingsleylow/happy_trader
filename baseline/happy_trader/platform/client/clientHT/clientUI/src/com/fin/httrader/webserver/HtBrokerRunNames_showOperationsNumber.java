/*
 * HtBrokerRunNames_showOperationsNumber.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;
import java.util.*;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames_showOperationsNumber extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtBrokerRunNames_showOperationsNumber */
	public HtBrokerRunNames_showOperationsNumber() {
	}

	// -------------------------------
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();
			setStringSessionValue(getUniquePageId(), req, "requests_to_broker", "-1");
			setStringSessionValue(getUniquePageId(), req, "responses_from_broker", "-1");
			setStringSessionValue(getUniquePageId(), req, "run_name", "-1");
			setStringSessionValue(getUniquePageId(), req, "sessions_number", "-1");



			doGetInfo(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	// --------------------------------
	private void doGetInfo(HttpServletRequest req) throws Exception {
		String run_name = getStringParameter(req, "run_name", false);
		setStringSessionValue(getUniquePageId(), req, "run_name", run_name);

		// resolve all session ids based upon that run id
		TreeSet<String> runids = new TreeSet<String>();
		runids.add(run_name);

		Map<String, Set<String>> sessionRunIdsMap =
						HtCommandProcessor.instance().get_HtDatabaseProxy().remote_resolveSessionRunIdsViaRunIds(runids);

		Vector<String> sessionsIds = new Vector<String>();
		sessionsIds.addAll(sessionRunIdsMap.keySet());
		setStringSessionValue(getUniquePageId(), req, "sessions_number", String.valueOf(sessionsIds.size()));

		long orders_number = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showOrdersAmount(sessionsIds);
		setStringSessionValue(getUniquePageId(), req, "orders_number", String.valueOf(orders_number));


		long broker_response_orders_number = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerResponseOrdersAmount(sessionsIds);
		setStringSessionValue(getUniquePageId(), req, "responses_from_broker", String.valueOf(broker_response_orders_number));


	}
}

    