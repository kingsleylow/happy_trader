/*
 * HtExtGlobalCommand.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

import com.fin.httrader.externalapi.utils.HtExtBoolBuffer;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.BackgroundJobFunctor;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.LongParam;
import java.util.Map;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtGlobalCommand extends HtExtCommandBase {

	protected String getContext() {
		return getClass().getSimpleName();
	}

	// --------------------------------
	/** Creates a new instance of HtExtGlobalCommand */
	public HtExtGlobalCommand(HtExtBaseLogger logger) {
		super(logger);
	}

	// --------------------------------
	public HtExtCommandResult importData(
					String importId,
					String profileName, // at what profile we are inserting
					int importFlag, //
					String rtProviderName, // rt Provider name to mark imported data as owned by
					String hstProviderName, // use to resolve history provider
					String hstProviderType, // history provider class name
					Map<String, String> hstProviderContext, // context to be passed to history provider
					long beginDate, // date of begin and end of importing - can be changed via dialog window
					long endDate,
					int importTimeShift, // time shift when importing data
					boolean generateResultEvents,
					boolean isBlocking) {
		HtExtCommandResult result = new HtExtCommandResult("importData");
		try {


			if (!isBlocking) {
				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_importData_Nonblocking(
								importId,
								profileName,
								importFlag,
								rtProviderName,
								hstProviderName,
								hstProviderType,
								hstProviderContext,
								beginDate,
								endDate,
								importTimeShift,
								generateResultEvents);
			} else {

				// non-blocking

				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_importData_Blocking(
								importId,
								profileName,
								importFlag,
								rtProviderName,
								hstProviderName,
								hstProviderType,
								hstProviderContext,
								beginDate,
								endDate,
								importTimeShift,
								generateResultEvents);


			} //

		} catch (Throwable e) {
			result.initialize(e);
		}
		return result;
	}

	// ------------------------------------
	public HtExtCommandResult exportData(
					String exportId,
					String profileName, // at what profile we are inserting
					String hstProviderName, // use to resolve history provider
					String hstProviderType,
					Map<String, String> hstProviderContext, // context to be passed to history provider
					long beginDate, // date of begin and end of importing - can be changed via dialog window
					long endDate,
					Vector<HtPair<String, String>> provSymMap, // this map contains the provider <-> symbol map used for data selection
					int exportTimeShift, // time shift when importing data
					boolean generateResultEvents, // whether to generate result events containing import results
					long selectionWholeChunk, // when selecting the data we restrict the number of rows per one selection
					boolean isBlocking) {
		HtExtCommandResult result = new HtExtCommandResult("importData");
		try {

			if (!isBlocking) {
				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_exportData_Nonblocking(
								exportId,
								profileName,
								hstProviderName,
								hstProviderType,
								hstProviderContext,
								beginDate,
								endDate,
								provSymMap,
								exportTimeShift,
								generateResultEvents,
								selectionWholeChunk);
			} else {

				// blocking
				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_exportData_Blocking(
								exportId,
								profileName, // at what profile we are inserting
								hstProviderName, // use to resolve history provider
								hstProviderType,
								hstProviderContext, // context to be passed to history provider
								beginDate, // date of begin and end of importing - can be changed via dialog window
								endDate,
								provSymMap, // this map contains the provider <-> symbol map used for data selection
								exportTimeShift, // time shift when importing data
								generateResultEvents, // whether to generate result events containing import results
								selectionWholeChunk // when selecting the data we restrict the number of rows per one selection
					);




			}
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	public HtExtCommandResult startRtProvider(
					String providerId,
					String providertype,
					Map<String, String> initdata,
					int hourshift) {
		HtExtCommandResult result = new HtExtCommandResult("startRtProvider");
		try {
			HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_startProvider(providerId, providertype, initdata, hourshift);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	public HtExtCommandResult checkRTProviderStatus(
					String providerId,
					BackgroundJobStatus providerStatus) {
		HtExtCommandResult result = new HtExtCommandResult("checkRTProviderStatus");
		try {

			BackgroundJobStatus providerStatus_r = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_getProviderStatus(providerId);
			providerStatus.setUp(providerStatus_r.getStatusCode(), providerStatus_r.getStatusReason());

		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	public HtExtCommandResult checkRtProviderRunning(String providerId, HtExtBoolBuffer isRunningStatus) {
		HtExtCommandResult result = new HtExtCommandResult("checkRtProviderRunning");
		try {
			isRunningStatus.setBoolean(HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_queryProviderRunning(providerId));
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	public HtExtCommandResult stopRtProvider(String providerId) {
		HtExtCommandResult result = new HtExtCommandResult("stopRtProvider");
		try {
			HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_stopProvider(providerId);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	public HtExtCommandResult startEventPlugin(
					String pluginId,
					String plugintype,
					int load_flag,
					Map<String, String> initdata) {
		HtExtCommandResult result = new HtExtCommandResult("startEventPlugin");
		try {
			HtCommandProcessor.instance().get_EventPluginProxy().remote_startPlugin(pluginId, plugintype, load_flag, initdata);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}

	// -------------------------------------
	public HtExtCommandResult stopEventPlugin(String pluginId) {
		HtExtCommandResult result = new HtExtCommandResult("stopEventPlugin");
		try {
			HtCommandProcessor.instance().get_EventPluginProxy().remote_stopPlugin(pluginId);
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
	// -------------------------------------

	public HtExtCommandResult checkEventPluginRunning(String pluginId, HtExtBoolBuffer isRunningStatus) {
		HtExtCommandResult result = new HtExtCommandResult("checkEventPluginStatus");
		try {
			isRunningStatus.setBoolean(HtCommandProcessor.instance().get_EventPluginProxy().remote_queryPluginRunning(pluginId));
		} catch (Throwable e) {
			result.initialize(e);
		}

		return result;
	}
}
