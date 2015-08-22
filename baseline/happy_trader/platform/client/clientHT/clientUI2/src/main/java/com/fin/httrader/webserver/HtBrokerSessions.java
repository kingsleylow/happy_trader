/*
 * HtBrokerSessions.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerSessions extends HtServletsBase {

	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 300;

	/** Creates a new instance of HtBrokerSessions */
	public HtBrokerSessions() {
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

	
}
