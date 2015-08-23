/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.alertviewer.implement.utils;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import jsync.Event;

/**
 *
 * @author DanilinS
 * class which listens for input packets
 */
public class HTTPListener extends Thread{

	private StringBuilder host_m = new StringBuilder();

	private int port_m = -1;

	private Socket evSock_m = null;

	private DataInputStream rtIn_m = null;

	private DataOutputStream rtOut_m = null;

	private Event toShutdown_m = new Event();

	private Event started_m = new Event();

	private Event errorInThread_m = new Event();

	HTTPListenerMessages messageListener_m = null;
	

	protected String getClassContext() {
		return getClass().getSimpleName();
	}
	
	public HTTPListener(String host, int port)
	{
		host_m.append(host);
		port_m = port;
	}

	public void run() {
		try {

			started_m.signal();

			InetSocketAddress connectTo = new InetSocketAddress(host_m.toString(), port_m);
			Socket socketObj = new Socket();
			socketObj.connect(connectTo, 10000);

			rtIn_m = new DataInputStream(evSock_m.getInputStream());
			rtOut_m = new DataOutputStream(evSock_m.getOutputStream());

			while (true) {

				// read data and parse it
				

				if (toShutdown_m.waitEvent(0)) {
						break;
				}
			}

		} catch (Throwable e) {

		}

		// finish
		cleanResources();

	}

	public void signalToShutdown() {

		toShutdown_m.signal();

		try {
			join(5000);
		}
		catch(Throwable e)
		{
		}
		finally 
		{
			// clean finally if any
			cleanResources();
		}

	
	}

	public void startProcessing() throws Exception
	{
		started_m.reset();
		toShutdown_m.reset();

		start();

		if (!started_m.waitEvent(5000))
			throw new Exception("Cannot start HTTP Listener");
		

	}

	// --------------
	// helpers

	private void cleanResources() {
		try {
			if (rtIn_m != null) {
				rtIn_m.close();
			}
		} catch (Exception e) {
		}

		try {
			if (rtOut_m != null) {
				rtOut_m.close();
			}
		} catch (Exception e) {
		}

		try {
			if (evSock_m != null) {
				evSock_m.close();
			}
		} catch (Exception e) {
		}

	}
	

}
