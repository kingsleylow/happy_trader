/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.managers;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author DanilinS
 */
public class RtCriticalErrorManager implements RtManagerBase {

	private final List<CriticalErrorEntry> criticalErrors_m = new ArrayList<CriticalErrorEntry>();
	private File criticalFile_m = null;

	// number of critical errors
	private int criticalErrorsCount_m = 0;


	static private RtCriticalErrorManager instance_m = null;

	// -------------------

	static public RtCriticalErrorManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtCriticalErrorManager();
		}

		return instance_m;
	}

	static public RtCriticalErrorManager instance() {
	
		return instance_m;

	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	private RtCriticalErrorManager() {
		criticalFile_m = new File(
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory() +
						File.separatorChar + 
						RtConfigurationManager.StartUpConst.FATAL_ERROR_FILE
			);
		
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}


					
	public void signalCriticalError(String context, String srvSessionName, Throwable e, int urgency_flag) {
			signalCriticalError(context, srvSessionName, HtException.getFullErrorString(e), urgency_flag);
		
	}

	public void signalCriticalError(String context, String srvSessionName, String reason, int urgent_flag) {
		synchronized (criticalErrors_m) {

			CriticalErrorEntry entry = new CriticalErrorEntry(context, srvSessionName, reason, "", urgent_flag);
			writeEntryToFile(entry);
			criticalErrors_m.add(entry);

			if (entry.getUrgencyFlag() == CriticalErrorEntry.URGENCY_FLAG_EXCEPT) {
				criticalErrorsCount_m++;
				System.out.println(entry.toString());
			}
			else if (entry.getUrgencyFlag() != CriticalErrorEntry.URGENCY_FLAG_INFO) {
				System.out.println(entry.toString());
			}

			doCheckOnAbort();

		}
	}

	
	public List<CriticalErrorEntry> getCriticalErrors() {
		List<CriticalErrorEntry> result = new ArrayList<CriticalErrorEntry>();
		synchronized (criticalErrors_m) {
			result.addAll(criticalErrors_m);
		}

		return result;
	}

	public void clearAllCriticalErrors()
	{
		synchronized (criticalErrors_m) {
			criticalErrors_m.clear();
			criticalErrorsCount_m = 0;
		}
	}

	public boolean doWeHaveCriticalErrors() {
		synchronized (criticalErrors_m) {
			return (criticalErrorsCount_m > 0);
		}
	}

	
	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Shutdown");
	}

	/*
	 * Helpers
	 */

	private void doCheckOnAbort()
	{

		if (RtConfigurationManager.StartUpConst.MAX_CRITICAL_ERRORS_ALLOWED <= criticalErrorsCount_m) {
			HtLog.log(Level.SEVERE, getContext(), "doAbort", "Finishing application as the number of critical errors more than: " + RtConfigurationManager.StartUpConst.MAX_CRITICAL_ERRORS_ALLOWED);

			RtBootManager.instance().callSystemExit(-14);
		}
	}
	
	private void doForceAbort()
	{
			RtBootManager.instance().callSystemExit(-15);
		
	}

	
	private void writeEntryToFile(CriticalErrorEntry centry)
	{
		FileOutputStream fo = null;
		try {
			fo = new FileOutputStream(this.criticalFile_m, true);
			fo.write(centry.toString().getBytes());
			fo.write('\r');
			fo.write('\n');
			
			fo.flush();
		}
		catch(Throwable e)
		{
			HtLog.log(Level.WARNING, getContext(), "writeEntryToFile", "Exception on writing critical error file: " + e.getMessage() );
		}
		finally {
			if (fo != null) {
				try {
					fo.close();
				} catch (IOException ex) {

				}
			}
		}
	}

}
