/*
 * RtAlertPluginManager.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.configprops.HtEventPluginProp;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtReflectionUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import java.util.TreeMap;
import java.util.logging.Level;
import jsync.Event;
//import jsync.Queue;

/**
 *
 * @author Victor_Zoubok
 * this is the class to manage Java plugins reacting for alerts
 */
public class RtAlertPluginManager implements RtManagerBase {

	// constants for sync events
	public static final int SYNC_EVENT_PLUGIN_START = 1;
	public static final int SYNC_EVENT_PLUGIN_FINISH = 2;
	
	public static final int PLUGIN_FOUND_JAR = 0x0001;
	public static final int PLUGIN_FOUND_NATIVE = 0x0002;
	public static final int PLUGIN_NOT_FOUND = 0x0000;
	
	

	// nested thread class
	private static class PluginWorkingThread extends Thread implements HtEventListener {

		private Event threadStarted_m = new Event();
		private Event toShutdown_m = new Event();
		private SingleReaderQueue queue_m = new SingleReaderQueue("Alert Plugin Manager Thread Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
		private HtEventPlugin plugin_m = null;
		
		// multithread
		private BackgroundJobStatus status_m = new BackgroundJobStatus();

		// -----------------
		protected String getContext() {
			return PluginWorkingThread.class.getSimpleName() + " " + this.hashCode();
		}

		public PluginWorkingThread(HtEventPlugin plugin, Map<String, String> initdata) throws Exception {

			plugin_m = plugin;

			status_m.initializeOk();

			plugin_m.initialize(initdata);

			start();

			if (!threadStarted_m.waitEvent(1000)) {
				throw new HtException(getContext(), getContext(), "Working thread cannot be started");
			}

		}

		public BackgroundJobStatus getJobStatus()
		{
			return status_m;
		}

		public HtEventPlugin getPlugin() {
			return plugin_m;
		}

		// no exception here
		public void finish() {
			toShutdown_m.signal();
			XmlEvent shutevent = new XmlEvent();
			shutevent.setEventType(XmlEvent.ET_SynchronizationEvent);

			queue_m.shutdown();
			/*
			try {
				queue_m.put(shutevent);
			} catch (Throwable e) {
				// ignore
			}
			*/

			try {
				// wait 5000 msec and proceed further
				join(5000);
			} catch (InterruptedException e) {
				// ignore
			}

			deinitializePlugin();

		}

		@Override
		public void run() {

			HtLog.log(Level.INFO, getContext(), "run", "Serving thread started");

			// subscribe
			try {

				Integer[] sevents = plugin_m.returnSubscribtionEventTypes();
				if (sevents == null) {
					throw new HtException(getContext(), "run", "Empty event list on subscription: ");
				}

				Set<Integer> available = XmlEvent.getAllAvalableEventsForServer();
				for (int i = 0; i < sevents.length; i++) {

					if (available.contains(sevents[i])) {
						RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(sevents[i], 25, this);
					} else {
						throw new HtException(getContext(), "run", "Unknown event type on subscription: " + sevents[i]);
					}

				}

			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "run", "Exception when subscribing to events: \"" + e.getMessage() + "\"");

				deinitializePlugin();

				RtCriticalErrorManager.instance().signalCriticalError("Alert Plugin Manager", "", e, CriticalErrorEntry.URGENCY_FLAG_WARN);
				return;
			}

			XmlEvent syncEvent = new XmlEvent();
			createAlertPluginSynchronizationEvent(syncEvent, RtAlertPluginManager.SYNC_EVENT_PLUGIN_START);
			RtGlobalEventManager.instance().pushCommonEvent(syncEvent);

			threadStarted_m.signal();

			LinkedList list = new LinkedList();
			XmlEvent event = null;

			// main loop
			while (true) {

				try {

					queue_m.get(list);
					for (Iterator it = list.iterator(); it.hasNext();) {

						event = (XmlEvent) it.next();

						if (toShutdown_m.waitEvent(0)) {
							break;
						}

						processEvent(event);

					}

				} catch (Throwable e) {
					HtLog.log(Level.WARNING, getContext(), "run", "Internal exception in serving thread: \"" + e.getMessage() + "\"");
				}

				// wait for 1 sec
				if (toShutdown_m.waitEvent(0)) {
					break;
				}

				event = null;

			} // end loop


			// unsubscribe
			try {
				RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(25, this);

			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "run", "Exception when unsubscribing from ET_AlgorithmAlert event : \"" + e.getMessage() + "\"");

			}

			queue_m.releaseMonitoring();

			HtLog.log(Level.INFO, getContext(), "run", "Serving thread finishing...");
		}

		@Override
		public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
			queue_m.put(event);

		}

		@Override
		public String getListenerName() {
			return " listener: [ " + getContext() + " ] ";
		}

		// ----------
		private void deinitializePlugin() {
			try {
				plugin_m.deinitialize();
			} catch (Throwable e) {
				HtLog.log(Level.WARNING, getContext(), "finish", "Exception on deinitialization plugin: " + plugin_m.getEventPluginId()
								+ " - " + e.getMessage());
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Exception on deinitialization plugin: " + plugin_m.getEventPluginId()
								+ " - " + e.getMessage());

				//status
				status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, "Exception on deinitialization plugin: " + plugin_m.getEventPluginId()
								+ " - " + e.getMessage());
			}
		}

		private void processEvent(XmlEvent alertEvent) throws Exception {


			try {
				plugin_m.execute(alertEvent);
			} catch (Throwable e) {
				HtLog.log(Level.INFO, getContext(), "processAlerts", "Exception on running alert plugin: " + plugin_m.getEventPluginId() + " - " + e.getMessage());
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Exception on running alert plugin: " + plugin_m.getEventPluginId() + " - " + e.getMessage());

				status_m.setUp(BackgroundJobStatus.STATUS_ERRORED,"Exception on running alert plugin: " + plugin_m.getEventPluginId() + " - " + e.getMessage());
			}

		}
	}; // end of class
	// --------------------------------------------------
	//
	//
	// map to store available plugins
	private final HashMap<String, PluginWorkingThread> providers_m = new HashMap<String, PluginWorkingThread>();
	static private RtAlertPluginManager instance_m = null;

	static public RtAlertPluginManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtAlertPluginManager();
		}

		return instance_m;
	}

	static public RtAlertPluginManager instance() {
		return instance_m;
	}

	// --------------------------------------------
	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	// --------------------------------------------
	/** Creates a new instance of RtAlertPluginManager */
	public RtAlertPluginManager() throws Exception {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	// ------------------------------------------
	@Override
	public void shutdown() {

		// deinitialize plugins
		Set<String> plugins = getRunningPluginNames();
		for (Iterator<String> it = plugins.iterator(); it.hasNext();) {
			// unload providers
			try {
				unloadPlugin(it.next());
			} catch (Throwable e) {
				HtLog.log(Level.INFO, getContext(), "shutdown", "Exception on finishing plugins: " + e.getMessage());
			}
		}


		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	public void checkWherePluginFound(List< HtPair<String, File> > providers, String pluginType, StringBuilder jarPath, IntParam foundFlag) throws Exception
	{
			  int r = PLUGIN_NOT_FOUND;
				
				
				
				for(int i = 0; i < providers.size(); i++) {
						HtPair<String, File> p = providers.get(i);
						
						if (p.first.equalsIgnoreCase(pluginType)) {
								if (p.second != null) {
										r |= PLUGIN_FOUND_JAR;
										jarPath.setLength(0);
										jarPath.append(p.second.getAbsolutePath());
								}
								else {
										r |= PLUGIN_FOUND_NATIVE;
								}
						}
						
						// found all
						if ((r & PLUGIN_FOUND_JAR) > 0 && (r & PLUGIN_FOUND_NATIVE) > 0)
								break;
						
				}
				
			foundFlag.setInt(r);
			
	}
	
	// -----------------------------------------
	// loads provider, creates RtProviderCallback instance and subscribe for events
	public void loadPlugin(
		String pluginId, 
		String plugintype, 
		int launch_flag, // enforce load from jar
		Map<String, String> initdata
	) throws Exception {

		synchronized (providers_m) {
			try {
				// check whether we have standard classes
				HtEventPlugin cur_provider = null;

				if (providers_m.containsKey(pluginId)) {
					throw new HtException(getContext(), "loadPlugin", "Plugin is already started: " + plugintype);
				}
				
			
				// here 
				List< HtPair<String, File> > providers = getAvailableProviders(false);
				
				StringBuilder jarPath = new StringBuilder();
				IntParam foundFlag = new IntParam();
				
				checkWherePluginFound(providers, plugintype, jarPath, foundFlag);
				
				boolean found_native = ((foundFlag.getInt() & PLUGIN_FOUND_NATIVE) > 0);
				boolean found_jar = ((foundFlag.getInt() & PLUGIN_FOUND_JAR) > 0);
				
				File jarfile = null;
				if (found_jar)
						jarfile = new File(jarPath.toString());
				
				
				if (!found_jar && !found_native)
						throw new HtException(getContext(), "loadPlugin", "Plugin cannot be loaded: " + pluginId + " - not found as native or as jar");
				
				if (found_jar && launch_flag == HtEventPluginProp.LAUNCH_EXT_JAR) {
						cur_provider = HtReflectionUtils.<HtEventPlugin>jarObjectCreate(jarfile.getAbsolutePath(), plugintype);
						HtLog.log(Level.INFO, getContext(), "loadPlugin", "Loaded plugin: " + pluginId + " as jar: " + jarfile.getAbsolutePath());
				}
				else if (found_native && launch_flag == HtEventPluginProp.LAUNCH_INTERNAL) {
						cur_provider = HtReflectionUtils.<HtEventPlugin>simpleObjectCreate(plugintype);
						HtLog.log(Level.INFO, getContext(), "loadPlugin", "Loaded plugin: " + pluginId + " as native");
				}
				else if (launch_flag == HtEventPluginProp.LAUNCH_EXT_PROVIDER)
						throw new HtException(getContext(), "loadPlugin", "LAUNCH_EXT_PROVIDER method is not supported");
			
				
				if (cur_provider == null) {
						throw new HtException(getContext(), "loadPlugin", "Plugin: " + pluginId + " cannot be launched");
				}
				

				cur_provider.setEventPluginId(pluginId);


				if (cur_provider == null) {
					throw new HtException(getContext(), "loadPlugin", "Plugin cannot be loaded: " + pluginId);
				}



				// register this is started
				PluginWorkingThread wthread = new PluginWorkingThread(cur_provider, initdata);
				providers_m.put(pluginId, wthread);

				HtLog.log(Level.INFO, getContext(), "loadPlugin", "Loaded plugin: " + pluginId);
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_INFO, "Loaded plugin OK: " + pluginId);


			} catch (Throwable e) {
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Exception on load plugin: " + pluginId + " - " + e.getMessage());
				throw new HtException(getContext(), "unloadPlugin", "Exception on load plugin: " + pluginId + " - " + e.getMessage());

			}
		}
	}

	// dos the opposite thing
	// -----------------------------------------
	public void unloadPlugin(String pluginId) throws Exception {
		synchronized (providers_m) {
			try {
				if (!providers_m.containsKey(pluginId)) {
					throw new HtException(getContext(), "unloadPlugin", "Plugin is already stopped: " + pluginId);
				}

				// unsubscribe
				PluginWorkingThread wthread = (PluginWorkingThread) providers_m.get(pluginId);
				wthread.finish();
				providers_m.remove(pluginId);

				HtLog.log(Level.INFO, getContext(), "unloadPlugin", "Unload plugin: " + pluginId);
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_INFO, "Unload plugin: " + pluginId);
			} catch (Throwable e) {
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Exception on unload plugin: " + pluginId + " - " + e.getMessage());
				throw new HtException(getContext(), "unloadPlugin", "Exception on unload plugin: " + pluginId + " - " + e.getMessage());

			}

		}
	}

	// -----------------------------------------
	public boolean isPluginRunning(String pluginId) {
		synchronized (providers_m) {
			if (providers_m.containsKey(pluginId)) {
				return true;
			}

			return false;
		}

	}

	public int getPluginStatusCode(String providerId) throws Exception {
		synchronized (providers_m) {
			if (!providers_m.containsKey(providerId)) {
				throw new HtException(getContext(), "getPluginStatusCode", "Plugin is already stopped: " + providerId);
			}

			PluginWorkingThread cur_provider = (PluginWorkingThread) providers_m.get(providerId);
			return cur_provider.getJobStatus().getStatusCode();
		}
	}

	public String getPluginStatusString(String providerId) throws Exception {
		synchronized (providers_m) {
			if (!providers_m.containsKey(providerId)) {
				throw new HtException(getContext(), "getPluginStatusString", "Plugin is already stopped: " + providerId);
			}

			PluginWorkingThread cur_provider = (PluginWorkingThread) providers_m.get(providerId);
			return cur_provider.getJobStatus().getStatusReason();
		}
	}

	public BackgroundJobStatus getPluginStatus(String providerId) throws Exception {
		synchronized (providers_m) {
			if (!providers_m.containsKey(providerId)) {
				throw new HtException(getContext(), "getPluginStatus", "Plugin is already stopped: " + providerId);
			}

			PluginWorkingThread cur_provider = (PluginWorkingThread) providers_m.get(providerId);
			return cur_provider.getJobStatus();
		}
	}

	// -----------------------------------------
	public List< HtPair<String, File> > getAvailableProviders(boolean check_interface) throws Exception {
		 List< HtPair<String, File> > result =  new ArrayList< HtPair<String, File> >();

		Set<String> packaged = HtFileUtils.readPackagedFilesIncudingMainJar("com.fin.httrader.eventplugins", true, HtEventPlugin.class);

		for (Iterator<String> it = packaged.iterator(); it.hasNext();) {
			HtPair<String, File> p = new HtPair<String, File>(it.next(), null);
			result.add(p);
		}

		Map<String, File> external = HtFileUtils.readAllClassesFromJarDirectory(
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_get_Java_AlertPluginsProvidersDir(),
						true, check_interface ? HtEventPlugin.class : null);
		
		for (String key : external.keySet()) {
				File f = external.get(key);
				HtPair<String, File> p = new HtPair<String, File>(key, f);
				result.add(p);
		}
		

		return result;
	}

	// -----------------------------------------
	public Set<String> getRunningPluginNames() {
		HashSet<String> keys = new HashSet<String>();

		synchronized (providers_m) {
			keys.addAll(providers_m.keySet());
			return keys;
		}
	}

	// -----------------------------------------
	public Set<HtEventPlugin> getRunningPlugins() {
		HashSet<HtEventPlugin> values = new HashSet<HtEventPlugin>();

		synchronized (providers_m) {
			for (Iterator<String> it = providers_m.keySet().iterator(); it.hasNext();) {
				values.add(providers_m.get(it.next()).getPlugin());
			}
			return values;
		}
	}

	// -----------------------------------------
	public int getRunningPluginsCount() {
		synchronized (providers_m) {
			return providers_m.size();
		}
	}

	// this creates synchronization event
	public static void createAlertPluginSynchronizationEvent(XmlEvent event, int syncType) {
		event.clear();

		event.getAsXmlParameter(true).setCommandName("Alert Plugin Sync");
		event.getAsXmlParameter().setInt("type", (long) syncType);
		event.setEventType(XmlEvent.ET_AlertPluginSynchronizationEvent);
		event.setEventDate(HtDateTimeUtils.getCurGmtTime());

	}
	// -------------------------------------------
}
