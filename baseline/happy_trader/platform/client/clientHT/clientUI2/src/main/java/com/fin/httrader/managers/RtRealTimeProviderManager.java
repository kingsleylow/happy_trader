/*
 * RtRealTimeProviderManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.HtMain;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.managers.RtManagerBase;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtReflectionUtils;
import com.fin.httrader.utils.hlpstruct.HtLevel2Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class RtRealTimeProviderManager implements RtManagerBase {

	

	// all instances of active providers
	private HashMap<String, HtRealTimeProvider> providers_m = new HashMap();
	static private RtRealTimeProviderManager instance_m = null;

	static public RtRealTimeProviderManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtRealTimeProviderManager();
		}

		return instance_m;
	}

	static public RtRealTimeProviderManager instance() {
		return instance_m;
	}
	//

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	private RtRealTimeProviderManager() {

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	@Override
	public void shutdown() {

		Set<String> providers = this.getRunningProviders();
		for (Iterator<String> it = providers.iterator(); it.hasNext();) {
			// unload providers
			try {
				this.unloadProvider(it.next());
			} catch (Throwable e) {
				HtLog.log(Level.INFO, getContext(), "shutdown", "Exception on finishing provider: " + e.getMessage());
			}
		}

		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// loads provider, creates RtProviderCallback instance and subscribe for events
	public synchronized void loadProvider(String providerId, String providerclass, Map<String, String> initdata, int hourshift) throws Exception {


		// check whether we have standard classes
		HtRealTimeProvider cur_provider = null;

		if (providers_m.containsKey(providerId)) {
			throw new HtException(getContext(), "loadProvider", "Provider is already started: " + providerclass);
		}

		File jarfile = getAvailableProviders(false).get(providerclass);
		if (jarfile==null) {
			cur_provider = HtReflectionUtils.<HtRealTimeProvider>simpleObjectCreate(providerclass);
		}
		else {
			cur_provider = HtReflectionUtils.<HtRealTimeProvider>jarObjectCreate(jarfile.getAbsolutePath(), providerclass);
		}

		if (cur_provider == null) {
			throw new HtException(getContext(), "loadProvider", "Provider cannot be loaded: " + providerId);
		}

		cur_provider.setProviderId(providerId);
		cur_provider.setTimeShift(hourshift);

		cur_provider.initialize(initdata);


		// register this is started
		providers_m.put(providerId, cur_provider);

		HtLog.log(Level.INFO, getContext(), "loadProvider", "Loaded provider: " + providerId, null);
	}

	// dos the opposite thing
	public synchronized void unloadProvider(String providerId) throws Exception {
		if (!providers_m.containsKey(providerId)) {
			throw new HtException(getContext(), "unloadProvider", "Provider is already stopped: " + providerId);
		}

		// unsubscribe
		HtRealTimeProvider cur_provider = (HtRealTimeProvider) providers_m.get(providerId);

		cur_provider.deinitialize();
		providers_m.remove(providerId);



		HtLog.log(Level.INFO, getContext(), "unloadProvider", "Unload provider: " + providerId, null);
	}

	public synchronized boolean isProviderRunning(String providerId) {

		if (providers_m.containsKey(providerId)) {
			return true;
		}

		return false;

	}

	public synchronized int getProviderStatusCode(String providerId) throws Exception {
		if (!providers_m.containsKey(providerId)) {
			throw new HtException(getContext(), "getProviderStatusCode", "Provider is already stopped: " + providerId);
		}

		HtRealTimeProvider cur_provider = (HtRealTimeProvider) providers_m.get(providerId);
		return cur_provider.returnProviderStatus().getStatusCode();
	}

	public synchronized String getProviderStatusString(String providerId) throws Exception {
		if (!providers_m.containsKey(providerId)) {
			throw new HtException(getContext(), "getProviderStatusString", "Provider is already stopped: " + providerId);
		}

		HtRealTimeProvider cur_provider = (HtRealTimeProvider) providers_m.get(providerId);
		return cur_provider.returnProviderStatus().getStatusReason();
	}

	public synchronized BackgroundJobStatus getProviderStatus(String providerId) throws Exception {
		if (!providers_m.containsKey(providerId)) {
			throw new HtException(getContext(), "getProviderStatusString", "Provider is already stopped: " + providerId);
		}

		HtRealTimeProvider cur_provider = (HtRealTimeProvider) providers_m.get(providerId);
		return cur_provider.returnProviderStatus();
	}

	/*
	 * returns the list of available providers registered in the system
	 */
	

	public Map< String, File > getAvailableProviders(boolean check_interface) throws Exception {
		TreeMap< String, File > result = new TreeMap< String, File >();

		Set<String> packaged = HtFileUtils.readPackagedFilesIncudingMainJar("com.fin.httrader.rtproviders", true, HtRealTimeProvider.class);
		
		
		for(Iterator<String> it = packaged.iterator(); it.hasNext(); ) {
			result.put( it.next(), null );
		}

		Map<String, File> external = HtFileUtils.readAllClassesFromJarDirectory(
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_RTProvidersDir(),
						true, check_interface ? HtRealTimeProvider.class: null);

		result.putAll(external);
	
		return result;
	}

	public synchronized Set<String> getRunningProviders() {
		HashSet<String> keys = new HashSet<String>();

		keys.addAll(providers_m.keySet());
		return keys;
	}

	public synchronized int getRunningProvidersCount() {
		return providers_m.size();
	}
	
	public void returnAvailableSymbolList(String providerId, List<String> symbols)
	{
		if (providers_m.containsKey(providerId)) {
			providers_m.get(providerId).returnAvailableSymbolList(symbols);
		}
	}

// helper to create xml event from rtdata
	public static void createXmlEventRtData(HtRtData rtdata, XmlEvent event) {
		event.clear();

		
		event.getAsRtData(true).create(rtdata);
		event.setEventType(XmlEvent.ET_RtProviderTicker);
	}

	// this creates synchronization event
	public static void createXmlEventSynchronization(
			XmlEvent event,
			long eventTime,
			String providerName,
			int syncType,
			String message
	)
	{
		event.clear();

		XmlParameter eparam = event.getAsXmlParameter(true);
		eparam.setCommandName("RT Provider sync");
		eparam.setInt("type", (long)syncType);
		eparam.setString("provider_name", providerName);
		eparam.setString("message", message != null ? message: "");
		
		event.setEventType(XmlEvent.ET_RtProviderSynchronizationEvent);
		event.setEventDate(eventTime);
		
	}

	public static void createXmlEventLevel1(HtLevel1Data level1data, XmlEvent event) {
		event.clear();
		
		event.getAsLevel1Data(true).create(level1data);
		event.setEventType(XmlEvent.ET_Level1Data);
	}

	public static void createXmlEventLevel2(HtLevel2Data level2data, XmlEvent event) {
		event.clear();
		
		
		event.getAsLevel2Data(true).create(level2data);
		event.setEventType(XmlEvent.ET_Level2Data);
	}

	public static void createXmlEventDrawableObject(HtDrawableObject drawableObject, XmlEvent event) {
		event.clear();
		
		
		event.getAsDrawableObject(true).create(drawableObject);
		event.setEventType(XmlEvent.ET_DrawableObject);
	}

	
	
}
