/*
 * HtShowBrokerPlusOrder.java
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
import java.io.IOException;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowBrokerPlusOrder extends HtServletsBase {

	/** Creates a new instance of HtShowBrokerPlusOrder */
	public HtShowBrokerPlusOrder() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new Uid and set initial values
			generateUniquePageId();

			// logging
			//HtUtils.log(Level.INFO, getContext(), "initialize", "Servlet URL: " + req.getRequestURL() + "?" + req.getQueryString() );

			// empty order fields
			setStringSessionValue(getUniquePageId(), req, "broker_data", "");
			setStringSessionValue(getUniquePageId(), req, "order_data", "");


			String brokerUid_str = getStringParameter(req, "broker_uid", false);

			HtCommonRowSelector.CommonHeader orderHeader = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonHeader brokerHeader = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonRow resultOrderRow = new HtCommonRowSelector.CommonRow(orderHeader);
			HtCommonRowSelector.CommonRow resultBrokerRow = new HtCommonRowSelector.CommonRow(brokerHeader);

			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerResponseParentOrder(new Uid(brokerUid_str),
							orderHeader, brokerHeader, resultOrderRow, resultBrokerRow);





			// broker data

			if (resultBrokerRow.getValue(brokerHeader.getFieldId("id")) == null) {
				throw new HtException(getContext(), "initialize", "Cannot locate Broker Response for ID: \"" + brokerUid_str + "\"");
			}


			HtCommonRowSelector.HtCommonFieldFormatter priceFormatter = HtCommonFieldFormatter_PriceFormatter.getInstance();
			HtCommonRowSelector.HtCommonFieldFormatter gmtFormatter = HtCommonFieldFormatter_GmtTime.getInstance();
			HtCommonRowSelector.HtCommonFieldFormatter booleanFormatter = HtCommonFieldFormatter_Boolean.getInstance();

			for (int i = 1; i <= brokerHeader.getSize(); i++) {
				if (brokerHeader.getFieldName(i).equalsIgnoreCase("parent_id")) {
					brokerHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											if (sval.equals(Uid.INVALID_UID)) {
												return "no parent order";
											} else {
												return sval;
											}

										}
									});
				}
				//
				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_expiration_time")) {
					brokerHeader.addFormatter(i, gmtFormatter);
				}

				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_validity_flag")) {
					brokerHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OrderValidity.getOrderValidityName(HtUtils.parseInt(sval));
										}
									});
				}

				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_pos_type")) {
					brokerHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OrderType.getTypeName(HtUtils.parseInt(sval));
										}
									});
				}

				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_issue_time")) {
					brokerHeader.addFormatter(i, gmtFormatter);
				}

				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_time")) {
					brokerHeader.addFormatter(i, gmtFormatter);
				}


				if (brokerHeader.getFieldName(i).equalsIgnoreCase("create_time")) {
					brokerHeader.addFormatter(i, gmtFormatter);
				}




				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_price")) {
					brokerHeader.addFormatter(i, priceFormatter);
				}
				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_stop_price")) {
					brokerHeader.addFormatter(i, priceFormatter);
				}

				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_tp_price")) {
					brokerHeader.addFormatter(i, priceFormatter);
				}
				if (brokerHeader.getFieldName(i).equalsIgnoreCase("op_sl_price")) {
					brokerHeader.addFormatter(i, priceFormatter);
				}

				//
				if (brokerHeader.getFieldName(i).equalsIgnoreCase("result_code")) {
					brokerHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OperationResult.getOperationResultName(HtUtils.parseInt(sval));
										}
									});
				}


				if (brokerHeader.getFieldName(i).equalsIgnoreCase("result_detail_code")) {
					brokerHeader.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return OperationDetailResult.getOperationDetailresultName(HtUtils.parseInt(sval));
										}
									});
				}


				if (brokerHeader.getFieldName(i).equalsIgnoreCase("is_simulation")) {
					brokerHeader.addFormatter(i, booleanFormatter);
				}




			}

			StringBuilder out = new StringBuilder();
			for (int i = 1; i <= brokerHeader.getSize(); i++) {
				out.append("<tr>");
				out.append("<td align=right class=x3 bgcolor='buttonface' width=200px>");
				out.append(brokerHeader.getDisplayName(i));
				out.append("</td>");
				out.append("<td align=left>");
				out.append(resultBrokerRow.getFormattedValue(i));
				out.append("</td>");
				out.append("<td></td>");
				out.append("</tr>");
			}

			setStringSessionValue(getUniquePageId(), req, "broker_data", out.toString());

			out.setLength(0);
			String orderParentUid = resultBrokerRow.getValue(brokerHeader.getFieldId("parent_id"));
			if (!orderParentUid.equals(Uid.INVALID_UID)) {

				// order data
				if (resultOrderRow.getValue(orderHeader.getFieldId("id")) != null) {

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


					// create HTML on order values

					for (int i = 1; i <= orderHeader.getSize(); i++) {
						out.append("<tr>");
						out.append("<td align=right class=x3 bgcolor='buttonface' width=200px>");
						out.append(orderHeader.getDisplayName(i));
						out.append("</td>");
						out.append("<td align=left>");
						out.append(resultOrderRow.getFormattedValue(i));
						out.append("</td>");
						out.append("<td></td>");
						out.append("</tr>");
					}


					setStringSessionValue(getUniquePageId(), req, "order_data", out.toString());
				}

			} else {
				// parent id is not valid
				out.append("<tr>");
				out.append("<td align=right class=x3 bgcolor='buttonface' width=200px>");
				out.append("No parent order found");
				out.append("</td>");
				out.append("</tr>");

				setStringSessionValue(getUniquePageId(), req, "order_data", out.toString());
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;
	}
}
