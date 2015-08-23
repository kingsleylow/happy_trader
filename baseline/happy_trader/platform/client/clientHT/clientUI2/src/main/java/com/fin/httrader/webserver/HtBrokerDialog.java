/*
 * HtBrokerDialog.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;

import java.io.IOException;

import java.util.*;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerDialog extends HtServletsBase {

	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 300;

	/**
	 * Creates a new instance of HtBrokerDialog
	 */
	public HtBrokerDialog() {
	}

	public int getPageSize() {
		return ROWS_PER_PAGE;
	}

	public int getNumberPagesToShow() {
		return NUMBER_OF_PAGES;
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			generateUniquePageId();

			setStringSessionValue(getUniquePageId(), req, "operation", "empty_page");
			//readAvailableProvidersAndProfiles(req);
			//readAllTypesOfPriceToUse(req);



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return true;
		}
		
		return false;

	}
	; //

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {


			// whether we generate new page uid via POST
			boolean new_puid = getBooleanParameter(req, "generate_new_page_uid", true);
			if (new_puid) {
				generateUniquePageId();
			} else {
				setUniquePageId(getStringParameter(req, "page_uid", false));
			}


			//readAvailableProvidersAndProfiles(req);
			//readAllTypesOfPriceToUse(req);


			// operation
			String operation = getStringParameter(req, "operation", false);
			setStringSessionValue(getUniquePageId(), req, "operation", operation);


			if (operation.equalsIgnoreCase("pass_runs")) {

				createSessionIdsViaRuns(req);


			} else if (operation.equalsIgnoreCase("pass_sessions")) {

				createSessionIds(req);

			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	//

	
	
	// creating array of syssions and run ids
	private void createSessionIds(HttpServletRequest req) throws Exception {

		String cookie = getStringParameter(req, "cookie_broker_sessions_set", false);
		Object obj = getObjectSessionValue(cookie, req, "broker_sessions_set");

		TreeSet<String> sessionids = new TreeSet<String>();
		if (obj != null) {
			HashSet<String> sobj = (HashSet<String>) obj;
			sessionids.addAll(sobj);

		}

		Map<String, Set<String>> sessionRunIdsMap = HtCommandProcessor.instance().get_HtDatabaseProxy().
						remote_resolveSessionRunIdsViaSessions(sessionids);


		initializeSessionRunIdsJScriptData(req, sessionRunIdsMap);
		initializeSessionRunIdsTree(req, sessionRunIdsMap);

	}

	// creating array of sessions and run ids
	private void createSessionIdsViaRuns(HttpServletRequest req) throws Exception {
		// need to extract session ids via runs


		//Vector<String> runids = getStringListParameter(req, "run_ids", true);
		String cookie = getStringParameter(req, "cookie_broker_runs_set", false);
		Object obj = getObjectSessionValue(cookie, req, "broker_runs_set");


		TreeSet<String> runids = new TreeSet<String>();
		if (obj != null) {
			HashSet<String> sobj = (HashSet<String>) obj;
			runids.addAll(sobj);

		}


		Map<String, Set<String>> sessionRunIdsMap = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_resolveSessionRunIdsViaRunIds(runids);


		//
		initializeSessionRunIdsJScriptData(req, sessionRunIdsMap);
		initializeSessionRunIdsTree(req, sessionRunIdsMap);

	}

	private void initializeSessionRunIdsJScriptData(HttpServletRequest req, Map<String, Set<String>> sessionRunIdsMap) throws Exception {

	
		StringBuilder out = new StringBuilder();

		StringBuilder out2 = new StringBuilder();

		out.append("new Array( ");
		for (Iterator<String> it = sessionRunIdsMap.keySet().iterator(); it.hasNext();) {

			String session_i = it.next();
			Set<String> runIdsSet = sessionRunIdsMap.get(session_i);

			out2.append("'").append(session_i).append("'");
			out2.append(",");
			out2.append("new Array(");

			for (Iterator<String> it2 = runIdsSet.iterator(); it2.hasNext();) {
				String runId_i = it2.next();

				out2.append("'").append(runId_i).append("'");
				out2.append(",");

			}

			out2.deleteCharAt(out2.length() - 1);
			out2.append(")");

			out.append(out2);
			out.append(",");

			out2.setLength(0);

		}

		out.deleteCharAt(out.length() - 1);
		out.append(")");

		setStringSessionValue(getUniquePageId(), req, "session_runid_list", out.toString());

	}

	private void initializeSessionRunIdsTree(HttpServletRequest req, Map<String, Set<String>> sessionRunIdsMap) throws Exception {
		StringBuilder xtrees = new StringBuilder();
		xtrees.append("<ul>");

		for (Iterator<String> it = sessionRunIdsMap.keySet().iterator(); it.hasNext();) {
			// resolve run ids

			String session_i = it.next();


			xtrees.append("<li>");
			xtrees.append(session_i);


			Set<String> runIds = sessionRunIdsMap.get(session_i);

			xtrees.append("<ul>");
			for (Iterator<String> it2 = runIds.iterator(); it2.hasNext();) {

				xtrees.append("<li>");
				xtrees.append(it2.next());
				xtrees.append("</li>");

			}

			xtrees.append("</ul>");

			xtrees.append("</li>");


		}
		;

		xtrees.append("</ul>");
		//xtrees.append("</li></ul>");

		setStringSessionValue(getUniquePageId(), req, "session_runid_ul", xtrees.toString());
	}
}
