/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;


import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowAlertData_HistoryProvider extends HtServletsBase {

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

			// paging
			// -1 means all
			int page_num = (int) getIntParameter(req, "read_page_num", false);
			int read_rows_per_page = (int) getIntParameter(req, "read_rows_per_page", false);

			String content_filter = getStringParameter(req, "content_filter", true);
			String run_name_filter = getStringParameter(req, "run_name_filter", true);
			String session_filter = getStringParameter(req, "session_filter", true);
			int priority_filer = (int) getIntParameter(req, "priority_filter", true);

			// load history data
			final HttpServletResponse fres = res;
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
						if (pheader.getFieldName(i).equalsIgnoreCase("alert_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime_FullFormat.getInstance());
						}
						
						//

						if (pheader.getFieldName(i).equalsIgnoreCase("priority")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return AlertPriorities.getAlertpriorityByName(HtUtils.parseInt(sval));
												}
											});
						}

						/*
						if (pheader.getFieldName(i).equalsIgnoreCase("data")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return restrictLength(sval, 100);
												}
											});
						}
						*/
						

					} // end loop

				}

				public void newRowArrived(CommonRow row) throws Exception {
					sendXmlRow(ostrm, row);
				}

				public void onSelectionFinish(long cntr) throws Exception {
					sendXmlFooter(ostrm);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showAlerts_Callback(
							content_filter,
							run_name_filter,
							session_filter,
							priority_filer,
							begDate,
							endDate,
							page_num,
							read_rows_per_page,
							dataselector, "asc", null);




		} catch (Throwable e) {

			initErrorXmlInfo(req, e);


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

	private void sendXmlRow(ServletOutputStream ostrm, HtCommonRowSelector.CommonRow row) throws Exception {

		String id_str = row.getFormattedValue(row.getHeader().getFieldId("id"));

		ostrm.print("<row id=\"");
		ostrm.print(id_str);
		ostrm.print("\">");

		ostrm.print("<cell>");
		String alertTime_str = row.getFormattedValue(row.getHeader().getFieldId("alert_time"));
		ostrm.print(alertTime_str);
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		String priorityStr = row.getFormattedValue(row.getHeader().getFieldId("priority"));
		ostrm.print(priorityStr);
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("run_name")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("source")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("alg_brk_pair")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("thread")));
		ostrm.print("</cell>");

		
		ostrm.print("<cell><![CDATA[");
		ostrm.print("<a href='javascript:void(0)' ");
		ostrm.print(" onclick='");
		ostrm.print("d(\"");
		ostrm.print(id_str);
		ostrm.print("\");");
		ostrm.print("' >");
		ostrm.print(id_str);
		ostrm.print("</a>");
		ostrm.print("]]></cell>");

	
		ostrm.print("<cell><![CDATA[");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("small_data")));
		ostrm.print("]]></cell>");

		ostrm.print("<cell><![CDATA[");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("data")));
		ostrm.print("]]></cell>");


		ostrm.print("</row>");
	}
}
