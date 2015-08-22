/*
 * HtConfigurationProxy.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.configprops.HtBaseProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.configprops.helper.LoadedDllsHelper;
import com.fin.httrader.configprops.helper.PluginPageDescriptor;

import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.hlpstruct.ProviderSymbolInfoEntry;
import com.fin.httrader.hlpstruct.RtProviderSubscriptionEntry;
import com.fin.httrader.managers.RtAlertPluginManager;
import com.fin.httrader.managers.RtBootManager;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtCriticalErrorManager;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.managers.RtDatabaseSchemaManager;
import com.fin.httrader.managers.RtDatabaseSelectionManager;
import com.fin.httrader.managers.RtIdleManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import com.fin.httrader.utils.win32.HtSysUtils;
import java.io.File;
import java.io.FileReader;

import java.util.*;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok This manages all server configuration - this is bridge
 * to jsp/servlets
 */
public class HtConfigurationProxy extends HtProxyBase {

	// objects ecuring operations with data profile
	private final Object dataProfileMtx_m = new Object();
	private final Object rtProviderMtx_m = new Object();
	private final Object eventApiPluginMtx_m = new Object();
	private static Set<String> allowedFilesToDownload_m = null;
	private static final Object allowedFilesToDownloadMtx_m = new Object();
	private LoadedDllsHelper dllHelperStruct_m = null;

	public HtConfigurationProxy() {
	}

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public String getProxyName() {
		return "history_config_proxy";
	}

	/**
	 * Common configuration
	 */
	public String remote_getBaseDirectory() {
		return RtBootManager.instance().getBaseDirectory();
	}

	public String remote_getExternalScriptsDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_EXTERNAL_PACKAGES_DIR;
	}

	public String remote_getExternalTemplatesScriptsDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_EXTERNAL_PACKAGES_DIR + File.separator + HtStartupConstants.BASE_EXTERNAL_PACKAGES_TEMPLATES_DIR;
	}

	public String remote_getLogDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_LOG_DIRECTORY_NAME;
	}

	public String remote_getConfigDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_CONFIG_DIRECTORY_NAME;
	}
	
	public String remote_getJavaLibInstallDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_LIB_DIRECTORY_NAME;
	}
	
	// returns null if cannot find
	public String remote_getJavaMainJarPath() {
		String fn = remote_getJavaLibInstallDirectory() +   File.separator + HtStartupConstants.MAIN_JAR_FILE;
		File f=  new File(fn);
		if (f.exists())
				return f.getAbsolutePath();
		else 
				return null;
			
	}

	public String remote_getBinDirectory() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_BIN_DIRECTORY_NAME;
	}

	public String remote_get_Java_RTProvidersDir() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_JAVA_PLUGIN_DIRECTORY_NAME
						+ File.separator + HtStartupConstants.BASE_JAVA_RT_PROVIDERS_DIRECTORY_NAME;
	}

	public String remote_get_Java_HistoryProvidersDir() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_JAVA_PLUGIN_DIRECTORY_NAME
						+ File.separator + HtStartupConstants.BASE_JAVA_HST_PROVIDERS_DIRECTORY_NAME;
	}

	public String remote_get_Java_AlertPluginsProvidersDir() {
		return RtBootManager.instance().getBaseDirectory() + File.separator + HtStartupConstants.BASE_JAVA_PLUGIN_DIRECTORY_NAME
						+ File.separator + HtStartupConstants.BASE_JAVA_ALERT_PLUGINS_DIRECTORY_NAME;
	}

	public String remote_getWebRootDir() {
		return RtBootManager.instance().getBaseDirectory() + File.separator
						+ "platform" + File.separator
						+ "client" + File.separator
						+ "clientHT" + File.separator
						+ "clientUI" + File.separator
						+ "console";
	}

	public String remote_getWebStartDir() {
		return remote_getWebRootDir() + File.separator + HtStartupConstants.BASE_WEB_START_DIR;
	}

	// this will find all executables
	// necessary to fins server executable
	public Set<File> remote_findAllExecutables() throws Exception {
		TreeSet<File> binaries = new TreeSet<File>();
		HtFileUtils.readAllfilesRecursively("exe", remote_getBinDirectory(), binaries);

		return binaries;
	}
	
	public Set<File> remote_findAllSrvExecutables() throws Exception {
		TreeSet<File> binaries = new TreeSet<File>();
		HtFileUtils.readAllfilesRecursively("exe", remote_getBinDirectory(), binaries);

		TreeSet<File> binaries_result = new TreeSet<File>();
		for(Iterator<File> it = binaries.iterator(); it.hasNext(); ) {
			File f_i = it.next();
			if (f_i.getAbsolutePath().toUpperCase().indexOf(RtConfigurationManager.StartUpConst.SERVER_EXECUTABLE_PREFIX) >= 0) {
				binaries_result.add(f_i.getAbsoluteFile());
			}
		}
		
		return binaries_result;
	}
	
	public boolean remote_isDebugVersion(String path) throws Exception
	{
		File f = new File(path);
		if (!f.exists())
			throw new HtException(getContext(), "remote_isDebugVersion", "File does not exist: " + path);
		
		String apath = f.getAbsolutePath();
		
		return apath.toUpperCase().indexOf("_D") >=0;
	}

	// this will find all executables
	// necessary to fins server executable
	public Set<File> remote_findAllDlls() throws Exception {
		TreeSet<File> binaries = new TreeSet<File>();
		HtFileUtils.readAllfilesRecursively("dll", remote_getBinDirectory(), binaries);

		return binaries;
	}
	
	public Set<File> remote_findAllAlgorithmDlls() throws Exception {
		TreeSet<File> binaries = new TreeSet<File>();
		TreeSet<File> binaries_result = new TreeSet<File>();
		
		HtFileUtils.readAllfilesRecursively("dll", remote_getBinDirectory(), binaries);
			for (File f_i : binaries) {
					String path_i = f_i.getAbsolutePath();
					if (path_i.toUpperCase().indexOf(RtConfigurationManager.StartUpConst.AVOID_SEARCH_LIBS_SUBSTR) > 0)
							continue;
					
					if(HtSysUtils.dllIsAlgorithmLib(path_i) == true)
							binaries_result.add(f_i);
			}

		return binaries_result;
	}
	
	public Set<File> remote_findAllBrokerDlls() throws Exception {
		TreeSet<File> binaries = new TreeSet<File>();
		TreeSet<File> binaries_result = new TreeSet<File>();
		
		HtFileUtils.readAllfilesRecursively("dll", remote_getBinDirectory(), binaries);
			for (File f_i : binaries) {
					String path_i = f_i.getAbsolutePath();
					if (path_i.toUpperCase().indexOf(RtConfigurationManager.StartUpConst.AVOID_SEARCH_LIBS_SUBSTR) > 0)
							continue;
					
					if(HtSysUtils.dllIsBrokerLib(path_i) == true)
							binaries_result.add(f_i);
			}

		return binaries_result;
	}

	/**
	 * History providers
	 */
	public Set<String> remote_getRegisteredHistoryProvidersList() throws Exception {
		return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_HISTORY_PROV));
	}

	public HtHistoryProviderProp remote_getHistoryProviderProperty(String provider) throws Exception {
		return (HtHistoryProviderProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_HISTORY_PROV, provider);
	}

	public void remote_setHistoryProviderProperty(String provider, HtHistoryProviderProp prop) throws Exception {
		RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_HISTORY_PROV, provider, prop);
	}

	public void remote_removeHistoryProviderProperty(String provider) throws Exception {
		RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_HISTORY_PROV, provider);
	}

	/**
	 * RT providers
	 */
	//
	public Set<String> remote_getRegisteredRTProvidersList() throws Exception {
		synchronized (rtProviderMtx_m) {

			return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_RT_PROV));
		}
	}

	public HtRTProviderProp remote_getRTProviderProperty(String provider) throws Exception {
		synchronized (rtProviderMtx_m) {

			return (HtRTProviderProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_RT_PROV, provider);
		}
	}

	public void remote_setRTProviderProperty(String provider, HtRTProviderProp prop) throws Exception {
		synchronized (rtProviderMtx_m) {
			if (RtRealTimeProviderManager.instance().isProviderRunning(provider)) {
				throw new HtException(getContext(), "remote_setRTProviderProperty", "RT Provider must be stopped: " + provider);
			}


			RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_RT_PROV, provider, prop);
		}
	}

	public void remote_removeRTProviderProperty(String provider) throws Exception {
		synchronized (rtProviderMtx_m) {
			if (RtRealTimeProviderManager.instance().isProviderRunning(provider)) {
				throw new HtException(getContext(), "remote_removeRTProviderProperty", "RT Provider must be stopped: " + provider);
			}


			RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_RT_PROV, provider);
		}
	}

	/**
	 * Data profiles
	 */
	// helper
	public HashMap<String, Integer> remote_getProfilesPartitionMap() throws Exception {
		synchronized (dataProfileMtx_m) {
			HashMap<String, Integer> result = new HashMap<String, Integer>();
			Set<String> profiles = remote_getRegisteredDataProfilesList();
			for (Iterator<String> it = profiles.iterator(); it.hasNext();) {
				String key_i = it.next();
				HtDataProfileProp prop = remote_getDataProfileProperty(key_i);
				result.put(key_i, prop.getPartitionId());
			}

			return result;
		}
	}

	public int remote_getNextIdForDataProfiles() throws Exception {
		synchronized (dataProfileMtx_m) {
			Set<String> profiles = remote_getRegisteredDataProfilesList();
			TreeSet<Integer> properties = new TreeSet<Integer>();
			for (Iterator<String> it = profiles.iterator(); it.hasNext();) {
				HtDataProfileProp prop = remote_getDataProfileProperty(it.next());
				properties.add(prop.getPartitionId());
			}

			if (properties.size() != profiles.size()) {
				throw new HtException(getContext(), "remote_getNextIdForDataProfiles", "Non-unique property IDs");
			}


			int cnt = 0;
			int max_part_id = 0;
			for (Iterator<Integer> it = properties.iterator(); it.hasNext();) {
				int propertyId = it.next();

				if (cnt++ == 0) {
					max_part_id = propertyId;
					continue;
				}

				if ((propertyId - max_part_id) == 1) {
					// advance
					max_part_id = propertyId;
				} else {
					break;
				}


			}

			max_part_id++;

			if (max_part_id >= HtStartupConstants.MYSQL_MAX_PARTITIONS) {
				throw new HtException(getContext(), "remote_getNextIdForDataProfiles", "Exceeded the number of available partitions");
			}

			return max_part_id;
		}
	}

	public Set<String> remote_getRegisteredDataProfilesList() throws Exception {
		synchronized (dataProfileMtx_m) {
			return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_DATA_PROFILE));
		}
	}

	public HtDataProfileProp remote_getDataProfileProperty(String provider) throws Exception {
		synchronized (dataProfileMtx_m) {
			return (HtDataProfileProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_DATA_PROFILE, provider);
		}
	}

	public void remote_createDataProfileTables(String profile, int partId, LongParam result, StringBuilder textResult) throws Exception {
		try {
			result.setLong(0);
			textResult.setLength(0);
			textResult.append("OK");

			if (!HtCommandProcessor.instance().get_HtDatabaseProxy().startLengthyOperation("changing data profile")) {
				throw new HtException(getContext(), "remote_createDataProfileTables", "Server is busy performing lengthy operation: " + HtCommandProcessor.instance().get_HtDatabaseProxy().getLengthyOperationDescription());
			}


			synchronized (dataProfileMtx_m) {

				if (RtRealTimeProviderManager.instance().getRunningProviders().size() > 0) {
					throw new HtException(getContext(), "remote_createDataProfileTables", "Some of the RT providers are running, cannot update profile");
				}

				if (partId < 0) {
					throw new HtException(getContext(), "remote_createDataProfileTables", "Invalid partition ID");
				}

				List<String> errorMessages = new ArrayList<String>();

				RtDatabaseSchemaManager.instance().createNewProfile(partId, errorMessages);

				if (!errorMessages.isEmpty()) {
					result.setLong(-1);
					textResult.setLength(0);
					textResult.append("Exception on creating new data profile table: ").append(HtUtils.<String>convertCollectionIntoString(errorMessages));

					HtLog.log(Level.WARNING, getContext(), "remote_createDataProfileTables", "Exception on creating new data profile table: " + HtUtils.<String>convertCollectionIntoString(errorMessages));
				}
			}
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_createDataProfileTables", e.getMessage());
		} finally {
			HtCommandProcessor.instance().get_HtDatabaseProxy().finishLengthyOperation();
		}
	}

	public void remote_setDataProfileProperty(String profile, HtDataProfileProp prop, LongParam result, StringBuilder textResult) throws Exception {
		try {

			result.setLong(0);
			textResult.setLength(0);
			textResult.append("OK");

			if (!HtCommandProcessor.instance().get_HtDatabaseProxy().startLengthyOperation("changing data profile")) {
				throw new HtException(getContext(), "remote_setDataProfileProperty", "Server is busy performing lengthy operation: " + HtCommandProcessor.instance().get_HtDatabaseProxy().getLengthyOperationDescription());
			}


			synchronized (dataProfileMtx_m) {

				if (RtRealTimeProviderManager.instance().getRunningProviders().size() > 0) {
					throw new HtException(getContext(), "remote_setDataProfileProperty", "Some of the RT providers are running, cannot update profile");
				}

				// need to validate entry
				Set<String> rtproviders = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_RT_PROV);
				for (Iterator<String> it = rtproviders.iterator(); it.hasNext();) {
					prop.validateEntry(it.next());
				}

				if (prop.getPartitionId() < 0) {
					prop.setPartitionId(remote_getNextIdForDataProfiles());
				}

				// create table only one time
				if (!RtDatabaseSchemaManager.instance().checkProfileTablesExist(prop.getPartitionId())) {
					List<String> errorMessages = new ArrayList<String>();

					// ignore some errors as may be we recreating 2 tables off 3
					RtDatabaseSchemaManager.instance().createNewProfile(prop.getPartitionId(), errorMessages);

					if (!errorMessages.isEmpty()) {
						result.setLong(-1);
						textResult.setLength(0);
						textResult.append("Exception on creating new data profile table: ").append(HtUtils.<String>convertCollectionIntoString(errorMessages));

						HtLog.log(Level.WARNING, getContext(), "remote_setDataProfileProperty", "Exception on creating new data profile table: " + HtUtils.<String>convertCollectionIntoString(errorMessages));
					}
				} 



				RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_DATA_PROFILE, profile, prop);

				// then  we need to update subscription map
				RtDatabaseManager.instance().updateProfileSubscription(
								getDataProfileSubscriptionMap_v2(),
								remote_getProfilesPartitionMap());


			}

		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_setDataProfileProperty", e.getMessage());
		} finally {
			HtCommandProcessor.instance().get_HtDatabaseProxy().finishLengthyOperation();
		}
	}

	public void remote_removeDataProfileProperty(String provider, LongParam result, StringBuilder textResult) throws Exception {

		try {
			result.setLong(0);
			textResult.setLength(0);
			textResult.append("OK");

			if (!HtCommandProcessor.instance().get_HtDatabaseProxy().startLengthyOperation("removing data profile")) {
				throw new HtException(getContext(), "remote_removeDataProfileProperty", "Server is busy performing lengthy operation: " + HtCommandProcessor.instance().get_HtDatabaseProxy().getLengthyOperationDescription());
			}

			synchronized (dataProfileMtx_m) {


				// if RT providers are running
				if (RtRealTimeProviderManager.instance().getRunningProviders().size() > 0) {
					throw new HtException(getContext(), "remote_removeDataProfileProperty", "Some of the RT providers are running, cannot remove profile");
				}

				HtDataProfileProp propToRemove = (HtDataProfileProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_DATA_PROFILE, provider);
				if (propToRemove.getPartitionId() <= 0) {
					throw new HtException(getContext(), "remote_removeDataProfileProperty", "Invalid partition ID for: " + provider);
				}

				// remove entry in either case
				RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_DATA_PROFILE, provider);

				// event if exception with DB will happen
				List<String> errorMessages = new ArrayList<String>();



				RtDatabaseSchemaManager.instance().removeProfile(propToRemove.getPartitionId(), errorMessages);

				if (!errorMessages.isEmpty()) {
					result.setLong(-1);
					textResult.setLength(0);
					textResult.append("When trying to delete data profile from exception happened: \n").append(HtUtils.<String>convertCollectionIntoString(errorMessages));
				}

				// then  we need to update subscription map
				RtDatabaseManager.instance().updateProfileSubscription(
								getDataProfileSubscriptionMap_v2(),
								remote_getProfilesPartitionMap());


			}
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_removeDataProfileProperty", e.getMessage());
		} finally {
			HtCommandProcessor.instance().get_HtDatabaseProxy().finishLengthyOperation();
		}


	}

	public Map<String, List<HtPair<String, String>>> remote_queryProfileProviderSymbolFromDb() throws Exception {
		synchronized (dataProfileMtx_m) {
			Map<String, List<HtPair<String, String>>> profProvSymbMap = new HashMap<String, List<HtPair<String, String>>>();

			Set<String> dprofiles = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_DATA_PROFILE);
			List<HtPair<String, String>> provSymbMap = RtDatabaseSelectionManager.instance().getStoredSymbols();

			for (Iterator<String> it = dprofiles.iterator(); it.hasNext();) {
				String profile_i = it.next();
				profProvSymbMap.put(profile_i, provSymbMap);
			}


			return profProvSymbMap;
		}
	}

	// differs from upper that return only actual symbols
	public Map<String, List<HtPair<String, String>>> remote_queryProfileProviderSymbolFromDb_ForProfile() throws Exception {
		synchronized (dataProfileMtx_m) {
			Map<String, List<HtPair<String, String>>> profProvSymbMap = new HashMap<String, List<HtPair<String, String>>>();

			Set<String> dprofiles = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_DATA_PROFILE);
			for (Iterator<String> it = dprofiles.iterator(); it.hasNext();) {
				String profile_i = it.next();

				List<HtPair<String, String>> provSymbols = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory(profile_i);
				profProvSymbMap.put(profile_i, provSymbols);
			}

			return profProvSymbMap;
		}
	}

	// differs from upper that return only actual symbols
	public Map<String, List<ProviderSymbolInfoEntry>> remote_queryProfileProviderSymbolFromDb_ForProfile_WithDates() throws Exception {
		synchronized (dataProfileMtx_m) {
			Map<String, List<ProviderSymbolInfoEntry>> profProvSymbMap = new HashMap<String, List<ProviderSymbolInfoEntry>>();

			Set<String> dprofiles = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_DATA_PROFILE);
			for (Iterator<String> it = dprofiles.iterator(); it.hasNext();) {
				String profile_i = it.next();

				List<ProviderSymbolInfoEntry> provSymbols = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredSymbols_ForProfile_WithDates(profile_i);
				profProvSymbMap.put(profile_i, provSymbols);
			}

			return profProvSymbMap;
		}
	}

	public HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>> remote_getDataProfileSubscriptionMap_v2() throws Exception {
		synchronized (dataProfileMtx_m) {
			return getDataProfileSubscriptionMap_v2();
		}
	}

	/**
	 * Trading servers
	 */
	public Set<String> remote_getTradingServerList() throws Exception {
		return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_SRV));
	}

	public HtServerProp remote_getTradingServerProperty(String server) throws Exception {
		return (HtServerProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_SRV, server);
	}

	public void remote_setTradingServerProperty(String server, HtServerProp prop) throws Exception {
		RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_SRV, server, prop);
	}

	public void remote_removeTradingServerProperty(String server) throws Exception {
		RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_SRV, server);
	}

	// returns enviroment entries
	// adds additional entries - can be null
	public String[] remote_getEnviromentEntries(Map<String, String> additionalEntries) throws Exception {
		HashMap<String, String> env = new HashMap<String, String>();
		HtSystem.getAllEnvironmentEntries(env);

		if (env.isEmpty()) {
			return null;
		}

		if (additionalEntries != null) {
			// add additional
			for (Iterator<String> it = additionalEntries.keySet().iterator(); it.hasNext();) {
				String key_i = it.next();
				env.put(key_i, additionalEntries.get(key_i));
			}
		}

		String[] result = new String[env.size()];

		int i = 0;
		for (Iterator<String> it = env.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			result[i++] = key + "=" + env.get(key);
		}
		return result;
	}

	/**
	 * Helpers
	 */
	// this returns current subscription map
	private HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>> getDataProfileSubscriptionMap_v2() throws Exception {
		HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>> result = new HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>>();

		// list of all RT providers
		Set<String> rtproviders = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_RT_PROV);
		Set<String> dataprofiles = RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_DATA_PROFILE);

		for (Iterator<String> dp_it = dataprofiles.iterator(); dp_it.hasNext();) {

			String dataprofile_it = dp_it.next();

			HashMap<String, RtProviderSubscriptionEntry> dconfig = new HashMap<String, RtProviderSubscriptionEntry>();

			// get configuration information
			HtDataProfileProp prop = (HtDataProfileProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_DATA_PROFILE, dataprofile_it);
			int part_id = prop.getPartitionId();

			// iterate through RT providers
			for (Iterator<String> prv_it = rtproviders.iterator(); prv_it.hasNext();) {

				String rtprovider_it = prv_it.next();
				RtProviderSubscriptionEntry subscrData = new RtProviderSubscriptionEntry();


				boolean to_insert = false;
				if (prop.isProviderSubscribed_RTHist(rtprovider_it)) {

					int multFact = prop.getProviderMultFactor(rtprovider_it);
					int timeScale = prop.getProviderTimeScale(rtprovider_it);

					subscrData.subscribedRtHist_m = true;
					subscrData.multFact_m = multFact;
					subscrData.timeScale_m = timeScale;

					to_insert = true;
				}

				if (prop.isProviderSubscribed_Level1(rtprovider_it)) {
					subscrData.subscribeLevel1_m = true;
					to_insert = true;
				}

				if (prop.isProviderSubscribed_DrawObj(rtprovider_it)) {
					subscrData.subscribeDrawableObj_m = true;
					to_insert = true;
				}

				if (to_insert) {
					dconfig.put(rtprovider_it, subscrData);
				}

			}

			// add to the map
			result.put(part_id, dconfig);
		}

		return result;
	}

	/**
	 * Startup parameters
	 */
	// this reads only once
	/*
	 * public HtStartupConstants remote_getStartupParameters() throws Exception {
	 * return RtConfigurationManager.instance().getStartupConstants(); }
	 *
	 */
	public void remote_setStartupParameters(HtStartupConstants prop) throws Exception {
		RtConfigurationManager.instance().setEntry(HtBaseProp.ID_GONFIGURE_STARTUP_CONST, prop);
	}

	/**
	 * Algorithm Broker Pairs
	 */
	public Set<String> remote_getRegisteredAlgBrkPairList() throws Exception {
		return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_ALG_BROKER));
	}

	public HtAlgBrkPairProp remote_getAlgBrkPairProperty(String provider) throws Exception {
		return (HtAlgBrkPairProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_ALG_BROKER, provider);
	}

	public void remote_setAlgBrkPairProperty(String provider, HtAlgBrkPairProp prop) throws Exception {
		RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_ALG_BROKER, provider, prop);
	}

	public void remote_removeAlgBrkPairProperty(String provider) throws Exception {
		RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_ALG_BROKER, provider);
	}

	/**
	 * Simulation Profiles
	 */
	public Set<String> remote_getSimulationProfileList() throws Exception {
		return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_CONFIGURE_SIMUL_PROF));
	}

	public HtSimulationProfileProp remote_getSimulationProfileProperty(String name) throws Exception {
		return (HtSimulationProfileProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_CONFIGURE_SIMUL_PROF, name);
	}

	// create based on the list of symbols/providers
	public HtSimulationProfileProp remote_createDummyProfileProperty(List<HtPair<String, String>> provSymbolList) throws Exception {
		HtSimulationProfileProp prop = new HtSimulationProfileProp();

		for (int i = 0; i < provSymbolList.size(); i++) {
			HtPair<String, String> ps_i = provSymbolList.get(i);
			String provider = ps_i.first;
			String symbol = ps_i.second;

			HtSimulationProfileProp.SymbolRelatedProperty p = new HtSimulationProfileProp.SymbolRelatedProperty();

			p.simulationSpread_m = 0;
			p.simulationSlippage_m = 0;
			p.minStopLossDistance_m = 0;
			p.minTakeProfitDistance_m = 0;
			p.minLimitOrderDistance_m = 0;
			p.minStopOrderDistance_m = 0;
			p.comission_m = 0;
			p.comissionPct_m = 0;
			p.signDigits_m = 4;
			p.pointValue_m = 0.01;


			p.executePosExactPrices_m = true;

			Uid uid = prop.resolveProviderSymbol(provider, symbol);
			prop.setSymbolproperty(uid, p);

		}

		return prop;
	}

	public void remote_setSimulationProfileProperty(String name, HtSimulationProfileProp prop) throws Exception {
		RtConfigurationManager.instance().setEntry(HtBaseProp.ID_CONFIGURE_SIMUL_PROF, name, prop);
	}

	public void remote_removeSimulationProfileProperty(String name) throws Exception {
		RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_CONFIGURE_SIMUL_PROF, name);
	}

	// misc
	public List<CriticalErrorEntry> remote_getCriticalErrors() {
		return RtCriticalErrorManager.instance().getCriticalErrors();
	}

	public boolean remote_doWeHaveCriticalErrors() {
		return RtCriticalErrorManager.instance().doWeHaveCriticalErrors();
	}

	public void remote_clearCriticalErrors() {
		RtCriticalErrorManager.instance().clearAllCriticalErrors();
	}

	/*
	public Set<Integer> remote_getAllTypesOfPriceToTranslateToReportSymbol() {
		Set<Integer> allTypes = new TreeSet<Integer>();
		Integer[] s = Position.getAllPositionTranslationToReportSymbol();

		for (int i = 0; i < s.length; i++) {
			allTypes.add(s[i]);
		}

		return allTypes;
	}
	* 
	*/ 

	/*
	 * public String remote_getPositionTranslationToReportSymbolName(int pt) {
	 * return Position.positionTranslationToReportSymbolName(pt); }
	 */
	/**
	 * Event plugins
	 */
	public Set<String> remote_getRegisteredEventPluginList() throws Exception {
		synchronized (eventApiPluginMtx_m) {

			return new TreeSet<String>(RtConfigurationManager.instance().getAllEntries(HtBaseProp.ID_GONFIGURE_EVENT_PLUGIN));
		}
	}

	public HtEventPluginProp remote_getEventPluginProperty(String pluginId) throws Exception {
		synchronized (eventApiPluginMtx_m) {

			return (HtEventPluginProp) RtConfigurationManager.instance().getEntry(HtBaseProp.ID_GONFIGURE_EVENT_PLUGIN, pluginId);
		}
	}

	public void remote_setEventPluginProperty(String pluginId, HtEventPluginProp prop) throws Exception {
		synchronized (eventApiPluginMtx_m) {
			if (RtAlertPluginManager.instance().isPluginRunning(pluginId)) {
				throw new HtException(getContext(), "remote_setEventPluginProperty", "Event plugin must be stopped: " + pluginId);
			}


			RtConfigurationManager.instance().setEntry(HtBaseProp.ID_GONFIGURE_EVENT_PLUGIN, pluginId, prop);
		}
	}

	public void remote_removeEventPluginProperty(String pluginId) throws Exception {
		synchronized (eventApiPluginMtx_m) {
			if (RtAlertPluginManager.instance().isPluginRunning(pluginId)) {
				throw new HtException(getContext(), "remote_removeEventPluginProperty", "Event plugin must be stopped: " + pluginId);
			}


			RtConfigurationManager.instance().removeEntry(HtBaseProp.ID_GONFIGURE_EVENT_PLUGIN, pluginId);
		}
	}
	
	
	// plugin name -> pages
	public Map<String, List< PluginPageDescriptor > > remote_getPluginPages() throws Exception {
	  TreeMap<String, List< PluginPageDescriptor> > web_plugins = new TreeMap<String,List< PluginPageDescriptor > >();

		String baseDir = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebRootDir()
						+ File.separatorChar + ".." + File.separatorChar + ".." + File.separatorChar;

		File f = new File(baseDir);
		if (f.isDirectory()) {
			File[] dirs = f.listFiles();
			for (int i = 0; i < dirs.length; i++) {
				File d_i = dirs[i];


				if (d_i.isDirectory()) {
					if (d_i.getName().equalsIgnoreCase("clientUI")) {
						continue;
					}

					//
					// if jnlp found
					String descriptor = d_i.getAbsolutePath() + File.separatorChar + "descriptor" + File.separatorChar + RtConfigurationManager.StartUpConst.WEB_PLUGIN_DESCRIPTOR_NAME;
					File descriptor_f = new File(descriptor);
					if (descriptor_f.exists()) {
					  // parse that file
					  CSVReader reader = null;

					  try {
						  reader = new CSVReader(new FileReader(descriptor_f), ',', '"');
						  String[] nextLine;

						  while ((nextLine = reader.readNext()) != null) {
							if (nextLine.length != 3)
							  continue;

							String id = nextLine[0].trim();
							String url = nextLine[1].trim();
							String description = nextLine[2].trim();
							
							if (id != null && id.length() > 0) {
							  if (url != null && url.length() > 0) {
								List< PluginPageDescriptor > lst = web_plugins.get(d_i.getName());
								if (lst == null) {
								  lst = new ArrayList< PluginPageDescriptor >();
								  web_plugins.put(d_i.getName(), lst );
								}
								
								lst.add( new PluginPageDescriptor(HtUtils.parseInt(id), url, description) );
								
							  }
							}
							 
						  }
					  }
					  catch(Throwable e)  {
								throw new HtException(getContext(), "remote_getPluginPages", "Exception on processing file: " + descriptor + " - " + e.getMessage());
					  }
					  finally
					  {
								if (reader != null) {
										reader.close();
								}
					  }
					  
					}
				}
			}
		}

		return web_plugins;
	}
	
	
	public Set<String> remote_getWebApplications() throws Exception {
		TreeSet<String> webapps = new TreeSet<String>();

		String baseDir = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebRootDir()
						+ File.separatorChar + ".." + File.separatorChar + ".." + File.separatorChar;

		File f = new File(baseDir);
		if (f.isDirectory()) {
			File[] dirs = f.listFiles();
			for (int i = 0; i < dirs.length; i++) {
				File d_i = dirs[i];


				if (d_i.isDirectory()) {
					if (d_i.getName().equalsIgnoreCase("clientUI")) {
						continue;
					}

					//
					// if jnlp found
					String jnlpFile = d_i.getAbsolutePath() + File.separatorChar + "dist" + File.separatorChar + RtConfigurationManager.StartUpConst.WEB_START_DESCRIPTOR_NAME;
					File jnlpFile_f = new File(jnlpFile);
					if (jnlpFile_f.exists()) {
						webapps.add(d_i.getName());
					}
				}
			}
		}

		return webapps;
	}

	public Set<String> remote_AllowedJars() throws Exception {
		synchronized (allowedFilesToDownloadMtx_m) {
			if (allowedFilesToDownload_m == null) {
				allowedFilesToDownload_m = new HashSet<String>();

				String baseDir = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebRootDir()
								+ File.separatorChar + ".." + File.separatorChar + ".." + File.separatorChar;

				File f = new File(baseDir);
				if (f.isDirectory()) {
					File[] dirs = f.listFiles();
					for (int i = 0; i < dirs.length; i++) {
						File d_i = dirs[i];


						if (d_i.isDirectory()) {
							if (d_i.getName().equalsIgnoreCase("clientUI")) {
								continue;
							}


							HashSet<File> jars = new HashSet<File>();
							HtFileUtils.readAllfilesRecursively("jar", d_i.getAbsolutePath(), jars);


							for (Iterator<File> it = jars.iterator(); it.hasNext();) {
								allowedFilesToDownload_m.add(it.next().getCanonicalFile().getAbsolutePath());
							}

							String jnlpFile = d_i.getAbsolutePath() + File.separatorChar + "dist" + File.separatorChar + RtConfigurationManager.StartUpConst.WEB_START_DESCRIPTOR_NAME;
							File jnlpFile_f = new File(jnlpFile);
							if (jnlpFile_f.exists()) {
								allowedFilesToDownload_m.add(jnlpFile_f.getCanonicalFile().getAbsolutePath());
							}

						}
					}

				}

			}
			return allowedFilesToDownload_m;
		}
	}
	
	public LoadedDllsHelper remote_getDllDescriptor()
	{
			return dllHelperStruct_m;
	}
	
	public void  remote_setDllDescriptor( LoadedDllsHelper dllDescr)
	{
			dllHelperStruct_m  = dllDescr;
	}
	
}
