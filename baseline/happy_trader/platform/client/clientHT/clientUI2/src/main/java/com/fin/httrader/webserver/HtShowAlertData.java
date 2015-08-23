/*
 * HtShowAlertData.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.AlertPriorities;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowAlertData extends HtServletsBase {

	public static final int NUMBER_OF_PAGES = 200;
	public static final int ROWS_PER_PAGE = 1000;

	public String getContext() {
		return getClass().getSimpleName();
	}

	public HtShowAlertData() {
	}

	public int getPageSize()
	{
		return ROWS_PER_PAGE;
	}

	public int getNumberPagesToShow()
	{
		return NUMBER_OF_PAGES;
	}

	
	// this is for initial navigation
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new Uid and set initial values
			this.generateUniquePageId();

			

			setUpPriorities(req, -1);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		
		return true;
	}

	// --------------------------------------------------

	private void setUpPriorities(HttpServletRequest req, int defPriority) throws Exception
	{

		setStringSessionValue(getUniquePageId(), req, "priorities", HtWebUtils.createOptionMappedList(defPriority,  AlertPriorities.getAllAlertPrioritiesMap()));

	}

	
}
