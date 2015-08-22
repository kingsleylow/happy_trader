/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.configprops.helper.LoadedDllsHelper;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.managers.idlefunctors.HealthFunctor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.managers.idlefunctors.QueuesLoadStatusFunctor;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.win32.HtSysUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author DanilinS
 */
public class RtBootManager implements RtManagerBase {

		StringBuilder baseDir_m = new StringBuilder();
		// --------------------
		static private RtBootManager instance_m = null;

		static public RtBootManager instance() {

				if (instance_m == null) {
						instance_m = new RtBootManager();
				}

				return instance_m;

		}

		static public RtBootManager create() throws Exception {
				if (instance_m == null) {
						instance_m = new RtBootManager();
				}

				return instance_m;
		}

		protected String getContext() {
				return this.getClass().getSimpleName();
		}

		private RtBootManager() {

				HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
		}

		// --------------------------------------
		public void startServer(String[] args) {
				
				System.out.println("Starting Happy Trader Server: " + HtStartupConstants.VERSION);
				//dumpCommandLineString(args);
				//
				String databaseUrl = null;
				if (args.length >= 1) {
						databaseUrl = args[0];
				}

		//System.out.println("db url: " + databaseUrl);
				String logLevel = null;
				String winstoneLogLevel = null;
				//String binRoot = null;

				if (args.length >= 2) {
						logLevel = args[1];
				} else {
						logLevel = "FINE";
				}

				if (args.length >= 3) {
						winstoneLogLevel = args[2];
				} else {
						winstoneLogLevel = "FULL_DEBUG";
				}

				// here a special flag to make management operations like recreate all tables
				String manageOperation = "";
				if (args.length >= 4) {
						manageOperation = args[3];
				} else {
						manageOperation = null;
				}

				//
				startupServer(databaseUrl, logLevel, winstoneLogLevel, manageOperation);

				System.out.println("Started, ready for commands!");

		}

		@Override
		public void shutdown() {
				System.out.println("Finishing...");
				shutdownManagers();
				System.out.println("Finished");

				HtLog.log(Level.INFO, getContext(), getContext(), "Server finished totally, special thanks to Victor Zoubok");

				callSystemExit(0);
		}

		public String getBaseDirectory() {
				if (baseDir_m.length() <= 0) {
						System.out.println("Base directory is not initialized, it's a severe error!");
						callSystemExit(-1);
				}

				return baseDir_m.toString();
		}

	// helpers
		// ---------------------------------------
		public void callSystemExit(int code) {

				System.exit(code);

		}

		private void startupServer(
			String databaseUrl,
			String logLevel,
			String winstoneLogLevel,
			String manageOperation) {

				try {

						if (databaseUrl == null || databaseUrl.length() <= 0) {
								throw new HtException(getContext(), "startupServer", "Invalid database connection URL");
						}

						baseDir_m.setLength(0);
						baseDir_m.append(HtSystem.getEnvironmentValue("HT_SERVER_DIR"));

						if (baseDir_m == null || baseDir_m.length() <= 0) {
								throw new HtException(getContext(), "startupServer", "Invalid base directory, set up HT_SERVER_DIR enviroment entry");
						}
						
						System.out.println("Base directory is: " + baseDir_m);

						// create local directories
						createDirectories();

			// now need to find
				} catch (Throwable e) {
						System.out.println("Startup exception: " + e.toString());
						
						e.printStackTrace();
						
						callSystemExit(-2);
				}

		//
				// init logger
				try {
						HtLog.create(Level.parse(logLevel));

						System.out.println("Log level: " + logLevel);

				} catch (Throwable e) {
						System.out.println("Cannot initialize logger: " + e.toString());
						e.printStackTrace();
						callSystemExit(-3);
				}

				HtLog.log(Level.INFO, getContext(), "startupServer", "Platform client starting...");
				HtLog.log(Level.INFO, getContext(), "startupServer", "Base directory: \"" + getBaseDirectory() + "\"");

				// instantiating managers
				try {

						// load driver
						//Class dbDriverClazz = Class.forName(databaseDriverClass);

						// managers
						instantiateManagers(databaseUrl, winstoneLogLevel);

						String javaVmS = getJavaPlatforInfo();
						System.out.println("Java VM: " + javaVmS);
						HtLog.log(Level.INFO, getContext(), "startupServer", "Java VM: " + javaVmS);

						System.out.println("Winstone log level: " + winstoneLogLevel);
						HtLog.log(Level.INFO, getContext(), "startupServer", "Winstone log level: " + winstoneLogLevel);

						System.out.println("Database client encoding: " + RtDatabaseManager.instance().getClientEncodingString());
						HtLog.log(Level.INFO, getContext(), "startupServer", "Database client encoding: " + RtDatabaseManager.instance().getClientEncodingString());

						System.out.println(RtDatabaseManager.instance().getEnviromentInfo());
						HtLog.log(Level.INFO, getContext(), "startupServer", RtDatabaseManager.instance().getEnviromentInfo());

						// jniPath tells what path to get as JNI
						loadJNILibraries();

				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "startupServer", "Exception on starting managers " + e.toString(), null);
						
						e.printStackTrace();
						//System.out.println(e.getMessage());

						// exit here
						callSystemExit(-4);
				}

				// now if we need to perform some management operations
				completeManagingOperation(manageOperation);

		// check for enviroment variable to suceed with C++ debug
				//String ntSymbolPath = HtSystem.getEnvironmentValue("_NT_SYMBOL_PATH");
				//HtLog.log(Level.WARNING, getContext(), "startupServer", "_NT_SYMBOL_PATH=[" + ntSymbolPath+"]");
				//System.out.println("_NT_SYMBOL_PATH=[" + ntSymbolPath+"]");
				// log
				XmlEvent.createSimpleAttributedLog_2("GLOBAL", "GLOBAL", CommonLog.LL_INFO, "Сервер удачно стартовал", "время - " + HtDateTimeUtils.time2String_Gmt(HtDateTimeUtils.getCurGmtTime()), "Версия - " + HtStartupConstants.VERSION);
				HtLog.log(Level.INFO, getContext(), "startupServer", "Server successfully started");

		}

		private void completeManagingOperation(String manageOperation) {
				if (manageOperation != null) {
						try {
								HtLog.log(Level.INFO, getContext(), "startupServer", "About to perform managing operation: " + manageOperation);
								System.out.println("About to perform managing operation: " + manageOperation);

								if (manageOperation.equalsIgnoreCase("recreate_tables")) {

										HtCommandProcessor.instance().get_HtDatabaseProxy().remote_recreateGlobalTables();

								} else {
										throw new HtException(getContext(), "startupServer", "Invalid managing operation: " + manageOperation);
								}

						} catch (Throwable e) {
								HtLog.log(Level.WARNING, getContext(), "startupServer", "Exception on managining operation: "
									+ manageOperation + " - " + e.getMessage());
								System.out.println("Exception on managining operation: " + manageOperation + " - " + e.getMessage());
						}

						System.out.println("Returning from managing operation....");
						callSystemExit(0);
				}
		}

		private void createDirectories() throws Exception {

				File config_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory());
				config_dir.mkdirs();

				File log_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory());
				log_dir.mkdirs();

				File java_rt_prov_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_RTProvidersDir());
				java_rt_prov_dir.mkdirs();

				File java_hst_prov_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_HistoryProvidersDir());
				java_hst_prov_dir.mkdirs();

				File java_alert_plug_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_AlertPluginsProvidersDir());
				java_alert_plug_dir.mkdirs();

				File ext_jar_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory());
				ext_jar_dir.mkdirs();

				File ext_jar_tmpl_dir = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalTemplatesScriptsDirectory());
				ext_jar_tmpl_dir.mkdirs();
		}

		private void instantiateManagers(String connectionUrl, String winstoneLogLevel) throws Exception {

				RtCriticalErrorManager.create();
				RtConfigurationManager.create(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory());

				RtConsoleManager.create();
				RtIdleManager.create();

				// registering functors
				RtIdleManager.instance().registerFunctor(new HealthFunctor());
				RtIdleManager.instance().registerFunctor(QueuesLoadStatusFunctor.getInstance());

				RtBackgroundJobManager.create();

				RtGlobalEventManager.create();
				RtAlertPluginManager.create();

				RtWebServletManager.create(RtConfigurationManager.StartUpConst.HTTP_LISTEN_PORT, connectionUrl,  winstoneLogLevel);
				HtCommandProcessor.create();

				RtRealTimeProviderManager.create();

				RtDatabaseManager.create(RtConfigurationManager.StartUpConst.JNDI_CONNECT_POOL_LOCATION);

				RtTradingServerManager.create();
				RtCommonServiceManager.create();
				RtHistoryProviderManager.create();

				RtDatabaseSelectionManager.create();
				RtDatabaseSchemaManager.create();

				RtExternalApiRunManager.create();
				RtSecurityManager.create();

				RtLongPollManager.create();

		}

		public void shutdownManagers() {
				HtLog.log(Level.INFO, getContext(), "shutdownManagers", "Begin global shutdown");

				RtLongPollManager.instance().shutdown();

				RtIdleManager.instance().startShutDown();

				RtSecurityManager.instance().shutdown();
				RtExternalApiRunManager.instance().shutdown();

				HtCommandProcessor.instance().shutdown();
				RtAlertPluginManager.instance().shutdown();

				RtDatabaseSelectionManager.instance().shutdown();

				RtCommonServiceManager.instance().shutdown();
				RtTradingServerManager.instance().shutdown();

				RtHistoryProviderManager.instance().shutdown();
				RtDatabaseManager.instance().shutdown();

				RtRealTimeProviderManager.instance().shutdown();

				RtGlobalEventManager.instance().shutdown();
				RtDatabaseSchemaManager.instance().shutdown();

				RtWebServletManager.instance().shutdown();

				RtBackgroundJobManager.instance().shutdown();
				RtIdleManager.instance().shutdown();
				RtConsoleManager.instance().shutdown();

				RtConfigurationManager.instance().shutdown();

				RtCriticalErrorManager.instance().shutdown();

				HtLog.log(Level.INFO, getContext(), "shutdownManagers", "Global shutdown finished");
		}

		/**
		 * 
		 * Helpers 
		 */
		private void loadJNILibraries() throws Exception {

		// try to resolve without _d - e.i. release versions , if fail try to resolve with _d
				String binDir = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getBinDirectory();
				HtLog.log(Level.INFO, getContext(), "loadJNILibraries", "Will perform JNI libraries search: " + binDir);

				TreeSet<File> binaries = new TreeSet<File>();
				TreeMap<String, String> binaries_names = new TreeMap<String, String>();

				HtFileUtils.readAllfilesRecursively("dll", binDir, binaries);
			
				for (File file_i : binaries) {
						binaries_names.put(file_i.getName(), file_i.getAbsolutePath());
				}

				int foundLibsFalg = LoadedDllsHelper.DETECTED_DUMMY;
				int loadedLibsFalg = LoadedDllsHelper.DETECTED_DUMMY;
				
				List<String> debug_libs = new ArrayList<String>();
				List<String> release_libs = new ArrayList<String>();
				for (String JNI_LIBS : RtConfigurationManager.StartUpConst.JNI_LIBS) {
						String jni_lib_release = JNI_LIBS + ".dll";
						String jni_lib_debug = JNI_LIBS +  RtConfigurationManager.StartUpConst.DEBUG_POSTFIX+".dll";
						if (binaries_names.containsKey(jni_lib_release)) {
								release_libs.add(binaries_names.get(jni_lib_release));
								foundLibsFalg |= LoadedDllsHelper.DETECTED_DEBUG_VER;
						}
						if (binaries_names.containsKey(jni_lib_debug)) {
								debug_libs.add(binaries_names.get(jni_lib_debug));
								foundLibsFalg |= LoadedDllsHelper.DETECTED_RELEASE_VER;
						}
				}

				if (release_libs.size() == RtConfigurationManager.StartUpConst.JNI_LIBS.length) {
						HtLog.log(Level.INFO, getContext(), "loadJNILibraries", "Found RELEASE versions...");
						for (String full_path_i : release_libs) {
								System.load(full_path_i);
								HtLog.log(Level.INFO, getContext(), "loadJNILibraries", "Loaded RELEASE JNI library: " + full_path_i);
						}
						
						loadedLibsFalg = LoadedDllsHelper.DETECTED_RELEASE_VER;

				} else if (debug_libs.size() == RtConfigurationManager.StartUpConst.JNI_LIBS.length) {
						HtLog.log(Level.INFO, getContext(), "loadJNILibraries", "Found DEBUG versions...");
						for (String full_path_i : debug_libs) {
								System.load(full_path_i);
								HtLog.log(Level.INFO, getContext(), "loadJNILibraries", "Loaded DEBUG JNI library: " + full_path_i);
						}
						loadedLibsFalg = LoadedDllsHelper.DETECTED_DEBUG_VER;
				} else {
						throw new HtException(getContext(), "loadJNILibraries", "Cannot resolve all JNI libraries");
				}
				
				LoadedDllsHelper dllHelper = new LoadedDllsHelper(foundLibsFalg, loadedLibsFalg);
				
				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setDllDescriptor(dllHelper);
				
			

		}

		private String getJavaPlatforInfo() {
				return System.getProperty("java.version") + " - " + System.getProperty("java.vendor");
		}
		
		private void dumpCommandLineString(String[] args)
		{
				for(int i = 0; i < args.length; i++) {
						System.out.println("The parameter #"+i+": \"" + args[i] + "\"");
				}
		}
}
