/*
 * HtRTProviders.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;

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
public class HtRTProviders extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtRTProviders */
	public HtRTProviders() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new UID
			this.generateUniquePageId();

		
			readProviderProperty(req, null, false);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {

			setUniquePageId(getStringParameter(req, "page_uid", false));

			setStringSessionValue(getUniquePageId(), req, "provider_list", "");
			setStringSessionValue(getUniquePageId(), req, "provider_classes", "");
			setStringSessionValue(getUniquePageId(), req, "provider_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "shift_hours", "");
			setStringSessionValue(getUniquePageId(), req, "sign_digits", "");

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("refresh_page")) {
				// no-op
				readProviderProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("apply_changes")) {
				applyChanges(req);
				readProviderProperty(req, null, true);

			} else if (operation.equalsIgnoreCase("add_new_provider")) {
				String new_provider = addNewProvider(req);
				readProviderProperty(req, new_provider, false);

			} else if (operation.equalsIgnoreCase("delete_provider")) {
				deleteProvider(req);
				readProviderProperty(req, null, false);

			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	} 

	private String addNewProvider(HttpServletRequest req) throws Exception {

		// new provider name
		String new_provider = getStringParameter(req, "new_provider", false);

		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();
		if (providers.contains(new_provider)) {
			throw new HtException(getContext(), "addNewProvider", "RT Provider already exists: \"" + new_provider + "\"");
		}

		HtRTProviderProp prop = new HtRTProviderProp();
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setRTProviderProperty(new_provider, prop);

		return new_provider;
	}

	private void deleteProvider(HttpServletRequest req) throws Exception {
		String cur_provider = getStringParameter(req, "cur_provider", false);

		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();
		if (!providers.contains(cur_provider)) {
			throw new HtException(getContext(), "deleteProvider", "RT Provider does not exist: \"" + cur_provider + "\"");
		}

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeRTProviderProperty(cur_provider);

	// reset current provider


	}

	private void applyChanges(HttpServletRequest req) throws Exception {
		// current provider - raise an exception if not found
		String cur_provider = getStringParameter(req, "cur_provider", false);
		boolean is_alien = (getStringParameter(req, "is_alien", false).equalsIgnoreCase("true") ? true:false);
		if (is_alien) {

			// if alien save empty property and return
			HtRTProviderProp prop_empty = new HtRTProviderProp();
			prop_empty.setAlien(true);
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setRTProviderProperty(cur_provider, prop_empty);
			return;
		}

		// properies to update
		//List<HtPair<String, String>> properties = getStringPairListParameter(req, "properties", true);
		List<HtPair<String, String>> properties = ExportImportXGridUtils.getAsPropertiesList(getStringParameter(req, "properties", true));

		// other parameters
		HtRTProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(cur_provider);
	

		

		
		String provider_class = getStringParameter(req, "provider_class", false);
		Map< String, File > classes  = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_queryAvailableProviders();
		if (!classes.containsKey(provider_class)) {
			throw new HtException(getContext(), "applyChanges", "Class: \"" + provider_class + "\" is not valid RT Provider class");
		}

		prop.setProviderClass(provider_class);

		//
		prop.setAlien(false);
		prop.setHourShift( (int) getIntParameter(req, "hour_shift", false) );
		prop.setSignDigits ((int) getIntParameter(req, "sign_digits", false));

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
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setRTProviderProperty(cur_provider, prop);

	}

	private void readProviderProperty(HttpServletRequest req, String cur_provider_passed, boolean read_param) throws Exception {
		// list of providers
		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();

		// list is empty
		if (providers.size() <= 0) {
			return;
		}

		// current provider:
		String cur_provider = null;


		if (cur_provider_passed == null) {
			if (read_param) {
				cur_provider = getStringParameter(req, "cur_provider", true);
			}
		} else {
			cur_provider = cur_provider_passed;
		}

		// fisr parameter if no current
		if (cur_provider == null || cur_provider.length() <= 0) {
			cur_provider = (String) providers.toArray()[0];
		}

		// provider property
		HtRTProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(cur_provider);

		setStringSessionValue(getUniquePageId(), req, "provider_list", HtWebUtils.createOptionList(cur_provider, providers, false));

		StringBuilder out = new StringBuilder();
		Map< String, File > classes = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_queryAvailableProviders();

		String resolvedProviderClass = null;
		for (Iterator<String> it = classes.keySet().iterator(); it.hasNext();) {
			String value = it.next();
			boolean is_internal = (classes.get(value)==null);
			String style_str = (is_internal ? "background-color:khaki" : "");


			if (value.equals(prop.getProviderClass())) {
				out.append("<option selected invalid=false style='").append(style_str).append("'>").append(value).append("</option>");
				resolvedProviderClass = prop.getProviderClass();
			} else {
				out.append("<option invalid=false style='").append(style_str).append("'>").append(value).append("</option>");
			}
		}

		// if the class not found
		if (resolvedProviderClass == null) {

			// only if we have initialized one
			if (prop.getProviderClass().length() > 0) {
				out.append("<option selected style='background-color:orange' invalid=true>").append(prop.getProviderClass()).append(" - ???</option>");
			}
		}


		setStringSessionValue(getUniquePageId(), req, "provider_classes", out.toString());

		setStringSessionValue(getUniquePageId(), req, "provider_parameters", HtWebUtils.createHtmlParametersTable(prop.getParameters()));

		// export import hours
		setStringSessionValue(getUniquePageId(), req, "shift_hours", HtWebUtils.createOptionListFromRange(-12,12,prop.getHourShift()));
		setStringSessionValue(getUniquePageId(), req, "sign_digits", String.valueOf(prop.getSignDigits()));

		//
		if (prop.isAlien()) {
			setStringSessionValue(getUniquePageId(), req, "is_alien_chb", "CHECKED");
		}
		else {
			setStringSessionValue(getUniquePageId(), req, "is_alien_chb", "");
		}

	}
}
