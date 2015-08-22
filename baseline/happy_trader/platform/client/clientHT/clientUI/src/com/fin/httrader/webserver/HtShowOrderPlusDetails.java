/*
 * HtShowOrderPlusDetails.java
 *
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.CloseReason;
import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.OrderValidity;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.PriceConst;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowOrderPlusDetails extends HtServletsBase {

	private static final int MAX_ORDER_ROWS = 13;

	/** Creates a new instance of HtShowOrderPlusDetails */
	public HtShowOrderPlusDetails() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// logging
			//HtUtils.log(Level.INFO, getContext(), "initialize", "Servlet URL: " + req.getRequestURL() + "?" + req.getQueryString() );

			this.generateUniquePageId();

			setStringSessionValue(getUniquePageId(), req, "order_data", "");

			setStringSessionValue(getUniquePageId(), req, "bresp_history_data", "");

			String orderUid_str = getStringParameter(req, "order_uid", false);

			HtCommonRowSelector.CommonHeader orderHeader = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonHeader brokerHeader = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonRow resultOrderRow = new HtCommonRowSelector.CommonRow(orderHeader);
			List<HtCommonRowSelector.CommonRow> resultBrokerSet = new ArrayList<HtCommonRowSelector.CommonRow>();

			Uid orderUid = new Uid(orderUid_str);
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showOrderDetailBrokerResponses(
							orderUid,
							orderHeader,
							brokerHeader,
							resultOrderRow,
							resultBrokerSet);

			// whether we have data

			if (resultOrderRow.getValue(orderHeader.getFieldId("id")) == null) {
				throw new HtException(getContext(), "initialize", "Invalid Order for ID: \"" + orderUid_str + "\"");
			}


			HtCommonRowSelector.HtCommonFieldFormatter priceFormatter = HtCommonFieldFormatter_PriceFormatter.getInstance();
			HtCommonRowSelector.HtCommonFieldFormatter gmtFormatter = HtCommonFieldFormatter_GmtTime.getInstance();
			HtCommonRowSelector.HtCommonFieldFormatter booleanFormatter = HtCommonFieldFormatter_Boolean.getInstance();


			for (int i = 1; i <= orderHeader.getSize(); i++) {
				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_issue_time")) {
					orderHeader.addFormatter(i, gmtFormatter);
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_expiration_time")) {
					orderHeader.addFormatter(i, gmtFormatter);
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_validity_flag")) {
					orderHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										@Override
										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OrderValidity.getOrderValidityName(HtUtils.parseInt(sval));
										}
									});
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_time")) {
					orderHeader.addFormatter(i, gmtFormatter);
				}
				if (orderHeader.getFieldName(i).equalsIgnoreCase("last_connect_time")) {
					orderHeader.addFormatter(i, gmtFormatter);
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("create_time")) {
					orderHeader.addFormatter(i, gmtFormatter);
				}

				//

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_price")) {
					orderHeader.addFormatter(i, priceFormatter);
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_stop_price")) {
					orderHeader.addFormatter(i, priceFormatter);
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_price_type")) {
					orderHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return PriceConst.getPriceConstName(HtUtils.parseInt(sval));
										}
									});

				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_stop_price_type")) {
					orderHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return PriceConst.getPriceConstName(HtUtils.parseInt(sval));
										}
									});

				}


				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_sl_price")) {
					orderHeader.addFormatter(i, priceFormatter);
				}
				if (orderHeader.getFieldName(i).equalsIgnoreCase("or_tp_price")) {
					orderHeader.addFormatter(i, priceFormatter);
				}

				//
				if (orderHeader.getFieldName(i).equalsIgnoreCase("order_type")) {
					orderHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OrderType.getTypeName(HtUtils.parseInt(sval));
										}
									});
				}

				if (orderHeader.getFieldName(i).equalsIgnoreCase("is_simulation")) {
					orderHeader.addFormatter(i, booleanFormatter);
				}

			}

			StringBuilder out = new StringBuilder();

			int total_values = orderHeader.getSize();
			int columns = (int) (total_values / MAX_ORDER_ROWS);
			int div_columns = total_values % MAX_ORDER_ROWS;

			if (div_columns != 0) {
				columns++;
			}



			ArrayList<ArrayList<HtPair<String, String>>> table = new ArrayList<ArrayList<HtPair<String, String>>>();

			// add columns


			for (int i = 0; i < MAX_ORDER_ROWS; i++) {

				// add column
				ArrayList<HtPair<String, String>> row_i = new ArrayList<HtPair<String, String>>();
				table.add(row_i);


				for (int k = 0; k < columns; k++) {

					HtPair<String, String> val = null;

					int idx = MAX_ORDER_ROWS * k + i + 1;

					if (idx <= total_values) {
						val = new HtPair<String, String>(orderHeader.getDisplayName(idx), resultOrderRow.getFormattedValue(idx));

					}

					row_i.add(val);

				}

			}


			// create HTML on order values

			for (int i = 0; i < table.size(); i++) {
				ArrayList<HtPair<String, String>> row_i = table.get(i);
				out.append("<tr>");

				for (int j = 0; j < row_i.size(); j++) {
					HtPair<String, String> p = row_i.get(j);

					String f = (p != null ? p.first : "");
					String s = (p != null ? p.second : "");
					out.append("<td align=right class=x3 bgcolor='buttonface' width=150px>");
					out.append(f);
					out.append("</td>");
					out.append("<td align=left width=150px>");
					out.append(s);
					out.append("</td>");

				}

				out.append("<td></td>");
				out.append("</tr>");

			}

			setStringSessionValue(getUniquePageId(), req, "order_data", out.toString());



			// fill in the detail table

			out.setLength(0);
			out.append(createBrokerResponsesHtmlHeader(brokerHeader));
			out.append(createBrokerResponseHtmlPage(brokerHeader, resultBrokerSet));
			setStringSessionValue(getUniquePageId(), req, "bresp_history_data", out.toString());




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			//writeStackTrace(req,e);
			
			return false;
		}
		
		return true;
	}

	private String createBrokerResponsesHtmlHeader(HtCommonRowSelector.CommonHeader header) {


		StringBuilder out = new StringBuilder();
		out.append("<tr>");

		for (int i = 1; i <= header.getSize(); i++) {
			out.append("<td nowrap='true' type='ro'>");
			out.append(header.getDisplayName(i));
			out.append("</td>");
		}

		HtCommonRowSelector.HtCommonFieldFormatter priceFormatter = HtCommonFieldFormatter_PriceFormatter.getInstance();
		HtCommonRowSelector.HtCommonFieldFormatter gmtFormatter = HtCommonFieldFormatter_GmtTime.getInstance();
		HtCommonRowSelector.HtCommonFieldFormatter booleanFormatter = HtCommonFieldFormatter_Boolean.getInstance();

		for (int i = 1; i <= header.getSize(); i++) {

			//
			if (header.getFieldName(i).equalsIgnoreCase("op_expiration_time")) {
				header.addFormatter(i, gmtFormatter);
			}

			if (header.getFieldName(i).equalsIgnoreCase("op_issue_time")) {
				header.addFormatter(i, gmtFormatter);
			}


			if (header.getFieldName(i).equalsIgnoreCase("op_time")) {
				header.addFormatter(i, gmtFormatter);
			}
			if (header.getFieldName(i).equalsIgnoreCase("create_time")) {
				header.addFormatter(i, gmtFormatter);
			}

			if (header.getFieldName(i).equalsIgnoreCase("is_simulation")) {
				header.addFormatter(i, booleanFormatter);
			}






			if (header.getFieldName(i).equalsIgnoreCase("op_stop_price")) {
				header.addFormatter(i, priceFormatter);
			}
			if (header.getFieldName(i).equalsIgnoreCase("op_price")) {
				header.addFormatter(i, priceFormatter);
			}
			if (header.getFieldName(i).equalsIgnoreCase("op_tp_price")) {
				header.addFormatter(i, priceFormatter);
			}
			if (header.getFieldName(i).equalsIgnoreCase("op_sl_price")) {
				header.addFormatter(i, priceFormatter);
			}

			if (header.getFieldName(i).equalsIgnoreCase("op_validity_flag")) {
				header.addFormatter(i,
								new HtCommonRowSelector.HtCommonFieldFormatter() {

									public String format(HtCommonRowSelector.CommonRow row, String sval) {
										return OrderValidity.getOrderValidityName(HtUtils.parseInt(sval));
									}
								});
			}

			if (header.getFieldName(i).equalsIgnoreCase("op_pos_type")) {
				header.addFormatter(i,
								new HtCommonRowSelector.HtCommonFieldFormatter() {

									public String format(HtCommonRowSelector.CommonRow row, String sval) {
										return OrderType.getTypeName(HtUtils.parseInt(sval));
									}
								});
			}


			//
			if (header.getFieldName(i).equalsIgnoreCase("result_code")) {
				header.addFormatter(i,
								new HtCommonRowSelector.HtCommonFieldFormatter() {

									public String format(HtCommonRowSelector.CommonRow row, String sval) {
										return OperationResult.getOperationResultName(HtUtils.parseInt(sval));
									}
								});
			}



			if (header.getFieldName(i).equalsIgnoreCase("result_detail_code")) {
				header.addFormatter(i,
								new HtCommonRowSelector.HtCommonFieldFormatter() {

									public String format(HtCommonRowSelector.CommonRow row, String sval) {
										return OperationDetailResult.getOperationDetailresultName(HtUtils.parseInt(sval));
									}
								});
			}


			if (header.getFieldName(i).equalsIgnoreCase("is_simulation")) {
				header.addFormatter(i, booleanFormatter);
			}

		}
		out.append("</tr>");
		return out.toString();
	}

	private String createBrokerResponseHtmlPage(
					HtCommonRowSelector.CommonHeader header,
					List<HtCommonRowSelector.CommonRow> bresp_resultSet) {
		StringBuilder out = new StringBuilder();



		for (int i = 0; i < bresp_resultSet.size(); i++) {
			HtCommonRowSelector.CommonRow data_i = bresp_resultSet.get(i);


			out.append("<tr");
			out.append(" id='");
			out.append(i + 1);
			out.append("'>");

			//

			//

			for (int k = 1; k <= header.getSize(); k++) {
				out.append("<td nowrap=true>");
				out.append(data_i.getFormattedValue(k));
				out.append("</td>");
			}


			//
			out.append("</tr>");
		}

		return out.toString();
	}
}
