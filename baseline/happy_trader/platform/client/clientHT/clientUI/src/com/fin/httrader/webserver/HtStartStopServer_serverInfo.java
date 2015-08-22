/*
 * HtStartStopServer_serverInfo.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtStartStopServer_serverInfo extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtStartStopServer_serverInfo */
	public HtStartStopServer_serverInfo() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// new UID
			this.generateUniquePageId();

			if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
				throw new HtException(getContext(), "initializeServerInfo", "Server is busy with lengthy operation: " +
								HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
			}


			// this calculate only server state
			String server_id = getStringParameter(req, "server_id", false);

		
			setStringSessionValue(getUniquePageId(), req, "trading_server_name", server_id);
			XmlParameter resp = HtCommandProcessor.instance().get_HtServerProxy().remote_requestServerState(server_id);

			StringBuilder out = new StringBuilder();
			int level = 0;

			createServerResponse(resp, out, level);
			

			// subscribed providers
			Set<String> subscribed_rt_providers = HtCommandProcessor.instance().get_HtServerProxy().remote_getSubscribedRTProviders(server_id);
			XmlParameter rt_xml_param = new XmlParameter();
			rt_xml_param.setStringList("Subscribed RT providers", new Vector<String>(subscribed_rt_providers));

			createServerResponse(rt_xml_param, out, level);


			//
			setStringSessionValue(getUniquePageId(), req, "trading_server_parameters", out.toString());

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;
	}
	

	// --
	// helpers
	private void createServerResponse(XmlParameter xmlparameter, StringBuilder out, int level) throws Exception {
		for (Iterator<String> it = xmlparameter.getKeys().iterator(); it.hasNext();) {
			String key = it.next();

			if (key.equals("status")) {
				continue;
			}
			if (key.equals("status_code")) {
				continue;
			}

			StringBuilder indent = new StringBuilder();
			for (int i = 0; i < level; i++) {
				indent.append("<td width=5px></td>");
			}

			out.append("\n<tr>");
			out.append("<td><table class=x8 width=100%><tr>" + indent + "<td align=left class=x2><div align=left>" + key + "</div></td></tr></table></td>");
			out.append("</tr>\n");

			if (xmlparameter.getType(key) == XmlParameter.VT_XmlParameter) {
				XmlParameter kid_param = xmlparameter.getXmlParameter(key);
				createServerResponse(kid_param, out, level + 1);
			} else if (xmlparameter.getType(key) == XmlParameter.VT_String) {
				String value = xmlparameter.getString(key);
				if (value.length() > 0) {
					out.append("<tr><td><table class=x8 width=100%><tr>");
					out.append(indent);
					out.append("<td width=5px>-</td>");
					out.append("<td align=left>" + value + "</td>");
					out.append("<td></td>");
					out.append("</tr></table></td></tr>\n");
				}

			} else if (xmlparameter.getType(key) == XmlParameter.VT_StringList) {
				List<String> vals = xmlparameter.getStringList(key);
				for (int i = 0; i < vals.size(); i++) {
					if (vals.get(i).length() > 0) {
						out.append("<tr><td><table class=x8 width=100%><tr>");
						out.append(indent);
						out.append("<td width=5px>-</td>");
						out.append("<td align=left>" + vals.get(i) + "</td>");
						out.append("<td></td>");
						out.append("</tr></table></td></tr>\n");
					}
				}
			} else {
				throw new HtException(getContext(), "createServerResponse", "Invalid xmlparameter type");
			}

		}

	}
}
