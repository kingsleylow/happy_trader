/*
 * HtExtConfiguration.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

import com.fin.httrader.externalapi.utils.HtExtPropertyHolder;
import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.configprops.HtServerProp;
import com.fin.httrader.externalapi.utils.HtExtBoolBuffer;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtConfiguration {

	public HtExtConfiguration() {
	}

	// --------------------------------
	// common
	// return all executables

	public HtExtCommandResult getStoredProviderSymbolsForProfile(List<HtPair<String, String>> provSymbolList, String profilename)
	{
		HtExtCommandResult result = new HtExtCommandResult("getStoredProviderSymbolsForProfile");
		try {
			provSymbolList.clear();
			provSymbolList.addAll( HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory(profilename) );

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -----------------------------

	public HtExtCommandResult getAllExecutables(Set<File> executables) {
		HtExtCommandResult result = new HtExtCommandResult("getAllExecutables");
		try {
			executables.clear();
			executables.addAll( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllExecutables() );
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	public HtExtCommandResult getAllSrvExecutables(Set<File> executables) {
		HtExtCommandResult result = new HtExtCommandResult("getAllSrvExecutables");
		try {
			executables.clear();
			executables.addAll( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllSrvExecutables() );
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	public HtExtCommandResult isDebugVesrion(String path, HtExtBoolBuffer res) {
		HtExtCommandResult result = new HtExtCommandResult("isDebugVesrion");
		try {
			
			 boolean r = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_isDebugVersion(path);
			 res.setBoolean(r);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getAllDlls(Set<File> dlls) {
		HtExtCommandResult result = new HtExtCommandResult("getAllDlls");
		try {
			dlls.clear();
			dlls.addAll( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllDlls() );
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	public HtExtCommandResult getAllAlgrorithmsDlls(Set<File> dlls) {
		HtExtCommandResult result = new HtExtCommandResult("getAllAlgrorithmsDlls");
		try {
			dlls.clear();
			dlls.addAll( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllAlgorithmDlls() );
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	public HtExtCommandResult getAllBrokersDlls(Set<File> dlls) {
		HtExtCommandResult result = new HtExtCommandResult("getAllBrokersDlls");
		try {
			dlls.clear();
			dlls.addAll( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllBrokerDlls() );
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	// --------------------------------
	// alg brk pairs
	/**
	Algorithm Broker Pairs
	 */
	public HtExtCommandResult getAlgBrkPairList(Set<String> algBrkPairList) {
		HtExtCommandResult result = new HtExtCommandResult("getAlgBrkPairList");

		try {
			algBrkPairList.clear();
			algBrkPairList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}


	public HtExtCommandResult getAlgBrkPairProperty(String algBrkPair, HtExtPropertyHolder propHolder) {
		HtExtCommandResult result = new HtExtCommandResult("getAlgBrkPairProperty");

		try {
			propHolder.setProperty( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getAlgBrkPairProperty(algBrkPair) );


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setAlgBrkPairProperty(String algBrkPair, HtAlgBrkPairProp prop) {
		HtExtCommandResult result = new HtExtCommandResult("setAlgBrkPairProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setAlgBrkPairProperty(algBrkPair, prop);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult removeAlgBrkPairProperty(String algBrkPair) {
		HtExtCommandResult result = new HtExtCommandResult("removeAlgBrkPairProperty");
		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeAlgBrkPairProperty(algBrkPair);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// --------------------------------
	// history providers
	public HtExtCommandResult getRegisteredHistoryProviderList(Set<String> providerList) {
		HtExtCommandResult result = new HtExtCommandResult("getRegisteredHistoryProviderList");

		try {
			providerList.clear();
			providerList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredHistoryProvidersList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getHistoryProviderProperty(String provider,HtExtPropertyHolder propHolder) {
		HtExtCommandResult result = new HtExtCommandResult("getHistoryProviderProperty");

		try {

			propHolder.setProperty( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getHistoryProviderProperty(provider) );


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setHistoryProviderProperty(String provider, HtHistoryProviderProp prop) {
		HtExtCommandResult result = new HtExtCommandResult("setHistoryProviderProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setHistoryProviderProperty(provider, prop);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult removeHistoryProviderProperty(String provider) {
		HtExtCommandResult result = new HtExtCommandResult("removeHistoryProviderProperty");
		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeHistoryProviderProperty(provider);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------
	// RT providers
	public HtExtCommandResult getRegisteredRtProviderList(Set<String> providerList) {
		HtExtCommandResult result = new HtExtCommandResult("getRegisteredRtProviderList");

		try {
			providerList.clear();
			providerList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getRtProviderProperty(String provider, HtExtPropertyHolder propHolder) {
		HtExtCommandResult result = new HtExtCommandResult("getRtProviderProperty");

		try {

			propHolder.setProperty(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(provider) );


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setRtProviderProperty(String provider, HtRTProviderProp prop) {
		HtExtCommandResult result = new HtExtCommandResult("setRtProviderProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setRTProviderProperty(provider, prop);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult removeRtProviderProperty(String provider) {
		HtExtCommandResult result = new HtExtCommandResult("removeRtProviderProperty");
		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeRTProviderProperty(provider);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------
	// data profile
	public HtExtCommandResult getRegisteredDataProfileList(Set<String> profileList) {
		HtExtCommandResult result = new HtExtCommandResult("getRegisteredDataProfileList");

		try {
			profileList.clear();
			profileList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getDataProfileProperty(String dprofile, HtExtPropertyHolder dprofProp) {
		HtExtCommandResult result = new HtExtCommandResult("getDataProfileProperty");

		try {

			dprofProp.setProperty( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileProperty(dprofile) );
			

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setDataProfileProperty(String dprofile, HtDataProfileProp prop,  LongParam resultVar, StringBuilder textResult) {
		HtExtCommandResult result = new HtExtCommandResult("setDataProfileProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setDataProfileProperty(dprofile, prop, resultVar, textResult);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	
	public HtExtCommandResult removeDataProfileProperty(String dprofile,  LongParam resultVar, StringBuilder textResult) {
		HtExtCommandResult result = new HtExtCommandResult("removeDataProfileProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeDataProfileProperty(dprofile, resultVar, textResult);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	


	// --------------------------------------
	// Server prop
	public HtExtCommandResult getTradingServerList(Set<String> serverList) {
		HtExtCommandResult result = new HtExtCommandResult("getTradingServerList");

		try {
			serverList.clear();
			serverList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getTradingServerProperty(String serverId, HtExtPropertyHolder propHolder) {
		HtExtCommandResult result = new HtExtCommandResult("getTradingServerProperty");

		try {

			propHolder.setProperty( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getTradingServerProperty(serverId) );


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setTradingServerProperty(String serverId, HtServerProp prop) {
		HtExtCommandResult result = new HtExtCommandResult("setTradingServerProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setTradingServerProperty(serverId, prop);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult removeTradingServerProperty(String serverId) {
		HtExtCommandResult result = new HtExtCommandResult("removeTradingServerProperty");
		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeTradingServerProperty(serverId);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ---------------------------------------
	public HtExtCommandResult getTradingServerEnviromentEntries(Vector<String> entries) {
		HtExtCommandResult result = new HtExtCommandResult("getTradingServerStartupEntries");
		try {

			entries.clear();
			String[] execEnv = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEnviromentEntries(null);

			for (int i = 0; i < execEnv.length; i++) {
				entries.add(execEnv[i]);
			}

		} catch (Throwable e) {
			result.initialize(e);
		}


		return result;
	}

	// -------------------------------------
	// Event
	public HtExtCommandResult getRegisteredEventPluginList(Set<String> pluginList) {
		HtExtCommandResult result = new HtExtCommandResult("getRegisteredEventPluginList");

		try {
			pluginList.clear();
			pluginList.addAll(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredEventPluginList());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult getEventPluginProperty(String pluginId, HtExtPropertyHolder propHolder) {
		HtExtCommandResult result = new HtExtCommandResult("getEventPluginProperty");

		try {

			propHolder.setProperty( HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getEventPluginProperty(pluginId) );


		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult setEventPluginProperty(String pluginId, HtEventPluginProp prop) {
		HtExtCommandResult result = new HtExtCommandResult("setEventPluginProperty");

		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setEventPluginProperty(pluginId, prop);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult removeEventPluginProperty(String pluginId) {
		HtExtCommandResult result = new HtExtCommandResult("removeEventPluginProperty");
		try {
			HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeEventPluginProperty(pluginId);

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// ------------------------------------------
	// helper commands
	public void addToProfileProviderSymbolList(
					Map<String, Vector<HtPair<String, String>>> profProvSymbMap,
					String profileName,
					String providerName,
					String[] symbolList) {
		Vector<HtPair<String, String>> profSymbList = null;
		if (profProvSymbMap.containsKey(profileName)) {
			profSymbList = profProvSymbMap.get(profileName);
		} else {
			profSymbList = new Vector<HtPair<String, String>>();
			profProvSymbMap.put(profileName, profSymbList);
		}


		for (int i = 0; i < symbolList.length; i++) {
			profSymbList.add(new HtPair<String, String>(providerName, symbolList[i]));
		}


	}

	public void addToProfileProviderSymbolList(
					Map<String, Vector<HtPair<String, String>>> profProvSymbMap,
					String profileName,
					String providerName,
					String symbol) {
		Vector<HtPair<String, String>> profSymbList = null;
		if (profProvSymbMap.containsKey(profileName)) {
			profSymbList = profProvSymbMap.get(profileName);
		} else {
			profSymbList = new Vector<HtPair<String, String>>();
			profProvSymbMap.put(profileName, profSymbList);
		}

		profSymbList.add(new HtPair<String, String>(providerName, symbol));

	}
}
