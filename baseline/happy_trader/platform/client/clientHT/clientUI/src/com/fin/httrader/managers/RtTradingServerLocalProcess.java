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
import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class RtTradingServerLocalProcess implements RtTradingServerProcess {

	private ProcessRunner pr_m = null;

	static String getContext() {
		return RtTradingServerLocalProcess.class.getSimpleName();
	}

	public RtTradingServerLocalProcess() {

	}

	private static class ProcessRunner implements Runnable {

		private String[] cmdarr_m = null;
		private String[] execEnviroment_m = null;
		private File f_dir_m = null;
		Process pr_m;
		Thread th_m = null;
		
		//BufferedReader bre_m = null;
		BufferedReader bri_m = null;
		
	

		static String getContext() {
			return ProcessRunner.class.getSimpleName();
		}

		public ProcessRunner(String[] cmdarr, String[] execEnviroment, File f_dir) {
			cmdarr_m = cmdarr;
			execEnviroment_m = execEnviroment;
			f_dir_m = f_dir;

			th_m = new Thread(this);

		}

		public void doStart() throws Exception {

			// start thread
			th_m.start();

		}

		public void waitForFinish() {

			
			try {
				/*
				if (pr_m != null) {
					pr_m.waitFor();
					pr_m = null;
				}
				*/

				// gracefully wait for thread
				th_m.join();

			} catch (Throwable e) {
			}
		}

		public void destroyProcess() {
	
			try {
				if (pr_m != null) {
					pr_m.destroy();
					pr_m = null;
				}

				HtUtils.silentlyCloseReader( bri_m );
				
				th_m.join();

			} catch (Throwable e) {
			}
		}

		@Override
		public void run() {
			
								
			try {
				Runtime rn = Runtime.getRuntime();
				pr_m = rn.exec(cmdarr_m, execEnviroment_m, f_dir_m);

				HtLog.log(Level.INFO, getContext(), "run", "Started trading server process thread");

				bri_m = new BufferedReader(new InputStreamReader(pr_m.getInputStream()));
				//bre_m = new BufferedReader(new InputStreamReader(pr_m.getErrorStream()));
				
				String line = null;
				while ((line = bri_m.readLine()) != null) {
					System.out.println(line);
				}
				
				HtUtils.silentlyCloseReader( bri_m );
				bri_m = null;
				
				/*
				while ((line = bre_m.readLine()) != null) {
					System.out.println(line);
					
					if (killer_m.get() == true) {
						throw new HtException(getContext(), "run", "Terminating process ungracefully");
					}
				
				}
				
				HtUtils.silentlyCloseReader( bre);
				bre = null;
				*/
				
				pr_m.waitFor();

			} catch (Exception e) {
				HtLog.log(Level.WARNING, getContext(), "run", "Exception happanend on trading server process thread: " + e.getMessage());
			}
			
			

			HtLog.log(Level.INFO, getContext(), "run", "Finished trading server process thread");
		}
	};

		// -------------
	@Override
	public void startProcess(
					String serverExecutable,
					int serverPort,
					int servicePort,
					String serverId,
					Map<String, String> newEntries
	) throws Exception {

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

		pr_m = new ProcessRunner(cmdarr, execEnviroment, f_dir);
				//Runtime rn = Runtime.getRuntime();
		//pr_m = rn.exec(cmdarr, execEnviroment, f_dir);

		pr_m.doStart();
		HtLog.log(Level.INFO, getContext(), "startProcess", "Local process was started: " + HtUtils.arrayToString(cmdarr));

		// wait a bit
		HtUtils.Sleep(0.5);
	}

	@Override
	public void waitForFinish() {
		try {
			if (pr_m != null) {
				pr_m.waitForFinish();
				pr_m = null;
			}

		} catch (Throwable e2) {
		}
	}

	@Override
	public void destroyProcess() {
		try {
			if (pr_m != null) {
				pr_m.destroyProcess();
				pr_m = null;
			}

		} catch (Throwable e2) {
		}
	}
}
