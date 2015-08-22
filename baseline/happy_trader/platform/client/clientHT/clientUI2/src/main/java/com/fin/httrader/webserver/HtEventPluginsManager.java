/*
 * HtEventPluginsManager.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
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
public class HtEventPluginsManager extends HtServletsBase {

	/** Creates a new instance of HtEventPluginsManager */
	public HtEventPluginsManager() {
	}

	// ----------------------------------------------
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			setUniquePageId(getStringParameter(req, "page_uid", false));
			String operation = getStringParameter(req, "operation", false);


			if (operation.equals("start_plugin")) {
				startPlugin(req);
			} else if (operation.equals("stop_plugin")) {
				stopPlugin(req);
			} else if (operation.equals("refresh")) {
				// no-op
			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}

			readPluginData(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	// -------------------------------------------------
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new UID
			generateUniquePageId();

			readPluginData(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	// ----------------------------------------------
	private void startPlugin(HttpServletRequest req) throws Exception {
		String plugin_id = getStringParameter(req, "plugin_id", false);

		HtEventPluginProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(plugin_id);

		HtCommandProcessor.instance().get_EventPluginProxy().remote_startPlugin(
						plugin_id,
						prop.getPluginClass(),
						prop.getLaunchFlag(),
						prop.getParameters());
	}

	private void stopPlugin(HttpServletRequest req) throws Exception {
		String plugin_id = getStringParameter(req, "plugin_id", false);
		HtCommandProcessor.instance().get_EventPluginProxy().remote_stopPlugin(plugin_id);
	}

	private void readPluginData(HttpServletRequest req) throws Exception {
		StringBuilder out = new StringBuilder();

		out.append("<tr NOWRAP=true>");
		out.append("<td type='ro'>RT Plugin</td>");
		out.append("<td type='ro'>Status</td>");
		out.append("<td type='ro'>Action</td>");
		out.append("<td type='ro'>Status Code</td>");
		out.append("<td type='ro'>Status String</td>");
		out.append("</tr>");


		Set<String> plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();

		int cnt = 0;
		for (Iterator<String> it = plugins.iterator(); it.hasNext();) {
			String value = it.next();

			BackgroundJobStatus providerStatus = null;
			boolean isRunning = HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(value);
			if (isRunning) {
				providerStatus = HtCommandProcessor.instance().get_EventPluginProxy().remote_getPluginStatus(value);
			}

			out.append("<tr NOWRAP=true id='");
			out.append(++cnt);
			out.append("'>");

			out.append("<td>");
			out.append(value);
			out.append("</td>");

			out.append("<td>");
			out.append(isRunning ? "Running" : "Idle");
			out.append("</td>");

			out.append("<td>");

			
			if (!isRunning) {
				out.append("<a href='#' onclick='do_start_plugin(\""); out.append(value); out.append("\");'>Start</a>");
			//action.append("&nbsp;");
			} else {
				out.append("<a href='#' onclick='do_stop_plugin(\""); out.append(value); out.append( "\");'>Stop</a>");
			}

			out.append("</td>");

			if (isRunning) {
				out.append("<td>");
				out.append(BackgroundJobStatus.getStatusString(providerStatus.getStatusCode()));
				out.append("</td>");

				out.append("<td>");
				out.append(providerStatus.getStatusReason());
				out.append("</td>");
			}
			else {
				out.append("<td>");
				out.append("</td>");

				out.append("<td>");
				out.append("</td>");
			}
			
			out.append("</tr>");

		}
		

		setStringSessionValue(getUniquePageId(), req, "plugin_list", out.toString());
	}
}
