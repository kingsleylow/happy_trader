/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.prochandlers;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtBaseIntReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtClearUsersReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetAccountHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetAutoloadUsers;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetPositionHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetStatusReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtRestartInstanceReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtRestartMtReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtStartInstanceReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtStartPollUsersReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtStopInstanceReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtSubscribeUserReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtUnSubscribeUserReq;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import javax.servlet.http.HttpServletRequest;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class HtProcHandlerBase {
		
		private static final HtPair[] HandlerMap = {
				new HtPair(HtGetAccountHistoryReq.COMMAND_NAME,HtGetAccountHistoryReq.class ),
				new HtPair(HtGetPositionHistoryReq.COMMAND_NAME, HtGetPositionHistoryReq.class),
				new HtPair(HtGetStatusReq.COMMAND_NAME,	HtGetStatusReq.class),
				new HtPair(HtClearUsersReq.COMMAND_NAME,	HtClearUsersReq.class),
				new HtPair(HtStartInstanceReq.COMMAND_NAME,	HtStartInstanceReq.class),
				new HtPair(HtStartPollUsersReq.COMMAND_NAME,	HtStartPollUsersReq.class),
				new HtPair(HtStopInstanceReq.COMMAND_NAME,	HtStopInstanceReq.class),
				new HtPair(HtSubscribeUserReq.COMMAND_NAME,	HtSubscribeUserReq.class),
				new HtPair(HtUnSubscribeUserReq.COMMAND_NAME,	HtUnSubscribeUserReq.class),
				new HtPair(HtRestartMtReq.COMMAND_NAME,	HtRestartMtReq.class),
				new HtPair(HtGetAutoloadUsers.COMMAND_NAME,	HtGetAutoloadUsers.class),
				new HtPair(HtRestartInstanceReq.COMMAND_NAME,	HtRestartInstanceReq.class)
		};
	
		private static String getContext()
		{
				return HtProcHandlerBase.class.getSimpleName();
		}
		
		// return necessary instance
		// statiuc initializer to process request
		public static HtBaseIntReq parseCommandData(String request_data) throws Exception
		{
				// firstly parse input data as JSON request
			
				if (HtUtils.nvl(request_data))
						throw new HtException(getContext(), "parseCommandData", "Empty data");
				
				
				JSONObject root_json = (JSONObject) JSONValue.parseWithException(request_data);
				
				String hashed_user  =(String)root_json.get("HUSR");
				String hashed_password = (String)root_json.get("HPW");
				Boolean simple_answer_b = HtMtJsonUtils.getBooleanJsonFieldNull(root_json, "SIMPLE_ANSWER");
				
				
				
				
				processSecurity(hashed_user, hashed_password);
				
				// make maap here
				String command_name = (String)root_json.get("COMMAND_NAME");
				if (HtUtils.nvl(command_name))
						throw new HtException(getContext(), "parseCommandData", "Invalid <COMMAND_NAME> JSON parameter");
				
				HtBaseIntReq baseRequest = null;
				
				for(HtPair me: HandlerMap) {
					
						String commandNameObject = (String)me.first;
						Class<?> o = (Class<?>) me.second;
						if (commandNameObject.equalsIgnoreCase(command_name) ) {
								if (!getCommandNameFromClass(o).equalsIgnoreCase(commandNameObject))
										throw new HtException(getContext(), "parseCommandData", "Invalid command mapping entry for class: " + o.getSimpleName());
								baseRequest = instantiate(o, command_name);
								break;
						}
				}
				
				if (baseRequest == null)
						throw new HtException(getContext(), "parseCommandData", "<COMMAND_NAME> not recognized: " + command_name);
				
				// initialize
				baseRequest.initialize(root_json);
				if (simple_answer_b != null) {
					baseRequest.setSimpleResponseFlag(simple_answer_b);
				}
				
				return baseRequest;
		}
		
		static void processSecurity(String user, String hashed_password) throws Exception
		{
				boolean res = HtCommandProcessor.instance().get_HtSecurityProxy().remote_checkUserCredentialsWithHashedPasswordAndHashedUser(user, hashed_password);

				if (!res) {
						throw new HtException(HtProcHandlerBase.class.getSimpleName(), "processSecurity", "Invalid user or password");
				}
		}
		
		/**
		 * Helpers
		 */
		
		private static String getCommandNameFromClass(Class classObj) throws Exception
		{
				
				String result = null;
				try {
						Field f_cm = classObj.getField("COMMAND_NAME");
						Object result_o = f_cm.get(null);
						if (result_o instanceof String)
								result = (String)result_o;
				}
				catch(Exception e1)
				{
				}
				
				if (result == null)
						throw new HtException(getContext(), "getCommandNameFromClass", "Response class must have <COMMAND_NAME> String static field");
				
				return result;
		}
		
		private static HtBaseIntReq instantiate(Class classObj, String command)  throws Exception
		{
				try {
						Constructor<?> cons = classObj.getConstructor();
						Object object = cons.newInstance();
						if (object instanceof HtBaseIntReq)
								return (HtBaseIntReq)object;
				}
				catch(Exception e1)
				{
				}
				
				throw new HtException(getContext(), "instantiate", "Cannot instantiate requet object for command: [" + command + "]");
				
		}
}
