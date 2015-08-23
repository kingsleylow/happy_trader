/*
 * HtShowHistoryData.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
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
 * @author Administrator
 */
public class HtShowHistoryData extends HtServletsBase {

	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 300;
	
	public static final int DIG_NUM = 4;

	public int getPageSize()
	{
		return ROWS_PER_PAGE;
	}

	public int getNumberPagesToShow()
	{
		return NUMBER_OF_PAGES;
	}
	
	public String getContext() {
		return this.getClass().getSimpleName();
	}


	// ----------------------------------------------------
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			this.generateUniquePageId();

			initProfilesAndSymbols(req);

			setStringSessionValue(getUniquePageId(), req, "dig_num", String.valueOf(DIG_NUM));

		//


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;
	}

	/**
	Helpers
	 */
	private void initProfilesAndSymbols(HttpServletRequest req) throws Exception {
		

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();

	

		
		setStringSessionValue(getUniquePageId(), req, "all_profiles", HtWebUtils.createOptionList(null, profiles, true));
		Map<String, List<HtPair<String, String>>> profProvSymbMap= new HashMap<String, List<HtPair<String, String>>>();

		
		for (Iterator<String> it = profiles.iterator(); it.hasNext();) {
			String profile_i = it.next();

		
			// symbols for that profile
			List<HtPair<String, String>> provSymbols = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory(profile_i);

			profProvSymbMap.put(profile_i, provSymbols);

		}



		setStringSessionValue(getUniquePageId(), req, "symbols_for_each_profile", HtWebUtils.createJScriptForProfileProviderMap(profProvSymbMap));

	}

	
}
