/*
 * HtInquotePacket.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.transport;

import com.fin.httrader.utils.*;
import java.io.DataInputStream;
import java.io.DataOutputStream;

/**
 *
 * @author Victor_Zoubok
 * This is packet which sent to CPP server
 */
public class HtInquotePacket {

	public static final byte FLAG_RT_PACKET = 1;
	public static final byte FLAG_LEVEL2_PACKET = 2;
	public static final byte FLAG_LEVEL1_PACKET = 3;
	public static final byte FLAG_XMLEVENT_PACKET = 4;
	public static final byte FLAG_COMMON_SERVICE_REQUEST_HISTORY = 5;
	public static final byte FLAG_TRANSMITION_FINISH = 6;
	public static final byte FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN = 7; // this tells to run external jur package


	// -------------------------
	// this is the flag showing the type of packet
	private byte[] flag_m = new byte[1];
	// data itself
	private byte[] data_m = null;

	// ---------
	private byte[] dataLength_m = new byte[HtUtils.SOCKET_LENGTH_HEADER_BYTES];

	public void setFlag(byte flag) {
		flag_m[0] = flag;
	}

	public byte getFlag()
	{
		return flag_m[0];
	}
	
	public byte[] getFlag2()
	{
		return flag_m;
	}
	
	public String getIntType()
	{
	
		return String.valueOf( (int)flag_m[0] );
	}

	public void setData(byte[] data)
	{
		data_m = data;
	}


	public byte[] getData()
	{
		return data_m;
	}

	public static String getContext() {
		return HtInquotePacket.class.getSimpleName();
	}

	public <T extends DataInputStream> void read(T layer) throws Exception {

		layer.readFully(this.flag_m);
		layer.readFully(this.dataLength_m, 0, this.dataLength_m.length);

		long in_to_read = HtMathUtils.convertByteToLong(this.dataLength_m);
		if (in_to_read <= 0) {
			throw new HtException(getContext(), "read", "Zero read data length");
		}

		this.data_m = new byte[(int)in_to_read];
		layer.readFully(this.data_m, 0, (int)in_to_read);

	}


	public static <T extends DataOutputStream> void send(byte flag, byte[] data, T layer) throws Exception {
		byte[] consequent_length = HtMathUtils.convertLongToByte(data.length);

		layer.write(flag);
		layer.write(consequent_length, 0, consequent_length.length);
		layer.write(data, 0, data.length);
		layer.flush();

	}

	public static <T extends DataOutputStream> void sendTransmisionFinish( T layer) throws Exception
	{
		byte[] dummy_data = new byte[1];
		dummy_data[0] = 0;
		send(HtInquotePacket.FLAG_TRANSMITION_FINISH, dummy_data, layer);
	}

}
