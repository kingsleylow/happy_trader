/*
 * HtExternalApiRun_ServletThread.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.externalapi.HtExtScriptLogger;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.Uid;
import java.io.IOException;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;



/**
 *
 * @author Victor_Zoubok
 * this is a servlet running external API in its body
 */
public class HtExternalApiRun_ServletThread extends HtServletsBase {

	
	
	private static class HtExtLoggerImpl extends HtExtScriptLogger {

		HtExternalApiRun_ServletThread base_m = null;
		HttpServletResponse res_m = null;

		public HtExtLoggerImpl(HttpServletResponse res, Uid runUid, HtExternalApiRun_ServletThread base) throws Exception{

			super(runUid);
			
			base_m = base;
			res_m = res;
		}

		// assume no entry
		@Override
		public void addStringEntry_Derived(String data) throws Exception {
			try {
				base_m.addStringEntry(res_m, data);
			} catch (Throwable e) {
				
				throw new HtException("HtExtLoggerImpl", "addStringEntry_Derived", e.getMessage());
			}
		}

		@Override
		public void addErrorEntry_Derived(String data) throws Exception{
			try {
				base_m.addErrorEntry(res_m, data);
			} catch (Throwable e) {
				
				throw new HtException("HtExtLoggerImpl", "addErrorEntry_Derived", e.getMessage());
			}
		}
	}

	// ----------------------------------------
	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtExternalApiRun_ServletThread */
	public HtExternalApiRun_ServletThread() {
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		//res.setContentType("text/plain; charset=windows-1251");

		forceNoCache(res);
		String run_uid_s = "";

		try {

			run_uid_s = getStringParameter(req, "run_uid", false);
			Uid run_uid = new Uid(run_uid_s);

			// move to HtExternalApiRun_v2_Helper
			//uploadToTempFile(req, res, packageLoadPath, packageName);

			HtLog.log(Level.INFO, getContext(), "doGet", "Started external API: " + run_uid_s );
			sendFirstDummyBytes(res);
		
			HtExtLoggerImpl newLogger = new HtExtLoggerImpl(res, run_uid, this);
			HtCommandProcessor.instance().get_HtExternalApiProxy().remote_executeWholeJarInCallingThread(newLogger, run_uid);

			sendFinished(res);
			

			HtLog.log(Level.INFO, getContext(), "doGet", "Finished external API: " + run_uid_s);

		// do nothing
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "doGet", "Error of external API run: " + e.getMessage() + " of id: " + run_uid_s);
		}

		HtLog.log(Level.INFO, getContext(), "doGet", "Finished external API run of id: " + run_uid_s);

	}

	// -----------------------------------
	// sends data and flushes it
	private void sendDataChunk(HttpServletResponse res, String data) throws Exception {

		res.getOutputStream().print("<script type='text/javascript'>");
		res.getOutputStream().print("window.parent.insertHTMLpiece_c('");
		res.getOutputStream().print(data);
		res.getOutputStream().print("');");
		res.getOutputStream().print("</script>\n");
		res.flushBuffer();
	}

	private void sendFinished(HttpServletResponse res) throws Exception {

		res.getOutputStream().print("<script type='text/javascript'>");
		res.getOutputStream().print("window.parent.tellFinished_c();");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	// sends first dummy bytes
	private void sendFirstDummyBytes(HttpServletResponse res) throws Exception {
		// send this to force IE to parse
		StringBuilder dummydata = new StringBuilder();
		dummydata.setLength(256);
		res.getOutputStream().print(dummydata.toString());
		res.flushBuffer();
	}

	
	
	
	private void addErrorEntry(HttpServletResponse res, String data) throws Exception {

		sendDataChunk(res, data);
	}



	private void addStringEntry(HttpServletResponse res, String data) throws Exception {
	
		sendDataChunk(res, data);

	}
}
