/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtEventListener;
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
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.Unmarshaller;

/**
 *
 * @author Victor_Zoubok
 */
public class HtHTTPCommandGateway extends HtServletsBase {

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

		final HtAjaxCaller ajaxCaller = new HtAjaxCaller();
		ajaxCaller.setOk();

		String sessionId = null;

		try {

			// cehck user session
			String user = this.getStringParameter(req, "user", false);
			String hashed_password = this.getStringParameter(req, "hashed_password", false);
			String userIp = req.getRemoteHost();

			checkCredentials(user, hashed_password);


			String content = getStringParameter(req, "content", false);
			XmlParameter request_param = XmlHandler.deserializeParameterStatic(content);

			String command_data = request_param.getString("command_data");
			String event_name = request_param.getString("event_name");
			String command_name = request_param.getString("command_name");
			String non_blocking_s = request_param.getString("non_blocking");
			boolean non_blocking = non_blocking_s.equalsIgnoreCase("true");
			final int tout_sec = HtUtils.parseInt(request_param.getString("tout_sec"));


			// create event
			XmlEvent xmlevent = new XmlEvent();


			xmlevent.setEventType(XmlEvent.ET_CustomEvent);
			xmlevent.setType(XmlEvent.DT_CommonXml);

			xmlevent.getAsXmlParameter().setCommandName(event_name);
			xmlevent.getAsXmlParameter().setString("command", command_name);
			xmlevent.getAsXmlParameter().setString("command_data", command_data);
			xmlevent.getAsXmlParameter().setInt("non_blocking", non_blocking ? 1 : 0);
			xmlevent.getAsXmlParameter().setInt("tout_sec", tout_sec);



			// blocking - must spawn the thread and wait for a result

			final SingleReaderQueue arrived = new SingleReaderQueue("HTTPCommandGateway event queue", false, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);

			final LongParam success = new LongParam(-1);
			final ArrayList list = new ArrayList();



			//

			HtEventListener eventReceiver = new HtEventListener() {

				@Override
				public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
					if (event.getEventType() == XmlEvent.ET_CustomEvent) {
						if (event.getType() == XmlEvent.DT_CommonXml) {

							XmlParameter xml_param = event.getAsXmlParameter();
							String custom_event_name = xml_param.getCommandName();

							if (custom_event_name != null) {
								if (custom_event_name.equals("custom_alg_event_return_result")) {
									// ours
									arrived.put(event);
								}
							}
						}
					}
				}

				@Override
				public String getListenerName() {
					return "listener: [HTTPCommandGateway event listener]";
				}
			};

			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_CustomEvent, 5008, eventReceiver);



			Thread runnable = new Thread() {

				@Override
				public void run() {
					boolean to_stop = false;
					HtTimeCalculator tcalc = new HtTimeCalculator();

					while (true) {

						if (to_stop) {
							break;
						}

						// wait 1 sec
						arrived.get(list, 1000);

						for (Iterator it = list.iterator(); it.hasNext();) {
							XmlEvent returned = (XmlEvent) it.next();

							if (returned != null) {
								ajaxCaller.setData(returned.getAsXmlParameter().getString("out"));
								to_stop = true;
							}
						}

						if (tcalc.elapsed() > tout_sec * 1000) {
							break;
						}


					} // end loop

					if (to_stop) {
						// success
						success.setLong(0);
					}


				}
			};

			runnable.start();

			// push event
			// this event must go through HtAlgorithmRequstBroker
			// and then return response here
			RtGlobalEventManager.instance().pushCommonEvent(xmlevent);

			runnable.join();

			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(eventReceiver);


			if (success.getLong() == -1) {
				throw new HtException(getContext(), "doPost", "Timeout exceeded its value when trying to get results");
			}




		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());

		}

		ajaxCaller.serializeToXml(res);
	}
	/**
	 * Helpers
	 */


	private void checkCredentials(String user, String hashed_password) throws Exception {
		boolean res = HtCommandProcessor.instance().get_HtSecurityProxy().remote_checkUserCredentialsWithHashedPassword(user, hashed_password);

		if (!res) {
			throw new HtException(getContext(), "initialize", "Invalid user or password");
		}

	}

	
}
