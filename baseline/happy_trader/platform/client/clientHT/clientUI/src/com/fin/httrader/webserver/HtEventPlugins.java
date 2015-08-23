/*
 * HtEventPlugins.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtEventPlugins extends HtServletsBase {

		// --------------------------------
		@Override
		public String getContext() {
				return this.getClass().getSimpleName();
		}

		/**
		 * Creates a new instance of HtRTProviders
		 */
		public HtEventPlugins() {
		}

		@Override
		public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
				res.setContentType("text/html; charset=windows-1251");
				forceNoCache(res);

				try {

						this.setUniquePageId(getStringParameter(req, "page_uid", false));
						String operation = getStringParameter(req, "operation", false);

						if (operation.equalsIgnoreCase("refresh_page")) {
								// no-op
								readPluginProperty(req, null, true);
						} else if (operation.equalsIgnoreCase("apply_changes")) {
								applyChanges(req);
								readPluginProperty(req, null, true);

						} else if (operation.equalsIgnoreCase("add_new_plugin")) {
								String new_provider = addNewPlugin(req);
								readPluginProperty(req, new_provider, false);

						} else if (operation.equalsIgnoreCase("delete_plugin")) {
								deletePlugin(req);
								readPluginProperty(req, null, false);

						} else {
								throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
						}

				} catch (Throwable e) {
						writeHtmlErrorToOutput(res, e);
						return false;
				}

				return true;
		}

		@Override
		public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
				res.setContentType("text/html; charset=windows-1251");
				forceNoCache(res);

				try {
						// generate new UID
						this.generateUniquePageId();

						// this is initial load
						readPluginProperty(req, null, false);

				} catch (Throwable e) {
						writeHtmlErrorToOutput(res, e);
						return false;
				}

				return true;
		}

		// ----------------------------------------
		private String addNewPlugin(HttpServletRequest req) throws Exception {

				// new provider name
				String new_plugin = getStringParameter(req, "new_plugin", false);

				Set<String> plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				if (plugins.contains(new_plugin)) {
						throw new HtException(getContext(), "addNewPlugin", "Event plugin already exists: \"" + new_plugin + "\"");
				}

				HtEventPluginProp prop = new HtEventPluginProp();
				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setEventPluginProperty(new_plugin, prop);

				return new_plugin;
		}

		private void deletePlugin(HttpServletRequest req) throws Exception {
				String cur_plugin = getStringParameter(req, "cur_plugin", false);

				Set<String> plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();
				if (!plugins.contains(cur_plugin)) {
						throw new HtException(getContext(), "deletePlugin", "Event plugin does not exist: \"" + cur_plugin + "\"");
				}

				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeEventPluginProperty(cur_plugin);

		}

		private void applyChanges(HttpServletRequest req) throws Exception {
				// current provider - raise an exception if not found
				String cur_plugin = getStringParameter(req, "cur_plugin", false);
				String launch_flag_s = getStringParameter(req, "launch_flag", false);
				
				int launch_flag = HtEventPluginProp.getLaunchFlagIdByName(launch_flag_s);
				
				
				

				// properies to update
				List<HtPair<String, String>> properties = ExportImportXGridUtils.getAsPropertiesList(getStringParameter(req, "properties", true));

				// other parameters
				HtEventPluginProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(cur_plugin);

				String plugin_class = getStringParameter(req, "plugin_class", false);
				List< HtPair<String, File>> classes = HtCommandProcessor.instance().get_EventPluginProxy().remote_queryAvailableProviders();

				boolean found_native = false;
				boolean found_jar = false;
				

				for (int i = 0; i < classes.size(); i++) {
						HtPair<String, File> p = classes.get(i);

						if (p.first.equalsIgnoreCase(plugin_class)) {
								if (p.second != null) {
										found_jar = true;
										
								} else {
										found_native = true;
								}
						}

						// found all
						if (found_jar && found_native) {
								break;
						}

				}

				prop.setLaunchFlag(HtEventPluginProp.LAUNCH_DUMMY);
				if (found_jar) {
						if (launch_flag == HtEventPluginProp.LAUNCH_EXT_JAR)
								prop.setLaunchFlag(launch_flag);
				}
				else if (found_native) {
						if (launch_flag == HtEventPluginProp.LAUNCH_INTERNAL)
								prop.setLaunchFlag(launch_flag);
				}
				
				if (prop.getLaunchFlag() == HtEventPluginProp.LAUNCH_DUMMY)
						throw new HtException(getContext(), "applyChanges", "For class: \"" + plugin_class + "\" and plugin id: " + cur_plugin + " the system was not able to resolve launch parameter" );
				

				prop.setPluginClass(plugin_class);

				//
				prop.getParameters().clear();

				for (int i = 0; i < properties.size(); i++) {

						HtPair<String, String> entry = properties.get(i);
						if (entry.first != null && entry.first.length() > 0) {
								prop.getParameters().put(entry.first, entry.second);
						} else {
								throw new HtException(getContext(), "applyChanges", "Invalid parameter entry");
						}

				}

				// save
				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setEventPluginProperty(cur_plugin, prop);

		}

		private void readPluginProperty(HttpServletRequest req, String cur_plugin_passed, boolean read_param) throws Exception {
				// list of providers
				Set<String> plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList();

				// list is empty
				if (plugins.size() <= 0) {
						return;
				}

				// current provider:
				String cur_plugin = null;

				if (cur_plugin_passed == null) {
						if (read_param) {
								cur_plugin = getStringParameter(req, "cur_plugin", true);
						}
				} else {
						cur_plugin = cur_plugin_passed;
				}

				// fisr parameter if no current
				if (cur_plugin == null || cur_plugin.length() <= 0) {
						cur_plugin = (String) plugins.toArray()[0];
				}

				// provider property
				HtEventPluginProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(cur_plugin);
				setStringSessionValue(getUniquePageId(), req, "plugin_list", HtWebUtils.createOptionList(cur_plugin, plugins, false));

				StringBuilder out = new StringBuilder();
				List< HtPair<String, File>> classes = HtCommandProcessor.instance().get_EventPluginProxy().remote_queryAvailableProviders();

				String resolvedProviderClass = null;
								
				for (int i = 0; i < classes.size(); i++) {
						HtPair<String, File> p = classes.get(i);

						boolean is_internal = p.second == null;
						String style_str = (is_internal ? "background-color:green" : "");
						String internal_attrib =  (is_internal ? " _internal_class=true" : " _internal_class=false");

						if (p.first.equals(prop.getPluginClass())) {
								out.append("<option").append(internal_attrib).append(" selected invalid=false style='").append(style_str).append("'>").append(p.first).append("</option>");
								resolvedProviderClass = prop.getPluginClass();
						} else {
								out.append("<option ").append(internal_attrib).append(" invalid=false style='").append(style_str).append("'>").append(p.first).append("</option>");
						}
				}

				// if the class not found
				if (resolvedProviderClass == null) {

						// only if we have initialized one
						if (prop.getPluginClass().length() > 0) {
								out.append("<option selected style='background-color:orange' invalid=true>").append(prop.getPluginClass()).append(" - ???</option>");
						}
				}

				setStringSessionValue(getUniquePageId(), req, "plugin_classes", out.toString());
				
				setStringSessionValue(getUniquePageId(), req, "launch_flag_select_obj", 
					HtWebUtils.createOptionMappedList(prop.getLaunchFlag(), HtEventPluginProp.getLaunchFlagNameMap())
				);
				
				setStringSessionValue(getUniquePageId(), req, "plugin_parameters", HtWebUtils.createHtmlParametersTable(prop.getParameters()));

		}

}
