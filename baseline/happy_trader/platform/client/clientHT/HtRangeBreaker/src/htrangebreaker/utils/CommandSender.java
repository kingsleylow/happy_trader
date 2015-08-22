/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;

import com.fin.httrader.utils.hlpstruct.*;
import com.fin.httrader.webserver.*;
import java.io.BufferedInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import com.fin.httrader.utils.*;
import htrangebreaker.utils.gui.MsgBox;

/**
 *
 * @author DanilinS
 * to send commands
 */
public class CommandSender {

	public static class OrderDescriptor
	{
		public String symbol= "";
		public boolean isLocalInstall = false;
		public double issuePrice = -1;
		public String orderPlace = "";
		public String orderMethod = "";
		public double volume = -1;
		public String add_order_info = "";
		public int order_type = -1;
		public JavaUid orderUid = new JavaUid();
		public String description = "";

		public OrderDescriptor()
		{
		}
		
		public OrderDescriptor(
				String symbol,
				boolean isLocalInstall,
				double issuePrice,
				String orderPlace,
				String orderMethod,
				double volume,
				String add_order_info,
				int order_type,
				JavaUid orderUid,
				String description
			)
		{
			this.symbol= symbol;
			this.isLocalInstall = isLocalInstall;
			this.issuePrice = issuePrice;
			this.orderPlace = orderPlace;
			this.orderMethod = orderMethod;
			this.volume = volume;
			this.add_order_info = add_order_info;
			this.order_type = order_type;
			this.orderUid.fromUid(orderUid);
			this.description = description;
		}

		public OrderDescriptor(
				String symbol,
				boolean isLocalInstall,
				double issuePrice,
				String orderPlace,
				String orderMethod,
				double volume,
				String add_order_info,
				int order_type,
				String orderUids,
				String description
			)
		{
			this.symbol= symbol;
			this.isLocalInstall = isLocalInstall;
			this.issuePrice = issuePrice;
			this.orderPlace = orderPlace;
			this.orderMethod = orderMethod;
			this.volume = volume;
			this.add_order_info = add_order_info;
			this.order_type = order_type;
			this.orderUid.fromString(orderUids);
			this.description = description;
		}

		public static OrderDescriptor createOrderStructure()
		{
			return new OrderDescriptor();
		}
	}

	private StringBuilder serverName_m = new StringBuilder();
	private StringBuilder algLibPair_m = new StringBuilder();
	private int tid_m = -1;
	HtSettingsStruct ss_m = null;
	private StringBuilder baseurl_m = new StringBuilder();

	

	static {
		System.getProperties().put("HTTPClient.dontChunkRequests", "false");
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public CommandSender(	HtSettingsStruct ss)
	{
	
		ss_m = ss;
	}

	public void setupRecipient(
		String serverName,
		String algLibPair,
		int tId
	)
	{
		serverName_m.setLength(0);
		serverName_m.append(serverName);

		algLibPair_m.setLength(0);
		algLibPair_m.append(algLibPair);
		tid_m = tId;
	}

	public List<String> cmd_Force_Ping() throws Exception
	{
	
		String result = issueCommand("Force_Ping", "dummy", false);
		XmlParameter xmlparameter = XmlHandler.deserializeParameterStatic(result);

		List<String> symbols = xmlparameter.getStringList("symbols");
		return symbols;

	}



	public String cmd_Force_TE_InstallOrder(
			OrderDescriptor odescr
	) throws Exception
	{
		return issueCommand("Force_TE_InstallOrder", orderDescrToMetacommandStruct(odescr), false);
	}

	public String cmd_Force_TE_UserConfirmFix(JavaUid orderJavaUid) throws Exception
	{
		StringBuilder custom_data = new StringBuilder();
		custom_data.append("UID:").append(orderJavaUid.toString()).append("\n");
		return issueCommand("Force_TE_UserConfirmFix", custom_data.toString(), false);
	}

	public String cmd_Force_TE_CancelOrder(JavaUid orderJavaUid) throws Exception
	{
		StringBuilder custom_data = new StringBuilder();
		custom_data.append("UID:").append(orderJavaUid.toString()).append("\n");
		return issueCommand("Force_TE_CancelOrder", custom_data.toString(), false);
	}

	// initiates breakthrough patter up
	public String cmd_Force_Run_Breakthrough_Up(
		JavaUid machineUid,
		String machineDescr,
		OrderDescriptor orderUpStruct,
		OrderDescriptor orderSLStruct,
		OrderDescriptor orderTPStruct
	) throws Exception
	{
		return issueMetacommadProcCommand(machineUid, machineDescr, orderUpStruct,  orderSLStruct, orderTPStruct, "Force_Run_Breakthrough_Up" );
	}

	public String cmd_Force_Run_Breakthrough_Down(
		JavaUid machineUid,
		String machineDescr,
		OrderDescriptor orderDownStruct,
		OrderDescriptor orderSLStruct,
		OrderDescriptor orderTPStruct
	) throws Exception
	{
		return issueMetacommadProcCommand(machineUid, machineDescr, orderDownStruct, orderSLStruct, orderTPStruct, "Force_Run_Breakthrough_Up" );
	}

	public List<HtLoadedalgsStruct> returnAvailableServers() throws Exception
	{
		List<HtLoadedalgsStruct> result = new ArrayList<HtLoadedalgsStruct>();
		String data = issueCommand(null, null, true);
		XmlParameter rlist = XmlHandler.deserializeParameterStatic(data);

		List<XmlParameter> data_list_p = rlist.getXmlParameterList("data_list");


		for(int i = 0; i < data_list_p.size(); i++) {
			XmlParameter param_i = data_list_p.get(i);
			HtLoadedalgsStruct r = new HtLoadedalgsStruct();
			r.algBrkLib = param_i.getString("alg_brk_pair");
			r.serverID = param_i.getString("server_id");
			r.threadID  = (int)param_i.getInt("tid");

			result.add(r);
			
		}

		return result;
	}

	/**
	 * Helpers
	 */


	private String issueMetacommadProcCommand(
		JavaUid machineUid,
		String machineDescr,
		OrderDescriptor orderBreakStruct,
		OrderDescriptor orderSLStruct,
		OrderDescriptor orderTPStruct,
		String commandName
	) throws Exception
	{
		XmlParameter custom_data_param = new XmlParameter();
		custom_data_param.setString("order_sign_str", orderDescrToMetacommandStruct(orderBreakStruct));
		custom_data_param.setString("order_tp_sign_str", orderDescrToMetacommandStruct(orderSLStruct));
		custom_data_param.setString("order_sl_sign_str", orderDescrToMetacommandStruct(orderTPStruct));
		custom_data_param.setString("machine_uid", machineUid.toString());
		custom_data_param.setString("machine_descr", machineDescr);

		return issueCommand(commandName, XmlHandler.serializeParameterStatic(custom_data_param), false);

	}

	private String orderDescrToMetacommandStruct(OrderDescriptor odescr)
	{
		StringBuilder custom_data = new StringBuilder();
		custom_data.append("SYMBOL:").append(odescr.symbol).append("\n");
		custom_data.append("UID:").append(odescr.orderUid.toString()).append("\n");
		custom_data.append("ORDER_TYPE:").append(odescr.order_type).append("\n");
		custom_data.append("ORDER_METHOD:").append(odescr.orderMethod).append("\n");
		custom_data.append("ORDER_PLACE:").append(odescr.orderPlace).append("\n");
		custom_data.append("ISSUE_PRICE:").append(String.valueOf(odescr.issuePrice)).append("\n");
		custom_data.append("VOLUME:").append(String.valueOf(odescr.volume)).append("\n");
		custom_data.append("ADD_ORDER_INFO:").append(odescr.add_order_info).append("\n");
		custom_data.append("DESCRIPTION: [").append(odescr.description).append("]\n");
		custom_data.append("REGISTER_LOCALLY:").append(odescr.isLocalInstall ? "true" : "false").append("\n");

		return custom_data.toString();
	}

	private String issueCommand(String customName, String customEventData, boolean request_servers) throws Exception {
		baseurl_m.append("http://" + ss_m.host + ":" + ss_m.port + "/htHTTPCommandServlet.jsp");

		baseurl_m.append("?user=" + ss_m.user);
		baseurl_m.append("&hashed_password=" + ss_m.hashed_password);

		XmlHandler handler = new XmlHandler();
		HttpClient httpclient = new DefaultHttpClient();

		if (request_servers) {
			baseurl_m.append("&operation=list_available_servers");
			
		}
		else {
			baseurl_m.append("&operation=send_custom_data");

			baseurl_m.append("&target_thread=" + tid_m);
			baseurl_m.append("&alg_brk_pair=" + HtWebUtils.encodeURIComponent(algLibPair_m.toString()));
			baseurl_m.append("&server_id=" + HtWebUtils.encodeURIComponent(serverName_m.toString()));
		}
		

		String data = null;

		HttpPost httppost = new HttpPost(baseurl_m.toString());

		if (!request_servers) {
			List<NameValuePair> formparams = new ArrayList<NameValuePair>();
			formparams.add(new BasicNameValuePair("custom_event_data", customEventData));
			formparams.add(new BasicNameValuePair("custom_name", customName));
			UrlEncodedFormEntity entity = new UrlEncodedFormEntity(formparams, "UTF-8");
			httppost.setEntity(entity);
		}

	
		
		Utils.initHeaders(httppost);
	
	
		// POST
		// custom_event_data=
		// custom_name

		HttpResponse response = httpclient.execute(httppost);


		int statusCode = response.getStatusLine().getStatusCode();
		if (statusCode != HttpStatus.SC_OK) {
			throw new HtFrameException("Cannot read data from the web service");
		}



		HttpEntity entity_resp = response.getEntity();
		
		if (entity_resp == null) {
			throw new HtFrameException("Cannot read data from the web service");
		}

		InputStream is = entity_resp.getContent();
	
		BufferedInputStream isb = new BufferedInputStream(is);
		HtXmlEventExchangePacket.Packet packet = new HtXmlEventExchangePacket.Packet();

		HtXmlEventExchangePacket.readPacketFromStream(packet, isb);

		if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_ERROR) {
			String errMessage = HtXmlEventExchangePacket.deserializeAsErrorMessage(packet);
			throw new HtException(getContext(), "sendCommand", " Error happened on receiving data: " + errMessage);


		} else if (packet.packet_type_m == HtXmlEventExchangePacket.PACKET_SINGLE_EVENT) {
			XmlEvent event_i = HtXmlEventExchangePacket.deserializeSingleEvent(handler, packet);
			data = event_i.getAsXmlParameter(true).getString("output_data");

		}

		return data;
	}
}
