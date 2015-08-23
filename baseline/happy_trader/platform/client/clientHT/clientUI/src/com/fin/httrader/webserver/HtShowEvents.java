/*
 * HtShowEvents.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.utils.HtTimeCalculator;
import java.io.IOException;
import java.util.Map;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowEvents extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void initialize(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			//setStringSessionValue(getContext(), req, "event_server_port", String.valueOf(HtMain.EVENT_PROPAGATOR_LISTEN_PORT));

			// calculate host name
			setStringSessionValue(getContext(), req, "event_server_host", req.getLocalName());


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
		}
	}
}
