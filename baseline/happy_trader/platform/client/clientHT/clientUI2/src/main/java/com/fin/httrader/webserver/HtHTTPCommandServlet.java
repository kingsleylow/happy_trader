/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtTradingServerManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtXmlEventExchangePacket;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtHTTPCommandServlet extends HtServletsBase {

	public static class LengthListenerHelper implements HtXmlEventExchangePacket.LengthListener {

		private ServletOutputStream ostrm_m = null;

		public LengthListenerHelper(ServletOutputStream ostrm) {
			ostrm_m = ostrm;
		}

		public void onContentLength(int contentLength) throws Exception {

			ostrm_m.print(Integer.toHexString(contentLength));
			ostrm_m.print("\r\n");
		}

		public void onChunkFinish() throws Exception {
			ostrm_m.print("\r\n");
			ostrm_m.flush();
		}
	};

	// -----------------
	private LengthListenerHelper ll_m = null;

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		forceNoCache(res);
		res.setHeader("Transfer-Encoding", "chunked");
		res.setContentType("anybinary/type;charset=ISO-8859-1");
		res.flushBuffer();

		ServletOutputStream ostrm = res.getOutputStream();
		ll_m = new LengthListenerHelper(ostrm);

		try {
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "initialize_Get", "Exception: \"" + e.getMessage() + "\"");
			sendError("Exception: \"" + e.getMessage() + "\"", res);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		forceNoCache(res);
		ServletOutputStream ostrm = res.getOutputStream();

		res.setHeader("Transfer-Encoding", "chunked");
		res.setContentType("anybinary/type;charset=ISO-8859-1");
		res.flushBuffer();

		
		ll_m = new LengthListenerHelper(ostrm);

		try {


			// credentials
			String user = this.getStringParameter(req, "user", false);
			String hashed_password = this.getStringParameter(req, "hashed_password", false);
			//String userIp = req.getRemoteHost();

			checkCredentials(user, hashed_password);

			String operation = this.getStringParameter(req, "operation", false);
			if (operation.equalsIgnoreCase("send_custom_data")) {

				// stop session

				int trg_thread = (int) getIntParameter(req, "target_thread", false);
				String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
				String server_id = getStringParameter(req, "server_id", false);
				String custom_name = getStringParameter(req, "custom_name", false);

				// can be POST
				String custom_data = getStringParameter(req, "custom_event_data", false);

				// send request
				String result = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
								server_id,
								alg_brk_pair,
								trg_thread,
								custom_name,
								custom_data,
								RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC);


				// return result
				sendResult(res, result);
			}
			else if (operation.equalsIgnoreCase("list_available_servers")) {
				XmlParameter rlist = new XmlParameter();
				List<XmlParameter> data = new ArrayList<XmlParameter>();
				
				Set<String> servers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList();
				for(Iterator<String> it = servers.iterator(); it.hasNext();)
				{
					String srv_i = it.next();
					int server_status = HtCommandProcessor.instance().get_HtServerProxy().remote_returnServerStatus(srv_i);
					if (RtTradingServerManager.SERVER_RUN_OK == server_status) {
						List<Integer> threads = HtCommandProcessor.instance().get_HtServerProxy().remote_requestStartedThreads(srv_i);

						for(int t = 0; t < threads.size(); t++) {
							int thread_id = threads.get(t);

							Map<String, Integer> loaded = HtCommandProcessor.instance().get_HtServerProxy().remote_requestLoadedAlgorithmBrokerPairs(srv_i, -1);

							for(Iterator<String> it3 = loaded.keySet().iterator(); it3.hasNext(); )
							{
								String algBrkPair = it3.next();
								XmlParameter entry_i = new XmlParameter();

								entry_i.setInt("tid", thread_id);
								entry_i.setString("alg_brk_pair", algBrkPair);
								entry_i.setString("server_id", srv_i);

								data.add(entry_i);

							}
							
						}
					}
				}

				rlist.setXmlParameterList("data_list", data);
				sendResult(res, XmlHandler.serializeParameterStatic(rlist) );
				
			}
			else
				throw new HtException(getContext(), "initialize_Post", "Invalid command");

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "initialize_Post", "Exception: \"" + e.getMessage() + "\"");
			sendError("Exception: \"" + e.getMessage() + "\"", res);

		}


	 
		// close stream
		try {
			// finish chunked encoded
			ostrm.print("0\r\n\r\n");
			ostrm.close();
		} catch (Throwable e) {
		}
		
		return true;

	}

	/**
	 * Helpers
	 *
	 */
	private void checkCredentials(String user, String hashed_password) throws Exception {
		boolean result = HtCommandProcessor.instance().get_HtSecurityProxy().remote_checkUserCredentialsWithHashedPassword(user, hashed_password);

		if (!result) {
			throw new HtException(getContext(), "initialize", "Invalid user or password");
		}

		
	}


	private void sendError(String message, HttpServletResponse res) {
		try {

			HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
			HtXmlEventExchangePacket.serializeAsErrorMessage(message, packet);
			final ServletOutputStream ostrm = res.getOutputStream();

			HtXmlEventExchangePacket.writePacketToStream(packet, ostrm, ll_m);


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "sendError", "Exception in sending error: \"" + e.getMessage() + "\"");

		}
	}

	private void sendResult(HttpServletResponse res, String data) throws Exception {
		XmlHandler handler = new XmlHandler();
		XmlEvent event = new XmlEvent();

		event.getAsXmlParameter(true).setString("output_data", HtUtils.wrapToCDataTags(data));

		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();
		HtXmlEventExchangePacket.serializeSingleEvent(handler, event, packet);

		HtXmlEventExchangePacket.writePacketToStream(packet, res.getOutputStream(), ll_m);
	}
}
