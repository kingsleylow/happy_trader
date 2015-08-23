/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers.idlefunctors;

import com.fin.httrader.interfaces.RtIdleManagerFunctor;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtCriticalErrorManager;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import com.fin.httrader.utils.win32.HtSysUtils;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 * periodically executes commit functor
 */
public class HealthFunctor extends RtIdleManagerFunctor {

	private String getContext()
	{
			return HealthFunctor.class.getSimpleName();
	}
	
	public HealthFunctor() {
		super("Health functor", RtConfigurationManager.StartUpConst.IDLE_THREAD_HEALTH_FTOR_PERIOD);


	}

	@Override
	public void execute() throws Exception {

		/*
		RtDatabaseManager dbm = RtDatabaseManager.instance();

		if (dbm != null) {
			dbm.commitRtThread();
		}
		*/
		
		// next thing we need to asl
		String result = "\r\n" + checkSystemStatus();
		RtCriticalErrorManager.instance().signalCriticalError("Idle Manager", "System health", result, CriticalErrorEntry.URGENCY_FLAG_INFO);


	}
	
	private String checkSystemStatus()
	{
			String result = "";
			try {
					result = HtSysUtils.returnSystemAnalisysString();
			}
			catch(Throwable e)
			{
					HtLog.log(Level.WARNING, getContext(), "checkSystemStatus", "Exception on obtaining system information: " + e.getMessage());
			}
			
			return result;
	}
}
