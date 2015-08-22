/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.interfaces.HtDrawableObjectSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtShowDrawableObjects_HistoryProvider extends HtServletsBase {
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);

		try {

			// set this page UID
			setUniquePageId(getStringParameter(req, "page_uid", false));
			clearErrorXmlInfo(req);

			String begDateStr = getStringParameter(req, "bdate", true);
			String endDateStr = getStringParameter(req, "edate", true);

			String shortTextFilter = getStringParameter(req, "short_text_filter", true);
			String longTextFilter = getStringParameter(req, "long_text_filter", true);
			String runNameFilter = getStringParameter(req, "run_name_filter", true);

			// if provide milliseconds
			final boolean time_in_millisec = getBooleanParameter(req, "time_in_millisec", true);
			boolean allIfMapEmpty = getBooleanParameter(req, "all_if_no_symb", true);

			long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
			long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);

			// paging
			// -1 means all
			long page_num = getIntParameter(req, "read_page_num", false);
			long read_rows_per_page =  getIntParameter(req, "read_rows_per_page", false);



			String profile = getStringParameter(req, "profile", true);
			List<HtPair<String, String> > provSymbMapPage = getStringPairListParameter(req, "ps", true);
			List<Integer> type = getIntListParameter(req, "type", true);

			// load history data
			final HttpServletResponse fres = res;
			final ServletOutputStream ostrm = res.getOutputStream();


			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {

				@Override
				public void newHeaderArrived(CommonHeader pheader) throws Exception {

					// initialize our header
					// that's the first call

					sendXmlHeader(ostrm);

					// init type formatters for all double values
					pheader.addFormatterForTheType(java.sql.Types.DOUBLE,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return HtUtils.formatPriceValue(HtUtils.parseDouble(sval), 4, false);
												}
											} );
					

					// initialize formatters
					for (int i = 1; i <= pheader.getSize(); i++) {

						//
						if (!time_in_millisec) {
							// if string time
							if (pheader.getFieldName(i).equalsIgnoreCase("ttime")) {
								pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime_FullFormat.getInstance());
							}

							if (pheader.getFieldName(i).equalsIgnoreCase("create_ttime")) {
								pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime_FullFormat.getInstance());
							}
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("priority")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return AlertPriorities.getAlertpriorityByName(HtUtils.parseInt(sval));
												}
											});
						}


						//

						if (pheader.getFieldName(i).equalsIgnoreCase("type")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return HtDrawableObject.getDrawableObjectName(HtUtils.parseInt(sval));
												}
											});
						}


					}	// end loop

				}

				@Override
				public void newRowArrived(CommonRow row) throws Exception {
					sendXmlRow(ostrm, row);
				}

				@Override
				public void onSelectionFinish(long cntr) throws Exception {
					sendXmlFooter(ostrm);
				}
			};

			// do load
			// execute that
			// read all as one chunk



			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDrawableObjects_Callback(
							profile,
							provSymbMapPage,
							allIfMapEmpty,
							begDate,
							endDate,
							type,
							shortTextFilter,
							longTextFilter,
							runNameFilter,
							dataselector,
							null,
							page_num,
							read_rows_per_page);




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

		/*
		ostrm.print("<head>");
		for(int i = 1; i < row.getHeader().getSize(); i++) {
			ostrm.print("<column width=\"100\" type=\"ro\" align=\"left\" sort=\"str\">"+row.getHeader().getDisplayName(i)+"</column>");
		}
		
		ostrm.print("</head>");
		*/
		
		ostrm.print("<row id=\"");
		ostrm.print(id_str);
		ostrm.print("\">");

		for(int i = 0; i < row.getHeader().getSize(); i++) {
			String value = row.getFormattedValue(i+1);

			ostrm.print("<cell>");
			ostrm.print("<![CDATA[");
			ostrm.write(HtStringByteConverter.encodeUTF8(value));
			ostrm.print("]]>");
			ostrm.print("</cell>");

		}

		ostrm.print("</row>");

	}
}
