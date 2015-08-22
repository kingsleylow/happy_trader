/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.interfaces.RtTradingServerProcess;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class RtTradingServerRemoteProcess implements RtTradingServerProcess {

	private String serverHost_m = null;
	private int spawnerPort_m = -1;
	private String serverId_m = null;

	protected String getContext() {
		return RtTradingServerRemoteProcess.class.getSimpleName();
	}

	public RtTradingServerRemoteProcess(String serverHost, int spawnerPort) {
		serverHost_m = serverHost;
		spawnerPort_m = spawnerPort;

	}

	@Override
	public void startProcess(
					String serverExecutable,
					int serverPort,
					int servicePort,
					String serverId,
					Map<String, String> newEntries) throws Exception {


		XmlParameter command = new XmlParameter();
		command.setCommandName("start_executable");
		command.setString("executable", serverExecutable);
		command.setString("server_id", serverId);
		command.setInt("service_port", servicePort);
		command.setInt("server_port", serverPort);
		command.setStringMap("new_entries", newEntries);


		checkResult(sendData(command));

		serverId_m = serverId;


	}

	@Override
	public void waitForFinish() {
		if (serverId_m != null) {
			XmlParameter command = new XmlParameter();
			command.setCommandName("wait_for_finish");
			command.setString("server_id", serverId_m);

			try {
				checkResult(sendData(command));
			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "destroyProcess", "For " + serverId_m + " when finishing process exception occured: " + e.getMessage());
			}

			serverId_m = null;
		}
	}

	@Override
	public void destroyProcess() {
		if (serverId_m != null) {
			XmlParameter command = new XmlParameter();
			command.setCommandName("destroy_process");
			command.setString("server_id", serverId_m);

			try {
				checkResult(sendData(command));
			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "destroyProcess", "For " + serverId_m + " when destroying process exception occured: " + e.getMessage());
			}

			serverId_m = null;
		}
	}

	/**
	 * Helpers
	 */
	private void checkResult(XmlParameter result) throws Exception {
		if (result.getInt("operation_code") != 0) {
			throw new HtException(getContext(), "checkResult", "Spawner returned the error: " + result.getString("operation_result"));
		}
	}

	private XmlParameter sendData(XmlParameter data) throws Exception {

		String result_s = sendData(XmlHandler.serializeParameterStatic(data));
		return XmlHandler.deserializeParameterStatic(result_s);

	}

	private String sendData(String data) throws Exception {
		Socket socketObj = new Socket();
		String response_s = null;
		InetSocketAddress connectTo = new InetSocketAddress(serverHost_m, spawnerPort_m);
		try {
			socketObj.connect(connectTo);
		} catch (IOException e) {
			throw new HtException(getContext(), "sendData", "Cannot connect to spawner service at: " + connectTo.toString() + " because of: " + e.getMessage());
		}

		DataInputStream sin = null;
		DataOutputStream sout = null;

		try {

			sin = new DataInputStream(socketObj.getInputStream());
			sout = new DataOutputStream(socketObj.getOutputStream());

			byte[] data_buf = data.getBytes();
			byte[] data_buf_length = HtMathUtils.convertLongToByte(data_buf.length);

			sout.write(data_buf_length, 0, data_buf_length.length);
			sout.write(data_buf, 0, data_buf.length);

			// read response
			sin.readFully(data_buf_length, 0, HtUtils.SOCKET_LENGTH_HEADER_BYTES);
			long in_to_read = HtMathUtils.convertByteToLong(data_buf_length);
			if (in_to_read <= 0) {
				throw new HtException(getContext(), "issueCommand", "Zero response length");
			}

			byte[] response_bytes = new byte[(int) in_to_read];
			sin.readFully(response_bytes, 0, (int) in_to_read);

			response_s = new String(response_bytes);
		} catch (Throwable e) {
			throw new HtException(getContext(), "sendData", "Exception on making request: " + e.getMessage() );
		} finally {


			if (sin != null) {
				try {
					sin.close();
					sin = null;
				}
				catch(Throwable e)
				{
				}
			}

			if (sout != null) {
				try {
					sout.close();
					sout = null;
				}
				catch(Throwable e)
				{
				}
			}

			try {
				socketObj.close();
				socketObj = null;
			}
			catch(Throwable e)
			{
			}



		}

		return response_s;

	}
}
