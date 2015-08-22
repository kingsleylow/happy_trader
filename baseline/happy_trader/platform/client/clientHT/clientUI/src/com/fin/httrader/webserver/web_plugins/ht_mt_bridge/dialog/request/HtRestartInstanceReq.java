/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request;

import com.fin.httrader.eventplugins.ht_mt_bridge.HtMtManager;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.HtMtConstants;
import static com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtStartInstanceReq.COMMAND_NAME;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtBaseIntResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtRestartInstanceResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtStartInstanceResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.utils.HtHelperUtils;
import java.util.logging.Level;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Administrator
 */
public class HtRestartInstanceReq extends HtBaseIntReq {

	public static final String COMMAND_NAME = "RESTART_INSTANCE_REQ";

	private boolean isDebug_m = false;
	private String settingsFile_m = null;
	private String actualSrvName_m = null;
	private boolean createForce_m = false; // flag showing that we must forcible recreate entries

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public String getCommandName() {
		return COMMAND_NAME;
	}

	@Override
	public void initialize(JSONObject root_json) throws Exception {
		isDebug_m = HtMtJsonUtils.<Boolean>getJsonField(root_json, "DEBUG");
		settingsFile_m = HtMtJsonUtils.<String>getJsonField(root_json, "SETTINGS_FILE");
		createForce_m = HtMtJsonUtils.<Boolean>getJsonField(root_json, "FORCE_CREATE");

	}

	@Override
	public HtBaseIntResp processRequest() throws Exception {

		HtRestartInstanceResp resp = new HtRestartInstanceResp(getCommandName());

		boolean sucess = false;

		// need to start
		try {
			performRestart();
			sucess = true;
		} catch (Throwable e) {
			throw new HtException(getContext(), "processRequest", "Cannot boot because of: " + e.toString() + ", will shutdown...");
			//XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Cannot boot because of: " + e.getMessage()+ ", will shutdown...");
		} finally {
			// TODO

		}

		return resp;
	}

	void performRestart() throws Exception {
		if (HtMtManager.getInstance().isInitialized()) {

			try {
				HtHelperUtils.stopServer(HtMtManager.getInstance().getInitParams().getCurrentServerName());
			} catch (Throwable e) {
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_WARN, "Cannot shutdown the main server because of: " + e.getMessage() + ", ignoring...");
			}

		}

		// expect no exceptions
		HtHelperUtils.checkEventPluginStoppedNoExcept(HtMtConstants.EVENT_PLUGIN_M);

		// then we need to stat !!!
		// --------------
		if (HtMtManager.getInstance().isInitialized()) {
			throw new HtException(getContext(), "performRestart", "Already initialized");
		}

		// need to store server name
		this.actualSrvName_m = HtHelperUtils.checkSrvCreated(
						HtMtConstants.SRV_NAME_M,
						HtMtConstants.SRV_NAME_BASE_EXE,
						settingsFile_m,
						isDebug_m,
						createForce_m
		);

		HtHelperUtils.checkServerIsStarted(this.actualSrvName_m);

		// check if alglib pair created
		String algLibPair_resolved = HtHelperUtils.checkAlgLibPairCreated(
						HtMtConstants.ALGLIB_NAME_M,
						HtMtConstants.ALGLIB_DLL,
						isDebug_m,
						this.settingsFile_m,
						createForce_m
		);

		HtHelperUtils.loadAndResolveAlgLibPair(this.actualSrvName_m, algLibPair_resolved);
		int tid = HtHelperUtils.loadAndResolveAlgorithBrokerThread(this.actualSrvName_m, algLibPair_resolved);

		// get metadata
		Req_G_GET_ALL_METADATA internalrequestObj = new Req_G_GET_ALL_METADATA();
		Resp_G_GET_ALL_METADATA internalResponseObj = new Resp_G_GET_ALL_METADATA();

		JSONObject custom_data_json = new JSONObject();
		internalrequestObj.toJson(custom_data_json);

		String get_minfo_command_data = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toAlgorithm(
						this.actualSrvName_m,
						algLibPair_resolved,
						tid,
						internalrequestObj.getMtCommand(),
						JSONValue.toJSONString(custom_data_json),
						RtConfigurationManager.StartUpConst.WAIT_RESPONSE_FROM_TRADING_SERVER_SEC
		);

		internalResponseObj.fromJson((JSONObject) JSONValue.parseWithException(get_minfo_command_data));
		// check internal response status
		HtHelperUtils.checkAlgLibResponse(internalResponseObj);

		// check event plugin is created
		String eventPlugin_resolved = HtHelperUtils.checkEventPluginCreated(HtMtConstants.EVENT_PLUGIN_M, createForce_m);

		HtHelperUtils.checkEventPluginStarted(
						eventPlugin_resolved,
						this.actualSrvName_m,
						algLibPair_resolved,
						tid,
						internalResponseObj.getEventPluginParams());

	}

}
