/*
 * RtHistoryProviderManager.java
 *
 * Created on February 10, 2007, 9:29 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.interfaces.*;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.*;
import java.io.File;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.logging.Level;

/**
 *
 * Class managing all the history providers
 */
public class RtHistoryProviderManager implements RtManagerBase {

	static private RtHistoryProviderManager instance_m = null;

	static public RtHistoryProviderManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtHistoryProviderManager();
		}

		return instance_m;
	}

	static public RtHistoryProviderManager instance() {

		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of RtHistoryProviderManager */
	private RtHistoryProviderManager() {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}


	public Map< String, File > getAvailableProviders(boolean check_interface) throws Exception {
		TreeMap< String, File > result = new TreeMap< String, File >();

		Set<String> packaged = HtFileUtils.readPackagedFilesIncudingMainJar("com.fin.httrader.hstproviders", true, HtHistoryProvider.class);

		for(Iterator<String> it = packaged.iterator(); it.hasNext(); ) {
			result.put( it.next(), null );
		}

		Map<String, File> external = HtFileUtils.readAllClassesFromJarDirectory(
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_HistoryProvidersDir(),
						true, check_interface ? HtHistoryProvider.class: null);

		result.putAll(external);

		return result;
	}




	// this function returns the instance of history provider class.
	// the life time of this object is only during the requested operation
	public HtHistoryProvider resolveProvider(
					String providerId,
					String providertype,
					int exporthourshift,
					int importhourshift) throws Exception {
		HtHistoryProvider cur_provider = null;


		File jarfile = getAvailableProviders(false).get(providertype);
		if (jarfile==null) {
			cur_provider = HtReflectionUtils.<HtHistoryProvider>simpleObjectCreate(providertype);
		}
		else {
			cur_provider = HtReflectionUtils.<HtHistoryProvider>jarObjectCreate(jarfile.getAbsolutePath(), providertype);
		}

	
		cur_provider.setProviderId(providerId);
		cur_provider.setExportTimeShift(exporthourshift);
		cur_provider.setImportTimeShift(importhourshift);

		if (cur_provider == null) {
			throw new HtException(getContext(), "resolveProvider",
							"Provider cannot be loaded: " + providerId);
		}

		return cur_provider;
	}

	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// creates this via class name
	// must return always new version of class as we could substitute this!
	
}



