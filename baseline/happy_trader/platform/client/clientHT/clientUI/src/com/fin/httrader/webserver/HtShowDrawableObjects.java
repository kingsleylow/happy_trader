/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtShowDrawableObjects extends HtServletsBase {
	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 300;

	public int getPageSize()
	{
		return ROWS_PER_PAGE;
	}

	public int getNumberPagesToShow()
	{
		return NUMBER_OF_PAGES;
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			this.generateUniquePageId();

			initProfilesAndSymbols(req);

			
		//


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		
		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);
		HtAjaxCaller ajaxCaller = new HtAjaxCaller();

		try {
			

			
			// all these can be lengthy start another thread
			final ServletOutputStream ou = res.getOutputStream();
			String operation = getStringParameter(req, "operation", false);
			if (operation.equalsIgnoreCase("get_last_runname")) {
				String profilename = getStringParameter(req, "profilename", false);
				String last_drawable = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getLastBrokerRunName_FromDrawable(profilename);
				if (last_drawable!=null) {
					ajaxCaller.setData(last_drawable);
				} else {
					ajaxCaller.setData("");
				}

				ajaxCaller.setOk();

			}
			else
				throw new HtException(getContext(), "initialize_Post", "Invalid operation");

		  //


		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
			
		}

		ajaxCaller.serializeToXml(res);
		
		return true;
		
	}

	/*
	 * Helpers
	 */

	private void initProfilesAndSymbols(HttpServletRequest req) throws Exception {
	

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();
		setStringSessionValue(getUniquePageId(), req, "all_profiles", HtWebUtils.createOptionList(null, profiles, true));

		Map<String, List<HtPair<String, String>>> profProvSymbMap= new HashMap<String, List<HtPair<String, String>>>();


		for (Iterator<String> it = profiles.iterator(); it.hasNext();) {
			String profile_i = it.next();

			// symbols for that profile
			List<HtPair<String, String>> provSymbols = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromDrawable(profile_i);

			profProvSymbMap.put(profile_i, provSymbols);

		}

		setStringSessionValue(getUniquePageId(), req, "symbols_for_each_profile", HtWebUtils.createJScriptForProfileProviderMap(profProvSymbMap));
		setStringSessionValue(getUniquePageId(), req, "all_obj_types", HtWebUtils.createJScriptMappedList( HtDrawableObject.getAllTypesMap(), true));
	}

}
