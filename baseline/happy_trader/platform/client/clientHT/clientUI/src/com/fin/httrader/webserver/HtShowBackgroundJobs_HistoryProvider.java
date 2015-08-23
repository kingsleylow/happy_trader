/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.interfaces.HtCommonRowSelector;

import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.hlpstruct.CommonLog;
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
public class HtShowBackgroundJobs_HistoryProvider extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml; charset=UTF-8");

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
						if (pheader.getFieldName(i).equalsIgnoreCase("start_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}
						
						if (pheader.getFieldName(i).equalsIgnoreCase("finish_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}

						//

						

						/*
						if (pheader.getFieldName(i).equalsIgnoreCase("result_string")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return HtUtils.prepareValidXml(sval);
												}
											});
						}
						 */




					}	// end loop

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


			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBackgroundJobs_Callback(
							begDate,
							endDate,
							page_num,
							read_rows_per_page,
							dataselector);




		} catch (Throwable e) {

			initErrorXmlInfo(req, e);


		}
	}

//
	private void sendXmlHeader(ServletOutputStream ostrm) throws Exception {
		ostrm.print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
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
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("string_id")));
		ostrm.print("</cell>");


		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("type_id")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("start_time")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("finish_time")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(row.getFormattedValue(row.getHeader().getFieldId("result_code")));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print("<![CDATA[");
		ostrm.write(HtStringByteConverter.encodeUTF8(row.getFormattedValue(row.getHeader().getFieldId("result_string"))));
		ostrm.print("]]>");
		ostrm.print("</cell>");

		ostrm.print("</row>");
	}
}
