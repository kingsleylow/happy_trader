/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.interfaces.RtTradingServerProcess;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import java.io.File;
import java.util.Map;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class RtTradingServerLocalProcess implements RtTradingServerProcess {

		private Process pr_m = null;
	

		protected String getContext() {
			return RtTradingServerLocalProcess.class.getSimpleName();
		}

		public RtTradingServerLocalProcess()
		{
				
		}

		@Override
		public void startProcess(
						String serverExecutable,
						int serverPort,
						int servicePort,
						String serverId,
						Map<String, String> newEntries
				) throws Exception
		{
				

				String[] cmdarr = new String[]{
						serverExecutable,
						"-port", String.valueOf(serverPort),
						"-service_port", String.valueOf(servicePort),
						"-errout", "false",
						"-sname", serverId
				};

				File f_file = new File(serverExecutable);
				if (!f_file.exists()) {
					throw new HtException(getContext(), "startProcess", "Server executable does not exist: \"" + serverExecutable + "\"");
				}

				String parent = f_file.getParent();
				File f_dir = new File(parent);

				// create execution enviroment
				String[] execEnviroment = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEnviromentEntries(newEntries);

				pr_m = Runtime.getRuntime().exec(cmdarr, execEnviroment, f_dir);
				HtLog.log(Level.INFO, getContext(), "startProcess", "Local process was started: " + HtUtils.arrayToString(cmdarr));

				// wait a bit
				HtUtils.Sleep(0.5);
		}

		@Override
		public void waitForFinish() {
				try {
						if (pr_m != null) {
								pr_m.waitFor();
								pr_m = null;
						}

				} catch (Throwable e2) {
				}
		}

		@Override
		public void destroyProcess() {
				try {
						if (pr_m != null) {
								pr_m.destroy();
								pr_m = null;
						}

				} catch (Throwable e2) {
				}
		}
}
