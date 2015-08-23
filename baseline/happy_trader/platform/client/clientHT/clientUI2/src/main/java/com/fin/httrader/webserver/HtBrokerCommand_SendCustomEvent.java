/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerCommand_SendCustomEvent extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			
			String alg_brk_pair = getStringParameter(req, "alg_brk_pair", false);
			setStringSessionValue(getUniquePageId(), req, "alg_brk_pair", alg_brk_pair);

			String server_id = getStringParameter(req, "server_id", false);
			setStringSessionValue(getUniquePageId(), req, "server_id", server_id);

			String target_thread = getStringParameter(req, "target_thread", false);
			setStringSessionValue(getUniquePageId(), req, "target_thread", target_thread);
			
			setStringSessionValue(getUniquePageId(), req, "def_resp_timeout", String.valueOf(RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC));



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

			if (operation.equalsIgnoreCase("send_custom_event")) {

				String total_data = getStringParameter(req, "total_data", false);
				XmlParameter request_param = XmlHandler.deserializeParameterStatic(total_data);

				String server_id = request_param.getString("server_id");
				String no_wait_call = request_param.getString("no_wait_call");
				
				int trg_thread = HtUtils.parseInt(request_param.getString("target_thread"));
				String alg_brk_pair = request_param.getString("alg_brk_pair");

				String custom_name =  request_param.getString("custom_name");
				String custom_data = request_param.getString("custom_data");
				int timeout_sec = HtUtils.parseInt(request_param.getString("time_out_sec"));
				if (timeout_sec <=0)
				  timeout_sec = RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC;

				String result = null;
				if (no_wait_call.equalsIgnoreCase("false")) {
					result = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
								server_id,
								alg_brk_pair,
								trg_thread,
								custom_name,
								custom_data,
								timeout_sec);
				}
				else {
					HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm_NoWait (
								server_id,
								alg_brk_pair,
								trg_thread,
								custom_name,
								custom_data
					);

					result = "NON-BLOCKING CALL";
				}


				ajaxCaller.setData(result);

			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
			
		}

		ajaxCaller.serializeToXml(res);
		return true;

	} //
}
