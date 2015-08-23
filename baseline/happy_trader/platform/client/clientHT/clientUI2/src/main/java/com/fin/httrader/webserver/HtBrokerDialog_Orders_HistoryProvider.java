/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.OrderValidity;
import com.fin.httrader.hlpstruct.PriceConst;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.*;
import java.io.IOException;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 * This fetches broker orders
 */
public class HtBrokerDialog_Orders_HistoryProvider extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=UTF-8");
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

			List<String> sessIds = getStringListParameter(req, "session_ids", true);

			// load history data
			final HttpServletResponse fres = res;
			final HttpServletRequest freq = req;
			final LongParam id = new LongParam();


			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {

				@Override
				public void newHeaderArrived(CommonHeader pheader) throws Exception {

					// initialize our header
					// that's the first call

					sendXmlHeader(fres);

					HtCommonRowSelector.HtCommonFieldFormatter priceFormatter = HtCommonFieldFormatter_PriceFormatter.getInstance();
					HtCommonRowSelector.HtCommonFieldFormatter booleanFormatter = HtCommonFieldFormatter_Boolean.getInstance();

					sendInit_XmlHeader(fres);
					// initialize formatters
					for (int i = 1; i <= pheader.getSize(); i++) {

						if (pheader.getFieldName(i).equalsIgnoreCase("id")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return "<a href='#' onclick=\"ordet('" + sval + "');\">" + sval + "</a>";
												}
											});
						}

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("create_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}



						if (pheader.getFieldName(i).equalsIgnoreCase("or_issue_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}


						if (pheader.getFieldName(i).equalsIgnoreCase("or_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}


						if (pheader.getFieldName(i).equalsIgnoreCase("or_expiration_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}


						if (pheader.getFieldName(i).equalsIgnoreCase("or_validity_flag")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OrderValidity.getOrderValidityName(HtUtils.parseInt(sval));
												}
											});
						}

						//

						if (pheader.getFieldName(i).equalsIgnoreCase("or_price")) {
							pheader.addFormatter(i, priceFormatter);
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("or_stop_price")) {
							pheader.addFormatter(i, priceFormatter);
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("or_price_type")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return PriceConst.getPriceConstName(HtUtils.parseInt(sval));
												}
											});

						}

						if (pheader.getFieldName(i).equalsIgnoreCase("or_stop_price_type")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return PriceConst.getPriceConstName(HtUtils.parseInt(sval));
												}
											});

						}

						if (pheader.getFieldName(i).equalsIgnoreCase("or_sl_price")) {
							pheader.addFormatter(i, priceFormatter);
						}
						if (pheader.getFieldName(i).equalsIgnoreCase("or_tp_price")) {
							pheader.addFormatter(i, priceFormatter);
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("is_simulation")) {
								pheader.addFormatter(i, booleanFormatter);
						}

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("order_type")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OrderType.getTypeName(HtUtils.parseInt(sval));
												}
											});
						}


						sendInit_XmlField(fres, pheader.getDisplayName(i));

					} // end loop

					sendInit_XmlFooter(fres);

				}

				@Override
				public void newRowArrived(CommonRow row) throws Exception {

					id.setLong(id.getLong() + 1);
					sendXmlRow(id.getLong(), fres, row);
				}

				@Override
				public void onSelectionFinish(long cntr) throws Exception {
				
					sendXmlFooter(id.getLong(), fres);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showOrders_Callback(
							sessIds,
							begDate,
							endDate,
							page_num,
							read_rows_per_page,
							dataselector);




		} catch (Throwable e) {

			//writeHtmlErrorToOutput(res, e);
			// must be in a separate place
			initErrorXmlInfo(req, e);


		}

		res.flushBuffer();
	}

	// ------------
	
	private void sendXmlHeader(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("<table  gridWidth='100%' gridHeight='100%' lightnavigation='false'  id='dataTblObj_Orders_Tbl' width=100% class='x8'>");
	}

	private void sendInit_XmlHeader(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("<tr class='x3' bgcolor='buttonface'>");
	}

	private void sendInit_XmlField(HttpServletResponse res, String fieldname) throws Exception {
		res.getOutputStream().print("<td type='ro' NOWRAP=true align=left>");
		res.getOutputStream().print(fieldname);
		res.getOutputStream().print("</td>");

	}

	private void sendInit_XmlFooter(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("</tr>");
	}

	private void sendXmlFooter(long lastid, HttpServletResponse res) throws Exception {

		res.getOutputStream().print("<tr style='display:none' id='dataTblObj_Orders_rowsnum'><td>");
		res.getOutputStream().print(lastid);
		res.getOutputStream().print("</td></tr>");
		res.getOutputStream().print("</table>");
	}

	private void sendXmlRow(long id, HttpServletResponse res, HtCommonRowSelector.CommonRow row) throws Exception {

		res.getOutputStream().print("<tr align=left internal_id='");
		res.getOutputStream().print(id);
		res.getOutputStream().print("'>");

		for (int k = 1; k <= row.getHeader().getSize(); k++) {
			res.getOutputStream().print("<td NOWRAP=true>");

			//String fieldName = row.getHeader().getFieldName(k);
			res.getOutputStream().write(HtStringByteConverter.encodeUTF8(row.getFormattedValue(k)));
			//res.getOutputStream().print(row.getFormattedValue(k));
			res.getOutputStream().print("</td>");
		}

		res.getOutputStream().print("</tr>");
	}
}
