/*
 * RtDatabaseWriter.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.hlpstruct.SessionEventType;
import com.fin.httrader.hlpstruct.ResponseTypes;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.Order;
import com.fin.httrader.hlpstruct.BrokerResponseClosePos;
import com.fin.httrader.hlpstruct.BrokerResponseMetaInfo;
import com.fin.httrader.hlpstruct.BrokerResponseBase;
import com.fin.httrader.hlpstruct.BrokerResponseOrder;
import com.fin.httrader.utils.hlpstruct.HtLastDataRow;
import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.hlpstruct.*;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.*;
import com.fin.httrader.interfaces.*;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.*;

import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import com.mysql.jdbc.NonRegisteringDriver;

import com.mysql.jdbc.exceptions.jdbc4.MySQLIntegrityConstraintViolationException;
import com.mysql.jdbc.jdbc2.optional.MysqlConnectionPoolDataSource;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.math.BigDecimal;
import java.net.InetAddress;

import java.sql.DatabaseMetaData;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.logging.Level;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.sql.DataSource;

import jsync.Event;

import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;
import javax.naming.NameClassPair;
import javax.naming.NamingEnumeration;
import javax.sql.ConnectionPoolDataSource;

/**
 *
 * @author victor_zoubok This is the base class responsible for any DB
 * operations It makes deleyed transactional write into DB from any RT providers
 * Also it allows saving any historical data
 */
public class RtDatabaseManager extends Thread implements RtManagerBase, HtEventListener {

		// helper class for getSequenceNum
		private static class TimeSequenceNumberHolder {

				public long roundedTime_m = -1;
				public long periodNum_m = -1;
		};

		// -----------------------------------
		@Override
		public String getListenerName() {
				return " listener: [ " + getContext() + " ] ";
		}
		// flags
		// RT queue flag
		public static final int RT_GENERAL = 0;
		public static final int RT_COMMIT = 1;
		//
		// history data flag
		public static final int HST_DATA_CLOSED = 0;
		public static final int HST_DATA_NONCLOSED = 1;
		// for History tickers just do this fro both types
		// globals
		private final Event toShutdown_m = new Event();
		//private final ReentrantLock toShutdown_m = new ReentrantLock();
		private final Event threadStarted_m = new Event();
		// hope we will have profile IDs here instead of names
		final private HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>> newSubscriptionMap_m = new HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>>();
		// map to resolve ids and subscriptions
		private final HashMap<String, Integer> profileNamesMap_m = new HashMap<String, Integer>();
		// data buffer for storing RT data
		SingleReaderQueue queue_m = new SingleReaderQueue("Database Manager Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
		// last sequence number
		private volatile long lastSequenceNumber_m = -1;
		// to suspend DB writing
		StringBuilder delayedProfile_m = new StringBuilder();
		int delayedProfileID_m = -1;
		final Object delayedProfileMtx_m = new Object();
		List<HtRtData> delayedTicksQueue_m = new ArrayList<HtRtData>();
		List<HtLevel1Data> delayedLevel1Queue_m = new ArrayList<HtLevel1Data>();
		// ------------------------
		private Connection rtConnection_m = null;
		private final Object rtConnectionMtx_m = new Object();

		private String jndiLocation_m = null;
		
		private DataSource ds_m = null;
		//MysqlConnectionPoolDataSource ds_m = null;
		
		private Context ic_m = null;
		private String clientCharEncoding_m = null;
		// some database info
		private String enviromentInfo_m = null;
		// this map aleviates retreiving symbols IDs from dab as it caches here locally
		private Map<String, Integer> cachedSymbol_m = new HashMap<String, Integer>();
		// in many places we have run name
		// as this value navere changes we may cache this and do not ask in DB
		private Map<String, Long> cachedRunNames_m = new HashMap<String, Long>();
		//
		static private RtDatabaseManager instance_m = null;

		static public RtDatabaseManager create(String jndiLocation) throws Exception {
				if (instance_m == null) {
						instance_m = new RtDatabaseManager(jndiLocation);
				}

				return instance_m;
		}

		static public RtDatabaseManager instance() {
				return instance_m;
		}

		protected String getContext() {
				return RtDatabaseManager.class.getSimpleName();
		}

		private RtDatabaseManager(String jndiLocation) throws Exception {

				Class dbclass = Class.forName(HtStartupConstants.MYSQL_DB_DRIVER);
				if (dbclass == null)
						throw new HtException(getContext(), "RtDatabaseManager", "Invalid driver class: " + HtStartupConstants.MYSQL_DB_DRIVER); 
		
		
		
				// jndi connections
				jndiLocation_m = jndiLocation;
				ic_m = new InitialContext();
				
				//NamingEnumeration<NameClassPair> alln = ic_m.list("");
				
				ds_m = (DataSource) ic_m.lookup(jndiLocation_m);
				
			
				
				
				//System.out.println("jndi: " + jndiLocation_m);

				Connection conn = resolveSQLConnection();

				//com.mysql.jdbc.Connection jdbcconn = (com.mysql.jdbc.Connection)conn;
				Map<String, String> urlData = getDbConnectData(conn);
				
				/*
				Enumeration<Driver> drvs = DriverManager.getDrivers();
				
				
				while(drvs.hasMoreElements()) {
						Driver drv_i = drvs.nextElement();
						
						if (drv_i instanceof com.mysql.jdbc.Driver  ) {
								com.mysql.jdbc.Driver msqldrv = (com.mysql.jdbc.Driver)drv_i;
								int i = 0;
								
						}
				}
				*/
				
				
				Map<String, String> infodbdata = RtDatabaseSelectionManager.getInformationData(conn);

				clientCharEncoding_m = urlData.get("db_client_character_set");
				enviromentInfo_m = " Connect: [ connected to " + urlData.get("db_host") + ":" + urlData.get("db_port") + " as " + urlData.get("db_user") + " ]\n "
					+ " Driver: [ " + conn.getMetaData().getDriverName() + ", " + conn.getMetaData().getDriverVersion() + " ]\n "
					+ " JDBC: [ " + conn.getMetaData().getJDBCMajorVersion() + "." + conn.getMetaData().getJDBCMinorVersion() + " ]\n "
					+ " Database: [ " + conn.getMetaData().getDatabaseProductName() + " " + infodbdata.get("VERSION") + " ]\n "
					+ " Java: " + " [ " + System.getProperty("java.version") + " " + System.getProperty("java.vendor") + " ] ";

				conn.close();

				// DB manager is the first priority event receiver
				RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribePriority0Listener(this);

				// this function updates subscription map
				updateProfileSubscription(
					HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileSubscriptionMap_v2(),
					HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getProfilesPartitionMap());

				// start DB writer
				// lock to release before shutdown
				//toShutdown_m.lock();
				toShutdown_m.reset();
				start();

				// wait thread is started or rais an exception
				
				if (!threadStarted_m.waitEvent(1000)) {
						throw new HtException(getContext(), getContext(), "DB writing thread cannot be started");
				}

				HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");

		}

		public String getClientEncodingString() {
				return clientCharEncoding_m;
		}

		public String getEnviromentInfo() {
				return enviromentInfo_m;
		}

		public int getProfilePartitionId(String profilename) throws Exception {
				synchronized (newSubscriptionMap_m) {

						if (!profileNamesMap_m.containsKey(profilename)) {
								throw new HtException(getContext(), "getProfilePartitionId", "Can't resolve profile: " + profilename);
						}

						return profileNamesMap_m.get(profilename);
				}

		}

		public String getProfileNameViaPartitionId(int partId) {
				synchronized (newSubscriptionMap_m) {
						for (Iterator<String> it = profileNamesMap_m.keySet().iterator(); it.hasNext();) {
								String key = it.next();

								if ((int) profileNamesMap_m.get(key) == partId) {
										return key;
								}
						}
				}

				return "";
		}

		public Map<String, String> getDbConnectData(Connection conn) throws Exception {
				HashMap data = new HashMap();
				if (conn == null) {
						throw new HtException(getContext(), "getDbConnectPassword", "Connection to DB is not established");
				}

				DatabaseMetaData md = conn.getMetaData();

				NonRegisteringDriver ndrv = new NonRegisteringDriver();
				Properties props = ndrv.parseURL(md.getURL(), new Properties());

				// fully quilified host
				String host = ndrv.host(props);
				int port = ndrv.port(props);

				InetAddress ia = InetAddress.getByName(ndrv.host(props));

				data.put("db_user", ndrv.property(NonRegisteringDriver.USER_PROPERTY_KEY, props));
				data.put("db_host", ia.getHostName());
				data.put("db_password", ndrv.property(NonRegisteringDriver.PASSWORD_PROPERTY_KEY, props));
				data.put("db_dbname", ndrv.database(props));
				data.put("db_port", String.valueOf(port));
				data.put("db_client_character_set", ndrv.property("characterEncoding", props));
				data.put("db_url", md.getURL());

				return data;
		}

		public String getDbConnectString(Connection conn) throws Exception {
				Map<String, String> dbdata = getDbConnectData(conn);

				// data source will be completed later
				return "Database=" + dbdata.get("db_dbname")
					+ //";Data Source="+ dbdata.get("db_host")+
					";User Id=" + dbdata.get("db_user")
					+ ";Password=" + dbdata.get("db_password");
		}

		// this the delayed writer thread
		@Override
		public void run() {
				// do whatever db write

				HtLog.log(Level.INFO, getContext(), "run", "DB writing thread started", null);

				// resolve separate connection
				boolean first_time = true;
				int cnt = 0;

				//HtDbWritePacket dbpacket = null;
				ArrayList list = new ArrayList();

				while (true) {

						try {

								if (first_time) {
										rtConnection_m = resolveSQLConnection();

										first_time = false;

										// thread successfully started
										threadStarted_m.signal();

								}

								//queue_m.get(list, 1000, 2000);
								queue_m.get(list);

								//queue_m.get(list);
								for (Iterator ite = list.iterator(); ite.hasNext();) {

										XmlEvent eventdata = (XmlEvent) ite.next();

										//if (1==1)
										//throw new Exception("Critical error");
										if (eventdata != null) {

												synchronized (this.rtConnectionMtx_m) {
														int event_type = eventdata.getEventType();

														if (event_type == XmlEvent.ET_CommonLog) {

																insertIntoLogTable_LogEvent(rtConnection_m, eventdata);
														} else if (event_type == XmlEvent.ET_BrokerResponseEventResp) // insert into order_responses
														{
																insertIntoOrderResponsesTable_LogEvent(rtConnection_m, eventdata);
														} else if (event_type == XmlEvent.ET_BrokerOrderEvent) {
																insertIntoBrokerOrderTable_LogEvent(rtConnection_m, eventdata);
														} else if (event_type == XmlEvent.ET_BrokerSessionEvent) {
																insertIntoBrokerSessionTable_LogEvent(rtConnection_m, eventdata);
														} else if (event_type == XmlEvent.ET_AlgorithmAlert) {
																insertIntoAlertTable_LogEvent(rtConnection_m, eventdata);
														} else if (event_type == XmlEvent.ET_RtProviderTicker) {
																// write to DB
																delegateRTHistDataToProfiles(rtConnection_m, eventdata.getAsRtData());
														} else if (event_type == XmlEvent.ET_DrawableObject) {
																// write to DB
																delegateDrawableObjectsToProfiles(rtConnection_m, eventdata.getAsDrawableObject());
														} else if (event_type == XmlEvent.ET_Level1Data) {
																delegateLevel1DataToProfiles(rtConnection_m, eventdata.getAsLevel1Data());
														}

			// for all events
														// mark last sequence number of the event
														lastSequenceNumber_m = eventdata.getSequenceNumber();

														//automatically commit
														if (++cnt >= RtConfigurationManager.StartUpConst.COMMIT_RECORDS_VALUE) {
																rtConnection_m.commit();
																cnt = 0;
														}
												} // end conn symnch
										} // eventdata != null

								} // end event loop

						} catch (Throwable e) {
								// this is expecetd to be FATAL error
								HtLog.log(Level.WARNING, getContext(), "run", "Internal exception in DB writing thread: \"" + e.getMessage() + "\"");

								// abort
								RtCriticalErrorManager.instance().signalCriticalError("Database Writer", "", e, CriticalErrorEntry.URGENCY_FLAG_EXCEPT);
						}

						if (toShutdown_m.waitEvent(0)) {
								break;
						}

						//if (toShutdown_m.tryLock())
						//		break;
				} // end loop

				
				
				// commit and close
				
				try {
						synchronized (this.rtConnectionMtx_m) {
								if (rtConnection_m != null) {
										rtConnection_m.commit();
										rtConnection_m.close();
										rtConnection_m = null;
								}
						}
				} catch (Throwable e) {
				}

				HtLog.log(Level.INFO, getContext(), "run", "DB writing thread finishing...", null);
		}

		/**
		 * Interface functions
		 *
		 */
		public long getLastEventSequenceNumber() {
				return lastSequenceNumber_m;
		}

		public Connection resolveSQLConnection() throws Exception {
				Connection conn = ds_m.getConnection();

				if (conn == null) {
						throw new HtException(getContext(), "resolveSQLConnection", "Cannot establish connection to DB");
				}

				conn.setAutoCommit(false);
				conn.setTransactionIsolation(Connection.TRANSACTION_READ_COMMITTED);

				return conn;
		}

		// this is called by model layer to update this subscription map
		// also this need sto be called initially on startup
		public void updateProfileSubscription(
			HashMap<Integer, HashMap<String, RtProviderSubscriptionEntry>> subscription2,
			HashMap<String, Integer> profilesPartMap) {
				synchronized (newSubscriptionMap_m) {

						// new version
						newSubscriptionMap_m.clear();
						newSubscriptionMap_m.putAll(subscription2);

						profileNamesMap_m.clear();
						profileNamesMap_m.putAll(profilesPartMap);

						//String dump = dumpSubscribtion();
						//HtLog.log(Level.INFO, getContext(), "updateProfileSubscription", dump);
				}
		}

		@Override
		public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {

				queue_m.put(event);
		}

		public synchronized void deleteRowsInRange(
			String profilename,
			List<Long> tids) throws Exception {
				Connection conn = resolveSQLConnection();

				try {
						int partitionId = getProfilePartitionId(profilename);

						deleteRowsInRangeHelper(conn, partitionId, tids);
				} catch (Throwable e) {
						throw new HtException(getContext(), "deleteRowsInRange", "Exception :" + e.getMessage());
				} finally {
						conn.commit();

						HtDbUtils.silentlyCloseConnection(conn);
				}

		}

		public synchronized void insertImportedLevel1Data(
			Connection conn,
			String importId,
			int partitionId,
			String rtProvider,
			String symbol,
			List<HtLevel1Data> data,
			int import_flag,
			boolean generateResultEvents) throws Exception {
				//String pn = checkProfileName(profilename);
				if ((symbol == null) || symbol.length() <= 0) {
						throw new HtException(getContext(), "insertLevel1Data", "Symbol must be defined!");
				}

				int sid = -1;
				while ((sid = getSymbolId(conn, symbol, rtProvider)) < 0) {
						insertNewSymbol(conn, symbol, rtProvider);
				}

				for (int i = 0; i < data.size(); i++) {
						HtLevel1Data data_level1 = data.get(i);

						// set RT provider
						data_level1.setProvider(rtProvider);

						if (!data_level1.isValid()) {
								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_EXCEPTION,
											"Invalid ticker",
											sid,
											data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());

								}

								throw new HtException(getContext(), "insertImportedLevel1Data", "Invalid level1 : " + data_level1.toString());
						}

						boolean remove_duplicate = false;
						boolean found_dublicate = false;
						try {
								found_dublicate = this.insertLevel1Helper(conn, partitionId, data_level1, false);

								// success
								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_OK,
											"Success",
											sid,
											data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());

								}

						} catch (MySQLIntegrityConstraintViolationException icv) {
								// for the special cases we need to intercept this

								if (import_flag == HtDatabaseProxy.IMPORT_FLAG_EXCEPTION) {
										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_EXCEPTION,
													icv.getMessage(),
													sid,
													data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());
										}

										throw new MySQLIntegrityConstraintViolationException("On import duplicate keys are found on inserting level 1: \""
											+ data_level1.toString() + "\"");
								} else if (import_flag == HtDatabaseProxy.IMPORT_FLAG_OVERWRITE) {
										remove_duplicate = true;

								} else if (import_flag == HtDatabaseProxy.IMPORT_FLAG_SKIP) {

										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_IGNORED,
													"Ignored",
													sid,
													data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());

										}

								}

						}

						//
						if (remove_duplicate) {
								// just ignore this
								long tid = getTidFromLevel1Table(conn, partitionId, data_level1.time_m, sid);
								if (tid < 0) {
										throw new HtException(getContext(), "insertImportedLevel1Data", "The system must resolve level1 tid on sid="
											+ sid + " ttime=" + HtDateTimeUtils.time2String_Gmt(data_level1.time_m));
								}

								// remove
								if (deleteSingleLevel1Helper(conn, partitionId, tid) < 0) {
										throw new HtException(getContext(), "insertImportedLevel1Data", "Row with tid: " + tid + " to be deleted!");
								}

								// repeat insertion, should not happen exception
								try {
										insertLevel1Helper(conn, partitionId, data_level1, false);
								} catch (MySQLIntegrityConstraintViolationException icv) {
										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_SUBSTITUTE_FAILED,
													"Substitution failed: " + icv.getMessage(),
													sid,
													data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());

										}

										throw new HtException(getContext(), "insertImportedLevel1Data", "On import duplicate keys are found on inserting level 1: \""
											+ data_level1.toString() + "\", \n but this should not happen is we already removed duplicates: " + icv.getMessage());
								}

								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_SUBSTITUTED,
											"Successfully substituted",
											sid,
											data_level1.getSymbol(), data_level1.getProvider(), data_level1.toString());

								}

						}

				} // end main loop

		} ///////////////////////////////

		// generally symbol is not imported from history data, but passed explecetely
		public synchronized HtLastDataRow insertImportedData(
			Connection conn,
			String importId,
			int partitionId,
			String rtProvider,
			String symbol,
			List<HtRtData> data,
			int import_flag,
			boolean generateResultEvents) throws Exception {

				//String pn = checkProfileName(profilename);
				if ((symbol == null) || symbol.length() <= 0) {
						throw new HtException(getContext(), "insertImportedData", "Symbol must be defined!");
				}

				// resolve symbol id
				int sid = -1;
				while ((sid = getSymbolId(conn, symbol, rtProvider)) < 0) {
						insertNewSymbol(conn, symbol, rtProvider);
				}

				// return last row before import
				HtLastDataRow last_row = getLastHtRtDataRow(conn, partitionId, symbol, rtProvider);

				// now mark this row as closed !!!!
				if (last_row.tid > 0) {
						updateTickerDataFlag(conn, partitionId, last_row.tid, HST_DATA_CLOSED);
				}

				if (import_flag == HtDatabaseProxy.IMPORT_BULK_SILENTLY) {

						insertTickerDataBulk(conn, partitionId, data, sid, HST_DATA_CLOSED);
						// the fastest way
						return last_row;
				}

				//
				//int import_status  = HtImportResultSelection.IMPORT_STATUS_OK;
				for (int i = 0; i < data.size(); i++) {
						HtRtData data_rt = data.get(i);

						// set RT provider
						data_rt.setProvider(rtProvider);

						if (!data_rt.isValid()) {
								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_EXCEPTION,
											"Invalid ticker",
											sid,
											data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());

								}

								throw new HtException(getContext(), "insertImportedData", "Invalid ticker: " + data_rt.toString());
						}

						boolean remove_duplicate = false;
						try {
								insertTickerData(conn, partitionId, data_rt, sid, HST_DATA_CLOSED);

								// success
								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_OK,
											"Success",
											sid,
											data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());

								}

						} catch (MySQLIntegrityConstraintViolationException icv) {
								// for the special cases we need to intercept this

								if (import_flag == HtDatabaseProxy.IMPORT_FLAG_EXCEPTION) {
										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_EXCEPTION,
													icv.getMessage(),
													sid,
													data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());
										}

										throw new MySQLIntegrityConstraintViolationException("On import duplicate keys are found on inserting ticker: \""
											+ data_rt.toString() + "\"");
								} else if (import_flag == HtDatabaseProxy.IMPORT_FLAG_OVERWRITE) {
										remove_duplicate = true;

								} else if (import_flag == HtDatabaseProxy.IMPORT_FLAG_SKIP) {

										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_IGNORED,
													"Ignored",
													sid,
													data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());

										}

								}

						}

						//
						if (remove_duplicate) {
								// just ignore this
								long tid = getTidFromHistoryTable(conn, partitionId, data_rt.time_m, sid);
								if (tid < 0) {
										throw new HtException(getContext(), "insertImportedData", "The system must resolve tid on sid="
											+ sid + " ttime=" + HtDateTimeUtils.time2String_Gmt(data_rt.time_m));
								}

								// remove
								if (deleteSingleRowsHelper(conn, partitionId, tid) < 0) {
										throw new HtException(getContext(), "insertImportedData", "Row with tid: " + tid + " to be deleted!");
								}

								// repeat insertion, should not happen exception
								try {
										insertTickerData(conn, partitionId, data_rt, sid, HST_DATA_CLOSED);
								} catch (MySQLIntegrityConstraintViolationException icv) {
										if (generateResultEvents) {
												insertIntoImportExporLogTable(
													conn,
													importId,
													false,
													HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_SUBSTITUTE_FAILED,
													"Substitution failed: " + icv.getMessage(),
													sid,
													data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());

										}

										throw new HtException(getContext(), "insertImportedData", "On import duplicate keys are found on inserting ticker: \""
											+ data_rt.toString() + "\", \n but this should not happen is we already removed duplicates: " + icv.getMessage());
								}

								if (generateResultEvents) {
										insertIntoImportExporLogTable(
											conn,
											importId,
											false,
											HtDatabaseProxy.IMPORT_STATUS_DUBLICATION_SUBSTITUTED,
											"Successfully substituted",
											sid,
											data_rt.getSymbol(), data_rt.getProvider(), data_rt.toString());

								}

						}

				} // end main loop

				return last_row;

		}

		// will finalize import to have consistent data
		public synchronized void finalizeImport(
			Connection conn,
			String hstProviderName,
			int partitionId,
			String rtProvider,
			String symbol //long beginImportedDataTime,
		//long endImportedDataTime
		) throws Exception {

				// even before_last_ret.tid < 0 - e.i.that was import to the empty table
				// we need to mark the last flag as unclosed !!!
				HtLastDataRow after_last_ret = getLastHtRtDataRow(conn, partitionId, symbol, rtProvider);
				HtLog.log(Level.FINE, getContext(), "finalizeImport", "This last bar will be marked as NON_CLOSED: " + after_last_ret.rtdata.toString()
					+ " having tid: " + after_last_ret.tid);

				if (after_last_ret.tid > 0) {
						updateTickerDataFlag(conn, partitionId, after_last_ret.tid, HST_DATA_NONCLOSED);
				}

		}

		public synchronized void deleteRowsInRange(
			String profilename,
			List<HtPair<String, String>> provSymMap,
			long begDate,
			long endDate) throws Exception {

				int partitionId = getProfilePartitionId(profilename);

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

				PreparedStatement stmt = null;
				ArrayList<Integer> sids = new ArrayList<Integer>();

				// return symbol & provider ids
				for (int i = 0; i < provSymMap.size(); i++) {
						String provider_i = provSymMap.get(i).first;
						String symbol_i = provSymMap.get(i).second;

						int sid = getSymbolId(conn, symbol_i, provider_i);
						if (sid < 0) {
								throw new HtException(getContext(), "deleteRowsInRange", "Invalid symbol ID for: " + provider_i + " - " + symbol_i);
						}

						sids.add(sid);
				}

				sql.append("delete from HISTORY_");
				sql.append(partitionId);
				sql.append("  where 1=1 ");

				if (begDate > 0) {
						sql.append(" and ttime>=? ");
				}
				if (endDate > 0) {
						sql.append(" and ttime<=? ");
				}

				// symbols and providers
				if (sids.size() > 0) {
						sql.append(" and ( ");
						for (int i = 1; i <= sids.size(); i++) {
								sql.append(" tsymbol_id=? ");

								if (i < sids.size()) {
										sql.append(" or ");
								}

						}

						sql.append(" )");
				}

				//
				int cnt = 0;

				try {

						stmt = conn.prepareStatement(sql.toString());

						if (begDate > 0) {
								stmt.setLong(++cnt, begDate);
						}

						if (endDate > 0) {
								stmt.setLong(++cnt, endDate);
						}

						for (int i = 0; i < sids.size(); i++) {
								stmt.setInt(++cnt, sids.get(i).intValue());
						}

						stmt.execute();
						conn.commit();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "deleteRowsInRange", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);
						HtDbUtils.silentlyCloseConnection(conn);

				}
		}
  //

		// this function must be called in a separate thread and it waits until through DB manager
		// en event with ID equal more then passed will come
		// thi is to make sure that we do not miss anything
		public void waitDatabaseManagerWritePendingEvents(long eventSequenceNumber, int waitSecs) throws Exception {
				HtLog.log(Level.INFO, getContext(), "waitDatabaseManagerWritePendingEvents", "Serving thread asked to complete DB operations");
				int cnt = 0;
				while (true) {
						if (getLastEventSequenceNumber() >= eventSequenceNumber) {
								break;
						}

						HtUtils.Sleep(1.0);

						if (++cnt > waitSecs) {
								throw new HtException(getContext(), "waitDatabaseManagerWritePendingEvents", "Cannot reach the point all the events are written into DB. Attempt was made " + cnt + " time(s).");
						}
				}

				HtLog.log(Level.INFO, getContext(), "waitDatabaseManagerWritePendingEvents", "Serving thread completed pending DB operations");

		}

		public void suspendTicksWriter(int profile_id) throws Exception {
				synchronized (delayedProfileMtx_m) {
						delayedProfileID_m = profile_id;
						HtLog.log(Level.INFO, getContext(), "suspendTicksWriter", "Signalled to suspend ticks DB writing");
				}
		}

		public void resumeTicksWriter() throws Exception {
				synchronized (this.delayedProfileMtx_m) {
						delayedProfileID_m = -1;

						int qsz = delayedTicksQueue_m.size();
						for (int i = 0; i < delayedTicksQueue_m.size(); i++) {
								// push to the queue
								XmlEvent event = new XmlEvent();
								RtRealTimeProviderManager.createXmlEventRtData(delayedTicksQueue_m.get(i), event);
								onEventArrived(event);
						}

						delayedTicksQueue_m.clear();

						//
						qsz = delayedLevel1Queue_m.size();
						for (int i = 0; i < delayedLevel1Queue_m.size(); i++) {
								// push to the queue
								XmlEvent event = new XmlEvent();
								RtRealTimeProviderManager.createXmlEventLevel1(delayedLevel1Queue_m.get(i), event);
								onEventArrived(event);
						}

						delayedLevel1Queue_m.clear();

						HtLog.log(Level.INFO, getContext(), "resumeTicksWriter", "Signalled to resume ticks DB writing and purged: " + qsz + " ticks back to the queue");

						//
				}
		}

		// as this thread exist constantly
		public void shutdown() {
				try {

						//resumeDbWriter_m.signal();
						toShutdown_m.signal();
						//toShutdown_m.unlock();

						// close the queue
						queue_m.shutdown();

						join();

	  // commit the cur trans
						// unsubscribe
						//RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(this);
						RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribePriority0Listener(this);

						queue_m.releaseMonitoring();

				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "shutdown", "Exception on shut down DB manager: " + e.getMessage(), null);
				}
				HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
		}

		public void commitRtThread() {
				synchronized (this.rtConnectionMtx_m) {
						try {
								if (rtConnection_m != null) {
										rtConnection_m.commit();
								}
						} catch (Throwable e) {
								HtLog.log(Level.WARNING, getContext(), "commitRtThread", "Exception on commit RT thread: " + e.getMessage());
						}

				}

		}

		public synchronized void clearTables(String profilename) throws Exception {
				PreparedStatement stmt = null;
				StringBuilder sql = new StringBuilder();
				//String pn = checkProfileName(profilename);
				int partitionId = getProfilePartitionId(profilename);

				Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
				try {

						sql.append(" delete from HISTORY_");
						sql.append(partitionId);
						stmt = conn.prepareStatement(sql.toString());
						stmt.execute();

						conn.commit();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "clearTables", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);
						HtDbUtils.silentlyCloseConnection(conn);
				}
		}

		// this is public version of export/import logging - it is synchronized
		public synchronized void insertIntoImportExporLogTable_External(
			Connection conn,
			String operId,
			boolean isExport,
			int operStatus,
			String operMessage,
			int sid,
			String symbol,
			String provider,
			String stringContext) throws Exception {
				insertIntoImportExporLogTable(conn, operId, isExport, operStatus, operMessage, sid, symbol, provider, stringContext);
		}

		public String dumpSubscribtion() {

				StringBuilder result = new StringBuilder();
				for (Iterator<Integer> it = newSubscriptionMap_m.keySet().iterator(); it.hasNext();) {
						int partId = it.next();
						String profileName = getProfileNameViaPartitionId(partId);

						result.append("\n Profile: '").append(profileName).append("' partition ID: ").append(partId).append(" subscribed for [\n");

						HashMap<String, RtProviderSubscriptionEntry> profile_value = newSubscriptionMap_m.get(partId);

						if (profile_value != null) {
								for (Iterator<String> it2 = profile_value.keySet().iterator(); it2.hasNext();) {
										String provider_i = it2.next();
										RtProviderSubscriptionEntry provider_data = profile_value.get(provider_i);

										result.append("( ");
										result.append(" Provider: '").append(provider_i).append("'");
										result.append(" Time Scale: ").append(HtDataProfileProp.timeScaleToString(provider_data.timeScale_m));
										result.append(" Mult Factor: ").append(provider_data.multFact_m);
										result.append(" Subscribed to RT/HIST: ").append(provider_data.subscribedRtHist_m ? "true" : "false");
										result.append(" Subscribed to Level1: ").append(provider_data.subscribeLevel1_m ? "true" : "false");
										result.append(" Subscribed to Drawable obj: ").append(provider_data.subscribeDrawableObj_m ? "true" : "false");
										result.append(" )\n");

								}
						} else {
								result.append(" ( No subscription ) \n");
						}

						result.append("]\n");

				}

				return result.toString();
		}

		public void updateBackgroundJobEntry(long id, String strId, String typeId, int resultCode, String resultString, long beginTime, long endTime) throws Exception {
				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
				try {

						sql.append("update background_jobs set string_id=?, type_id=?, start_time=?, finish_time=?, result_code=?, result_string=? where id=?");

						stmt = conn.prepareStatement(sql.toString());
						stmt.setString(1, strId);
						stmt.setString(2, typeId);
						stmt.setDouble(3, beginTime);
						stmt.setDouble(4, endTime);
						stmt.setInt(5, resultCode);
						stmt.setString(6, resultString);
						stmt.setLong(7, id);

						stmt.execute();

						conn.commit();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "updateBackgroundJobEntry", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);
						HtDbUtils.silentlyCloseConnection(conn);

				}
		}

		public long insertBackgroundJobEntry(String strId, String typeId) throws Exception {
				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				long id = -1;

				Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
				try {

						sql.append(" insert into background_jobs(string_id, type_id) values (?,?)");

						stmt = conn.prepareStatement(sql.toString(), Statement.RETURN_GENERATED_KEYS);
						stmt.setString(1, strId);
						stmt.setString(2, typeId);

						stmt.execute();

						ResultSet rs = stmt.getGeneratedKeys();
						if (rs.next()) {
								id = rs.getLong(1);
						} else {
								throw new HtException(getContext(), "insertBackgroundJobEntry", "Cannot return autogenerated ID");
						}

						conn.commit();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertBackgroundJobEntry", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);
						HtDbUtils.silentlyCloseConnection(conn);

				}

				return id;
		}

		/**
		 * ------------------------------------------------------ Common helpers
		 */
		private int deleteSingleLevel1Helper(
			Connection conn,
			int partitionId,
			long tid) throws Exception {

				int rows_affected = -1;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				PreparedStatement stmt = null;
				sql.append(" delete from LEVEL1_");
				sql.append(partitionId);
				sql.append(" where tid = ?");

				try {

						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, tid);
						rows_affected = stmt.executeUpdate();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "deleteSingleLevel1Helper", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
				return rows_affected;
		}

		private int deleteSingleRowsHelper(
			Connection conn,
			int partitionId,
			long tid) throws Exception {

				int rows_affected = -1;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				PreparedStatement stmt = null;
				sql.append(" delete from HISTORY_");
				sql.append(partitionId);
				sql.append(" where tid = ?");

				try {

						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, tid);
						rows_affected = stmt.executeUpdate();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "deleteSingleRowsHelper", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
				return rows_affected;
		}

		private void deleteRowsInRangeHelper(
			Connection conn,
			int partitionId,
			List<Long> tids) throws Exception {
				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				PreparedStatement stmt = null;
				sql.append(" delete from HISTORY_");
				sql.append(partitionId);
				sql.append(" where tid = ?");

				try {

						stmt = conn.prepareStatement(sql.toString());

						for (int i = 0; i < tids.size(); i++) {
								stmt.setLong(1, tids.get(i));
								stmt.execute();
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "deleteRowsInRangeHelper", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		// helper to insert into export import log table
		private void insertIntoImportExporLogTable(
			Connection conn,
			String operId,
			boolean isExport,
			int operStatus,
			String operMessage,
			int sid,
			String symbol,
			String provider,
			String stringContext) throws Exception {
				PreparedStatement stmt = null;
				StringBuilder sql = new StringBuilder();

				sql.append("insert into EXPORTIMPORTLOG (tlast_created, groupid, opertype, operstatus, opermessage, symbol_id, tdata) ");
				sql.append(" values (?, ?, ?, ?, ?, ?, ?) ");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, Calendar.getInstance().getTimeInMillis());
						stmt.setString(2, operId);
						stmt.setInt(3, isExport ? XmlEvent.ET_InternalExportResultsLog : XmlEvent.ET_InternalImportResultsLog);
						stmt.setInt(4, operStatus);
						stmt.setString(5, operMessage);

						if (sid < 0) {
								int newSid = getSymbolId(conn, symbol, provider);
								stmt.setInt(6, sid);
						} else {
								stmt.setInt(6, sid);
						}

						stmt.setString(7, stringContext);

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertIntoImportExporLogTable", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		// common log
		private void insertIntoLogTable_LogEvent(
			Connection conn,
			XmlEvent event) throws Exception {
				// special types of logs!!!

				if (event.getType() != XmlEvent.DT_Log) {
						throw new HtException(getContext(), "insertIntoLogTable_LogEvent", "Invalid event type");
				}

				CommonLog cl = event.getAsLog();
				PreparedStatement stmt = null;
				String sql = "insert into COMMONLOG (thread, session, level, tlast_created, logtype, context, content) values (?,?,?,?,?,?,?)";

				try {
						stmt = conn.prepareStatement(sql.toString());
						// the number of milisec
						// sesison
						stmt.setLong(1, cl.getLogThread());
						stmt.setString(2, cl.getSession());
						stmt.setInt(3, cl.getLogLevel());

						stmt.setLong(4, cl.getLogDate());

						stmt.setInt(5, event.getEventType());
						stmt.setString(6, HtUtils.truncateString(cl.getContext(), 64));

						stmt.setString(7, cl.getContent());

						// execute
						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertIntoLogTable_LogEvent", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private void insertIntoBrokerOrderTable_LogEvent(Connection conn, XmlEvent event) throws Exception {
				XmlParameter xmlparam = event.getAsXmlParameter();

				// resolve this
				Order order = new Order();
				HtXmlParameterSerialization.deserializeObjectFromXmlParameter(order, xmlparam);

				if (!order.ID_m.isValid()) {
						throw new HtException(getContext(), "insertIntoBrokerOrderTable_LogEvent", "Invalid order ID");
				}

				PreparedStatement stmt = null;

				String sql = "insert into ORDERS ("
					+ "source, id, broker_position_id, broker_order_id, order_type, provider, symbol,  "
					+ "comment, or_issue_time, or_time, or_price, or_stop_price, or_sl_price, or_tp_price, or_volume, or_expiration_time, or_validity_flag, "
					+ "or_place, or_method, or_additional_info, or_stop_price_type, or_price_type,context) values ( "
					+ "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? )";

				try {
						// and get what we need
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, event.getSource());
						stmt.setString(2, order.ID_m.toString());
						stmt.setString(3, order.brokerPositionID_m.toString());
						stmt.setString(4, order.brokerOrderID_m.toString());
						stmt.setInt(5, order.orderType_m);
						stmt.setString(6, order.provider_m.toString());
						stmt.setString(7, order.symbol_m.toString());

						stmt.setString(8, order.comment_m.toString());
						stmt.setLong(9, (long) (order.orderIssueTime_m > 0 ? order.orderIssueTime_m * 1000 : -1));
						stmt.setLong(10, (long) (order.orTime_m > 0 ? order.orTime_m * 1000 : -1));
						stmt.setDouble(11, order.orPrice_m);
						stmt.setDouble(12, order.orStopPrice_m);
						stmt.setDouble(13, order.orSLprice_m);
						stmt.setDouble(14, order.orTPprice_m);
						stmt.setInt(15, order.orVolume_m);

						stmt.setLong(16, (long) (order.orExpirationTime_m > 0 ? order.orExpirationTime_m * 1000 : -1));
						stmt.setInt(17, order.orderValidity_m);

						stmt.setString(18, order.place_m.toString());
						stmt.setString(19, order.method_m.toString());
						stmt.setString(20, order.additionalInfo_m.toString());
						stmt.setInt(21, order.orStopPriceType_m);
						stmt.setInt(22, order.orPriceType_m);
						stmt.setString(23, HtUtils.truncateString(order.context_m.toString(), 128));

						stmt.execute();

				} catch (SQLException sqe) {
						// need to out some debug info
						HtLog.log(Level.INFO, getContext(), "insertIntoBrokerOrderTable_LogEvent", "Order to be inserted: \n"
							+ " source: " + event.getSource() + "\n"
							+ " ID: " + order.ID_m.toString() + "\n"
							+ " broker position ID: " + order.brokerPositionID_m.toString() + "\n"
							+ " time: " + (order.orTime_m > 0 ? HtDateTimeUtils.time2String_Gmt((long) order.orTime_m * 1000) : "") + "\n"
							+ " type: " + OrderType.getTypeName(order.orderType_m) + "\n"
							+ " symbol : " + order.symbol_m.toString() + "\n"
							+ " provider: " + order.provider_m.toString() + "\n");

						throw new HtException(getContext(), "insertIntoBrokerOrderTable_LogEvent", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void insertIntoAlertTable_LogEvent(Connection conn, XmlEvent event) throws Exception {

				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				sql.append("insert into ALERTS (id, source, alg_brk_pair, thread, priority, alert_time, data, runname_id, small_data) values (?,?,?,?,?,?,?,?,?)");

				try {
						XmlParameter xmlparam = event.getAsXmlParameter();
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, xmlparam.getUid("id").toString());
						stmt.setString(2, event.getSource());
						stmt.setString(3, xmlparam.getString("alg_brk_pair"));
						stmt.setInt(4, (int) xmlparam.getInt("thread"));
						stmt.setInt(5, (int) xmlparam.getInt("priority"));

						long eventDateTime = event.getEventDate();
						stmt.setDouble(6, eventDateTime);

						StringBuilder sdata = new StringBuilder(xmlparam.getString("data"));
						if (sdata.length() > 16384) {
								sdata.setLength(16384);
						}

						stmt.setString(7, sdata.toString());
						String runName = xmlparam.getString("run_name");

						// run name id
						long runName_id = resolveIdFromRunNameTable(conn, runName);

						if (runName_id < 0) {
								// insert into run name
								insertIntoRunNameTable(conn, runName, "", eventDateTime);
								runName_id = resolveIdFromRunNameTable(conn, runName);
						}

						stmt.setLong(8, runName_id);

						StringBuilder small_sdata = new StringBuilder(xmlparam.getString("small_data"));
						if (small_sdata.length() > 256) {
								sdata.setLength(256);
						}

						stmt.setString(9, small_sdata.toString());

						stmt.execute();

						//HtLog.log(Level.INFO, getContext(), "insertIntoAlertTable_LogEvent", "Small data: " + small_sdata + " large data: " + sdata);
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertIntoAlertTable_LogEvent", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		// resolve run name id
		private void insertIntoRunNameTable(
			Connection conn,
			String runName,
			String comment,
			double optime) throws Exception {

				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				try {
						sql.append("insert into BROKER_RUNNAMES(run_name, comment, op_first_time) values (?,?,?)");
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, runName);
						if (comment.length() > 4096) {
								stmt.setString(2, comment.substring(0, 4095));
						} else {
								stmt.setString(2, comment);
						}

						stmt.setDouble(3, optime);
						stmt.execute();
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertIntoRunNameTable", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private long resolveIdFromRunNameTable(
			Connection conn,
			String runName) throws Exception {
				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				Long runNameID = cachedRunNames_m.get(runName);
				if (runNameID != null) {

						return runNameID;
				}

				try {
						sql.append("select b.id from broker_runnames b where b.run_name=?");
						stmt = conn.prepareStatement(sql.toString());
						stmt.setString(1, runName);

						ResultSet rs = stmt.executeQuery();

						if (rs.next()) {
								long runNameId = rs.getLong(1);
								cachedRunNames_m.put(runName, runNameId);

								return runNameId;
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "resolveIdFromRunNameTable", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return -1;

		}

		// resolve session from
		private void resolveBrokerSessionTable(
			Connection conn,
			String session_id,
			long create_time,
			int is_sumulation,
			String connect_string) throws Exception {

				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				try {

						sql.append("insert into BROKER_SESSIONS(session_id, create_time, is_simulation, connect_string)");
						sql.append(" values (?, ?, ?, ?)");

						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, session_id);
						stmt.setLong(2, create_time);
						stmt.setInt(3, is_sumulation);
						stmt.setString(4, connect_string);

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "resolveBrokerSessionTable", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void insertIntoBrokerSessionTable_LogEvent(Connection conn, XmlEvent event) throws Exception {
				StringBuilder sql_insert = new StringBuilder();
				PreparedStatement stmt = null;
				try {
						XmlParameter xmlparam = event.getAsXmlParameter();

						String session_id = event.getSource();
						long optime = (long) (xmlparam.getDouble("time") > 0 ? xmlparam.getDouble("time") * 1000 : -1);
						int operation = (int) xmlparam.getInt("operation");

						int is_sumulation = -1;
						String connect_string = null;

						String runName = xmlparam.getString("session_name");

						if (operation == SessionEventType.SET_Create) {
								is_sumulation = (int) xmlparam.getInt("is_simulation");
								connect_string = xmlparam.getString("connect_string");
								String comment = xmlparam.getString("comment");

								// resolve
								long runName_id = this.resolveIdFromRunNameTable(conn, runName);

								if (runName_id < 0) {
										// insert into run name
										insertIntoRunNameTable(conn, runName, comment, optime);
								}

								// into session table
								resolveBrokerSessionTable(conn, session_id, optime, is_sumulation, connect_string);
						}

						// resolve
						long runName_id = this.resolveIdFromRunNameTable(conn, runName);
						if (runName_id <= 0) {
								throw new HtException(getContext(), "insertIntoBrokerSessionTable_LogEvent", "Invalid BROKER_RUNNAMES.ID");
						}

						// insert new row
						sql_insert.append("insert into BROKER_SESSIONS_OPER ");
						sql_insert.append("(session_id, alg_brk_pair, thread, op_time, operation, runname_id)");
						sql_insert.append(" values ( ?, ?, ?, ?, ?, ? )");

						stmt = conn.prepareStatement(sql_insert.toString());

						stmt.setString(1, session_id);
						stmt.setString(2, xmlparam.getString("alg_brk_pair"));
						stmt.setInt(3, (int) xmlparam.getInt("thread"));
						stmt.setLong(4, optime);
						stmt.setInt(5, operation);
						stmt.setLong(6, runName_id);

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertIntoBrokerSessionTable_LogEvent", "SQL exception in \"" + sql_insert.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void insert_BrokerResponseOrder(Connection conn, BrokerResponseOrder brokerResponseOrder, String source, XmlParameter xmlparam) throws Exception {
				PreparedStatement stmt = null;
				String sql = "insert into ORDER_RESPONSES ("
					+ "source, id, op_issue_time, parent_id, broker_comment, broker_internal_code, result_code, result_detail_code, "
					+ "broker_position_id, broker_order_id, op_price, op_stop_price, op_tp_price, op_sl_price, op_volume, op_remaining_volume, "
					+ "provider, symbol, op_expiration_time, op_validity_flag, op_time, op_comission, op_pos_type, context ) values ("
					+ "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

				try {
						// and get what we need
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, source);
						stmt.setString(2, brokerResponseOrder.ID_m.toString());
						stmt.setLong(3, (long) (brokerResponseOrder.brokerIssueTime_m > 0 ? brokerResponseOrder.brokerIssueTime_m * 1000 : -1));
						stmt.setString(4, brokerResponseOrder.parentID_m.toString());
						stmt.setString(5, brokerResponseOrder.brokerComment_m.toString());
						stmt.setInt(6, brokerResponseOrder.brokerInternalCode_m);
						stmt.setInt(7, brokerResponseOrder.resultCode_m);
						stmt.setInt(8, brokerResponseOrder.resultDetailCode_m);

						stmt.setString(9, brokerResponseOrder.brokerPositionID_m.toString());
						stmt.setString(10, brokerResponseOrder.brokerOrderID_m.toString());

						stmt.setDouble(11, brokerResponseOrder.opPrice_m);
						stmt.setDouble(12, brokerResponseOrder.opStopPrice_m);
						stmt.setDouble(13, brokerResponseOrder.opTPprice_m);
						stmt.setDouble(14, brokerResponseOrder.opSLprice_m);
						stmt.setInt(15, brokerResponseOrder.opVolume_m);
						stmt.setInt(16, brokerResponseOrder.opRemainingVolume_m);

						stmt.setString(17, brokerResponseOrder.provider_m.toString());
						stmt.setString(18, brokerResponseOrder.symbol_m.toString());

						stmt.setLong(19, (long) (brokerResponseOrder.opExpirationTime_m > 0 ? brokerResponseOrder.opExpirationTime_m * 1000 : -1));
						stmt.setInt(20, brokerResponseOrder.opOrderValidity_m);
						stmt.setLong(21, (long) (brokerResponseOrder.opTime_m > 0 ? brokerResponseOrder.opTime_m * 1000 : -1));

						stmt.setDouble(22, brokerResponseOrder.opComission_m);
						stmt.setInt(23, brokerResponseOrder.opOrderType_m);
						stmt.setString(24, HtUtils.truncateString(brokerResponseOrder.context_m.toString(), 128));

						stmt.execute();

				} catch (SQLException sqe) {
						dumpError(xmlparam);

						throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage());
				} catch (Throwable e) {
						dumpError(xmlparam);

						throw new Exception("Exception in insertIntoOrderResponsesTable_LogEvent(...): " + e.getMessage());
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void insert_BrokerResponseClosePos(Connection conn, BrokerResponseClosePos brokerResponseclosePos, String source, XmlParameter xmlparam) throws Exception {
				PreparedStatement stmt = null;
				String sql = "insert into CLOSE_POS(source, id, broker_position_id, provider,symbol, volume, trade_direction, price_open, "
					+ "price_close, parent_id, broker_comment, broker_internal_code, result_code, result_detail_code, broker_issue_time, comission_abs, comission_pct, pos_open_time, pos_close_time )"
					+ " values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, source);
						stmt.setString(2, brokerResponseclosePos.ID_m.toString());
						stmt.setString(3, brokerResponseclosePos.brokerPositionID_m.toString());
						stmt.setString(4, brokerResponseclosePos.provider_m.toString());
						stmt.setString(5, brokerResponseclosePos.symbol_m.toString());
						stmt.setInt(6, brokerResponseclosePos.volume_m);
						stmt.setInt(7, brokerResponseclosePos.tradeDirection_m);
						stmt.setDouble(8, brokerResponseclosePos.priceOpen_m);
						stmt.setDouble(9, brokerResponseclosePos.priceClose_m);
						stmt.setString(10, brokerResponseclosePos.parentID_m.toString());
						stmt.setString(11, brokerResponseclosePos.brokerComment_m.toString());
						stmt.setInt(12, brokerResponseclosePos.brokerInternalCode_m);
						stmt.setInt(13, brokerResponseclosePos.resultCode_m);
						stmt.setInt(14, brokerResponseclosePos.resultDetailCode_m);
						stmt.setLong(15, (long) (brokerResponseclosePos.brokerIssueTime_m > 0 ? brokerResponseclosePos.brokerIssueTime_m * 1000 : -1));

						stmt.setDouble(16, brokerResponseclosePos.comissionAbs_m);
						stmt.setDouble(17, brokerResponseclosePos.comissionPct_m);

						stmt.setLong(18, (long) (brokerResponseclosePos.posOpenTime_m > 0 ? brokerResponseclosePos.posOpenTime_m * 1000 : -1));
						stmt.setLong(19, (long) (brokerResponseclosePos.posCloseTime_m > 0 ? brokerResponseclosePos.posCloseTime_m * 1000 : -1));

						stmt.execute();

				} catch (SQLException sqe) {
						dumpError(xmlparam);

						throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage());
				} catch (Throwable e) {
						dumpError(xmlparam);

						throw new Exception("Exception in insertIntoOrderResponsesTable_LogEvent(...): " + e.getMessage());
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void insert_BrokerResponseMetaInfo(Connection conn, BrokerResponseMetaInfo brokerResponseMetaInfo, String source, XmlParameter xmlparam) throws Exception {
				PreparedStatement stmt = null;
				String sql = "insert into BROKER_SYMBOLS_METAINFO(session_id, symbol, sign_digits, point_value,spread)"
					+ "values(?,?,?,?,?)";

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, source);
						stmt.setString(2, brokerResponseMetaInfo.symbol_m.toString());

						stmt.setInt(3, brokerResponseMetaInfo.signDigits_m);
						stmt.setDouble(4, brokerResponseMetaInfo.pointValue_m);
						stmt.setDouble(5, brokerResponseMetaInfo.spread_m);

						stmt.execute();

				} catch (SQLException sqe) {
						dumpError(xmlparam);

						throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage());
				} catch (Throwable e) {
						dumpError(xmlparam);

						throw new Exception("Exception in insertIntoOrderResponsesTable_LogEvent(...): " + e.getMessage());
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private void dumpError(XmlParameter xmlparam) {
				try {
						String filename = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory()
							+ File.separatorChar + "db_write_error_" + HtDateTimeUtils.getCurGmtTime() + ".xml";

						HtFileUtils.writeContentToFile(filename, XmlHandler.serializeParameterStatic(xmlparam));
				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "dumpError", "Exception: " + e.getMessage());
				}
		}

		private void insertIntoOrderResponsesTable_LogEvent(Connection conn, XmlEvent event) throws Exception {

				XmlParameter xmlparam = event.getAsXmlParameter();

				// resolve this
				BrokerResponseBase brokerResponseBase = BrokerResponseBase.instantiate(xmlparam);

				if (brokerResponseBase.type_m == ResponseTypes.RT_BrokerResponseOrder) {

						BrokerResponseOrder brokerResponseOrder = (BrokerResponseOrder) brokerResponseBase;
						HtXmlParameterSerialization.deserializeObjectFromXmlParameter(brokerResponseOrder, xmlparam);

						if (!brokerResponseOrder.ID_m.isValid()) {
								throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "Invalid broker response ID");
						}

						insert_BrokerResponseOrder(conn, brokerResponseOrder, event.getSource(), xmlparam);
				} else if (brokerResponseBase.type_m == ResponseTypes.RT_BrokerResponseClosePos) {
						BrokerResponseClosePos brokerResponseclosePos = (BrokerResponseClosePos) brokerResponseBase;
						HtXmlParameterSerialization.deserializeObjectFromXmlParameter(brokerResponseclosePos, xmlparam);

						if (!brokerResponseclosePos.ID_m.isValid()) {
								throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "Invalid broker close pos ID");
						}

						insert_BrokerResponseClosePos(conn, brokerResponseclosePos, event.getSource(), xmlparam);

				} // metainfo
				else if (brokerResponseBase.type_m == ResponseTypes.RT_BrokerResponseMetaInfo) {
						BrokerResponseMetaInfo brokerResponseMetaInfo = (BrokerResponseMetaInfo) brokerResponseBase;
						HtXmlParameterSerialization.deserializeObjectFromXmlParameter(brokerResponseMetaInfo, xmlparam);

						if (!brokerResponseMetaInfo.ID_m.isValid()) {
								throw new HtException(getContext(), "insertIntoOrderResponsesTable_LogEvent", "Invalid broker response ID");
						}

						insert_BrokerResponseMetaInfo(conn, brokerResponseMetaInfo, event.getSource(), xmlparam);

				}
		}

		private TimeSequenceNumberHolder getPeriodSequenceNum(
			int aggrperiod,
			int multfactor,
			long tickerTime) throws Exception {

				TimeSequenceNumberHolder tsnh = new TimeSequenceNumberHolder();

				GregorianCalendar curTime = new GregorianCalendar();
				curTime.setTimeInMillis(tickerTime);

				GregorianCalendar candleTime = new GregorianCalendar();
				candleTime.setTimeInMillis(curTime.getTimeInMillis());

				if (aggrperiod == ((Integer) HtDataProfileProp.TS_YEAR[0]).intValue()) {
						if (multfactor != 1) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = (curTime.get(Calendar.YEAR) + 1900) * 1000;

						candleTime.set(Calendar.DAY_OF_YEAR, 1);
						candleTime.set(Calendar.HOUR_OF_DAY, 0);
						candleTime.set(Calendar.MINUTE, 0);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);
				} else if (aggrperiod == ((Integer) HtDataProfileProp.TS_MONTH[0]).intValue()) {
						if (multfactor != 1) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = (curTime.get(Calendar.YEAR) + 1900) * 1000 + curTime.get(Calendar.MONTH);

						candleTime.set(Calendar.DAY_OF_MONTH, 1);
						candleTime.set(Calendar.HOUR_OF_DAY, 0);
						candleTime.set(Calendar.MINUTE, 0);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);

				} else if (aggrperiod == ((Integer) HtDataProfileProp.TS_WEEK[0]).intValue()) {
						if (multfactor != 1) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = (curTime.get(Calendar.YEAR) + 1900) * 1000 + curTime.get(Calendar.WEEK_OF_YEAR);

						candleTime.set(Calendar.DAY_OF_WEEK, Calendar.SUNDAY);
						candleTime.set(Calendar.HOUR_OF_DAY, 0);
						candleTime.set(Calendar.MINUTE, 0);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);

				} else if (aggrperiod == ((Integer) HtDataProfileProp.TS_DAY[0]).intValue()) {
						if (multfactor != 1) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = (curTime.get(Calendar.YEAR) + 1900) * 1000 + curTime.get(Calendar.DAY_OF_YEAR);

						candleTime.set(Calendar.HOUR_OF_DAY, 0);
						candleTime.set(Calendar.MINUTE, 0);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);

				} else if (aggrperiod == ((Integer) HtDataProfileProp.TS_HOUR[0]).intValue()) {
						BigDecimal[] tmp = BigDecimal.valueOf(curTime.get(Calendar.HOUR_OF_DAY)).divideAndRemainder(BigDecimal.valueOf(multfactor));

						if (multfactor < 1 || multfactor > 24) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = curTime.get(Calendar.YEAR) * 100000 + curTime.get(Calendar.DAY_OF_YEAR) * 100 + tmp[0].intValue();

						candleTime.set(Calendar.HOUR_OF_DAY, tmp[0].intValue() * multfactor);
						candleTime.set(Calendar.MINUTE, 0);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);

				} else if (aggrperiod == ((Integer) HtDataProfileProp.TS_MINUTE[0]).intValue()) {
						BigDecimal[] tmp = BigDecimal.valueOf(curTime.get(Calendar.MINUTE)).divideAndRemainder(BigDecimal.valueOf(multfactor));

						if (multfactor < 1 || multfactor > 60) {
								throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid multfactor: " + multfactor);
						}

						tsnh.periodNum_m = (curTime.get(Calendar.YEAR) + 1900) * 10000000
							+ curTime.get(Calendar.DAY_OF_YEAR) * 10000
							+ curTime.get(Calendar.HOUR_OF_DAY) * 100 + tmp[0].intValue();

						candleTime.set(Calendar.MINUTE, tmp[0].intValue() * multfactor);
						candleTime.set(Calendar.SECOND, 0);
						candleTime.set(Calendar.MILLISECOND, 0);

				} else {
						throw new HtException(getContext(), "getPeriodSequenceNum", "Invalid aggregation period");
				}

				tsnh.roundedTime_m = candleTime.getTimeInMillis();
				return tsnh;

		}

		private void delegateLevel1DataToProfiles(Connection conn, HtLevel1Data level1data) throws Exception {
				synchronized (newSubscriptionMap_m) {
						// iterate through all data profiles
						for (Iterator<Integer> it = newSubscriptionMap_m.keySet().iterator(); it.hasNext();) {
								Integer partitionId = it.next();

								// detect whether we have subscription for this
								HashMap<String, RtProviderSubscriptionEntry> profile_value = newSubscriptionMap_m.get(partitionId);

								// find for this provider
								RtProviderSubscriptionEntry provider_data = profile_value.get(level1data.getProvider());

								if (provider_data != null) {
		  // found!

										// will write here
										if (provider_data.subscribeLevel1_m) {

												if (!level1data.isValid()) {
														throw new HtException(getContext(), "delegateLevel1DataToProfiles", "Level1 data ticker is invalid: " + level1data.toString());
												}

												// IF DELAYED
												synchronized (delayedProfileMtx_m) {

														if (delayedProfileID_m == partitionId) {
																delayedLevel1Queue_m.add(level1data);
														} else {
																// update dublicates as what we can do here?
																insertLevel1Helper(conn, partitionId, level1data, true);
														}
												}
										}

								}

						}

				}
		}

		private void delegateRTHistDataToProfiles(Connection conn, HtRtData rt_data) throws Exception {
				synchronized (newSubscriptionMap_m) {
						// iterate through all data profiles
						for (Iterator<Integer> it = newSubscriptionMap_m.keySet().iterator(); it.hasNext();) {
								Integer partitionId = it.next();

								// detect whether we have subscription for this
								HashMap<String, RtProviderSubscriptionEntry> profile_value = newSubscriptionMap_m.get(partitionId);

								// find for this provider
								RtProviderSubscriptionEntry provider_data = profile_value.get(rt_data.getProvider());

								if (provider_data != null) {
		  // found!

										// will write here
										if (provider_data.subscribedRtHist_m) {

												if (!rt_data.isValid()) {
														throw new HtException(getContext(), "delegateRTHistDataToProfiles", "RT data ticker is invalid: " + rt_data.toString());
												}

												// IF DELAYED
												synchronized (delayedProfileMtx_m) {

														if (delayedProfileID_m == partitionId) {
																delayedTicksQueue_m.add(rt_data);
														} else {
																insertRtHelper(conn, partitionId, rt_data, provider_data.multFact_m, provider_data.timeScale_m);
														}
												}
										}

								}

						}

				}

		}

		private void delegateDrawableObjectsToProfiles(Connection conn, HtDrawableObject drawableObj) throws Exception {
				synchronized (newSubscriptionMap_m) {
						// iterate through all data profiles
						for (Iterator<Integer> it = newSubscriptionMap_m.keySet().iterator(); it.hasNext();) {
								Integer partitionId = it.next();

								// detect whether we have subscription for this
								HashMap<String, RtProviderSubscriptionEntry> profile_value = newSubscriptionMap_m.get(partitionId);

								// find for this provider
								RtProviderSubscriptionEntry provider_data = profile_value.get(drawableObj.getProvider());

								if (provider_data != null) {
										// found!

										if (provider_data.subscribeDrawableObj_m) {
												if (!drawableObj.isValid()) {
														throw new HtException(getContext(), "delegateDrawableObjectsToProfiles", "Drawable object is not valid");
												}

												insertDrawableObjHelper(rtConnection_m, partitionId, drawableObj);
										}

								}

						}

				}
		}

		private void insertDrawableObjHelper(
			Connection conn,
			int partitionId,
			HtDrawableObject obj) throws Exception {

				// insert symbol
				int sid = -1;

				while ((sid = getSymbolId(conn, obj.getSymbol(), obj.getProvider())) < 0) {
						insertNewSymbol(conn, obj.getSymbol(), obj.getProvider());
				}

				// resolve run name
				// run name id
				long cur_time = HtDateTimeUtils.getCurGmtTime();
				long runName_id = this.resolveIdFromRunNameTable(conn, obj.getRunName());

				if (runName_id < 0) {
						// insert into run name
						insertIntoRunNameTable(conn, obj.getRunName(), "", cur_time);
						runName_id = this.resolveIdFromRunNameTable(conn, obj.getRunName());
				}

				String sql = null;
				PreparedStatement stmt = null;

				try {

						if (obj.getType() == HtDrawableObject.DO_LINE_INDIC) {
								sql = "insert into DRAWABLE_" + partitionId
									+ "(tsymbol_id, create_ttime, run_name_id, ttime, type, name, lineindic_name1, lineindic_color1, lineindic_value1,  lineindic_name2, lineindic_color2, lineindic_value2, lineindic_name3, lineindic_color3, lineindic_value3, lineindic_name4, lineindic_color4, lineindic_value4,  lineindic_name5, lineindic_color5, lineindic_value5, lineindic_name6, lineindic_color6, lineindic_value6, lineindic_name7, lineindic_color7, lineindic_value7 )  values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? )";

								stmt = conn.prepareStatement(sql);

								stmt.setInt(1, sid);
								stmt.setLong(2, cur_time);
								stmt.setLong(3, runName_id);
								stmt.setLong(4, obj.getTime());
								stmt.setInt(5, obj.getType());
								stmt.setString(6, obj.getName());

								List<HtDrawableObject.LineIndicator> lobj = obj.getAsLineIndicator();
								if (HtDrawableObject.MAX_INDIC < lobj.size()) {
										throw new HtException(getContext(), "insertDrawableObjHelper", "Only " + HtDrawableObject.MAX_INDIC + " indicator objects is allowed");
								}

								int cnt = 7;

								for (int i = 0; i < HtDrawableObject.MAX_INDIC; i++) {
										if (i < lobj.size()) {
												HtDrawableObject.LineIndicator lindic_i = lobj.get(i);
												stmt.setString(cnt++, lindic_i.name_m.toString());
												stmt.setInt(cnt++, lindic_i.color_m);
												stmt.setDouble(cnt++, lindic_i.value_m);
										} else {
												stmt.setNull(cnt++, java.sql.Types.VARCHAR);
												stmt.setNull(cnt++, java.sql.Types.VARCHAR);
												stmt.setNull(cnt++, java.sql.Types.VARCHAR);
										}

								}

						} else if (obj.getType() == HtDrawableObject.DO_PRINT) {
								sql = "insert into DRAWABLE_"
									+ partitionId
									+ "(tsymbol_id, create_ttime, run_name_id, ttime, type, name, print_bid, print_ask, print_volume, print_color, print_operation ) values (?,?,?,?,?,?,?,?,?,?,?)";

								stmt = conn.prepareStatement(sql);
								stmt.setInt(1, sid);
								stmt.setLong(2, cur_time);
								stmt.setLong(3, runName_id);
								stmt.setLong(4, obj.getTime());
								stmt.setInt(5, obj.getType());
								stmt.setString(6, obj.getName());

								HtDrawableObject.Print pobj = obj.getAsPrint();
								stmt.setDouble(7, pobj.bid_m);
								stmt.setDouble(8, pobj.ask_m);
								stmt.setDouble(9, pobj.volume_m);
								stmt.setInt(10, pobj.color_m);
								stmt.setInt(11, pobj.operation_m);

						} else if (obj.getType() == HtDrawableObject.DO_OHLC) {
								sql = "insert into DRAWABLE_"
									+ partitionId + "(tsymbol_id, create_ttime, run_name_id, ttime, type, name, ohlc_open, ohlc_high, ohlc_low, ohlc_close, ohlc_volume) values (?,?,?,?,?,?,?,?,?,?,?)";

								stmt = conn.prepareStatement(sql);
								stmt.setInt(1, sid);
								stmt.setLong(2, cur_time);
								stmt.setLong(3, runName_id);
								stmt.setLong(4, obj.getTime());
								stmt.setInt(5, obj.getType());
								stmt.setString(6, obj.getName());

								HtDrawableObject.OHLC ohlcobj = obj.getAsOHLC();
								stmt.setDouble(7, ohlcobj.open_m);
								stmt.setDouble(8, ohlcobj.high_m);
								stmt.setDouble(9, ohlcobj.low_m);
								stmt.setDouble(10, ohlcobj.close_m);
								stmt.setDouble(11, ohlcobj.volume_m);

						} else if (obj.getType() == HtDrawableObject.DO_TEXT) {

								sql = "insert into DRAWABLE_" + partitionId + "(tsymbol_id, create_ttime, run_name_id, ttime, type, name, priority, short_text, text) values (?,?,?,?,?,?,?,?,?)";

								stmt = conn.prepareStatement(sql);
								stmt.setInt(1, sid);
								stmt.setLong(2, cur_time);
								stmt.setLong(3, runName_id);
								stmt.setLong(4, obj.getTime());
								stmt.setInt(5, obj.getType());
								stmt.setString(6, obj.getName());

								HtDrawableObject.Text textobj = obj.getAsText();
								stmt.setDouble(7, textobj.priority_m);

								//if (textobj.shortText_m.length() > 32) {
								//	stmt.setString(8, textobj.shortText_m.substring(0,31));
								//}
								//else {
								stmt.setString(8, textobj.shortText_m.toString());
		//}

								//if (textobj.text_m.length() > 8192 ) {
								//	stmt.setString(9, textobj.text_m.substring(0, 8191));
								//}
								//else {
								stmt.setString(9, textobj.text_m.toString());
								//}

						} else if (obj.getType() == HtDrawableObject.DO_LEVEL1) {
								sql = "insert into DRAWABLE_" + partitionId + "(tsymbol_id, create_ttime, run_name_id,ttime, type, name, level1_best_bid_price, level1_best_ask_price, level1_best_bid_volume, level1_best_ask_volume) values (?,?,?,?,?,?,?,?,?,?)";

								stmt = conn.prepareStatement(sql);
								stmt.setInt(1, sid);
								stmt.setLong(2, cur_time);
								stmt.setLong(3, runName_id);
								stmt.setLong(4, obj.getTime());
								stmt.setInt(5, obj.getType());
								stmt.setString(6, obj.getName());

								HtDrawableObject.Level1 lev1obj = obj.getAsLevel1();
								stmt.setDouble(7, lev1obj.best_bid_price_m);
								stmt.setDouble(8, lev1obj.best_ask_price_m);
								stmt.setDouble(9, lev1obj.best_bid_volume_m);
								stmt.setDouble(10, lev1obj.best_ask_volume_m);

						} else {
								throw new HtException(getContext(), "insertDrawableObjHelper", "Invalid type: " + obj.getType());
						}

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertDrawableObjHelper", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private boolean insertLevel1Helper(
			Connection conn,
			int partitionId,
			HtLevel1Data level1data,
			boolean re_write_dublicates
		) throws Exception {
				boolean found_dublicate = false;

				String provider = level1data.getProvider();
				String symbol = level1data.getSymbol();

				// resolve symbol
				int sid = -1;
				while ((sid = getSymbolId(conn, symbol, provider)) < 0) {
						insertNewSymbol(conn, symbol, provider);
				}

				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				String sql = "insert into LEVEL1_" + partitionId + " ("
					+ "tsymbol_id, ttime, bid, bidsize, ask, asksize, openint ) values "
					+ "( ?,?,?,?,?,?,?)";

				try {

						stmt = conn.prepareStatement(sql);

						stmt.setInt(1, sid);
						stmt.setLong(2, level1data.time_m);

						stmt.setDouble(3, level1data.best_bid_price_m);
						stmt.setDouble(4, level1data.best_bid_volume_m);
						stmt.setDouble(5, level1data.best_ask_price_m);
						stmt.setDouble(6, level1data.best_ask_volume_m);

						stmt.setDouble(7, level1data.open_interest_m);

						stmt.execute();

				} catch (MySQLIntegrityConstraintViolationException icv) {
						// for the special cases we need to intercept this
						HtLog.log(Level.FINE, getContext(), "insertLevel1Helper", "Dublicate keys found on insert: Ticker: \""
							+ level1data.toString() + "\", tsymbol_id=" + sid + ", exception: \"" + icv.getMessage() + "\", profile ID: \"" + partitionId + "\"");

						found_dublicate = true;
						if (re_write_dublicates) {
								updateLevel1DataHelper(conn, partitionId, level1data, sid);
						} else {
								throw new MySQLIntegrityConstraintViolationException(icv.getMessage());
						}
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertLevel1Helper", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\""
							+ "\"" + " sid=" + sid + ", failing ticker: \"" + level1data.toString() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return found_dublicate;
		}

		private void updateLevel1DataHelper(
			Connection conn,
			int partitionId,
			HtLevel1Data level1data,
			int sid
		) throws Exception {

				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				String sql = "update LEVEL1_" + partitionId + " set bid=?, bidsize=?, ask=?, asksize=?, openint=?  where tsymbol_id=? and ttime=?";

				try {
						stmt = conn.prepareStatement(sql);
						stmt.setDouble(1, level1data.best_bid_price_m);
						stmt.setDouble(2, level1data.best_bid_volume_m);
						stmt.setDouble(3, level1data.best_ask_price_m);
						stmt.setDouble(4, level1data.best_ask_volume_m);
						stmt.setDouble(5, level1data.open_interest_m);

						stmt.setInt(6, sid);
						stmt.setLong(7, level1data.time_m);

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "updateLevel1DataHelper", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\""
							+ "\"" + " sid=" + sid + ", failing ticker: \"" + level1data.toString() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		// another version working with stored procedure
		private void insertRtHelper2(
			Connection conn,
			int partitionId,
			HtRtData rtdata,
			int multfactor_val_i,
			int ts_val_i) throws Exception {
		}

		private void insertRtHelper(
			Connection conn,
			int partitionId,
			HtRtData rtdata,
			int multfactor_val_i,
			int ts_val_i) throws Exception {
				// this is expected to be last stored row
				//CArrivedStorage arrivedStorage = new CArrivedStorage();
				String provider = rtdata.getProvider();

				// resolve symbol
				int sid = -1;
				while ((sid = getSymbolId(conn, rtdata.getSymbol(), provider)) < 0) {
						insertNewSymbol(conn, rtdata.getSymbol(), provider);
				}

				// return last row
				HtLastDataRow last_ret = getLastHtRtDataRow(conn, partitionId, rtdata.getSymbol(), provider);

				//HtLog.log(Level.INFO, getContext(), "insertRtHelper", "Got RT data: \""+rtdata.toString() + "\"");
				// if arrived data time is less than already stored - just ignore this
				boolean norm_insert = true;
				if (rtdata.time_m <= last_ret.rtdata.time_m) {
						HtLog.log(Level.WARNING, getContext(), "insertRtHelper", "Provider: \"" + provider + "\" delivered data: \"" + rtdata.toString()
							+ "\" with timestamp that is less or equal of what is stored in DB : \"" + last_ret.rtdata + "\", will ignore...");

						norm_insert = false;
						return;
				}

				if (ts_val_i <= 0) {

						if (last_ret.aflag != HST_DATA_CLOSED) {
								// reset this flag
								updateTickerDataFlag(conn, partitionId, last_ret.tid, HST_DATA_CLOSED);
						}

						// no aggregation, simply write new ticker
						insertTickerData(conn, partitionId, rtdata, sid, HST_DATA_CLOSED);
						return;
				}

				// if there are noprevious data i.e. is empty
				if ((!last_ret.rtdata.isValid()) /*
					 * || (arrivedStorage.aflag_m == HST_DATA_CLOSED)
					 */) {
	  // if we cannot resolve last data from DB merely insert new arrived data and mark  this as non-closed

						// or may be the last read data is closed
						//aggregateRt(last_ret.rtdata, rtdata, true);
						aggregateRt_NewPeriod(last_ret, rtdata);
						insertTickerData(conn, partitionId, last_ret.rtdata, sid, HST_DATA_NONCLOSED);
						//HtLog.log(Level.INFO, getContext(), "insertRtHelper", "Provider: \"" + provider + "\" Inserted new unclosed ticker data: "+arrivedStorage.rtdata_m.toString() +
						//    " on sid: \""+arrivedStorage.sid_m+"\"", null);
						return;
				}

				// current and previous
				TimeSequenceNumberHolder prevSeqHld = getPeriodSequenceNum(ts_val_i, multfactor_val_i, last_ret.rtdata.time_m);
				TimeSequenceNumberHolder curSeqHld = getPeriodSequenceNum(ts_val_i, multfactor_val_i, rtdata.time_m);

				if (norm_insert) {

						// check we overcome over the preiod
						if (curSeqHld.periodNum_m > prevSeqHld.periodNum_m) {
								// mark last entry as closed, and round time which could be good practice
								updateTickerDataFlagAndTime(conn, partitionId, last_ret.tid, prevSeqHld.roundedTime_m, HST_DATA_CLOSED);

		//HtLog.log(Level.INFO, getContext(), "insertRtHelper", "Provider: \"" + provider +"\" Set closed status on ticker on tid: \""+ arrivedStorage.tid_m+
								//        "\" and round the time to : \"" + HtUtils.time2String_Local(-1)+ "\"");
								// and create new entry
								aggregateRt_NewPeriod(last_ret, rtdata);

								// insert new current entry
								insertTickerData(conn, partitionId, last_ret.rtdata, sid, HST_DATA_NONCLOSED);
								//HtLog.log(Level.INFO, getContext(), "insertRtHelper", "Provider: \"" + provider +"\" Inserted new unclosed ticker data: "+arrivedStorage.rtdata_m.toString() +
								//      " on sid: \""+arrivedStorage.sid_m+"\"");

						} else {
								// only intergate data
								long updated_time = rtdata.time_m;
								//aggregateRt(last_ret.rtdata, rtdata, false);
								aggregateRt_UpdateWithLaterTime(last_ret, rtdata);

								// and update
								updateTickerData(conn, partitionId, last_ret.rtdata, last_ret.tid, updated_time, HST_DATA_NONCLOSED);

								//HtLog.log(Level.INFO, getContext(), "insertRtHelper", "Provider: \"" + provider +"\" Updated ticker data: "+arrivedStorage.rtdata_m.toString() +
								//  " on tid: \""+arrivedStorage.tid_m+"\"" + "\" and round the time to : \"" + HtUtils.time2String_Local(updated_time)+ "\"", null);
						}
				}
		}

		private void updateTickerDataFlagAndTime(
			Connection conn,
			int partitionId,
			long tid,
			long newtime,
			int aflag) throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				sql.append("update HISTORY_");
				sql.append(partitionId);
				sql.append(" set aflag =?");
				sql.append(" , ttime=?");
				sql.append(" where tid=?");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setDouble(1, aflag);
						stmt.setLong(2, newtime);
						stmt.setLong(3, tid);

						stmt.execute();

				} catch (MySQLIntegrityConstraintViolationException ive) {
						HtLog.log(Level.WARNING, "getContext()", "updateTickerDataFlagAndTime", "Integrity violation: " + ive.getMessage());
						mergeTwoRows(conn, partitionId, tid, newtime, aflag);
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "updateTickerDataFlagAndTime", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}
		// update only flag

		private void updateTickerDataFlag(
			Connection conn,
			int partitionId,
			long tid,
			int aflag) throws Exception {

				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				sql.append("update HISTORY_");
				sql.append(partitionId);
				sql.append(" set aflag =?");
				sql.append(" where tid=?");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setDouble(1, aflag);
						stmt.setLong(2, tid);

						stmt.execute();

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "updateTickerDataFlag", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private void mergeTwoRows(Connection conn, int partitionId, long newTid, long oldTime, int aflag) throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				try {

						// 1st return new row (which is expected to be updated)
						HtLastDataRow newRow = getArbitraryRow(conn, partitionId, newTid);
						if (newRow.tid <= 0) {
								throw new HtException(getContext(), "mergeTwoRows", "this tid: \"" + newTid + "\" must points to valid row!");
						}

						String symbolToResolve = newRow.rtdata.getSymbol();
						String providerToResolve = newRow.rtdata.getProvider();

						HtLastDataRow oldRow = getArbitraryRow(conn, partitionId, symbolToResolve, providerToResolve, oldTime);
						if (oldRow.tid > 0) {
								// found duplicate
								HtLog.log(Level.INFO, getContext(), "mergeTwoRows", "Found duplicate, will merge 2 rows");

								// merge them
								//aggregateRt(oldRow.rtdata, newRow.rtdata, false);
								aggregateRt_UpdateWithLaterTime(oldRow, newRow.rtdata);

								// need to kill both rows and insert updated with
								ArrayList<Long> tids = new ArrayList<Long>();
								tids.add(newRow.tid);
								tids.add(oldRow.tid);
								commitRtThread();

								deleteRowsInRangeHelper(conn, partitionId, tids);
								HtLog.log(Level.INFO, getContext(), "mergeTwoRows", "Deleted invalid row with tid: " + oldRow.tid);
								HtLog.log(Level.INFO, getContext(), "mergeTwoRows", "Deleted invalid row with tid: " + newRow.tid);

								int sid = this.getSymbolId(conn, symbolToResolve, providerToResolve);
								if (sid <= 0) {
										throw new HtException(getContext(), "mergeTwoRows", "Cannot resolve symbol ID for  provider: \""
											+ providerToResolve + "\" and symbol: \"" + symbolToResolve + "\"");
								}

								// commit everything to avoid locks
								commitRtThread();
								conn.commit();

								insertTickerData(conn, partitionId, oldRow.rtdata, sid, aflag);
								HtLog.log(Level.INFO, getContext(), "mergeTwoRows", "Inserted new row: " + oldRow.rtdata.toString());
								// alles
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "mergeTwoRows", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private void updateTickerData(
			Connection conn,
			int partitionId,
			HtRtData rtdata,
			long tid,
			long newtime,
			int aflag) throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				// here color and operaion are not applicable
				sql.append("update HISTORY_");
				sql.append(partitionId);
				sql.append(" set ");
				sql.append(" topen=?, tclose=?, thigh=?, tlow=?, topen2=?, tclose2=?, thigh2=?, ");
				sql.append("tlow2=?, tvolume=?, tbid=?, task=?, aflag =?");
				if (newtime > 0) {
						sql.append(" , ttime=?");
				}

				sql.append(" where tid=?");

				try {

						stmt = conn.prepareStatement(sql.toString());

						stmt.setDouble(1, rtdata.open_m);
						stmt.setDouble(2, rtdata.close_m);
						stmt.setDouble(3, rtdata.high_m);
						stmt.setDouble(4, rtdata.low_m);
						stmt.setDouble(5, rtdata.open2_m);
						stmt.setDouble(6, rtdata.close2_m);
						stmt.setDouble(7, rtdata.high2_m);
						stmt.setDouble(8, rtdata.low2_m);
						stmt.setDouble(9, rtdata.volume_m);
						stmt.setDouble(10, rtdata.bid_m);
						stmt.setDouble(11, rtdata.ask_m);
						stmt.setDouble(12, aflag);

						if (newtime > 0) {

								stmt.setLong(13, newtime);
								stmt.setLong(14, tid);
						} else {
								stmt.setLong(13, tid);
						}

						stmt.execute();

				} catch (MySQLIntegrityConstraintViolationException icv) {
						// for the special cases we need to intercept this
						HtLog.log(Level.WARNING, getContext(), "updateTickerData", "Dublicate keys found on update: Ticker: \""
							+ rtdata.toString() + "\", tid=" + tid + ", exception: \"" + icv.getMessage() + "\", partition ID: \"" + partitionId + "\"" + "\nSQL:\n"
							+ "update HISTORY_" + partitionId + " set " + "', topen=" + rtdata.open_m + ", tclose=" + rtdata.close_m
							+ ", thigh=" + rtdata.high_m + ", tlow=" + rtdata.low_m
							+ ", topen2=" + rtdata.open2_m + ", tclose2=" + rtdata.close2_m + ", thigh2=" + rtdata.high2_m + ", tlow2=" + rtdata.low2_m
							+ ", tvolume=" + rtdata.volume_m + ", tbid=" + rtdata.bid_m + ", task=" + rtdata.ask_m
							+ ", aflag =" + aflag + ", ttime=" + newtime + " /*" + HtDateTimeUtils.time2String_Gmt(newtime) + "*/  where tid=" + tid);

						throw new MySQLIntegrityConstraintViolationException(icv.getMessage());
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "updateTickerData", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage()
							+ "\"" + " tid=" + tid + ", failing ticker: \"" + rtdata.toString() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private File createTempFileForBulkInsert(List<HtRtData> rtdatalist, int sid, int aflag) throws Exception {
				File f = File.createTempFile("happy_trader_import", ".DAT");

				BufferedWriter fwriter = null;

				try {
						fwriter = new BufferedWriter(new FileWriter(f));

						for (int i = 0; i < rtdatalist.size(); i++) {
								HtRtData data_i = rtdatalist.get(i);
								fwriter.write("\"");
								fwriter.write(String.valueOf(sid));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.time_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.open_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.high_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.low_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.close_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.open2_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.high2_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.low2_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.close2_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.volume_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.bid_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.ask_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.type_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(aflag));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.color_m));
								fwriter.write("\",\"");
								fwriter.write(String.valueOf(data_i.operation_m));
								fwriter.write("\"\n");
						}
				} catch (Throwable e) {
						throw new HtException(getContext(), "createTempFileForBulkInsert", e);
				} finally {
						try {
								fwriter.close();
						} catch (Throwable e) {
						}
				}

				return f;

		}

		private void insertTickerDataBulk(Connection conn, int partitionId, List<HtRtData> rtdatalist, int sid, int aflag) throws Exception {
				Statement stmt = null;

				String sql = "";

				try {

						File f = createTempFileForBulkInsert(rtdatalist, sid, aflag);
						String fpath = f.getAbsolutePath().replaceAll("\\\\", "/");

						sql = "LOAD DATA LOCAL INFILE '" + fpath + "' REPLACE into table HISTORY_" + partitionId
							+ " fields terminated by ',' ENCLOSED BY '\"' lines terminated by '\n' "
							+ "(tsymbol_id, ttime, topen, thigh, tlow, tclose, topen2, thigh2, tlow2, tclose2, tvolume, tbid, task, ttype, aflag, color, operation )";

						stmt = conn.createStatement();
						int inserted = stmt.executeUpdate(sql);
						f.delete();

						HtLog.log(Level.INFO, getContext(), "insertTickerDataBulk", "Bulk insert processed: " + inserted + " record(s)");

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertTickerDataBulk", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");

				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		private void insertTickerData(Connection conn, int partitionId, HtRtData rtdata, int sid, int aflag)
			throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				String sql = "insert into HISTORY_" + partitionId + " ("
					+ "tsymbol_id, ttime, topen, tclose, thigh, tlow, topen2, tclose2, thigh2, tlow2, tvolume, tbid, task, ttype, aflag, color, operation ) values "
					+ "( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

				try {

						stmt = conn.prepareStatement(sql);

						stmt.setInt(1, sid);

						stmt.setLong(2, rtdata.time_m);
						stmt.setDouble(3, rtdata.open_m);
						stmt.setDouble(4, rtdata.close_m);
						stmt.setDouble(5, rtdata.high_m);
						stmt.setDouble(6, rtdata.low_m);

						stmt.setDouble(7, rtdata.open2_m);
						stmt.setDouble(8, rtdata.close2_m);
						stmt.setDouble(9, rtdata.high2_m);
						stmt.setDouble(10, rtdata.low2_m);

						stmt.setDouble(11, rtdata.volume_m);
						stmt.setDouble(12, rtdata.ask_m);
						stmt.setDouble(13, rtdata.bid_m);
						stmt.setInt(14, rtdata.type_m);

						// aggregation flag
						stmt.setInt(15, aflag);
						stmt.setInt(16, rtdata.color_m);
						stmt.setInt(17, rtdata.operation_m);

						stmt.execute();

				} catch (MySQLIntegrityConstraintViolationException icv) {
						// for the special cases we need to intercept this
						HtLog.log(Level.FINE, getContext(), "insertTickerData", "Dublicate keys found on insert: Ticker: \""
							+ rtdata.toString() + "\", tsymbol_id=" + sid + ", exception: \"" + icv.getMessage() + "\", profile ID: \"" + partitionId + "\"" + "\nSQL:\n"
							+ "insert into HISTORY_" + partitionId + " ("
							+ "tsymbol_id, ttime, topen, tclose, thigh, tlow, topen2, tclose2, thigh2, tlow2, tvolume, tbid, task, ttype, aflag ) values "
							+ "( " + sid + ", " + HtDateTimeUtils.time2String_Gmt(rtdata.time_m) + ", " + rtdata.open_m + ", " + rtdata.close_m + ", " + rtdata.high_m + ", " + rtdata.low_m
							+ ", " + rtdata.open2_m + ", " + rtdata.close2_m + ", " + rtdata.high2_m + ", " + rtdata.low2_m + ", " + rtdata.volume_m
							+ ", " + rtdata.bid_m + ", " + rtdata.ask_m + ", " + rtdata.type_m + ", " + aflag + ", '" + "')");

						throw new MySQLIntegrityConstraintViolationException(icv.getMessage());
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertTickerData", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\""
							+ "\"" + " sid=" + sid + ", failing ticker: \"" + rtdata.toString() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

		}

		// perform initialization and / or aggregation
		private void aggregateRt_UpdateWithLaterTime(HtLastDataRow dataRow, HtRtData newRtdata) throws Exception {

				// common
				if (newRtdata.volume_m > 0 && dataRow.rtdata.volume_m > 0) {
						dataRow.rtdata.volume_m += newRtdata.volume_m;
				}

				// new period has begun
				if (newRtdata.getType() == HtRtData.HST_Type) {
						// agregate
						if (newRtdata.close_m > 0 && dataRow.rtdata.close_m > 0) {
								dataRow.rtdata.close_m = newRtdata.close_m;
						}

						if (dataRow.rtdata.high_m < newRtdata.high_m && newRtdata.high_m > 0 && dataRow.rtdata.high_m > 0) {
								dataRow.rtdata.high_m = newRtdata.high_m;
						}

						if (dataRow.rtdata.low_m > newRtdata.low_m && newRtdata.low_m > 0 && dataRow.rtdata.low_m > 0) {
								dataRow.rtdata.low_m = newRtdata.low_m;
						}

						////
						if (newRtdata.close2_m > 0 && dataRow.rtdata.close2_m > 0) {
								dataRow.rtdata.close2_m = newRtdata.close2_m;
						}

						if (dataRow.rtdata.high2_m < newRtdata.high2_m && newRtdata.high2_m > 0 && dataRow.rtdata.high2_m > 0) {
								dataRow.rtdata.high2_m = newRtdata.high2_m;
						}

						if (dataRow.rtdata.low2_m > newRtdata.low2_m && newRtdata.low2_m > 0 && dataRow.rtdata.low2_m > 0) {
								dataRow.rtdata.low2_m = newRtdata.low2_m;
						}

				} else if (newRtdata.getType() == HtRtData.RT_Type) {
						// ask
						if (newRtdata.ask_m > 0 && dataRow.rtdata.close_m > 0) {
								dataRow.rtdata.close_m = newRtdata.ask_m;
						}

						if (dataRow.rtdata.high_m < newRtdata.ask_m && newRtdata.ask_m > 0 && dataRow.rtdata.high_m > 0) {
								dataRow.rtdata.high_m = newRtdata.ask_m;
						}

						if (dataRow.rtdata.low_m > newRtdata.ask_m && newRtdata.ask_m > 0 && dataRow.rtdata.low_m > 0) {
								dataRow.rtdata.low_m = newRtdata.ask_m;
						}

						// bid
						if (newRtdata.bid_m > 0 && dataRow.rtdata.close2_m > 0) {
								dataRow.rtdata.close2_m = newRtdata.bid_m;
						}

						if (dataRow.rtdata.high2_m < newRtdata.bid_m && newRtdata.bid_m > 0 && dataRow.rtdata.high2_m > 0) {
								dataRow.rtdata.high2_m = newRtdata.bid_m;
						}

						if (dataRow.rtdata.low2_m > newRtdata.bid_m && newRtdata.bid_m > 0 && dataRow.rtdata.low2_m > 0) {
								dataRow.rtdata.low2_m = newRtdata.bid_m;
						}

						// color here is not applicable
				}

		}

		private void aggregateRt_NewPeriod(HtLastDataRow dataRow, HtRtData newRtdata) {
				// this is only history data
				dataRow.rtdata.type_m = HtRtData.HST_Type;

				dataRow.rtdata.setSymbol(newRtdata.getSymbol());

				if (newRtdata.volume_m > 0) {
						dataRow.rtdata.volume_m = newRtdata.volume_m;
				}

				dataRow.rtdata.time_m = newRtdata.time_m;

				// new period has begun
				if (newRtdata.getType() == HtRtData.HST_Type) {
						dataRow.rtdata.open_m = newRtdata.open_m;
						dataRow.rtdata.high_m = newRtdata.high_m;
						dataRow.rtdata.low_m = newRtdata.low_m;
						dataRow.rtdata.close_m = newRtdata.close_m;

						dataRow.rtdata.open2_m = newRtdata.open2_m;
						dataRow.rtdata.high2_m = newRtdata.high2_m;
						dataRow.rtdata.low2_m = newRtdata.low2_m;
						dataRow.rtdata.close2_m = newRtdata.close2_m;

				} else if (newRtdata.getType() == HtRtData.RT_Type) {
						// rt data are expected to have 2 storages - bid & ask
						dataRow.rtdata.open_m = newRtdata.ask_m;
						dataRow.rtdata.high_m = newRtdata.ask_m;
						dataRow.rtdata.low_m = newRtdata.ask_m;
						dataRow.rtdata.close_m = newRtdata.ask_m;

						dataRow.rtdata.open2_m = newRtdata.bid_m;
						dataRow.rtdata.high2_m = newRtdata.bid_m;
						dataRow.rtdata.low2_m = newRtdata.bid_m;
						dataRow.rtdata.close2_m = newRtdata.bid_m;

				}
		}

		// returns symbol id from the table or -1 if no symbol
		private int getSymbolId(Connection conn, String symbol, String provider) throws Exception {

				//StringBuilder key = new StringBuilder(symbol);
				//key.append( provider );
				String key = symbol + provider;
				Integer sid = cachedSymbol_m.get(key);

				if (sid != null) {
						return sid;
				}

				PreparedStatement stmt = null;

				StringBuilder sql = new StringBuilder();
				sql.append("select tsymbol_id from SYMBOLS where tsymbol=? and tprovider=?");

				try {
						stmt = conn.prepareStatement(sql.toString());
						stmt.setString(1, symbol);
						stmt.setString(2, provider);
						ResultSet rs = stmt.executeQuery();

						if (rs.next()) {
								int sid2 = rs.getInt(1);
								cachedSymbol_m.put(key.toString(), sid2);

								return sid2;
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getSymbolId", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return -1;
		}

		private void insertNewSymbol(Connection conn, String symbol, String provider) throws Exception {
				PreparedStatement stmt = null;
				StringBuilder sql = new StringBuilder();

				sql.append("insert into SYMBOLS (tsymbol,tprovider) values (?,?)");

				try {
						stmt = conn.prepareStatement(sql.toString());
						stmt.setString(1, symbol);
						stmt.setString(2, provider);

						stmt.execute();
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "insertNewSymbol", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}
		}

		private long getTidFromLevel1Table(
			Connection conn,
			int partitionId,
			long ttime,
			long sid) throws Exception {
				long tid = -1;
				//String pn = checkProfileName(profilename);

				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				sql.append("select tid from LEVEL1_");
				sql.append(partitionId);
				sql.append(" where tsymbol_id = ? and ttime=?");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, sid);
						stmt.setLong(2, ttime);

						ResultSet rs = stmt.executeQuery();

						if (rs.next()) {
								tid = rs.getLong(1);
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getTidFromLevel1Table", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return tid;
		}

		private long getTidFromHistoryTable(
			Connection conn,
			int partitionId,
			long ttime,
			long sid) throws Exception {
				long tid = -1;
				//String pn = checkProfileName(profilename);

				StringBuilder sql = new StringBuilder();
				PreparedStatement stmt = null;

				sql.append("select tid from HISTORY_");
				sql.append(partitionId);
				sql.append(" where tsymbol_id = ? and ttime=?");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, sid);
						stmt.setLong(2, ttime);

						ResultSet rs = stmt.executeQuery();

						if (rs.next()) {
								tid = rs.getLong(1);
						}

				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getTidFromHistoryTable", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return tid;

		}

		;

	/*
	 * Event helper functions
	 */
	private HtLastDataRow getArbitraryRow(
			Connection conn,
			int partitionId,
			long tid) throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				HtRtData data = new HtRtData();

				int ret_id = -1;
				int aflag = -1;
				//long last_created = -1;
				//long last_updated = -1;

				sql.append(" select sp2.tsymbol,  hp2.ttime, hp2.topen, hp2.thigh, hp2.tlow, hp2.tclose, hp2.topen2, ");
				sql.append("  hp2.thigh2, hp2.tlow2, hp2.tclose2, hp2.tvolume, hp2.tbid, hp2.task, hp2.ttype, hp2.tid, hp2.aflag, sp2.tprovider");
				sql.append("  from HISTORY_");
				sql.append(partitionId);
				sql.append(" hp2, SYMBOLS sp2 ");
				sql.append("  where hp2.tsymbol_id = sp2.tsymbol_id and ");
				sql.append("  hp2.tid=? ");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setLong(1, tid);

						ResultSet rs = stmt.executeQuery();
						if (rs.next()) {
								data.setSymbol(rs.getString(1));
								data.time_m = rs.getLong(2);
								data.open_m = rs.getDouble(3);
								data.high_m = rs.getDouble(4);
								data.low_m = rs.getDouble(5);
								data.close_m = rs.getDouble(6);

								data.open2_m = rs.getDouble(7);
								data.high2_m = rs.getDouble(8);
								data.low2_m = rs.getDouble(9);
								data.close2_m = rs.getDouble(10);

								data.volume_m = rs.getDouble(11);
								data.bid_m = rs.getDouble(12);
								data.ask_m = rs.getDouble(13);
								data.type_m = rs.getInt(14);

								ret_id = rs.getInt(15);
								aflag = rs.getInt(16);

								data.setProvider(rs.getString(17));
						}
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getArbitraryRow", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return new HtLastDataRow(ret_id, data, aflag);

		}

		private HtLastDataRow getArbitraryRow(
			Connection conn,
			int partitionId,
			String symbol,
			String provider,
			long ntime) throws Exception {
				PreparedStatement stmt = null;

				//String pn = checkProfileName(profilename);
				StringBuilder sql = new StringBuilder();

				HtRtData data = new HtRtData();
				data.setProvider(provider);

				int ret_id = -1;
				int aflag = -1;
				//long last_created = -1;
				//long last_updated = -1;

				sql.append("  SELECT hp2.ttime, hp2.topen, hp2.thigh, hp2.tlow, hp2.tclose, hp2.topen2, ");
				sql.append("  hp2.thigh2, hp2.tlow2, hp2.tclose2, hp2.tvolume, hp2.tbid, hp2.task, hp2.ttype, hp2.tid, hp2.aflag ");
				sql.append("  from HISTORY_");
				sql.append(partitionId);
				sql.append(" hp2 ");
				sql.append("  where hp2.ttime=? and ");
				sql.append("  hp2.tsymbol_id = ( select sp2.tsymbol_id from SYMBOLS sp2 where sp2.tsymbol=? and sp2.tprovider=? ) ");

				try {
						stmt = conn.prepareStatement(sql.toString());
						stmt.setLong(1, ntime);
						stmt.setString(2, symbol);
						stmt.setString(3, provider);

						ResultSet rs = stmt.executeQuery();
						if (rs.next()) {
								data.setSymbol(symbol);
								data.time_m = rs.getLong(1);
								data.open_m = rs.getDouble(2);
								data.high_m = rs.getDouble(3);
								data.low_m = rs.getDouble(4);
								data.close_m = rs.getDouble(5);

								data.open2_m = rs.getDouble(6);
								data.high2_m = rs.getDouble(7);
								data.low2_m = rs.getDouble(8);
								data.close2_m = rs.getDouble(9);

								data.volume_m = rs.getDouble(10);
								data.bid_m = rs.getDouble(11);
								data.ask_m = rs.getDouble(12);
								data.type_m = rs.getInt(13);

								ret_id = rs.getInt(14);
								aflag = rs.getInt(15);

								data.setProvider(provider);
						}
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getArbitraryRow", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return new HtLastDataRow(ret_id, data, aflag);

		}

		private HtLastDataRow getLastHtRtDataRow(Connection conn, int partitionId, String symbol, String provider) throws Exception {
				PreparedStatement stmt = null;

				StringBuilder sql = new StringBuilder();

				HtRtData data = new HtRtData();
				data.setProvider(provider);

				int ret_id = -1;
				int aflag = -1;
				long ttime1 = -1;
				long ttime2 = -1;

				sql.append(" select sp2.tsymbol,  hp2.ttime, hp2.topen, hp2.thigh, hp2.tlow, hp2.tclose, hp2.topen2, ");
				sql.append("  hp2.thigh2, hp2.tlow2, hp2.tclose2, hp2.tvolume, hp2.tbid, hp2.task, hp2.ttype, hp2.tid, hp2.aflag ");
				sql.append("  from HISTORY_");
				sql.append(partitionId);
				sql.append(" hp2, SYMBOLS sp2 ");
				sql.append("  where hp2.tsymbol_id = sp2.tsymbol_id and ");
				sql.append("  sp2.tsymbol = ? and ");
				sql.append("  sp2.tprovider = ? and ");
				sql.append("  hp2.ttime = ( ");
				sql.append("    select max(hp.ttime) from HISTORY_");
				sql.append(partitionId);
				sql.append(" hp ");
				sql.append(" 	where hp.tsymbol_id = ( select tsymbol_id from SYMBOLS sp where ");
				sql.append("    sp.tsymbol = ? and  sp.tprovider = ? ) )");

				try {
						stmt = conn.prepareStatement(sql.toString());

						stmt.setString(1, symbol);
						stmt.setString(2, provider);
						stmt.setString(3, symbol);
						stmt.setString(4, provider);

						ResultSet rs = stmt.executeQuery();
						if (rs.next()) {
								data.setSymbol(rs.getString(1));
								data.time_m = rs.getLong(2);
								data.open_m = rs.getDouble(3);
								data.high_m = rs.getDouble(4);
								data.low_m = rs.getDouble(5);
								data.close_m = rs.getDouble(6);

								data.open2_m = rs.getDouble(7);
								data.high2_m = rs.getDouble(8);
								data.low2_m = rs.getDouble(9);
								data.close2_m = rs.getDouble(10);

								data.volume_m = rs.getDouble(11);
								data.bid_m = rs.getDouble(12);
								data.ask_m = rs.getDouble(13);
								data.type_m = rs.getInt(14);

								ret_id = rs.getInt(15);
								aflag = rs.getInt(16);

						}
				} catch (SQLException sqe) {
						throw new HtException(getContext(), "getLastHtRtDataRow", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
				} catch (Throwable e) {
						throw new Exception(e);
				} finally {
						HtDbUtils.silentlyCloseStatement(stmt);

				}

				return new HtLastDataRow(ret_id, data, aflag);

		}
};
