/*
 * HtManageServer.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import java.io.IOException;
import java.util.Iterator;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtManageServer extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtManageServer */
	public HtManageServer() {
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		try {
			wrapParametersToSession(getContext(), req);
			res.sendRedirect("/htManageServer.jsp");

			setRedirectedAfterPost(getContext(), req, true);
		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
		}
	}

	// this is initialization function for dialog
	public void initialize(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {



			boolean afterPost = isRedirectedAfterPost(getContext(), req);


			if (afterPost) {
				setRedirectedAfterPost(getContext(), req, false);

			} else {
				// clear all parameters
				clearSessionValues(getContext(), req);
			}


			initThisPageGlobals(req);

			if (!afterPost) {
				// initial load
				readServerData(req);

				return;
			}

			String operation = getStringParameterWrappedToSession(getContext(), req, "operation", false);

			if (operation.equals("do_refresh")) {
				// no-op
			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}

			readServerData(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
		}

	}
	; //

	private void readServerData(HttpServletRequest req) throws Exception {
		StringBuilder out = new StringBuilder();
		Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();


		String cur_server_id = this.getStringParameterWrappedToSession(getContext(), req, "server_id", true);
		boolean atLeastOne = false;
		for (Iterator<String> it = servers.iterator(); it.hasNext();) {
			String server_id = it.next();
			int server_status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(server_id);

			if (server_status!=RtTradingServerManager.SERVER_IDLE) {
				// the first running server
				if (cur_server_id.length() <= 0) {
					cur_server_id = server_id;
				}

				if (cur_server_id.equals(server_id)) {
					out.append("<option selected >").append(server_id).append("</option>");
				} else {
					out.append("<option>").append(server_id).append("</option>");
				}

				atLeastOne = true;
			}
		}

		setStringSessionValue(getContext(), req, "running_server_list", out.toString());

		// no need to go on
		if (!atLeastOne) {
			return;
		}
	}

	private void initThisPageGlobals(HttpServletRequest req) throws Exception {
		setStringSessionValue(getContext(), req, "running_server_list", "");
	}
}
