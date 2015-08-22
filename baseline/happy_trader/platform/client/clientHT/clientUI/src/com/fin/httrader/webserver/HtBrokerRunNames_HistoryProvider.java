/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.*;
import java.io.IOException;
import java.util.HashSet;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames_HistoryProvider extends HtServletsBase {


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
			final LongParam id = new LongParam();
			final String cookie = getUniquePageId();
			final ServletOutputStream ostrm = res.getOutputStream();


			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {



				public void newHeaderArrived(CommonHeader pheader) throws Exception {

					// initialize our header
					// that's the first call

					sendXmlHeader(ostrm);

					// initialize formatters
					for (int i = 1; i <= pheader.getSize(); i++) {

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("op_first_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						};

						/*
						if (pheader.getFieldName(i).equalsIgnoreCase("run_name")) {
							pheader.addFormatter(i, 
									new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													if (sval!=null && sval.length() > 0)
														return sval;
													else
														return "[ EMPTY RUN NAME ]";
												}
											}
							);
						};
						*/


						//

						

					}; // end loop

				}

				public void newRowArrived(CommonRow row) throws Exception {
					id.setLong( id.getLong() + 1);
					sendXmlRow(cookie, id.getLong(), ostrm, freq, row);
				}

				public void onSelectionFinish(long cntr) throws Exception {
					
					sendXmlFooter(ostrm);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerRunNames_Callback(
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
	private void sendXmlHeader(ServletOutputStream ostrm) throws Exception {
		ostrm.print("<?xml version=\"1.0\"?>");
		ostrm.print("<rows>");

	}

	private void sendXmlFooter(ServletOutputStream ostrm) throws Exception {
		ostrm.print("</rows>");
	}

	private void sendXmlRow(String cookie, long id, ServletOutputStream ostrm, HttpServletRequest req, HtCommonRowSelector.CommonRow row) throws Exception {

		ostrm.print("<row id=\"");
		ostrm.print(id);
		ostrm.print("\">");

		String run_name = row.getFormattedValue(row.getHeader().getFieldId("run_name"));
		
		
		// need to restore that info
		Object obj = getObjectSessionValue(cookie, req, "broker_runs_set");

		if (obj != null) {
			HashSet<String> sobj = (HashSet<String>)obj;
			if (sobj.contains(run_name)) {
				ostrm.print("<cell>1</cell>");
			}
			else {
				ostrm.print("<cell>0</cell>");
			}
		}
		else {
			ostrm.print("<cell>0</cell>");
		}
		
		
		ostrm.print("<cell><![CDATA[");
		ostrm.print("<a href='#' onclick='navigate_broker_detail_run_name(\"");
		ostrm.print(run_name);
		ostrm.print("\")'>");
		ostrm.print(run_name);
		ostrm.print("</a>");
		ostrm.print("]]></cell>");

		String opFirstTimeStr = row.getFormattedValue(row.getHeader().getFieldId("op_first_time"));
		ostrm.print("<cell><![CDATA[");
		ostrm.print("<div class='basic' style='cursor:hand' title='");
		ostrm.print(opFirstTimeStr);
		ostrm.print("' rel='/htBrokerRunNames_showOperationsNumber.jsp?run_name=");
		ostrm.print(run_name);
		ostrm.print("'><b>");
		ostrm.print(opFirstTimeStr);
		ostrm.print("</b></div>");
		ostrm.print("]]></cell>");
	
		ostrm.print("<cell><![CDATA[");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("comment")));
		ostrm.print("]]></cell>");

		// user data
		ostrm.print("<cell><![CDATA[");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("user_data")));
		ostrm.print("]]></cell>");

		ostrm.print("<cell><![CDATA[");
		ostrm.print(run_name);
		ostrm.print("]]></cell>");
		
		ostrm.print("</row>");
	}

}
