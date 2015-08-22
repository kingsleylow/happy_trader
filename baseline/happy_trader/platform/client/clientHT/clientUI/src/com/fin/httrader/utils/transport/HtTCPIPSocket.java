/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.transport;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

/**
 *
 * @author Victor_Zoubok
 */
public class HtTCPIPSocket extends HtCommonSocket{
	private Socket socket_m = null;
	
	public HtTCPIPSocket()
	{
		socket_m = new Socket();
	}
	
	public Socket getTCPIPSocket()
	{
		return socket_m;
	}

	@Override
	public InputStream getInputStream() throws IOException {
		return socket_m.getInputStream();
	}

	@Override
	public OutputStream getOutputStream() throws IOException {
		return socket_m.getOutputStream();
	}

	@Override
	public void close() throws IOException {
		socket_m.close();
	}
	
}
