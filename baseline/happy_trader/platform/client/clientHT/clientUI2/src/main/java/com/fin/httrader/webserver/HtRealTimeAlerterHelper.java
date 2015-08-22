/*
 * HtRealTimeAlerterHelper.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRealTimeAlerterHelper extends HtServletsBase {

	private final int HISTORY_ROWS_PER_PAGE=500;

	// initially suggest to show that number of pages
	private final int PAGES_TO_SHOW = 40;

	/** Creates a new instance of HtRealTimeAlerterHelper */
	public HtRealTimeAlerterHelper()
	{
	}

	public int getPageSize()
	{
		return HISTORY_ROWS_PER_PAGE;
	}

	public int getNumberPagesToShow()
	{
		return PAGES_TO_SHOW;
	}

	public String getContext()
	{
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException
	{
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			String content_filter = getStringParameter(req, "content_filter", true);
			String run_name_filter = getStringParameter(req, "run_name_filter", true);
			String session_filter = getStringParameter(req, "session_filter", true);
			String begin_date_filter = getStringParameter(req, "bdate", true);
			int priority_filer = (int)getIntParameter(req, "priority_filter", true);

			long begin_date_filter_l = HtDateTimeUtils.parseDateTimeParameter(begin_date_filter, false);


			// empty everything
			setStringSessionValue(getUniquePageId(), req, "content_filter", content_filter);
			setStringSessionValue(getUniquePageId(), req, "run_name_filter", run_name_filter);
			setStringSessionValue(getUniquePageId(), req, "session_filter", session_filter);
			setStringSessionValue(getUniquePageId(), req, "priority_filter_s", AlertPriorities.getAlertpriorityByName(priority_filer));
			setStringSessionValue(getUniquePageId(), req, "priority_filter", String.valueOf( priority_filer ));
			setStringSessionValue(getUniquePageId(), req, "begin_date_s", begin_date_filter);
			setStringSessionValue(getUniquePageId(), req, "bdate_l", String.valueOf(begin_date_filter_l));

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}
}
