/*
 * HtDatabaseProxy.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.hlpstruct.BrokerMetaInfo;



import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.interfaces.HtDataSelection;
import com.fin.httrader.interfaces.HtHistoryProvider;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.hlpstruct.ProviderSymbolInfoEntry;
import com.fin.httrader.interfaces.*;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtDatabaseSchemaManager;
import com.fin.httrader.managers.RtDatabaseSelectionManager;
import com.fin.httrader.managers.RtHistoryProviderManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.*;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtBrokerDialogData;


import com.mysql.jdbc.exceptions.jdbc4.MySQLIntegrityConstraintViolationException;

import java.sql.Connection;
import java.util.*;
import java.util.logging.Level;

/**
 *
 * @author Administrator
 * This class is intended to provide all the data fro DB related operations
 */
public class HtDatabaseProxy extends HtProxyBase {

	// import flags
	public static final int IMPORT_FLAG_OVERWRITE = 1;
	public static final int IMPORT_FLAG_SKIP = 2;
	public static final int IMPORT_FLAG_EXCEPTION = 3;
	public static final int IMPORT_BULK_SILENTLY = 4; // special flag to ignore all, do not logg and insert as batches
	
	
	// import results
	public static final int IMPORT_STATUS_OK = 0;
	public static final int IMPORT_STATUS_EXCEPTION = 1;
	public static final int IMPORT_STATUS_DUBLICATION_IGNORED = 2;
	public static final int IMPORT_STATUS_DUBLICATION_SUBSTITUTED = 3;
	public static final int IMPORT_STATUS_DUBLICATION_SUBSTITUTE_FAILED = 4;
	// exports results
	public static final int EXPORT_STATUS_OK = 0;
	public static final int EXPORT_STATUS_EXCEPTION = 1;

	// to lock lengthy opers
	private String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtDatabaseProxy */
	public HtDatabaseProxy() {
		// reset if signalled
		resetLengthyOperationFlag();
	}

	// calls that can be be performed remotely
	public void remote_clearGlobalTables() throws Exception {
		RtDatabaseSchemaManager.instance().clearGlobalTables();
		
	}

	public void remote_clearDataTable(String profilename) throws Exception {
		RtDatabaseManager.instance().clearTables(profilename);
	
	}

	public boolean remote_isAnyRtProviderRunning() {
		return (RtRealTimeProviderManager.instance().getRunningProvidersCount() > 0);
	}

	public void remote_forceRtThreadCommit() throws Exception {
		RtDatabaseManager.instance().commitRtThread();
	}

	

	public void remote_recreateGlobalTables() throws Exception {

		try {

			if (!startLengthyOperation("Recreating global tables")) {
				throw new HtException(getContext(), "remote_recreateGlobalTables", "Server is busy performing lengthy operation:" + getLengthyOperationDescription());
			}

			
			RtDatabaseSchemaManager.instance().removeGlobalTables();
			RtDatabaseSchemaManager.instance().createGlobalTables();
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_recreateGlobalTables", e.getMessage());
		} finally {
			finishLengthyOperation();
		}


	}

	public void remote_deleteRowsByIds(
					String profileName,
					Vector<Long> tids) throws Exception {
		RtDatabaseManager dbmanager = RtDatabaseManager.instance();
		try {
			if (!startLengthyOperation("Deleting rows")) {
				throw new HtException(getContext(), "remote_deleteRowsByIds", "Server is busy performing lengthy operation:" + getLengthyOperationDescription());
			}

			//dbmanager.suspendDbWriter();
			dbmanager.suspendTicksWriter(dbmanager.getProfilePartitionId(profileName));
			dbmanager.commitRtThread();

			dbmanager.deleteRowsInRange(profileName, tids);

		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_deleteRowsByIds", e.getMessage());
		} finally {

			//dbmanager.resumeDbWriter();
			dbmanager.resumeTicksWriter();
			finishLengthyOperation();
		}


	}

	public void remote_deleteRowsByRange(
					String profileName,
					Vector<HtPair<String, String>> provSymMap,
					long begdate,
					long enddate) throws Exception {
		RtDatabaseManager dbmanager = RtDatabaseManager.instance();

		try {
			if (!startLengthyOperation("deleting rows")) {
				throw new HtException(getContext(), "remote_deleteRowsByRange", "Server is busy performing lengthy operation: " + getLengthyOperationDescription());
			}

			//dbmanager.suspendDbWriter();
			dbmanager.suspendTicksWriter(dbmanager.getProfilePartitionId(profileName));
			dbmanager.commitRtThread();

			dbmanager.deleteRowsInRange(profileName, provSymMap, begdate, enddate);
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_deleteRowsByRange", e.getMessage());
		} finally {
			//dbmanager.resumeDbWriter();
			dbmanager.resumeTicksWriter();
			finishLengthyOperation();

		}


	}

	// returns the stored provider <-> symbol map
	public List<HtPair<String, String>> remote_getStoredProvidersSymbols() throws Exception {
		RtDatabaseManager.instance().commitRtThread();
		return RtDatabaseSelectionManager.instance().getStoredSymbols();
	}

	// returns only provider <-> symbols available for that profile
	public List<HtPair<String, String>> remote_getStoredProvidersSymbols_ForProfile_FromHistory(String profilename) throws Exception {
		RtDatabaseManager.instance().commitRtThread();

		return  RtDatabaseSelectionManager.instance().getStoredSymbols_ForProfile_FromHistory(profilename);

	}

	public List<HtPair<String, String>> remote_getStoredProvidersSymbols_ForProfile_FromDrawable(String profilename) throws Exception {
		RtDatabaseManager.instance().commitRtThread();

		return  RtDatabaseSelectionManager.instance().getStoredSymbols_ForProfile_FromDrawable(profilename);

	}

	public List<ProviderSymbolInfoEntry> remote_getStoredSymbols_ForProfile_WithDates(String profilename) throws Exception {
		RtDatabaseManager.instance().commitRtThread();

		return  RtDatabaseSelectionManager.instance().getStoredSymbols_ForProfile_WithDates(profilename);

	}

	public Set<String> remote_getStoredProviders_ForProfile_FromHistory(String profilename) throws Exception {
		RtDatabaseManager.instance().commitRtThread();

		return  RtDatabaseSelectionManager.instance().getStoredProviders_ForProfile_FromHistory(profilename);
	}

	/*
	// the same as above but only providers
	public Set<String> remote_getStoredProviders_ForProfile(String profilename) throws Exception {
		RtDatabaseManager.instance().commitRtThread();

		Vector<HtPair<String, String>> all = RtDatabaseSelectionManager.instance().getStoredSymbols();

		LongParam minDate = new LongParam();
		LongParam maxDate = new LongParam();

		Set<String> result = new TreeSet<String>();

		for (int i = 0; i < all.size(); i++) {
			HtPair<String, String> pair_i = all.get(i);

			remote_getMinMaxDates(
							profilename,
							pair_i.first,
							pair_i.second,
							minDate,
							maxDate);

			if (minDate.getLong() > 0 && maxDate.getLong() > 0) // exists
			{
				result.add(pair_i.first);
			}
			
		}

		return result;
			
	}
	 */


	public Set<String> remote_getStoredSymbolsForProvider(String provider) throws Exception {
		return RtDatabaseSelectionManager.instance().getStoredSymbolsForProvider(provider);
	}

	public void remote_getMinMaxDates(
					String profileName,
					String rtProvider,
					String symbol,
					LongParam minDate,
					LongParam maxDate) throws Exception {
		RtDatabaseManager.instance().commitRtThread();
		RtDatabaseSelectionManager.instance().getLoadedMinMaxDates(profileName, symbol, rtProvider, minDate, maxDate);
	}

	// retruns the range of dates
	public void remote_getMinMaxDates(
					String profileName,
					Vector<HtPair<String, String>> provSymMap,
					LongParam minDate,
					LongParam maxDate) throws Exception {


		long mindate = -1;
		long maxdate = -1;



		int cnt = 0;
		for (int i = 0; i < provSymMap.size(); i++) {

			String provider_i = provSymMap.get(i).first;
			String symbol_i = provSymMap.get(i).second;

			RtDatabaseManager.instance().commitRtThread();
			RtDatabaseSelectionManager.instance().getLoadedMinMaxDates(profileName, symbol_i, provider_i, minDate, maxDate);

			long new_mindate = minDate.getLong();
			long new_maxdate = maxDate.getLong();



			if (++cnt == 1) {
				mindate = new_mindate;
				maxdate = new_maxdate;
				continue;
			}
			;

			if (mindate < 0) {
				mindate = new_maxdate;
			}

			if (maxdate < 0) {
				maxdate = new_maxdate;
			}

			if ((new_mindate < mindate) && (new_mindate > 0)) {
				mindate = new_mindate;
			}

			if ((new_maxdate > maxdate) && (new_maxdate > 0)) {
				maxdate = new_maxdate;
			}



		}
		;


		minDate.setLong(mindate);
		maxDate.setLong(maxdate);



	}

	public void remote_getMinMaxDatesGlobal(
					String serverName,
					Map<String, Vector<HtPair<String, String>>> profProvSymbMap, // map profile <-> ( provider - symbol )
					LongParam minDate,
					LongParam maxDate) throws Exception {




		int cnt = 0;
		long mindate = -1;
		long maxdate = -1;

		for (Iterator<String> it = profProvSymbMap.keySet().iterator(); it.hasNext();) {
			String profile_i = it.next();
			Vector<HtPair<String, String>> provSymbMap = profProvSymbMap.get(profile_i);

			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getMinMaxDates(profile_i, provSymbMap, minDate, maxDate);

			//
			long new_mindate = minDate.getLong();
			long new_maxdate = maxDate.getLong();

			if (++cnt == 1) {
				mindate = new_mindate;
				maxdate = new_maxdate;
				continue;
			}
			;

			if (mindate < 0) {
				mindate = new_maxdate;
			}

			if (maxdate < 0) {
				maxdate = new_maxdate;
			}

			if ((new_mindate < mindate) && (new_mindate > 0)) {
				mindate = new_mindate;
			}

			if ((new_maxdate > maxdate) && (new_maxdate > 0)) {
				maxdate = new_maxdate;
			}

		}
		

		minDate.setLong(mindate);
		maxDate.setLong(maxdate);



	}

	

	public void remote_waitDatabaseManagerWritePendingEvents(long eventSequenceNumber, int waitSecs) throws Exception
	{
		RtDatabaseManager.instance().waitDatabaseManagerWritePendingEvents(eventSequenceNumber, waitSecs);
	}

	// this reaps all rows into callback
	public void remote_showDataAll_Callback_NoPaging(
					String profilename,
					List<HtPair<String, String>> provSymMap,
					long begDate,
					long endDate,
					HtDataSelection dataselector,
					long rowsNotifyCount,
					boolean allIfMapEmpty
		) throws Exception
	{
		//
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getHistoricalData_NoPaging(
				profilename,
				provSymMap,
				begDate,
				endDate,
				dataselector,
				rowsNotifyCount,
				allIfMapEmpty
		);

	}

	public void remote_showData_Callback_Paging(
					String profileName,
					List<HtPair<String, String>> provSymMap,
					boolean allIfMapEmpty,
					long begDate,
					long endDate,
					long pageNum,
					long pageLimitNumber, // the total number of rows per selection
					HtDataSelection dataselector
		) throws Exception {
	
		//
		RtDatabaseManager.instance().commitRtThread();


		RtDatabaseSelectionManager.instance().getHistoricalData_Paging(
						profileName,
						provSymMap,
						allIfMapEmpty,
						begDate,
						endDate,
						dataselector,
						pageNum, 
						pageLimitNumber
			);

	}


	// export
	public void remote_exportData_Nonblocking(
					final String exportId,
					final String profileName, // at what profile we are inserting
					final String hstProviderName, // use to resolve history provider
					final String hstProviderType,
					final Map<String, String> hstProviderContext, // context to be passed to history provider
					final long datebegin, // date of begin and end of importing - can be changed via dialog window
					final long dateend,
					final List<HtPair<String, String>> provSymMap, // this map contains the provider <-> symbol map used for data selection
					final int exportTimeShift, // time shift when importing data
					final boolean generateResultEvents, // whether to generate result events containing import results
					final long selectionWholeChunk // when selecting the data we restrict the number of rows per one selection
		) throws Exception {

		BackgroundJobFunctor fnctr = new BackgroundJobFunctor()
		{

			@Override
			public void execute(BackgroundJobStatus status) throws Exception
			{
				remote_exportData_Blocking(
						exportId,
						profileName, // at what profile we are inserting
						hstProviderName, // use to resolve history provider
						hstProviderType,
						hstProviderContext, // context to be passed to history provider
						datebegin, // date of begin and end of importing - can be changed via dialog window
						dateend,
						provSymMap, // this map contains the provider <-> symbol map used for data selection
						exportTimeShift, // time shift when importing data
						generateResultEvents, // whether to generate result events containing import results
						selectionWholeChunk // when selecting the data we restrict the number of rows per one selection
				);
				
				status.setUp(0, "Export finished OK");
			}
			
		};
		
		
						
		// exclusive for type IMPORT
		HtCommandProcessor.instance().get_HtBackgroundJobProxy().remote_startBackgroundJob(fnctr, exportId, true, "EXPORT");
		

		
	}

	//
	public void remote_exportData_Blocking(
					String exportId,
					String profileName, // at what profile we are inserting
					String hstProviderName, // use to resolve history provider
					String hstProviderType,
					Map<String, String> hstProviderContext, // context to be passed to history provider
					long datebegin, // date of begin and end of exporting - can be changed via dialog window
					long dateend,
					List<HtPair<String, String>> provSymMap, // this map contains the provider <-> symbol map used for data selection
					int exportTimeShift, // time shift when importing data
					boolean generateResultEvents, // whether to generate result events containing import results
					long selectionWholeChunk // when selecting the data we restrict the number of rows per one selection
					) throws Exception
	{
		// resolve history provider
		final HtHistoryProvider providerForSelector = RtHistoryProviderManager.instance().resolveProvider(
						hstProviderName,
						hstProviderType,
						exportTimeShift,
						0);


		HtLog.log(Level.INFO, getContext(), "local_exportData", "Export operation \"" + exportId + "\" is to be done on profile: \"" + profileName + "\"" +
						" history provider: \"" + hstProviderName + "\"" +
						" begin date: \"" + HtDateTimeUtils.time2String_Gmt(datebegin) + "\"" +
						" end date: \"" + HtDateTimeUtils.time2String_Gmt(dateend) + "\"" +
						" time shift: \"" + exportTimeShift + "\"");

		pushExportImportEvent(exportId, hstProviderName, true, CommonLog.LL_INFO, "Export operation is to be done on profile: \"" +
						profileName + "\"" +
						" history provider: \"" + hstProviderName + "\"" +
						" begin date: \"" + HtDateTimeUtils.time2String_Gmt(datebegin) + "\"" +
						" end date: \"" + HtDateTimeUtils.time2String_Gmt(dateend) + "\"" +
						" time shift: \"" + exportTimeShift + "\"");



		final String exportIdFin = exportId;
		final String hstProviderNameFin = hstProviderName;
		final RtDatabaseManager dbmanager = RtDatabaseManager.instance();
		final boolean generateResultEventsFin = generateResultEvents;



		

	  final Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		// helper selector that will participate in retreiving the data fromm DB
		HtDataSelection dataselector = new HtDataSelection() {

			private StringBuilder prevchunksymbol_m = new StringBuilder();
			private Vector<HtRtData> rtdata_m = new Vector<HtRtData>();

			private void pushExportEvents(int exportStatus, String errMsg) {
				if (!generateResultEventsFin) {
					return;
				}

				try {
					for (int k = 0; k < rtdata_m.size(); k++) {

						dbmanager.insertIntoImportExporLogTable_External(
										conn,
										exportIdFin,
										true,
										exportStatus,
										errMsg,
										-1,
										rtdata_m.get(k).getSymbol(), rtdata_m.get(k).getProvider(), rtdata_m.get(k).toString());
					}
					
				} catch (Throwable e) {
					// just ignore
					HtLog.log(Level.WARNING, "local_exportData", "HtDataSelection.pushExportEvents", "Exception on creating export events: " + e.getMessage());
				}
			}

			public void onSelectionFinish(long cntr) throws Exception {
				if (rtdata_m.size() > 0) {

					try {
						providerForSelector.setDataChunk(rtdata_m, prevchunksymbol_m.toString());
						pushExportEvents(EXPORT_STATUS_OK, "OK");
					} catch (Throwable e) {
						pushExportEvents(EXPORT_STATUS_EXCEPTION, e.getMessage());
						throw new HtException("local_exportData", "HtDataSelection.onSelectionFinish", "Exception on finish of export: " + e.getMessage());
					}
				}
			}

			public void newRowArrived(
							long ttime,
							String tsymbol,
							String tprovider,
							int ttype,
							int aflag,
							double tbid,
							double task,
							double topen,
							double thigh,
							double tlow,
							double tclose,
							double topen2,
							double thigh2,
							double tlow2,
							double tclose2,
							double tvolume,
							long tid,
							int color,
							int operation
						
					) throws Exception {

				if (prevchunksymbol_m.length() > 0 && !prevchunksymbol_m.toString().equals(tsymbol)) {
					try {
						providerForSelector.setDataChunk(rtdata_m, prevchunksymbol_m.toString());
						pushExportEvents(EXPORT_STATUS_OK, "OK");
					} catch (Throwable e) {
						pushExportEvents(EXPORT_STATUS_EXCEPTION, e.getMessage());
						throw new HtException("local_exportData", "HtDataSelection.newRowArrived", "Exception on new chunk arrival: " + e.getMessage());
					}

					rtdata_m.clear();
				}
				

				HtRtData rtdt = new HtRtData();

				rtdt.time_m = ttime;
				rtdt.type_m = ttype;
				rtdt.ask_m = task;
				rtdt.bid_m = tbid;
				rtdt.open_m = topen;
				rtdt.high_m = thigh;
				rtdt.low_m = tlow;
				rtdt.close_m = tclose;

				rtdt.open2_m = topen2;
				rtdt.high2_m = thigh2;
				rtdt.low2_m = tlow2;
				rtdt.close2_m = tclose2;

				rtdt.color_m = color;
				rtdt.operation_m = operation;
				rtdt.aflag_m = aflag;

				rtdt.volume_m = tvolume;
				rtdt.setSymbol(tsymbol);
				rtdt.setProvider(tprovider);

				rtdata_m.add(rtdt);

				prevchunksymbol_m.setLength(0);
				prevchunksymbol_m.append(tsymbol);
			}


		}; // end of dataselector

		// begin export

		if (!providerForSelector.beginDataExport(hstProviderContext, datebegin, dateend)) {
			pushExportImportEvent(exportId, hstProviderName, true, CommonLog.LL_ERROR, "History provider does not support export");
			throw new HtException(getContext(), "local_exportData", "History provider: \"" + hstProviderName + "\" does not support export");
		}


		// go export

		try {

			RtDatabaseManager.instance().commitRtThread();
			RtDatabaseSelectionManager.instance().getHistoricalData_NoPaging(
							(String) profileName,
							provSymMap,
							datebegin,
							dateend,
							dataselector,
							selectionWholeChunk,
							false
			);

		} catch (Throwable e) {
			pushExportImportEvent(exportId, hstProviderName, true, CommonLog.LL_ERROR, "Export operation exception: " + e.getMessage());

			throw new HtException(getContext(), "local_exportData", e.getMessage());

		} finally {
			// finish
			providerForSelector.endDataExport();

			// commit
			conn.commit();

			// release connection
			HtDbUtils.silentlyCloseConnection(conn);
			
		}

		// seems finished
		// COMMIT
		HtLog.log(Level.INFO, getContext(), "local_exportData", "Export \"" + exportId + "\" finished");

		pushExportImportEvent(exportId, hstProviderName, true, CommonLog.LL_INFO, "Export operation finished");


	}

	// import
	public void remote_importData_Nonblocking(
					final String importId,
					final String profileName, // at what profile we are inserting
					final int importFlag, //
					final String rtProviderName, // rt Provider name to mark imported data as owned by
					final String hstProviderName, // use to resolve history provider
					final String hstProviderType, // history provider class name
					final Map<String, String> hstProviderContext, // context to be passed to history provider
					final long datebegin, // date of begin and end of importing - can be changed via dialog window
					final long dateend,
					final int importTimeShift, // time shift when importing data
					final boolean generateResultEvents // whether to generate result events containing import results
			) throws Exception {


		BackgroundJobFunctor fnctr = new BackgroundJobFunctor()
		{

			@Override
			public void execute(BackgroundJobStatus status) throws Exception
			{
				remote_importData_Blocking(
						importId,
						profileName,
						importFlag,
						rtProviderName,
						hstProviderName,
						hstProviderType,
						hstProviderContext,
						datebegin,
						dateend,
						importTimeShift,
						generateResultEvents
				);
				
				status.setUp(0, "Import finished OK");
			}
			
		};
		
		
						
		// exclusive for type IMPORT
		HtCommandProcessor.instance().get_HtBackgroundJobProxy().remote_startBackgroundJob(fnctr, importId, true, "IMPORT");
	}
	

	// local calls available for local clients
	public void remote_importData_Blocking(
					String importId,
					String profileName, // at what profile we are inserting
					int importFlag, //
					String rtProviderName, // rt Provider name to mark imported data as owned by
					String hstProviderName, // use to resolve history provider
					String hstProviderType, // history provider class name
					Map<String, String> hstProviderContext, // context to be passed to history provider
					long datebegin, // date of begin and end of importing - can be changed via dialog window
					long dateend,
					int importTimeShift, // time shift when importing data
					boolean generateResultEvents // whether to generate result events containing import results
		) throws Exception {
		// resolve hisory provider


		// initialy check whether we are able to perrom this operation - e.i. whether server is noy busy
		if (!startLengthyOperation("import data")) {
			throw new HtException(getContext(), "local_importData", "Server is busy performing lengthy operation: " + getLengthyOperationDescription());
		}

		RtDatabaseManager dbmanager = RtDatabaseManager.instance();
		int partitionId = dbmanager.getProfilePartitionId(profileName);

		HtLog.log(Level.INFO, getContext(), "local_importData", "Import operation \"" + importId + 
						"\" is to be done on profile: \"" + profileName + "\"" + " partition ID: "+ partitionId +
						" history provider: \"" + hstProviderName + "\"" +
						" begin date: \"" + HtDateTimeUtils.time2String_Gmt(datebegin) + "\"" +
						" end date: \"" + HtDateTimeUtils.time2String_Gmt(dateend) + "\"" +
						" time shift: \"" + importTimeShift + "\"" +
						" RT provider: \"" + rtProviderName + "\"");

		pushExportImportEvent(importId, hstProviderName, false, CommonLog.LL_INFO, "Import operation \"" + importId + 
						"\" is to be done on profile: \"" + profileName + "\"" + " partition ID: "+ partitionId +
						" history provider: \"" + hstProviderName + "\"" +
						" begin date: \"" + HtDateTimeUtils.time2String_Gmt(datebegin) + "\"" +
						" end date: \"" + HtDateTimeUtils.time2String_Gmt(dateend) + "\"" +
						" time shift: \"" + importTimeShift + "\"" +
						" RT provider: \"" + rtProviderName + "\"");

		HtHistoryProvider provider = RtHistoryProviderManager.instance().resolveProvider(hstProviderName, hstProviderType, 0, importTimeShift);
		



		

		int cnt_rtdata = 0;
		int cnt_level1data = 0;

		
		List<HtRtData> data = new ArrayList<HtRtData>();
		List<HtLevel1Data> data_level1 = new ArrayList<HtLevel1Data>();
		StringBuilder symbol = new StringBuilder();

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		// on import use RT provider as the target for import
		try {

			// begin import
			if (!provider.beginDataImport(hstProviderContext, datebegin, dateend)) {
				pushExportImportEvent(importId, hstProviderName, false, CommonLog.LL_ERROR, "Import operation does not support import");
				throw new HtException(getContext(), "local_importData", "History provider: \"" + hstProviderName + "\" does not support import");
			}

			// suspend DB writer and make sure we may see everything from here
			//dbmanager.suspendDbWriter();
			dbmanager.suspendTicksWriter(partitionId);
			dbmanager.commitRtThread();
			
			// firts loop
			while (provider.getLevel1Chunk(data_level1, symbol) > 0) {



				// need to insert the data , if symbol is not null
				if (symbol.length() > 0) {
					// symbol is resolved once


					//HtLastDataRow before_last_ret = null;
					try {
						dbmanager.insertImportedLevel1Data(conn,importId,  partitionId, rtProviderName, symbol.toString(), data_level1, importFlag, generateResultEvents);
					} catch (MySQLIntegrityConstraintViolationException e) {
						throw new HtException(getContext(), "local_importData", e.getMessage());
					} catch (Throwable e) {
						throw new HtException(getContext(), "local_importData", "When inserting imported level 1 data exception happened: " + e.getMessage());
					} 
					


				} else {
					// symbol is resolved upon each insert
					List<HtLevel1Data> data_i = new ArrayList<HtLevel1Data>();
					for (int i = 0; i < data_level1.size(); i++) {

						data_i.clear();
						HtLevel1Data data_cur_level1 = data_level1.get(i);
						data_i.add(data_cur_level1);


						//HtLastDataRow before_last_ret = null;
						try {
							dbmanager.insertImportedLevel1Data(conn,importId,  partitionId, rtProviderName, data_cur_level1.getSymbol(), data_i, importFlag, generateResultEvents);
						} catch (MySQLIntegrityConstraintViolationException e) {
							throw new HtException(getContext(), "local_importData", e.getMessage());
						} catch (Throwable e) {
							throw new HtException(getContext(), "local_importData", "When inserting single imported level 1 data exception happened: " + e.getMessage());
						}
					}


				}

				cnt_level1data += data_level1.size();

				// commit at each iteration
				conn.commit();

				// clear data
				data.clear();
			} // end loop
			

			// second loop
			while (provider.getDataChunk(data, symbol) > 0) {



				// need to insert the data , if symbol is not null
				if (symbol.length() > 0) {
					// symbol is resolved once


					//HtLastDataRow before_last_ret = null;
					try {
						dbmanager.insertImportedData(conn,importId, partitionId, rtProviderName, symbol.toString(), data, importFlag, generateResultEvents);
					} catch (MySQLIntegrityConstraintViolationException e) {
						throw new HtException(getContext(), "local_importData", e.getMessage());
					} catch (Throwable e) {
						throw new HtException(getContext(), "local_importData", "When inserting imported data exception happened: " + e.getMessage());
					} finally {
						// obligotary part of execution
						if (data.size() > 0) {
							// finalize this as this relates to one symbol
							//HtLog.log(Level.INFO, getContext(), "insertImportedData", "Last row result is passed: " + before_last_ret.tid);
						
							dbmanager.finalizeImport(conn,  hstProviderName, partitionId, rtProviderName, symbol.toString());
						}
					}
					


				} else {
					// symbol is resolved upon each insert
					List<HtRtData> data_i = new ArrayList<HtRtData>();
					for (int i = 0; i < data.size(); i++) {

						data_i.clear();
						HtRtData data_cur = data.get(i);
						data_i.add(data_cur);


						//HtLastDataRow before_last_ret = null;
						try {
							dbmanager.insertImportedData(conn,importId, partitionId, rtProviderName, data_cur.getSymbol(), data_i, importFlag, generateResultEvents);
						} catch (MySQLIntegrityConstraintViolationException e) {
							throw new HtException(getContext(), "local_importData", e.getMessage());
						} catch (Throwable e) {
							throw new HtException(getContext(), "local_importData", "When inserting single imported data exception happened: " + e.getMessage());
						} finally {
							// need to finalize after each row
							//HtLog.log(Level.INFO, getContext(), "insertImportedData", "Last row result is passed: " + before_last_ret.tid);
							
							dbmanager.finalizeImport(conn,  hstProviderName, partitionId, rtProviderName, data_cur.getSymbol());
						}
					}


				}

				cnt_rtdata += data.size();

				// commit at each iteration
				conn.commit();

				// clear data
				data.clear();
			} // end loop

		} catch (Throwable e) {
			pushExportImportEvent(importId, hstProviderName, false, CommonLog.LL_ERROR, "On import operation exception happened: " + e.getMessage());
			throw new HtException(getContext(), "local_importData", e.getMessage());
		} finally {
			provider.endDataImport();

			//dbmanager.resumeDbWriter();
			dbmanager.resumeTicksWriter();

			// commit
			conn.commit();

			finishLengthyOperation();

			// clear connection
			HtDbUtils.silentlyCloseConnection(conn);

		}

		HtLog.log(Level.INFO, getContext(), "local_importData", "Import \"" + importId + "\" finished, inserted RT Data: " + cnt_rtdata + ", level 1 data: "+ cnt_level1data );
		pushExportImportEvent(importId, hstProviderName, false, CommonLog.LL_INFO, "Import operation finished");

		
	}

	public Vector<String> remote_getSymbolsToBeImported(
					Map<String, String> hstProviderContext,
					String hstProviderName,
					String hstProviderType) throws Exception {
		Vector<String> symbols = new Vector<String>();
		HtHistoryProvider provider = RtHistoryProviderManager.instance().resolveProvider(hstProviderName, hstProviderType, 0, 0);

		try {
			if (!provider.beginDataImport(hstProviderContext, 0, 0)) {
				throw new HtException(getContext(), "remote_getSymbolsToBeImported", "History provider: \"" + hstProviderName + "\" does not support import");
			}


			provider.getAvailableSymbolsToImport(symbols);
			
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_getSymbolsToBeImported", e.getMessage());
		} finally {
			provider.endDataImport();
		}

		return symbols;
	}

	public void remote_getMaxMinDatesSymbolsToBeImported(
					String symbol,
					Map<String, String> hstProviderContext,
					String hstProviderName,
					String hstProviderType,
					int importTimeShift,
					LongParam minDate,
					LongParam maxDate) throws Exception {
		HtHistoryProvider provider = RtHistoryProviderManager.instance().resolveProvider(hstProviderName, hstProviderType, 0, importTimeShift);
		try {
			if (!provider.beginDataImport(hstProviderContext, 0, 0)) {
				throw new HtException(getContext(), "remote_getMaxMinDatesSymbolsToBeImported", "History provider: \"" + hstProviderName + "\" does not support import");
			}


			provider.getSymbolMinMaxDatesToImport(symbol, minDate, maxDate);

			if(minDate.getLong() >0 && maxDate.getLong() >0) {
				if (minDate.getLong() > maxDate.getLong())
					throw new HtException(getContext(), "remote_getMaxMinDatesSymbolsToBeImported", "Begin date is more then end date for import");
			}
		} catch (Throwable e) {
			throw new HtException(getContext(), "remote_getMaxMinDatesSymbolsToBeImported", e.getMessage());
		} finally {
			provider.endDataImport();
		}
	}

	// shows detailed info from alert table
	public void remote_showAlertDataRow(
					String idS,
					final HtCommonRowSelector.CommonHeader header,
					final HtCommonRowSelector.CommonRow resultRow) throws Exception {
		resultRow.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultRow.create(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};


		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getAlertDataRow(idS, dataselector);

	}

	// shows detailed info from log common table
	public void remote_showCompleteLogRow(
					long id,
					final HtCommonRowSelector.CommonHeader header,
					final HtCommonRowSelector.CommonRow resultRow) throws Exception {
		resultRow.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultRow.create(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		// do not show blobs here
		RtDatabaseSelectionManager.instance().getCommonLogDataRow(id, dataselector);

	}

	public Set<String> remote_getSessionsViaRunName(String run_name) throws Exception {
		return RtDatabaseSelectionManager.instance().resolveSessionIdsViaRunName(run_name);
	}

	public Set<String> remote_getRunNamesViaSession(String sessionId) throws Exception {
		return RtDatabaseSelectionManager.instance().resolveRunNamesViaSession(sessionId);
	}

	public Map<String, Set<String>> remote_resolveSessionRunIdsViaRunIds(Set<String> runIds) throws Exception {
		return RtDatabaseSelectionManager.instance().resolveSessionRunIdsViaRunIds(runIds);
	}

	public Map<String, Set<String> > remote_resolveSessionRunIdsViaSessions(Set<String> sessions) throws Exception {
		return RtDatabaseSelectionManager.instance().resolveSessionRunIdsViaSession(sessions);
	}

	public Set<String> remote_getAvailableProviders() throws Exception {
		return RtDatabaseSelectionManager.instance().getAvailableProviders();
	}

	public void remote_showBrokerDetailRunNames(
					String run_name_filter,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					final HtCommonRowSelector.CommonHeader header,
					final List<HtCommonRowSelector.CommonRow> resultSet // returns results here
					) throws Exception {
		resultSet.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			@Override
			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			@Override
			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);
			}

			@Override
			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerDetailRunNamesDataPage(
						run_name_filter,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	public void remote_showBrokerDetailRunNames_Callback(
					String run_name_filter,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					HtCommonRowSelector dataselector
			) throws Exception {
	
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerDetailRunNamesDataPage(
						run_name_filter,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	
	// this shows broker runs
	public void remote_showBrokerRunNames(
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					final HtCommonRowSelector.CommonHeader header,
					final List<HtCommonRowSelector.CommonRow> resultSet // returns results here
					) throws Exception {
		resultSet.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			@Override
			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			@Override
			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);
			}

			@Override
			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerRunNamesDataPage(
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	public void remote_showBrokerRunNames_Callback(
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk,
					HtCommonRowSelector dataselector) throws Exception {


		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerRunNamesDataPage(
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}
	
	public void remote_showBrokerRunNames_v2_Callback(
					long begDate,
					long endDate,
					HtRunNameObjectSelector dataselector
				
	) throws Exception {


		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerRunNamesDataPage_v2(
						begDate,
						endDate,
						dataselector
						);
	}
	
	public void remote_getOrdersAndResponseTree(
			Collection<String> sessionIdList,
			HtBrokerDialogData rdata
	) throws Exception
	{
		RtDatabaseManager.instance().commitRtThread();
		RtDatabaseSelectionManager.instance().getOrdersAndResponsesTree(sessionIdList, rdata);
	}

	public String remote_getLastBrokerRunName_FromDrawable( String profilename) throws Exception {


		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		return RtDatabaseSelectionManager.instance().getLastBrokerRunName(profilename);
	}

	public void remote_updateRunNameTableWithUserData(Map<String, String> dataMap) throws Exception
	{
		RtDatabaseSelectionManager.instance().updateRunNameTableWithUserData(dataMap);
	}

	// this shows broker sessions
	public void remote_showBrokerSession(
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					final HtCommonRowSelector.CommonHeader header,
					final List<HtCommonRowSelector.CommonRow> resultSet // returns results here
					) throws Exception {
		resultSet.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			@Override
			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			@Override
			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);
			}

			@Override
			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerSessionsDataPage(
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	public void remote_showBrokerSession_Callback(
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					HtCommonRowSelector dataselector
					) throws Exception {
	

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getBrokerSessionsDataPage(
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}
	
	
	public void remote_showBackgroundJobs_Callback(
			long begDate,
			long endDate,
			long pageNum,
			long selectionWholeChunk, // the total number of rows per selection
			HtCommonRowSelector dataselector	
	) throws Exception
	{
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();
		
		RtDatabaseSelectionManager.instance().getBackgroundJobsDataPage(
				begDate,
				endDate,
				dataselector,
				pageNum,
				selectionWholeChunk
		);
		
	}


	public long remote_getAlertLastDateTime(
					String contentFilter,
					String runNameFilter,
					String sessionFilter,
					int priorityFilter,
					long begDate,
					long endDate) throws Exception {
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		return RtDatabaseSelectionManager.instance().getAlertLastDateTime(
						begDate,
						endDate,
						contentFilter,
						runNameFilter,
						sessionFilter,
						priorityFilter);
	}

	// this do not cache results in a vector, instead you must provide yoyr own interface
	public void remote_showAlerts_Callback(
					String contentFilter,
					String runNameFilter,
					String sessionFilter,
					int priorityFilter,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk,
					HtCommonRowSelector dataselector,
					String order_time_value,
					String final_order_time_value) throws Exception {
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getAlertsDataPage(
						begDate,
						endDate,
						contentFilter,
						runNameFilter,
						sessionFilter,
						priorityFilter,
						dataselector,
						RtConfigurationManager.StartUpConst.MAX_SYMBOLS_IN_LARGE_COLUMN,
						pageNum,
						selectionWholeChunk,
						null,
						order_time_value,
						final_order_time_value);

	}

	public void remote_showAlerts(
					String contentFilter,
					String runNameFilter,
					String sessionFilter,
					int priorityFilter,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk,
					final HtCommonRowSelector.CommonHeader header,
					final Vector<HtCommonRowSelector.CommonRow> resultSet,
					LongParam rowCnt,
					String order_time_value,
					String final_order_time_value) throws Exception {

		header.clear();
		resultSet.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			public void onSelectionFinish(long cntr) throws Exception {
			}

			public void newRowArrived(CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);

			}

			public void newHeaderArrived(CommonHeader newheader) throws Exception {
				header.create(newheader);

			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getAlertsDataPage(
						begDate,
						endDate,
						contentFilter,
						runNameFilter,
						sessionFilter,
						priorityFilter,
						dataselector,
						RtConfigurationManager.StartUpConst.MAX_SYMBOLS_IN_LARGE_COLUMN,
						pageNum,
						selectionWholeChunk,
						rowCnt,
						order_time_value,
						final_order_time_value);
	}



	public void remote_showDrawableObjects_Callback(
			String profilename,
			List<HtPair<String, String>> provSymMap,
			boolean allIfMapEmpty,
			long begDate,
			long endDate,
			List<Integer> type,
			String shortTextFilter,
			String longTextFilter,
			String runNameFilter,
			HtCommonRowSelector dataselector,
			HtDrawableObjectSelector drawableselector,
			long pageNum,
			long selectionWholeChunk
	) throws Exception
	{
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getDrawableObjectsDataPage(
						profilename,
						provSymMap,
						allIfMapEmpty,
						begDate,
						endDate,
						type,
						shortTextFilter,
						longTextFilter,
						runNameFilter,
						dataselector,
						drawableselector,
						pageNum,
						selectionWholeChunk);
	}

	public void remote_showDrawableObjects_Callback_DO_TEXT(
			String profilename,
			List<HtPair<String, String>> provSymMap,
			boolean allIfMapEmpty,
			long begDate,
			long endDate,
			String shortTextFilter,
			String longTextFilter,
			String runNameFilter,
			HtCommonRowSelector dataselector,
			HtDrawableObjectSelector drawableselector,
			long pageNum,
			long selectionWholeChunk
	) throws Exception
	{
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().getDrawableObjectsDataPage_DO_TEXT(
						profilename,
						provSymMap,
						allIfMapEmpty,
						begDate,
						endDate,
						shortTextFilter,
						longTextFilter,
						runNameFilter,
						dataselector,
						drawableselector,
						pageNum,
						selectionWholeChunk);
	}
	
	public void remote_getCommonLogDataPage_Callback(
					String contextFilter,
					String contentSmallFilter,
					String threadFilter,
					int level,
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					HtCommonLogSelection datalogselector,
					long pageNum,
					long selectionWholeChunk,
					int cutContentLength //RtConfigurationManager.StartUpConst.MAX_SYMBOLS_IN_LARGE_COLUMN
		) throws Exception {




		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();



		// do not show blobs here
		RtDatabaseSelectionManager.instance().getCommonLogDataPage(
						contextFilter,
						contentSmallFilter,
						threadFilter,
						level,
						begDate,
						endDate,
						dataselector,
						datalogselector,
						pageNum,
						selectionWholeChunk,
						cutContentLength,
						null);

	}

	public void remote_showOrderDetailBrokerResponses_Selectors	(
			Uid orderUid,
			HtCommonRowSelector orderDataSelector,
			HtCommonRowSelector brokerDataSelector
	) throws Exception
	{
		RtDatabaseManager.instance().commitRtThread();

		//
		RtDatabaseSelectionManager.instance().get_Order_Detail_BrokerResponse_Row(orderUid, orderDataSelector, brokerDataSelector);
	}

	public void remote_showOrderDetailBrokerResponses(
					Uid orderUid,
					final HtCommonRowSelector.CommonHeader orderHeader,
					final HtCommonRowSelector.CommonHeader brokerHeader,
					final HtCommonRowSelector.CommonRow resultOrderRow,
					final List<HtCommonRowSelector.CommonRow> resultBrokerSet) throws Exception {

		resultBrokerSet.clear();
		orderHeader.clear();
		brokerHeader.clear();

		HtCommonRowSelector dataOrderSelector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader header) throws Exception {
				orderHeader.create(header);

			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(orderHeader);
				resultOrderRow.create(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};
		//

		HtCommonRowSelector dataBrokerSelector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader header) throws Exception {
				brokerHeader.create(header);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(brokerHeader);
				resultBrokerSet.add(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};
		//
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		//
		RtDatabaseSelectionManager.instance().get_Order_Detail_BrokerResponse_Row(orderUid, dataOrderSelector, dataBrokerSelector);

	}

	public void remote_showBrokerResponseParentOrder(
					Uid brokerUid,
					final HtCommonRowSelector.CommonHeader orderHeader,
					final HtCommonRowSelector.CommonHeader brokerHeader,
					final HtCommonRowSelector.CommonRow resultOrderRow,
					final HtCommonRowSelector.CommonRow resultBrokerRow) throws Exception {

		orderHeader.clear();
		brokerHeader.clear();
		resultOrderRow.clear();
		resultBrokerRow.clear();

		HtCommonRowSelector dataOrderSelector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader header) throws Exception {
				orderHeader.create(header);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(orderHeader);
				resultOrderRow.create(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};
		//

		HtCommonRowSelector dataBrokerSelector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader header) throws Exception {
				brokerHeader.create(header);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(brokerHeader);
				resultBrokerRow.create(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};
		//
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		//
		RtDatabaseSelectionManager.instance().get_BrokerResponse_Parent_Order_Row(brokerUid, dataOrderSelector, dataBrokerSelector);

	}

	// this is the same as following but returns the total amount
	public long remote_showOrdersAmount(Vector<String> sessionIds) throws Exception {
		return RtDatabaseSelectionManager.instance().get_OrderData_Amount(sessionIds);
	}

	// shows orders data
	public void remote_showOrders(
					Vector<String> sessionIds,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					final HtCommonRowSelector.CommonHeader header,
					final Vector<HtCommonRowSelector.CommonRow> resultSet // returns results here
					) throws Exception {

		resultSet.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);
			}

			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().get_OrderData_Page(
						sessionIds,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}
	
	
	
	public void remote_showClosePos_Callback(
			Collection<String> sessionIds,
			HtCommonRowSelector dataselector
	) throws Exception 
	{
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();
		
		RtDatabaseSelectionManager.instance().showClosePos_Callback(
				sessionIds,
				dataselector
		);
	}

	// shows orders data
	public void remote_showOrders_Callback(
					List<String> sessionIds,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					HtCommonRowSelector dataselector
	) throws Exception {


		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().get_OrderData_Page(
						sessionIds,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	// this is the same as following but returns the total amount
	public long remote_showBrokerResponseOrdersAmount(Vector<String> sessionIds) throws Exception {
		return RtDatabaseSelectionManager.instance().get_BrokerResponseOrder_Amount(sessionIds);
	}

	// shows broker response data
	public void remote_showBrokerResponses(
					List<String> sessionIdStr,
					Uid parentOrderUid,
					String brokerOrderId,
					String brokerPositionId,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					final HtCommonRowSelector.CommonHeader header,
					final List<HtCommonRowSelector.CommonRow> resultSet // returns results here
					) throws Exception {
		resultSet.clear();
		header.clear();

		HtCommonRowSelector dataselector = new HtCommonRowSelector() {

			@Override
			public void newHeaderArrived(HtCommonRowSelector.CommonHeader newheader) throws Exception {
				header.create(newheader);
			}

			@Override
			public void newRowArrived(HtCommonRowSelector.CommonRow row) throws Exception {
				row.setHeader(header);
				resultSet.add(row);
			}

			@Override
			public void onSelectionFinish(long cntr) throws Exception {
			}
		};

		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().get_BrokerResponses_Page(
						sessionIdStr,
						parentOrderUid,
						brokerOrderId,
						brokerPositionId,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}


	// shows broker response data
	public void remote_showBrokerResponses_Callback(
					List<String> sessionIdStr,
					Uid parentOrderUid,
					String brokerOrderId,
					String brokerPositionId,
					long begDate,
					long endDate,
					long pageNum,
					long selectionWholeChunk, // the total number of rows per selection
					HtCommonRowSelector dataselector
		) throws Exception {
	
		// commit before to see all changes
		RtDatabaseManager.instance().commitRtThread();

		RtDatabaseSelectionManager.instance().get_BrokerResponses_Page(
						sessionIdStr,
						parentOrderUid,
						brokerOrderId,
						brokerPositionId,
						begDate,
						endDate,
						dataselector,
						pageNum,
						selectionWholeChunk);
	}

	public Map<String, Set<BrokerMetaInfo>> remote_getAvailableTradingMetainfo(Set<String> sessions) throws Exception {
		return RtDatabaseSelectionManager.instance().getAvailableTradingMetainfo(sessions);
	}

	public Map<String, List<HtPair<String, String>>> remote_getAvailableTradingReportSymbols(
					List<String> sessionIdList, // list of sessions
					long begDate,
					long endDate) throws Exception {


		TreeMap<String, List<HtPair<String, String>>> result = new TreeMap<String, List<HtPair<String, String>>>();

		RtDatabaseSelectionManager.instance().getAvailableTradingReportSymbols(sessionIdList, begDate, endDate, result);

		return result;
	}

	

	

	

	@Override
	public String getProxyName() {
		return "db_proxy";
	}

	/**
	Helpers
	 */
	private void pushExportImportEvent(
					String operId,
					String histProviderName,
					boolean isExport,
					int logLevel,
					String message) {
		XmlEvent.createSimpleLog(operId, histProviderName, logLevel, (isExport ? "EXPORT OPERATION: " : "IMPORT OPERATION: ") + message);
	}
}
