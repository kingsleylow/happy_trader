/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok This class provides initialization of smart com helper
 * library passing necessary parameters It will be requested as a singleton for
 * parameters
 */
public class HtSmartComLoader implements HtEventPlugin {

  private int smartComPort_m = -1;
  private String smartComHost_m = null;
  private String smartComLogin_m = null;
  private String smartComPassword_m = null;
  private String server_id_m = null;
  private String alg_brk_pair_m = null;
  private String rt_provider_m = null;
  private String rt_provider_bar_postfix_m = "_bar"; // when retreiving 1 minute bars we add this postifix to provider name
  private boolean accompanyTickWith1MinuteBars_m = false;
  private List<String> portfolioNameList_m = new ArrayList<String>(); 
  private boolean automaticallyReconnect_m = false;
  private int reconnectTimeoutSec_m = 30;
  //private volatile boolean isConnected_m = false;
  // if set we periodycally request my trades
 
  private int smartcom_log_level_m = 2;
  private int broker_seq_num_m = -1;
  private StringBuilder name_m = new StringBuilder();
  private static final int TIMEOUT_SEC = 10;

  // --------------------------------------------
  public static String getContext() {
	return HtSmartComLoader.class.getSimpleName();
  }

  public HtSmartComLoader() throws Exception {
  }

  @Override
  public Integer[] returnSubscribtionEventTypes() {
	// catch event from broker
	return new Integer[]{XmlEvent.ET_Notification, XmlEvent.ET_RtProviderSynchronizationEvent};
  }

  @Override
  public void initialize(Map<String, String> initdata) throws Exception {

	//isConnected_m = false;

	smartcom_log_level_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "LOG_LEVEL"));
	smartComPort_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "SMARTCOM_PORT"));
	smartComHost_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_HOST");
	smartComLogin_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_LOGIN");
	smartComPassword_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_PASSWORD");
	rt_provider_m = HtUtils.getParameterWithException(initdata, "RT_PROVIDER");
	reconnectTimeoutSec_m = HtUtils.parseInt( HtUtils.getParameterWithException(initdata, "RECONNECT_TIMEOUT") );

	server_id_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_SERVER_ID");
	alg_brk_pair_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_ALG_BRK_PAIR");

	rt_provider_bar_postfix_m = HtUtils.getParameterWithException(initdata, "RT_PROVIDER_BAR_POSTFIX");
	accompanyTickWith1MinuteBars_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "ACCOMPANY_TICK_1MIN_BAR"));


	portfolioNameList_m.clear();
	String portfolioNameString = HtUtils.getParameterWithException(initdata, "PORTFOLIO_NAME");
	String[] s = portfolioNameString.split(",");
	if (s != null) {
	  for(int i = 0; i < s.length; i++) {
		if (s[i] != null && s[i].length() > 0) {
		  portfolioNameList_m.add( s[i].trim() );
		}
	  }
	}
	
	

	automaticallyReconnect_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "AUTOMATICALLY_RECONNECT"));

	broker_seq_num_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "BROKER_SEQ_NUM"));
	
  }

  @Override
  public void deinitialize() throws Exception {
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
  public void execute(XmlEvent alertData) throws Exception {
	// extract recepient

	if (XmlEvent.getNotificationType(alertData) == XmlEvent.ETNT_ThreadStarted) {
	  // event is thread started notification
	  String server_id = alertData.getAsXmlParameter().getString("server_id");
	  String alg_lib_pair = alertData.getAsXmlParameter().getString("alg_lib_pair");
	  //int tid = (int)alertData.getAsXmlParameter().getInt("tid");

	  if (server_id_m.equalsIgnoreCase(server_id) && alg_brk_pair_m.equalsIgnoreCase(alg_lib_pair)) {

		issueConnectCommand();

	  }

	}
	// automatically reconnect
	else if (alertData.getEventType() == XmlEvent.ET_RtProviderSynchronizationEvent) {
	  if (this.automaticallyReconnect_m) {
		String server_id = alertData.getAsXmlParameter().getString("server_id");
		String rt_provider = alertData.getAsXmlParameter().getString("provider_name");

		int msgType = (int) alertData.getAsXmlParameter().getInt("type");
		if (msgType == HtRtData.SYNC_EVENT_PROVIDER_FINISH) {
		  if (server_id_m.equalsIgnoreCase(server_id) && rt_provider.equalsIgnoreCase(rt_provider_m)) {
			//isConnected_m = false;
			
			HtLog.log(Level.INFO, getContext(), "execute", "Waiting for reconnect (sec): " + this.reconnectTimeoutSec_m);

			HtUtils.Sleep(this.reconnectTimeoutSec_m);
			// reconnect
			issueConnectCommand();
		  }
		} else if (msgType == HtRtData.SYNC_EVENT_PROVIDER_START) {
		  // success in connection
		  if (server_id_m.equalsIgnoreCase(server_id) && rt_provider.equalsIgnoreCase(rt_provider_m)) {
			//isConnected_m = true;
		  }
		}

	  }
	}

  }

  /**
   * Helpers
   */
  private void issueConnectCommand() throws Exception {
	// custom data
	// pass parameters to connect
	XmlParameter xmlparam_connect = new XmlParameter();
	xmlparam_connect.setString("host", smartComHost_m);
	xmlparam_connect.setInt("port", smartComPort_m);
	xmlparam_connect.setString("login", smartComLogin_m);
	xmlparam_connect.setInt("smartcom_log_level", this.smartcom_log_level_m);
	xmlparam_connect.setString("password", smartComPassword_m);
	xmlparam_connect.setStringList("portfolio_list", portfolioNameList_m);
	xmlparam_connect.setString("rt_provider", rt_provider_m);
	xmlparam_connect.setString("rt_provider_bar_postfix", rt_provider_bar_postfix_m);
	xmlparam_connect.setInt("accompany_ticks_with_bars", accompanyTickWith1MinuteBars_m ? 1 : 0);



	// send event
	String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			server_id_m,
			alg_brk_pair_m,
			-1, // delegate to all threads
			broker_seq_num_m,
			"connect",
			XmlHandler.serializeParameterStatic(xmlparam_connect),
			TIMEOUT_SEC);

	checkSmartComStatusString(out);

	
  }

  public static void checkSmartComStatusString(String status_string) throws Exception {
	XmlParameter status_param = XmlHandler.deserializeParameterStatic(status_string);
	if (!status_param.getCommandName().equalsIgnoreCase("smartcombrokerprovider_com_status")) {
	  throw new HtException(getContext(), "checkSmartComStatusString", "Invalid smartcomprovider response");
	}

	int code = (int) status_param.getInt("code");
	String message = status_param.getString("status");

	if (code != 0) {
	  throw new HtException(getContext(), "checkSmartComStatusString", "Exception happened: " + code + " - " + message);
	}

  }
}
