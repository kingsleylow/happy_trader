/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.CloseReason;
import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.OrderValidity;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import java.io.IOException;
import java.util.HashSet;
import java.util.List;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerDialog_BrkResp_HistoryProvider extends HtServletsBase {

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

			// get session ids via POST parameter
			List<String> sessIds = getStringListParameter(req, "session_ids", true);


			// load history data
			final HttpServletResponse fres = res;
			final HttpServletRequest freq = req;
			final LongParam id = new LongParam();


			// instantiate an anonymous class
			HtCommonRowSelector dataselector = new HtCommonRowSelector() {

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

								@Override
												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return "<a href='#' onclick=\"brdet('" + sval + "');\">" + sval + "</a>";
												}
											});
						}

						//
						if (pheader.getFieldName(i).equalsIgnoreCase("parent_id")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

								@Override
												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													if (sval.equals(Uid.INVALID_UID)) {
														return "no parent order";
													} else {
														return sval;
													}

												}
											});
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("create_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("op_expiration_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}


						if (pheader.getFieldName(i).equalsIgnoreCase("op_issue_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}


						if (pheader.getFieldName(i).equalsIgnoreCase("op_time")) {
							pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
						}



						if (pheader.getFieldName(i).equalsIgnoreCase("is_simulation")) {
							pheader.addFormatter(i, booleanFormatter);
						}

						//



						//

						if (pheader.getFieldName(i).equalsIgnoreCase("op_stop_price")) {
							pheader.addFormatter(i, priceFormatter);
						}
						if (pheader.getFieldName(i).equalsIgnoreCase("op_price")) {
							pheader.addFormatter(i, priceFormatter);
						}
						if (pheader.getFieldName(i).equalsIgnoreCase("op_tp_price")) {
							pheader.addFormatter(i, priceFormatter);
						}
						if (pheader.getFieldName(i).equalsIgnoreCase("op_sl_price")) {
							pheader.addFormatter(i, priceFormatter);
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("op_validity_flag")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OrderValidity.getOrderValidityName(HtUtils.parseInt(sval));
												}
											});
						}

						if (pheader.getFieldName(i).equalsIgnoreCase("op_pos_type")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OrderType.getTypeName(HtUtils.parseInt(sval));
												}
											});
						}

						////
						if (pheader.getFieldName(i).equalsIgnoreCase("result_code")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OperationResult.getOperationResultName(HtUtils.parseInt(sval));
												}
											});
						}



						if (pheader.getFieldName(i).equalsIgnoreCase("result_detail_code")) {
							pheader.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
													return OperationDetailResult.getOperationDetailresultName(HtUtils.parseInt(sval));
												}
											});
						}




						sendInit_XmlField(fres, pheader.getDisplayName(i));

					} // end loop

					sendInit_XmlFooter(fres);

				}

				public void newRowArrived(CommonRow row) throws Exception {

					id.setLong(id.getLong() + 1);
					sendXmlRow(id.getLong(), fres, row);
				}

				public void onSelectionFinish(long cntr) throws Exception {
					sendXmlFooter(id.getLong(), fres);
				}
			};

			// do load
			// execute that
			// read all as one chunk
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerResponses_Callback(
							sessIds,
							null,
							null,
							null,
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

		//
		res.flushBuffer();
	}

	//
	private void sendXmlHeader(HttpServletResponse res) throws Exception {
		res.getOutputStream().print("<table  gridWidth='100%' gridHeight='100%' lightnavigation='false' id='dataTblObj_Responses_Tbl' width=100% class='x8'>");
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
		res.getOutputStream().print("<tr style='display:none' id='dataTblObj_Responses_rowsnum'><td>");
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

			// here must encode because response encoding is UTF-8
			res.getOutputStream().write(HtStringByteConverter.encodeUTF8(row.getFormattedValue(k)));
			//res.getOutputStream().print(row.getFormattedValue(k));
			res.getOutputStream().print("</td>");
		}

		res.getOutputStream().print("</tr>");
	}
}
