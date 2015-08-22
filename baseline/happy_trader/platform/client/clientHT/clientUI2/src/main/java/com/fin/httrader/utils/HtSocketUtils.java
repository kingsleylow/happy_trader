/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import com.fin.httrader.utils.transport.HtCommonSocket;
import com.fin.httrader.utils.transport.HtTCPIPSocket;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSocketUtils {
	public static String getContext() {
		return HtSocketUtils.class.getSimpleName();
	}

	public static void readFullyFromStream(byte[] inpbuffer, int off, int len, InputStream is) throws Exception {
				if (len < 0) {
						throw new IndexOutOfBoundsException();
				}

				int n = 0;
				while (n < len) {
						int count = is.read(inpbuffer, off + n, len - n);
						if (count < 0) {
								throw new EOFException();
						}
						n += count;
				}
		}
	
	public static HtCommonSocket connectToServerUsingTCPIP(String host, int port, int timeout, int maxConnectAttempts) throws Exception
	{
		
		InetSocketAddress connectTo = new InetSocketAddress(host, port);

		HtTCPIPSocket socketObj = null;

		boolean success = false;
		for (int i = 1; i <= maxConnectAttempts; i++) {
			try {
				// establish connection to the server
				
				// recreate socket object each time
				socketObj = new HtTCPIPSocket();
				socketObj.getTCPIPSocket().connect(connectTo, timeout);
				success = true;
				break;

			} catch (IOException e) {
				HtLog.log(Level.WARNING, getContext(), "connectToServerUsingTCPIP", "Exception on connect: \"" + e.getMessage() + "\" ,repeating...");
				
				socketObj = null;
				HtUtils.Sleep(1.0);

			}
		}

		if (!success) {
			throw new HtException(getContext(), "connectToServerUsingTCPIP", "Attempt to connect to " + host + ":" + port + "\" failed");
		}

		return socketObj;
	}
	
	
}
