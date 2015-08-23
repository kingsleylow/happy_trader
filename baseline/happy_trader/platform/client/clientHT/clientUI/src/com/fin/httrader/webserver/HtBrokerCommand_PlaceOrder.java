/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.hlpstruct.Order;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.OrderValidity;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.PriceConst;
import com.fin.httrader.hlpstruct.broker.BrokerConnect;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerCommand_PlaceOrder extends HtServletsBase  {
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			// store some parameters
			String session_id = getStringParameter(req, "session_id", false);
			setStringSessionValue(getUniquePageId(), req, "session_id", session_id);

			String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
			setStringSessionValue(getUniquePageId(), req, "alg_brk_pair", alg_brk_pair);

			String server_id = getStringParameter(req, "server_id", false);
			setStringSessionValue(getUniquePageId(), req, "server_id", server_id);

			String target_thread = getStringParameter(req, "target_thread", false);
			setStringSessionValue(getUniquePageId(), req, "target_thread", target_thread);
			
			String broker_seq_num = getStringParameter(req, "broker_seq_num", false);
			setStringSessionValue(getUniquePageId(), req, "broker_seq_num", broker_seq_num);

			

			readInitialData(req);

			

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);

		HtAjaxCaller ajaxCaller = new HtAjaxCaller();
		ajaxCaller.setOk();
		
		try {

			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("send_order")) {
				sendOrder(ajaxCaller, req);
			}
			else if (operation.equalsIgnoreCase("poll_broker_response")) {
				pollBrokerResponse(ajaxCaller, req);
			}
	 		else
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);


		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
			
		}

		ajaxCaller.serializeToXml(res);
		return true;

	} //

	// -----------------------------
	// HELPERS
	// -----------------------------


	private void readInitialData(HttpServletRequest req) throws Exception
	{
		Map<Integer, String> allPrices = PriceConst.getPriceConstsNamesMap();
		setStringSessionValue(getUniquePageId(), req, "available_price_types", HtWebUtils.createOptionMappedList(PriceConst.PC_INPUT, allPrices));

		// validity
		Map<Integer, String> allValidities = OrderValidity.getAllValiditiesMap();
		setStringSessionValue(getUniquePageId(), req, "available_validity_types", HtWebUtils.createOptionMappedList(OrderValidity.OV_GTC, allValidities));

				
		
		// order types
		StringBuilder out = new StringBuilder();
		
		Map<Integer, String> orderTypes = OrderType.getNameMap();
		for (Iterator<Integer> it = orderTypes.keySet().iterator(); it.hasNext();) {
			int key_i  =it.next();
			String val_i = orderTypes.get(key_i);
			out.append("<tr><td align=left><input type=checkbox value=");
			out.append(key_i);
			out.append(" onclick='on_ordertype_checkbox_change()'>");
			out.append(val_i);
			out.append("</input></td></tr>");
			
		}
		

		setStringSessionValue(getUniquePageId(), req, "all_order_types", out.toString());


	}

	private void sendOrder(HtAjaxCaller ajaxCaller, HttpServletRequest req) throws Exception
	{
		
		String total_data = getStringParameter(req, "total_data", false);
		XmlParameter request_param = XmlHandler.deserializeParameterStatic(total_data);
		String server_id = request_param.getString("server_id");
		String session_id = request_param.getString("session_id");
		int trg_thread = HtUtils.parseInt(request_param.getString("target_thread"));
		String alg_brk_pair = request_param.getString("alg_brk_pair");

		Order order = new Order();
		order.ID_m.generate();
		ajaxCaller.setData(order.ID_m.toString());
		
		order.orderType_m = HtUtils.parseInt(request_param.getString("order_type"));

		order.orVolume_m = HtUtils.parseInt(request_param.getString("volume"));
		order.orSLprice_m = HtUtils.parseDouble(request_param.getString("stop_loss"));
		order.orTPprice_m = HtUtils.parseDouble(request_param.getString("take_profit"));
		order.orPrice_m = HtUtils.parseDouble(request_param.getString("price"));
		order.orStopPrice_m = HtUtils.parseDouble(request_param.getString("stop_price"));
		order.orPriceType_m = HtUtils.parseInt(request_param.getString("price_type"));
		order.orStopPriceType_m = HtUtils.parseInt(request_param.getString("stop_price_type"));

		order.orderValidity_m = HtUtils.parseInt(request_param.getString("validity"));
		order.orExpirationTime_m = HtDateTimeUtils.parseDateTimeParameter(request_param.getString("expiration"), false);

		order.comment_m.append(request_param.getString("comment"));
		order.additionalInfo_m.append(request_param.getString("additional_info"));
		order.method_m.append(request_param.getString("method"));
		order.place_m.append(request_param.getString("place"));

		order.symbol_m.append(request_param.getString("symbol"));
		//order.symbolDenom_m.append(request_param.getString("symol_denom"));
		order.provider_m.append(request_param.getString("provider"));

		order.brokerOrderID_m.append(request_param.getString("brk_order_id"));
		order.brokerPositionID_m.append(request_param.getString("brk_pos_id"));
		
		order.context_m.append(request_param.getString("context_str"));
		
		int broker_seq_num = HtUtils.parseInt(request_param.getString("broker_seq_num"));

	
		try {
			// resolve connection
			BrokerConnect connect = HtCommandProcessor.instance().get_HtServerProxy().remote_connectBrokerConnectionObject(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, new Uid(session_id), RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);

			//
			boolean result	= HtCommandProcessor.instance().get_HtServerProxy().remote_op_unblockingIssueOrder(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, connect, order, RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);
		

			if (!result) {
				ajaxCaller.setError(-2,  "Error returned on attempt to send order: " + order.ID_m.toString());
			}
		}
		catch(Throwable e)
		{
			ajaxCaller.setError(-3,  "Exception occured on attempt to send order: " + order.ID_m.toString() + " - "+ e.getMessage());
		}

		
	
		
	}

	private void pollBrokerResponse(HtAjaxCaller ajaxCaller, HttpServletRequest req) throws Exception
	{

		try {
			// this identifies the session
			String session_id = getStringParameter(req, "session_id", false);
			List<String> sessionIds = new ArrayList<String>();
			sessionIds.add(session_id);


			// either 3 parameters
			String parent_order_uid_s = getStringParameter(req, "parent_order_uid", true);
			String broker_order_id = getStringParameter(req, "broker_order_id", true);
			String broker_position_id = getStringParameter(req, "broker_position_id", true);

			if (parent_order_uid_s.length() == 0 && broker_order_id.length() == 0 && broker_position_id.length() == 0 )
				throw new HtException(getContext(), "pollBrokerResponse", "Either of 3 searching criteria must be set");
			
			Uid parent_order_uid = new Uid(parent_order_uid_s);

			

			final HtCommonRowSelector.CommonHeader brokerHeader = new HtCommonRowSelector.CommonHeader();
			final List<HtCommonRowSelector.CommonRow> resultBrokerSet = new ArrayList<HtCommonRowSelector.CommonRow>();

			HtCommonRowSelector brokerDataSelection = new HtCommonRowSelector()
			{

				@Override
				public void newHeaderArrived(CommonHeader header) throws Exception {
					brokerHeader.create(header);
				}

				@Override
				public void newRowArrived(CommonRow row) throws Exception {
					row.setHeader(brokerHeader);
					resultBrokerSet.add(row);
				}

				@Override
				public void onSelectionFinish(long cntr) throws Exception {
					
				}
			};

		
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerResponses_Callback(
				sessionIds,
				parent_order_uid,
				broker_order_id,
				broker_position_id,
				-1,
				-1,
				-1,
				-1,
				brokerDataSelection
			);

			// return data

			prepareBrokerResponsesFormatters(brokerHeader);
			
		
			ajaxCaller.setData(createBrokerResponseXml(brokerHeader, resultBrokerSet));

		}
		catch(Throwable e)
		{
			ajaxCaller.setError(-4,  "Exception occured on polling: " + e.getMessage());
		}

	}


	private void prepareBrokerResponsesFormatters(HtCommonRowSelector.CommonHeader header) {


		

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

			//
			if (header.getFieldName(i).equalsIgnoreCase("op_pos_type")) {
							header.addFormatter(i,
											new HtCommonRowSelector.HtCommonFieldFormatter() {

												public String format(HtCommonRowSelector.CommonRow row, String sval) {
														return OrderType.getTypeName(HtUtils.parseInt(sval));
												}
											});
					}



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
	
	}

	private String createBrokerResponseXml(
					HtCommonRowSelector.CommonHeader header,
					List<HtCommonRowSelector.CommonRow> bresp_resultSet) {
		StringBuilder out = new StringBuilder();

		out.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		out.append("<rows>");

		for (int i = 0; i < bresp_resultSet.size(); i++) {
			HtCommonRowSelector.CommonRow data_i = bresp_resultSet.get(i);

			out.append("<row id=\"");
			out.append(i+1);
			out.append("\">");

			//

			for (int k = 1; k <= header.getSize(); k++) {
				out.append("<cell>");
				out.append(data_i.getFormattedValue(k));
				out.append("</cell>");
			}
			//
			out.append("</row>");
		}

		out.append("</rows>");

		return out.toString();
	}

	

}
