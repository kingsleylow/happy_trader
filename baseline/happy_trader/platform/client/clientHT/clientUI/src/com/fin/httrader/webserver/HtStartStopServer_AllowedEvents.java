/*
 * HtStartStopServer_AllowedEvents.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtStartStopServer_AllowedEvents extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtStartStopServer_AllowedEvents */
	public HtStartStopServer_AllowedEvents() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// new UID
			this.generateUniquePageId();

			// here only we are initializing alg brk parameters
			setStringSessionValue(getUniquePageId(), req, "server_log_levels", "");
			setStringSessionValue(getUniquePageId(), req, "event_subscr_list", "");
			readLogEventsParams(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		
		return true;
	}
	

	// --
	// helpers
	private void readLogEventsParams(HttpServletRequest req) throws Exception {

		if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
			throw new HtException(getContext(), "readLogEventsParams", "Server is busy with lengthy operation: " +
							HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
		}

		String cur_server_id = getStringParameter(req, "server_id", false);
		int def_log_level = HtCommandProcessor.instance().get_HtServerProxy().remote_getLogLevel(cur_server_id);

		StringBuilder out = new StringBuilder();

		for (Iterator<Integer> it = CommonLog.getAvailableLogLevels().iterator(); it.hasNext();) {
			int loglevel = it.next().intValue();

			out.append("<option value='" + loglevel + "'");

			if (def_log_level == loglevel) {
				out.append(" selected >");
			} else {
				out.append(" >");
			}

			out.append(CommonLog.getLogLevelName(loglevel));
			out.append("</option>");
		}

		setStringSessionValue(getUniquePageId(), req, "server_log_levels", out.toString());
		//
		out.setLength(0);


		Map<Integer, Boolean> allowed = HtCommandProcessor.instance().get_HtServerProxy().remote_getAllowedEvents(cur_server_id);


		for (Iterator<Integer> it2 = XmlEvent.getAllAvalableEventsForServer().iterator(); it2.hasNext();) {
			int event_type = it2.next();
			String event_type_str = XmlEvent.getEventTypeName(event_type);

			out.append("<tr>");

			out.append("<td align=left internal_event_type='" + event_type + "'>");
			out.append(event_type_str);
			out.append("</td>");

			out.append("<td align=left>");
			boolean subscribed = false;
			if (allowed.containsKey(event_type)) {
				if (allowed.get(event_type).booleanValue()) {
					subscribed = true;
				}
			}

			if (event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm ||
							event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker) {
				out.append("<input type=checkbox checked=true disabled=true/>");
			} else {
				if (subscribed) {
					out.append("<input type=checkbox checked=true />");
				} else {
					out.append("<input type=checkbox />");
				}
			}
			out.append("</td>");

			out.append("<td>");
			out.append("</td>");

			out.append("</tr>");
		}
		setStringSessionValue(getUniquePageId(), req, "event_subscr_list", out.toString());
	}
}
