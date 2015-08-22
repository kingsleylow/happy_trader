/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.SessionEventType;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;

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
public class HtBrokerDetailRunNames_HistoryProvider extends HtServletsBase {

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


			String run_name_filter = getStringParameter(req, "run_name_filter", true);



			// load history data
			final HttpServletResponse fres = res;


		
			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {

				@Override
				public void newHeaderArrived(CommonHeader pheader) throws Exception {

					// initialize our header
					// that's the first call

					sendXmlHeader(fres);

					// initialize formatters
					for (int i = 1; i <= pheader.getSize(); i++) {

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("op_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						};
						//

						if (pheader.getFieldName(i).equalsIgnoreCase("operation")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

								@Override
												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return SessionEventType.getSessionEventTypeName(HtUtils.parseInt(sval));
												}
											});
						};

					



					}
					; // end loop

				}

				@Override
				public void newRowArrived(CommonRow row) throws Exception {
					sendXmlRow(fres, row);
				}

				@Override
				public void onSelectionFinish(long cntr) throws Exception {
					sendXmlFooter(fres);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerDetailRunNames_Callback(
							run_name_filter,
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
	private void sendXmlHeader(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("<?xml version=\"1.0\"?>");
		res.getOutputStream().print("<rows>");

	}

	private void sendXmlFooter(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("</rows>");
	}

	private void sendXmlRow(HttpServletResponse res, HtCommonRowSelector.CommonRow row) throws Exception {

		String session_id = row.getFormattedValue(row.getHeader().getFieldId("session_id"));

		res.getOutputStream().print("<row id=\"");
		res.getOutputStream().print(session_id);
		res.getOutputStream().print("\">");

	

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(session_id);
		res.getOutputStream().print("</cell>");

		res.getOutputStream().print("<cell><![CDATA[");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("run_name")));
		res.getOutputStream().print("]]></cell>");

		res.getOutputStream().print("<cell><![CDATA[");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("alg_brk_pair")));
		res.getOutputStream().print("]]></cell>");

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("thread")));
		res.getOutputStream().print("</cell>");

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("op_time")));
		res.getOutputStream().print("</cell>");

		res.getOutputStream().print("<cell>");
		res.getOutputStream().print(row.getFormattedValue(row.getHeader().getFieldId("operation")));
		res.getOutputStream().print("</cell>");


		res.getOutputStream().print("</row>");
	}
}
