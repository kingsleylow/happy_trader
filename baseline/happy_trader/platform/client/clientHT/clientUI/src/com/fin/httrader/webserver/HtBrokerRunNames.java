/*
 * HtBrokerRunNames.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames extends HtServletsBase {

	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 300;

	/** Creates a new instance of HtBrokerRunNames */
	public HtBrokerRunNames() {
	}

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

			String operation = getStringParameter(req, "operation", false);
			setStringSessionValue(getUniquePageId(), req, "operation", operation);
			
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

			// generate new Uid and set initial values
			setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);
			setStringSessionValue(getUniquePageId(), req, "operation", operation);

			if (operation.equalsIgnoreCase("update_user_data")) {
				updateUserData(req);
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	private void updateUserData(HttpServletRequest req) throws Exception {
		//long runname_id = getIntParameter(req, "runname_id", false);
		//String user_data = getStringParameter(req, "user_data", true);

		List<HtPair<String, String>> userdatalist = getStringPairListParameter(req, "user_data", true );

		Map<String, String> dataMap = new HashMap<String, String>();
		

		for(int i = 0; i < userdatalist.size(); i++) {
			HtPair<String, String> pair_i = userdatalist.get(i);
			dataMap.put(pair_i.first, pair_i.second);
		}

		HtCommandProcessor.instance().get_HtDatabaseProxy().remote_updateRunNameTableWithUserData(dataMap);
		
	}
	
}
