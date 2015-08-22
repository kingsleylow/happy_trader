/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;

import java.io.IOException;
import java.util.HashSet;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerSessions_HistoryProvider extends HtServletsBase  {
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml; charset=windows-1251");
		forceNoCache(res);

		try {

			// set this page UID
			setUniquePageId(getStringParameter(req, "page_uid", false));
			clearErrorXmlInfo(req);

			String begDateStr = getStringParameter(req, "bdate", true);
			String endDateStr = getStringParameter(req, "edate", true);

			long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
			long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);

			int page_num = (int) getIntParameter(req, "read_page_num", false);
			int read_rows_per_page = (int) getIntParameter(req, "read_rows_per_page", false);



			// load history data
			final HttpServletResponse fres = res;
			final HttpServletRequest freq = req;
			final String cookie = getUniquePageId();
		

			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {



				public void newHeaderArrived(CommonHeader pheader) throws Exception {

					// initialize our header
					// that's the first call

					sendXmlHeader(fres);

					// initialize formatters
					for (int i = 1; i <= pheader.getSize(); i++) {

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("create_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						};
						//



					}; // end loop

				}

				public void newRowArrived(CommonRow row) throws Exception {
					sendXmlRow( cookie, fres, freq, row);
				}

				public void onSelectionFinish(long cntr) throws Exception {
					sendXmlFooter(fres);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerSession_Callback(
							begDate,
							endDate,
							page_num,
							read_rows_per_page,
							dataselector);




		} catch (Throwable e) {

			//writeHtmlErrorToOutput(res, e);
			// must be in a separate place
			initErrorXmlInfo(req,e);


		}
	}

//
	private void sendXmlHeader(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("<?xml version=\"1.0\"?>");
		res.getOutputStream().print("<rows>");

	}

	private void sendXmlFooter(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("</rows>");

	
	}

	private void sendXmlRow( String cookie, HttpServletResponse res, HttpServletRequest req, HtCommonRowSelector.CommonRow row) throws Exception {

		String session_id = row.getFormattedValue(row.getHeader().getFieldId("session_id"));

		res.getOutputStream().print("<row id=\"");
		res.getOutputStream().print(session_id);
		res.getOutputStream().print("\">");

		// need to restore that info
		Object obj = getObjectSessionValue(cookie, req, "broker_sessions_set");

		if (obj != null) {
			HashSet<String> sobj = (HashSet<String>)obj;
			if (sobj.contains(session_id)) {
				res.getOutputStream().print("<cell>1</cell>");
			}
			else {
				res.getOutputStream().print("<cell>0</cell>");
			}
		}
		else {
			res.getOutputStream().print("<cell>0</cell>");
		}

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(session_id);
		res.getOutputStream().print("</cell>");

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("create_time")));
		res.getOutputStream().print("</cell>");

		res.getOutputStream().print("<cell><![CDATA[");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("connect_string")));
		res.getOutputStream().print("]]></cell>");

		String value = row.getFormattedValue(row.getHeader().getFieldId("is_simulation"));
		res.getOutputStream().print("<cell><![CDATA[");
		if (value.equals("0")) {

			// not checked
			res.getOutputStream().print("<input type=checkbox disabled=true></input>");
		}
		else {
			
			res.getOutputStream().print("<input type=checkbox checked=true disabled=true></input>");
		}

		
		res.getOutputStream().print("]]></cell>");

	
		res.getOutputStream().print("</row>");
	}

}
