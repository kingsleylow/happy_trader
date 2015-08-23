/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.managers;

import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.transport.HtCommonSocket;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author DanilinS
 * This is the base class for additional threads dedicated to RtTradingServer class
 */
public abstract class RtTradingServerBaseThread<T> extends Thread{
	private HtCommonSocket socket_m = null;
	private DataInputStream sin_m = null;
	private DataOutputStream sout_m = null;
	private T server_m = null;
	
	// event to shutdown
	private Event toShutdown_m = new Event();
	private Event started_m = new Event();
	private Event errorInThread_m = new Event();

	// --------------------------
	
	public RtTradingServerBaseThread()
	{
	}

	// this must initialze sockets and data streams
	// most likely must be called in doRun
	public void performDataStreamInitializations(HtCommonSocket socket, DataInputStream sin, DataOutputStream sout)
	{
		socket_m = socket;
		sin_m = sin;
		sout_m = sout;
	}

	public abstract void doRun() throws Exception;

	protected abstract String getClassContext();

	public HtCommonSocket getSocket()
	{
		return socket_m;
	}

	protected DataInputStream getInputStream()
	{
		return sin_m;
	}


	protected DataOutputStream getOutputStream()
	{
		return sout_m;
	}

	public void resetStarted() {
		started_m.reset();
		errorInThread_m.reset();
	}

	public void signalToShutdown() {
		toShutdown_m.signal();
	}

	public void resetToShutdown() {
		toShutdown_m.reset();
	}
	
	public boolean waitShutdownEvent(long timeout)
	{
		return toShutdown_m.waitEvent(timeout);
	}
	

	public void setBaseReference(T ts)
	{
		server_m = ts;
	}

	public T getBaseReference()
	{
		return server_m;
	}

	public boolean isStarted() {
		return started_m.waitEvent(0);
	}

	public void closeComChannel() {
		try {
			socket_m.close();
		} catch (Throwable e) {
		}
	}

	private void cleanResources() {
		try {
			if (sin_m != null) {
				sin_m.close();
			}
		} catch (Exception e) {
		}

		try {
			if (sout_m != null) {
				sout_m.close();
			}
		} catch (Exception e) {
		}

		closeComChannel();

	}

	public boolean waitForThreadInit(long msec) {
		return started_m.waitEvent(msec);
	}

	public void setErroredStatus() {
		if (!errorInThread_m.waitEvent(0)) {
			errorInThread_m.signal();
		}
	}

	public boolean isErrorOccured() {
		return errorInThread_m.waitEvent(0);
	}

	
	// the thread responsible for background sending of RT data to the server
	@Override
	public void run() {
		try {

			started_m.signal();

			HtLog.log(Level.INFO, getClassContext(), "run", "Thread is initialized");

			// run
			doRun();

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getClassContext(), "run", "Exception: " + e.getMessage(), null);
		}


		cleanResources();
		HtLog.log(Level.INFO, getClassContext(), "run", "Finishing...");

	}
}
