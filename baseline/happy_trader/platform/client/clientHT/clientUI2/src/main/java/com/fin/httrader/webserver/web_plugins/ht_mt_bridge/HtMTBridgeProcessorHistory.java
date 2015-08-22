/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge;

import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.interfaces.HtLongPollManagerEventFilter;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.HtAjaxCaller;
import com.fin.httrader.webserver.HtServletsBase;
import com.fin.httrader.webserver.ajaxutils.AjaxResponse;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.prochandlers.HtProcHandlerBase;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtBaseIntReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMTBridgeProcessorHistory extends HtServletsBase {

		// 2 msecs
		private static final int QUEUE_TOUT_MSEC = 2000;
		private static final AtomicInteger nextCookie_m = new AtomicInteger();

		// some event types
		private static final String MT4_DELIVERY_ENVELOPE_EVENT = "mt4_delivery_envelope";
		private static final String MT4_HEARTBEAT_EVENT = "mt4_heartbeat";
		


		@Override
		public String getContext() {
				return this.getClass().getSimpleName();
		}

		@Override
		public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
				throw new ServletException("GET not supported");
		}

		@Override
		public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
				res.setContentType("text/xml; charset=UTF-8");
				forceNoCache(res);

				AjaxResponse ajax_resp = new AjaxResponse();

				
				HtBaseIntReq reqObj = null;
				
				try {
						String request_data = this.getStringParameter(req, "request_data", false);
						reqObj = HtProcHandlerBase.parseCommandData(request_data);
						HtBaseIntResp respObj = reqObj.processRequest();
						
						if (respObj ==null)
								throw new HtException(getContext(), "doPost", "Functionality not implemented: " + request_data);
						
						String result = JSONValue.toJSONString(respObj.toJson());
						ajax_resp.setCustomData(result);
						

				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "doPost", "Exception on on HTTP request:\n" + HtException.getFullErrorString(e));
						if (reqObj != null)
								reqObj.onErrorHappened();
						
						ajax_resp.setError(-1, e.toString());
				}

				String data = ajax_resp.serializeToXmlString();
				
				makeOutput(res, reqObj, data);

		}
		
		/*
		helpers
		*/
		
		private void makeOutput(HttpServletResponse res, HtBaseIntReq reqObj, String data)
		{
				try {
						res.setContentLength(data.length());
						res.setHeader("Connection", "close");
						res.setHeader("Cache-Control", "public, must-revalidate, post-check=0, pre-check=0");
				
						res.getOutputStream().print(data);
				}
				catch(IOException e)
				{
						HtLog.log(Level.WARNING, getContext(), "makeOutput", "Exception on on HTTP request:\n" + e.toString());
						
						if (reqObj != null)
								reqObj.onErrorHappened();
				}
		}
		
		
}
