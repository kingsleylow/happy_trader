/*
 * HtBrokerCommand.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.*;
import com.fin.httrader.hlpstruct.broker.BrokerConnect;
import com.fin.httrader.hlpstruct.remotecall.CallingContext;
import com.fin.httrader.hlpstruct.remotecall.ParamBase;
import com.fin.httrader.hlpstruct.remotecall.String_Proxy;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 * This class is a user-end interface to work with broker messages sent to the defined thread
 */
public class HtBrokerCommand extends HtServletsBase {
  
			
	GridFieldDescriptor[] posFieldDescr_m = new GridFieldDescriptor[]{
							new GridFieldDescriptor("Broker Position ID"),
							new GridFieldDescriptor("Provider"),
							new GridFieldDescriptor("Symbol"),
							new GridFieldDescriptor("Direction"),
							new GridFieldDescriptor("Open Time"),
							new GridFieldDescriptor("Average Open Price"),
							new GridFieldDescriptor("Close Time"),
							new GridFieldDescriptor("Average Close Price"),
							new GridFieldDescriptor("Volume Imbalance"),
							new GridFieldDescriptor("Position Volume"),
							new GridFieldDescriptor("Position State ")
		};

		GridFieldDescriptor[] sessFieldDescr_m = new GridFieldDescriptor[]{
							new GridFieldDescriptor("Session ID"),
							new GridFieldDescriptor("Is Connected"),
							new GridFieldDescriptor("Connection String"),
							new GridFieldDescriptor("Current Run ID"),
							new GridFieldDescriptor("Session FSM Flag"),
							new GridFieldDescriptor("Re-Bounce Connection"),
							new GridFieldDescriptor("Delete Connection"),

		};

	// ----------------------------------------


	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtBrokerCommand */
	public HtBrokerCommand() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			//
			setEmptyPositionList(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {

			// whether we generate new page uid via POST
			boolean new_puid = getBooleanParameter(req, "generate_new_page_uid", true);
			if (new_puid) {
				generateUniquePageId();
			} else {
				setUniquePageId(getStringParameter(req, "page_uid", false));
			}


			


			String operation = getStringParameter(req, "operation", false);

			// this just return some data back
			storeFormRelatedData(req);

			//
			//StringBuilder out = new StringBuilder();

			setStringSessionValue(getUniquePageId(), req, "custom_out_data", "");

			if (operation.equalsIgnoreCase("initial_load")) {
				// no-op

				// clear this as this is initial load
				setEmptyPositionList(req);

			} else if (operation.equalsIgnoreCase("initial_refresh_load")) {
				// refresh button is pressed
			} else if (operation.equalsIgnoreCase("rebounce_connection")) {
				rebounceConnectionInitialData(req);

			} else if (operation.equalsIgnoreCase("send_custom_event")) {
				sendCustomEventData(req);

			}
			else if (operation.equalsIgnoreCase("delete_connection")) {
				deleteConnectionInitialData(req);

			} else if (operation.equalsIgnoreCase("create_new_connection")) {
				createNewConnectionInitialData(req);

			} else if (operation.equalsIgnoreCase("query_active_orders")) {
				queryActiveOrdersInitialData(req);
			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}

			readInitialData(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	} //

	
	private void setEmptyPositionList(HttpServletRequest req) throws Exception {
		GridDataSet dataset = new GridDataSet( posFieldDescr_m );
		setStringSessionValue(getUniquePageId(), req, "position_list", HtWebUtils.createHTMLForDhtmlGrid(dataset));
	}

	private void sendCustomEventData(HttpServletRequest req) throws Exception {
		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		String custom_name = getStringParameter(req, "custom_name", false);
		String custom_data = getStringParameter(req, "custom_event_data", false);

		String result = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
						 server_id,
						 alg_brk_pair,
						 trg_thread,
						 custom_name,
						 custom_data,
						 RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
		);

		setStringSessionValue(getUniquePageId(), req, "custom_out_data", result);
		
	}

	private void queryActiveOrdersInitialData(HttpServletRequest req) throws Exception {

		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		String session_id = getStringParameter(req, "session_id", false);
		boolean show_history = getBooleanParameter(req, "show_history_orders", false);
		int broker_seq_num = (int) getIntParameter(req, "broker_seq_num", false);

		// resolve connection
		BrokerConnect connect = HtCommandProcessor.instance().get_HtServerProxy().remote_connectBrokerConnectionObject(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, new Uid(session_id), RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);

		List<Position> posList = HtCommandProcessor.instance().get_HtServerProxy().remote_op_getBrokerPositionList(
						server_id, 
						alg_brk_pair, 
						trg_thread, 
						broker_seq_num, 
						show_history ? (PosStateQuery.QUERY_OPEN | PosStateQuery.QUERY_HISTORY) : PosStateQuery.QUERY_OPEN, 
						connect, 
						RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);


		

		GridDataSet dataset = new GridDataSet( posFieldDescr_m );
							
		
	
		int id = 0;
		for (int i = 0; i < posList.size(); i++) {


			Position pos_i = posList.get(i);
						
			if (!show_history) {
				if (pos_i.posState_m == PosState.STATE_HISTORY || pos_i.posState_m == PosState.STATE_DUMMY) {
					continue;
				}
			}

			dataset.addRow();
	

			

			// Position UID,Broker ID,Install Time,Execute Time,Install Price,Execute Price,Take Profit Price,Stop Loss Price,Volume,
			//RT Provider,Symbol Numerator,Symbol Denominator,Position Type,Position State
			
			
			dataset.setRowValue(id, 0, pos_i.brokerPositionID_m.toString());
			dataset.setRowValue(id, 1, pos_i.provider_m.toString());
			dataset.setRowValue(id, 2, pos_i.symbol_m.toString());
			dataset.setRowValue(id, 3, TradeDirection.getTradeDirectionName(pos_i.direction_m));
			dataset.setRowValue(id, 4, HtDateTimeUtils.time2SimpleString_Gmt(pos_i.timeOpen_m > 0 ? pos_i.timeOpen_m * 1000 : -1));
			dataset.setRowValue(id, 5, HtUtils.formatNonNegativePriceValue(pos_i.avrOpenPrice_m, 4));
			dataset.setRowValue(id, 6, HtDateTimeUtils.time2SimpleString_Gmt(pos_i.timeClose_m > 0 ? pos_i.timeClose_m * 1000 : -1));
			dataset.setRowValue(id, 7, HtUtils.formatNonNegativePriceValue(pos_i.avrClosePrice_m, 4));
			dataset.setRowValue(id, 8, HtUtils.formatNonNegativePriceValue(pos_i.volumeImbalance_m, 4));
			dataset.setRowValue(id, 9, HtUtils.formatNonNegativePriceValue(pos_i.volume_m, 4));
			dataset.setRowValue(id, 10, PosState.getPosStateName(pos_i.posState_m));
		

			id++;
		}
		

		
		setStringSessionValue(getUniquePageId(), req, "position_list", HtWebUtils.createHTMLForDhtmlGrid(dataset));
	}

	private void deleteConnectionInitialData(HttpServletRequest req) throws Exception {
		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		String session_id = getStringParameter(req, "session_id", false);
		String run_id = getStringParameter(req, "run_id", false);
		String connect_string = getStringParameter(req, "connect_string", false);
		int is_connected = (int) getIntParameter(req, "is_connected", false);
		int broker_seq_num = (int) getIntParameter(req, "broker_seq_num", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_deleteBrokerConnectionObject(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, new Uid(session_id), RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);
	}

	private void rebounceConnectionInitialData(HttpServletRequest req) throws Exception {
		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		String session_id = getStringParameter(req, "session_id", false);
		String run_id = getStringParameter(req, "run_id", false);
		String connect_string = getStringParameter(req, "connect_string", false);
		int is_connected = (int) getIntParameter(req, "is_connected", false);
		int broker_seq_num = (int) getIntParameter(req, "broker_seq_num", false);

		HtCommandProcessor.instance().get_HtServerProxy().remote_rebounceBrokerConnection(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, new Uid(session_id), RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);

	}

	private void createNewConnectionInitialData(HttpServletRequest req) throws Exception {
		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		String run_id = getStringParameter(req, "run_id", false);
		String connect_string = getStringParameter(req, "connect_string", false);
		String comment = getStringParameter(req, "comment_string", false);
		int broker_seq_num = (int) getIntParameter(req, "broker_seq_num", false);

		// create new connection but does not call connect
		HtCommandProcessor.instance().get_HtServerProxy().remote_createBrokerConnectionObject(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, run_id, comment, connect_string, RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);
	}

	
	
	//


	// function to save some form related data
	private void storeFormRelatedData(HttpServletRequest req) throws Exception {
		setStringSessionValue(getUniquePageId(), req, "cur_session_id", getStringParameter(req, "cur_session_id", true));
		setStringSessionValue(getUniquePageId(), req, "checked_all_orders_query", getStringParameter(req, "checked_all_orders_query", true));
	}

	private void readInitialData(HttpServletRequest req) throws Exception {
		int trg_thread = (int) getIntParameter(req, "target_thread", false);
		String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
		String server_id = getStringParameter(req, "server_id", false);
		int broker_seq_num = (int) getIntParameter(req, "broker_seq_num", false);

		setStringSessionValue(getUniquePageId(), req, "target_thread", String.valueOf(trg_thread));
		setStringSessionValue(getUniquePageId(), req, "target_alg_brk_pair", alg_brk_pair);
		setStringSessionValue(getUniquePageId(), req, "broker_seq_num", String.valueOf(broker_seq_num));

		// fill in the available sessions
		

		List<BrokerConnect> connections =
						HtCommandProcessor.instance().get_HtServerProxy().remote_returnAvailableBrokerConnections(
						server_id, alg_brk_pair, trg_thread, broker_seq_num, RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);

		GridDataSet dataset = new GridDataSet( sessFieldDescr_m );
		

		for (int i = 0; i < connections.size(); i++) {
			dataset.addRow();

			BrokerConnect connect_i = connections.get(i);
			dataset.setRowValue(i, 0, connect_i.getUid().toString());
			dataset.setRowValue(i, 1, connect_i.isConnected() ? "Connected" : "Not connected");
			dataset.setRowValue(i, 2, connect_i.getConnection());
			dataset.setRowValue(i, 3, connect_i.getRunName());
			dataset.setRowValue(i, 4, String.valueOf(connect_i.getState()));

			StringBuilder out = new StringBuilder();
			out.append("<a href='#' onclick=\"");
			out.append("do_rebounce_connection('");
			out.append(connect_i.getUid().toString());
			out.append("','");
			out.append(connect_i.getRunName());
			out.append("','");
			out.append(connect_i.getConnection());
			out.append("',");
			out.append((connect_i.isConnected() ? "true" : "false"));
			out.append(")");
			out.append("\">Click</a>");
			dataset.setRowValue(i, 5, out.toString());
			

			out.setLength(0);
			
			out.append("<a href='#' onclick=\"");
			out.append("do_delete_connection('");
			out.append(connect_i.getUid().toString());
			out.append("','");
			out.append(connect_i.getRunName());
			out.append("','");
			out.append(connect_i.getConnection());
			out.append("',");
			out.append((connect_i.isConnected() ? "true" : "false"));
			out.append(")");
			out.append("\">Click</a>");
			
			dataset.setRowValue(i, 6, out.toString());

			
		}
		

		setStringSessionValue(getUniquePageId(), req, "session_list", HtWebUtils.createHTMLForDhtmlGrid(dataset));

		//
		setStringSessionValue(getUniquePageId(), req, "server_id", server_id);

		// available pos types
		
		Map<Integer, String> allPosTypes = PosType.getAllPosTypesNamesMap();
		setStringSessionValue(getUniquePageId(), req, "available_pos_types", HtWebUtils.createOptionMappedList(1, allPosTypes));

		// available prices
		
		Map<Integer, String> allPrices = PriceConst.getPriceConstsNamesMap();
		setStringSessionValue(getUniquePageId(), req, "available_prices", HtWebUtils.createOptionMappedList(PriceConst.PC_INPUT, allPrices));

		// validity
		Map<Integer, String> allValidities = OrderValidity.getAllValiditiesMap();
		setStringSessionValue(getUniquePageId(), req, "available_validity", HtWebUtils.createOptionMappedList(OrderValidity.OV_GTC, allValidities));

	}
}
