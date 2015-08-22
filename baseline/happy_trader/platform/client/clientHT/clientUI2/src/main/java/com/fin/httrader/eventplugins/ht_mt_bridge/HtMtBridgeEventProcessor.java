/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_CLEAR_ALL_USERS;
import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.hlpstruct.remotecall.CallingContext;
import com.fin.httrader.hlpstruct.remotecall.ParamBase;
import com.fin.httrader.hlpstruct.remotecall.PositionList_Proxy;
import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import static com.fin.httrader.utils.hlpstruct.XmlEvent.DT_CommonXml;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtBridgeEventProcessor implements HtEventPlugin
{
		// evnet types
		public static final String MT4_DELIVERY_ENVELOPE_EVENT = "mt4_delivery_envelope";
		public static final String MT4_HEARTBEAT_EVENT = "mt4_heartbeat";
		public static final String MT4_CONNECT_STATUS_EVENT  = "mt4_connect_status";
		
		public static final String SERVERNAME_PARAM = "SERVER_NAME";
		public static final String ALGLIB_PARAM = "ALGLIB_NAME";
		public static final String TREADID_PARAM = "THREAD_ID";
		
		private final StringBuilder name_m = new StringBuilder();
		
		private String getContext() {
				return getClass().getSimpleName();
		}
		
		public HtMtBridgeEventProcessor()
		{
				
		}
		
		@Override
		public String getEventPluginId() {
				return name_m.toString();
		}
		
		@Override
		public void setEventPluginId(String pluginId) {
				name_m.setLength(0);
				name_m.append(pluginId);
		}
		
		@Override
		public Integer[] returnSubscribtionEventTypes() {
				return new Integer[]{XmlEvent.ET_CustomEvent};
		}
		
		@Override
		public void execute(XmlEvent alertData) throws Exception {
				if (alertData.getEventType() == XmlEvent.ET_CustomEvent && alertData.getType() == XmlEvent.DT_CommonXml) {
						XmlParameter xml_param = alertData.getAsXmlParameter();
					
						
						String cmd_name = xml_param.getCommandName();

						if (cmd_name.equalsIgnoreCase(MT4_HEARTBEAT_EVENT)) {
										HtLog.log(Level.FINE, getContext(), "execute", "Heartbeat from MT");
										//String data = xml_param.getString("data");
										
										HtMtManager.getInstance().putHeartBeat();
										//HtLog.log(Level.INFO, getContext(), "execute", "Delivery envelope:\n" + data);
										
																			

						} else if (cmd_name.equalsIgnoreCase(MT4_DELIVERY_ENVELOPE_EVENT)) {
								  HtLog.log(Level.FINE, getContext(), "execute", "Delivery MT");
									String data = xml_param.getString("data");
									
									// this is RAW json data
									// need to parse and put into DB
									HtMtManager.getInstance().putJsonData(data);
									//HtLog.log(Level.INFO, getContext(), "execute", "Delivery envelope:\n" + data);
						}
						else if (cmd_name.equalsIgnoreCase(MT4_CONNECT_STATUS_EVENT)) {
								 HtLog.log(Level.FINE, getContext(), "execute", "Connect status from MT");
								 
								 String data = xml_param.getString("data");
								 HtMtManager.getInstance().processConnectEvent(data);
								 
						}
						
				
				}
		}

		@Override
		public void initialize(Map<String, String> initdata) throws Exception {
				
				HtMtManager.getInstance().initialize(
					initdata,
					getEventPluginId()
				);
				
		}

		@Override
		public void deinitialize() throws Exception {
				HtMtManager.getInstance().deinitialize();
		}
}
