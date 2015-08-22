/*
 * HtServers.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.hlpstruct.TradeServerLogLevels;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.google.gson.Gson;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServers extends HtServletsBase {

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtServers */
	public HtServers() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new UID
			this.generateUniquePageId();
			readServerProperty(req, null, false);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	// -----------------------------------
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {



			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			setStringSessionValue(getUniquePageId(), req, "server_list", "");
			setStringSessionValue(getUniquePageId(), req, "rtport", "");
			setStringSessionValue(getUniquePageId(), req, "cmdport", "");
			setStringSessionValue(getUniquePageId(), req, "srv_path", "");
			setStringSessionValue(getUniquePageId(), req, "log_level_options", "");
			setStringSessionValue(getUniquePageId(), req, "event_subscr_list", "");


			String operation = getStringParameter(req, "operation", false);


			if (operation.equalsIgnoreCase("refresh_page")) {
				// no-op
				readServerProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("apply_changes")) {
				applyChanges(req);
				readServerProperty(req, null, true);

			} else if (operation.equalsIgnoreCase("add_new_server")) {
				String new_server = addNewServer(req);
				readServerProperty(req, new_server, false);

			} else if (operation.equalsIgnoreCase("delete_server")) {
				deleteServer(req);
				readServerProperty(req, null, false);

			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}


	/*
	 * Helpers
	 */
	private String addNewServer(HttpServletRequest req) throws Exception {

		// new provider name
		String new_server = getStringParameter(req, "new_server", false);

		Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
		if (servers.contains(new_server)) {
			throw new HtException(getContext(), "addNewProvider", "Trading server already exists: \"" + new_server + "\"");
		}

		HtServerProp prop = new HtServerProp();
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setTradingServerProperty(new_server, prop);

		return new_server;
	}

	private void deleteServer(HttpServletRequest req) throws Exception {
		String cur_server = getStringParameter(req, "cur_server", false);

		Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
		if (!servers.contains(cur_server)) {
			throw new HtException(getContext(), "deleteProvider", "Trading server does not exist: \"" + cur_server + "\"");
		}

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeTradingServerProperty(cur_server);
	}

	private void applyChanges(HttpServletRequest req) throws Exception {
		// current provider - raise an exception if not found
		String cur_server = getStringParameter(req, "cur_server", false);

		HtServerProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerProperty(cur_server);
		prop.setCommandPort( (int) getIntParameter(req, "cmd_port", false) );
		prop.setSpawnerPort((int)getIntParameter(req, "spawner_port", false) );

		prop.setUseSpawner(getBooleanParameter(req, "use_spawner", false));
		prop.setServerHost( getStringParameter(req, "server_host", false) );
		
		prop.setServerExecutable(getStringParameter(req, "srv_path", false));
		prop.setLogEventLevel((int) getIntParameter(req, "def_log_level", false));
		prop.setDebugLevel((int)getIntParameter(req, "srv_log_level", false));

		List<HtPair<String, String>> eventstosubscr = getStringPairListParameter(req, "subscribed_events", true);

		for (int i = 0; i < eventstosubscr.size(); i++) {
			HtPair<String, String> p = eventstosubscr.get(i);
			int event_type = Integer.valueOf(p.first);

			if (!XmlEvent.getAllAvalableEventsForServer().contains(event_type)) {
				throw new HtException(getContext(), "applyChanges", "For the server: \"" + cur_server + "\" invalid event type were applied: " + event_type);
			}


			if (p.second.equalsIgnoreCase("1")) {
				prop.getSubscribedEvents().put(event_type, true);
			} else if ((p.second.equalsIgnoreCase("0"))) {
				if (event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm ||
								event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker) {
					// always subscribed
					throw new HtException(getContext(), "applyChanges", "For the server: \"" + cur_server +
									"\" XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker or XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm must be always subscribed");

				}

				prop.getSubscribedEvents().put(event_type, false);
			} else {
				throw new HtException(getContext(), "applyChanges", "For the server: \"" + cur_server + "\" invalid event type flag: " + p.second);
			}


		}
		

		// save
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setTradingServerProperty(cur_server, prop);

	}

	private void readServerProperty(HttpServletRequest req, String cur_server_passed, boolean read_param) throws Exception {

		// list of providers
		Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();

		// list is empty
		if (servers.size() <= 0) {
			return;
		}

		// current provider:
		String cur_server = null;


		if (cur_server_passed == null) {
			if (read_param) {
				cur_server = getStringParameter(req, "cur_server", true);
			}
		} else {
			cur_server = cur_server_passed;
		}

		// fisr parameter if no current
		if (cur_server == null || cur_server.length() <= 0) {
			cur_server = (String) servers.toArray()[0];
		}

		// provider property
		HtServerProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerProperty(cur_server);


		setStringSessionValue(getUniquePageId(), req, "server_list", HtWebUtils.createOptionList(cur_server, servers, false));

		//
		setStringSessionValue(getUniquePageId(), req, "use_spawner", prop.isUseSpawner()? "true":"false");
		setStringSessionValue(getUniquePageId(), req, "cmdport", String.valueOf(prop.getCommandPort()));
		setStringSessionValue(getUniquePageId(), req, "serverhost", String.valueOf(prop.getServerHost()));
		setStringSessionValue(getUniquePageId(), req, "spawnerport", String.valueOf(prop.getSpawnerPort()));

		// server executable
	
		Set<File> available_executables = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllSrvExecutables();
		Set<String> available_paths = new TreeSet<String>();
		for (Iterator<File> it = available_executables.iterator(); it.hasNext();) {
			File file_i = it.next();
			available_paths.add(file_i.getAbsolutePath());
		}
		
		Gson gson = new Gson();
		gson.toJson( available_paths );

		
		setStringSessionValue(getUniquePageId(), req, "available_executables", gson.toJson( available_paths ));
		setStringSessionValue(getUniquePageId(), req, "srv_path", prop.getServerExecutable());
		setStringSessionValue(getUniquePageId(), req, "log_level_options", HtWebUtils.createOptionMappedList(prop.getLogEventLevel(), CommonLog.getAvailableLogLevelsMap()));

		// events to subscribe
		StringBuilder out = new StringBuilder();

		out.append("<tr NOWRAP=true>");
		out.append("<td type='ro'>Is Subscribed</td>");
		out.append("<td type='ro'>Event Type</td>");
		out.append("<td type='ro' width='0px'>EVENT_TYPE_NUM</td>");
		out.append("</tr>");

		int cnt = 0;
		for (Iterator<Integer> it2 = XmlEvent.getAllAvalableEventsForServer().iterator(); it2.hasNext();) {
			int event_type = it2.next();
			String event_type_str = XmlEvent.getEventTypeName(event_type);

			out.append("<tr NOWRAP=true id='");
			out.append(++cnt);
			out.append("'>");


			boolean subscribed = false;
			if (prop.getSubscribedEvents().containsKey(event_type)) {
				if (prop.getSubscribedEvents().get(event_type).booleanValue()) {
					subscribed = true;
				}
			}

		
			out.append("<td>");
			if (event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromAlgorithm ||
							event_type == XmlEvent.ET_AlgorithmBrokerEventResp_FromBroker) {
				// always subscribed
				out.append("<input type=checkbox checked=true disabled=true/>");

			} else {
				if (subscribed) {
					out.append("<input type=checkbox checked=true />");
				}
				else {
					out.append("<input type=checkbox />");
				}
			}
		
			out.append("</td>");

	
			out.append("<td>");
			out.append(event_type_str);
			out.append("</td>");

			out.append("<td>");
			out.append(event_type);
			out.append("</td>");
		

			out.append("</tr>");
		}

		setStringSessionValue(getUniquePageId(), req, "event_subscr_list", out.toString());

		// server log levels
		setStringSessionValue(getUniquePageId(), req, "srv_log_levels", HtWebUtils.createOptionMappedList(prop.getDebuglevel(), TradeServerLogLevels.getAllLogLevelsMap())  );
		
	}
} //
