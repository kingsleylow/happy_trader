/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.SimpleNumberParser;
import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.HeartBeatOut.COMMAND;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.OutPacketBase;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtBalanceUpdate;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtAccountMetainfo;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtAccountUpdate;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtCompanyMetainfo;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtGeneralConsts;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtPosition;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtPositionUpdate;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.HtMtServers;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.TradeDirection;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.math.BigDecimal;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class Mtproxy_Data extends MTProxyPacketBase{
		
		
		public static final String COMMAND="DATA_PACKET";
		
// ------------------------
		
		private int pendingSize_m = -1;
		private JSONObject data_m = null;
		
		private SimpleNumberParser parser_m = new SimpleNumberParser();
			
		private String getContext()
		{
				return Mtproxy_Data.class.getSimpleName();
		}
		
		public String getCommand()
		{
				return COMMAND;
		}
	
		public int getPendingSize() {
				return pendingSize_m;
		}

		public void setPendingSize(int pendingSize) {
				this.pendingSize_m = pendingSize;
		}

		public JSONObject getData() {
				return data_m;
		}

		public void setData(JSONObject data) {
				this.data_m = data;
		}
		
		// -------------------
		
		public void fromJson(JSONObject result_json) throws Exception
		{
				super.fromJson(result_json);
				setPendingSize( ((Number)result_json.get("PENDING_SIZE")).intValue() );
				setData( (JSONObject)result_json.get("DATA") );

		}
		
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				json_root_obj.put("PENDING_SIZE", new Integer(this.getPendingSize()));
				json_root_obj.put("DATA", getData());
		}
		
	
		// this parses data and returns some objects at upper level
		public void parseData(
			HtMtServers server, 
			HtMtAccountMetainfo accminfo,
			HtMtAccountUpdate accupdt,
			HtMtCompanyMetainfo compminfo,
			List< HtMtPosition > positions,
			HtMtBalanceUpdate balanceupd
			
		) throws Exception
		{
				if (data_m == null ) 
						throw new HtException(getContext(), "parseData", "Invalid data");
						
				JSONObject account_info_json = (JSONObject)data_m.get("ACCOUNT_INFO");
				JSONObject open_orders_json = (JSONObject)data_m.get("OPEN_ORDERS");
				JSONObject history_orders_json = (JSONObject)data_m.get("HISTORY_ORDERS");
				
				if (account_info_json==null)
						throw new HtException(getContext(), "parseData", "Invalid data: <ACCOUNT_INFO>");
				
				if (open_orders_json==null)
						throw new HtException(getContext(), "parseData", "Invalid data: <OPEN_ORDERS>");
				
				if (history_orders_json==null)
						throw new HtException(getContext(), "parseData", "Invalid data: <HISTORY_ORDERS>");
				
				Boolean connected = (Boolean)account_info_json.get("connected");
				if (connected) {
						server.setServerName((String)account_info_json.get("server"));
						
						// account
						accminfo.setAccountId((String)account_info_json.get("number"));
						accminfo.setHolderName((String)account_info_json.get("name"));
						accminfo.setCurrency((String)account_info_json.get("currency"));
						accminfo.setLeverage( ((Number)account_info_json.get("leverage")).intValue() );
						accminfo.setServer((String)account_info_json.get("server"));
						
						if (balanceupd != null) {
								// balance update
								balanceupd.setBalance( convertPriceToInternal((String)account_info_json.get("balance") ));
						}
						
						if (accupdt != null) {
								// account update
								accupdt.setEquity(convertPriceToInternal((String)account_info_json.get("equity") ));
								accupdt.setFreeMargine(convertPriceToInternal((String)account_info_json.get("free_margin") ));
								accupdt.setStopOutLevel(convertPriceToInternal((String)account_info_json.get("stopout_level") ));	
						
								accupdt.setStopOutMode(parser_m.parseInt((String)account_info_json.get("stopout_mode")));
								accupdt.setFreeMargineMode(parser_m.parseInt((String)account_info_json.get("free_margin_mode")));
						}
						
						// company metainfo
						compminfo.setCompanyName((String)account_info_json.get("company"));
						if (HtUtils.nvl(compminfo.getCompanyName()))
								// we can't proceed
								throw new HtException(getContext(), "parseData", "Company name is invalid, will not proceed with that entry");
						
						// position list
						// later must be denormalized and splitted into positions and positions update
						positions.clear();
						
						JSONArray open_orders = (JSONArray)open_orders_json.get("ORDER_LIST");
						for(int i  =0; i < open_orders.size(); i++) {
								JSONObject oppos_i = (JSONObject)open_orders.get(i);
								HtMtPosition pos = posFromJson(oppos_i, true);
								
								if (pos != null)
										positions.add(  pos );
						}
						
						JSONArray hist_orders = (JSONArray)history_orders_json.get("ORDER_LIST");
						for(int i  =0; i < hist_orders.size(); i++) {
								JSONObject hstpos_i = (JSONObject)hist_orders.get(i);
								HtMtPosition pos =  posFromJson(hstpos_i, false);
								
								if (pos != null)
										positions.add(  pos );
						}
						
					
				}
				else 
						throw new HtException(getContext(), "parseData", "The packet from MT4 returned we are disconnected");
						
				
		}
		
		/**
		 * Helpers
		 *
		 */
		
		long convertPriceToInternal(String value)
		{
				double val = parser_m.parseDouble(value);
				long res = (long)(val * HtMtGeneralConsts.PRICE_GENERAL_MULTIPLIER);
				
				return res;
		}
		
		HtMtPosition posFromJson(JSONObject pos_ele_json, boolean isOpen) throws Exception
		{
				HtMtPosition pos = new HtMtPosition();
				
				pos.setTradeSymbol(	(String)pos_ele_json.get("symbol"));
				pos.setTicket((String)pos_ele_json.get("ticket"));
				pos.setComment((String)pos_ele_json.get("comment") );
				
				pos.setOpenTime( HtDateTimeUtils.convertUinxSecToUnixMsec( parser_m.parseLong((String)pos_ele_json.get("open_time"))));
				pos.setCloseTime(HtDateTimeUtils.convertUinxSecToUnixMsec( parser_m.parseLong((String)pos_ele_json.get("close_time"))));
				
				pos.setOpenPrice(convertPriceToInternal((String)pos_ele_json.get("open_price")) );	
				pos.setClosePrice(convertPriceToInternal((String)pos_ele_json.get("close_price")) );
				
				pos.setInitialStopLoss(convertPriceToInternal((String)pos_ele_json.get("stop_loss")) );	
				pos.setInitialTakeProfit(convertPriceToInternal((String)pos_ele_json.get("take_profit")) );
				
				
				
				pos.setExpiration(HtDateTimeUtils.convertUinxSecToUnixMsec( parser_m.parseLong((String)pos_ele_json.get("expiration"))));
				
				pos.setProfit(convertPriceToInternal((String)pos_ele_json.get("profit")));
				pos.setPosState(isOpen ? PosState.STATE_OPEN : PosState.STATE_HISTORY); // open pos
				
				int typecode = ((Number)pos_ele_json.get("type")).intValue();
				if (typecode == HtMtGeneralConsts.OP_BUY) {
						pos.setDirection(TradeDirection.TD_LONG );
						pos.setPosType(isOpen? PosType.POS_BUY: PosType.POS_BUY);
				}
				else if (typecode == HtMtGeneralConsts.OP_SELL) {
						pos.setDirection(TradeDirection.TD_SHORT );
						pos.setPosType(isOpen? PosType.POS_SELL: PosType.POS_SELL);
				}
				else if (typecode == HtMtGeneralConsts.OP_BUYLIMIT) {
						pos.setDirection(TradeDirection.TD_LONG );
						pos.setPosType(isOpen?PosType.POS_LIMIT_BUY: PosType.POS_BUY);
				}
				else if (typecode == HtMtGeneralConsts.OP_BUYSTOP) {
						pos.setDirection(TradeDirection.TD_LONG );
						pos.setPosType(isOpen?PosType.POS_STOP_BUY: PosType.POS_BUY);
				}
				else if (typecode == HtMtGeneralConsts.OP_SELLLIMIT) {
						pos.setDirection(TradeDirection.TD_SHORT );
						pos.setPosType(isOpen?PosType.POS_LIMIT_SELL: PosType.POS_SELL);
				}
				else if (typecode == HtMtGeneralConsts.OP_SELLSTOP) {
						pos.setDirection(TradeDirection.TD_SHORT );
						pos.setPosType(isOpen?PosType.POS_STOP_SELL: PosType.POS_SELL);
				}
				else
						return null;
				
			
				
				return pos;
				
		}
}
