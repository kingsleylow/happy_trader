/*
 * RtDatabaseSelectionManager.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.hlpstruct.BrokerMetaInfo;
import com.fin.httrader.hlpstruct.CloseReason;
import com.fin.httrader.hlpstruct.HistogramEntry;

import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.hlpstruct.PosType;
import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.hlpstruct.ProviderSymbolInfoEntry;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;

import com.fin.httrader.interfaces.*;
import com.fin.httrader.utils.HtDbUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.*;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtBrokerDialogData;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtOrderResponseEntry;
import java.sql.Blob;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.*;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok The class provides selection functionality from
 * database. It manages the connection pool as all the transaction are readonly
 * and theoriticall do not negatively affect DB
 */
public class RtDatabaseSelectionManager implements RtManagerBase {

	static private RtDatabaseSelectionManager instance_m = null;

	// --------------------------------------------------
	static public RtDatabaseSelectionManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtDatabaseSelectionManager();
		}

		return instance_m;
	}

	static public RtDatabaseSelectionManager instance() {
		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	/**
	 * Creates a new instance of RtDatabaseSelectionManager
	 */
	private RtDatabaseSelectionManager() throws Exception {

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	/**
	 */
	public static Map<String, String> getInformationData(Connection conn) throws Exception {
		TreeMap<String, String> result = new TreeMap<String, String>();

		String sql = "select 'VERSION', version()  from dual";
		PreparedStatement stmt = null;


		try {
			stmt = conn.prepareStatement(sql);


			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				result.put(rs.getString(1), rs.getString(2));
			}

		} catch (SQLException sqe) {
			throw new HtException("RtDatabaseSelectionManager", "getInformationData", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			HtDbUtils.silentlyCloseStatement(stmt);
		}

		return result;
	}

	public Set<String> resolveRunNamesViaSession(String sessionId) throws Exception {
		TreeSet<String> runNames = new TreeSet<String>();

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select distinct bso.run_name ");
		sql.append("from BROKER_SESSIONS_OPER bso, BROKER_RUNNAMES b where bso.RUNNAME_ID=b.ID and bso.session_id=?");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, sessionId);

			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				runNames.add(rs.getString(1));
			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "resolveRunNamesViaSession", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);

		}

		return runNames;
	}

	public Set<String> resolveSessionIdsViaRunName(String run_name) throws Exception {
		TreeSet<String> sessions = new TreeSet<String>();

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select distinct bso.session_id ");
		sql.append("from BROKER_SESSIONS_OPER bso, BROKER_RUNNAMES b where bso.RUNNAME_ID=b.ID and b.run_name=?");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, run_name);

			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				sessions.add(rs.getString(1));
			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "resolveSessionIdsViaRunId", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

		return sessions;
	}

	//return session -run is map
	public Map<String, Set<String>> resolveSessionRunIdsViaRunIds(Set<String> runIds) throws Exception {
		int MAX_CHUNK = 20;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		// result
		TreeMap<String, Set<String>> sessionRunIdsMap = new TreeMap<String, Set<String>>();

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;



		Set<String> sRunIds = new TreeSet<String>();
		sRunIds.addAll(runIds);

		Set<String> runIdsChunk = new TreeSet<String>();

		while (sRunIds.size() > 0) {
			runIdsChunk.clear();
			sql.setLength(0);



			sql.append("select distinct b1.session_id, br1.run_name  ");
			sql.append(" from BROKER_SESSIONS_OPER b1, BROKER_RUNNAMES br1 where b1.RUNNAME_ID=br1.ID and ");


			int cnt = 0;
			for (Iterator<String> it = sRunIds.iterator(); it.hasNext();) {

				String runId_i = it.next();
				runIdsChunk.add(runId_i);

				if (++cnt > MAX_CHUNK) {
					break;
				}
			}
			//

			// remove added runIds
			sRunIds.removeAll(runIdsChunk);
			sql.append("( ");
			for (int i = 1; i <= runIdsChunk.size(); i++) {
				sql.append("br1.run_name=? ");
				if (i < runIdsChunk.size()) {
					sql.append(" or ");
				} else {
					sql.append(" ) ");
				}
			}



			// issue SQL
			cnt = 0;
			try {
				stmt = conn.prepareStatement(sql.toString());
				for (Iterator<String> it = runIdsChunk.iterator(); it.hasNext();) {

					stmt.setString(++cnt, it.next());
				}

				ResultSet rs = stmt.executeQuery();

				while (rs.next()) {
					String session_i = rs.getString(1);
					String runid_i = rs.getString(2);


					//
					if (sessionRunIdsMap.containsKey(session_i)) {
						Set<String> runIdsSet = sessionRunIdsMap.get(session_i);
						runIdsSet.add(runid_i);
					} else {
						Set<String> runIdsSet = new TreeSet<String>();
						runIdsSet.add(runid_i);

						sessionRunIdsMap.put(session_i, runIdsSet);


					}
				}

				HtDbUtils.silentlyCloseStatement(stmt);

			} catch (SQLException sqe) {
				HtDbUtils.silentlyCloseConnection(conn);

				throw new HtException(getContext(), "resolveSessionRunIdsViaRunIds", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
			} catch (Throwable e) {
				HtDbUtils.silentlyCloseConnection(conn);
				throw new Exception(e);
			}
			//



		} // end of main loop

		// close statement
		HtDbUtils.silentlyCloseConnection(conn);

		return sessionRunIdsMap;
	}

	/*
	 * //return session -run is map public TreeMap<String, Set<String>>
	 * resolveSessionRunIdsViaRunIds(Set<String> runIds) throws Exception { int
	 * MAX_CHUNK = 20;
	 *
	 * Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
	 *
	 * // result TreeMap<String, Set<String>> sessionRunIdsMap = new
	 * TreeMap<String, Set<String>>();
	 *
	 * StringBuilder sql = new StringBuilder(); PreparedStatement stmt = null;
	 *
	 *
	 *
	 * Set<String> sRunIds = new TreeSet<String>(); sRunIds.addAll(runIds);
	 *
	 * Set<String> runIdsChunk = new TreeSet<String>();
	 *
	 * while (sRunIds.size() > 0) { runIdsChunk.clear(); sql.setLength(0);
	 *
	 *
	 * sql.append(" select distinct b.session_id, br.run_name from
	 * BROKER_SESSIONS_OPER b, BROKER_RUNNAMES br, "); sql.append(" (select
	 * distinct b1.session_id "); sql.append(" from BROKER_SESSIONS_OPER b1,
	 * BROKER_RUNNAMES br1 where b1.RUNNAME_ID=br1.ID and ");
	 *
	 *
	 * int cnt = 0; for (Iterator<String> it = sRunIds.iterator(); it.hasNext();)
	 * {
	 *
	 * String runId_i = it.next(); runIdsChunk.add(runId_i);
	 *
	 * if (++cnt > MAX_CHUNK) { break; } } //
	 *
	 * // remove added runIds sRunIds.removeAll(runIdsChunk); sql.append("( ");
	 * for (int i = 1; i <= runIdsChunk.size(); i++) { sql.append("br1.run_name=?
	 * "); if (i < runIdsChunk.size()) { sql.append(" or "); } else { sql.append("
	 * ) "); } }
	 *
	 * sql.append(" ) a "); sql.append(" where b.RUNNAME_ID=br.ID and
	 * a.session_id=b.session_id ");
	 *
	 * // issue SQL cnt = 0; try { stmt = conn.prepareStatement(sql.toString());
	 * for (Iterator<String> it = runIdsChunk.iterator(); it.hasNext();) {
	 *
	 * stmt.setString(++cnt, it.next()); }
	 *
	 * ResultSet rs = stmt.executeQuery();
	 *
	 * while (rs.next()) { String session_i = rs.getString(1); String runid_i =
	 * rs.getString(2);
	 *
	 *
	 * // if (sessionRunIdsMap.containsKey(session_i)) { Set<String> runIdsSet =
	 * sessionRunIdsMap.get(session_i); runIdsSet.add(runid_i); } else {
	 * Set<String> runIdsSet = new TreeSet<String>(); runIdsSet.add(runid_i);
	 *
	 * sessionRunIdsMap.put(session_i, runIdsSet);
	 *
	 *
	 * }
	 * }
	 *
	 * HtDbUtils.silentlyCloseStatement(stmt);
	 *
	 * } catch (SQLException sqe) { HtDbUtils.silentlyCloseConnection(conn);
	 *
	 * throw new HtException(getContext(), "resolveSessionRunIdsViaRunIds", "SQL
	 * exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() +
	 * "\""); } catch (Throwable e) { HtDbUtils.silentlyCloseConnection(conn);
	 * throw new Exception(e); } //
	 *
	 *
	 *
	 * } // end of main loop
	 *
	 * // close statement HtDbUtils.silentlyCloseConnection(conn);
	 *
	 * return sessionRunIdsMap; }
	 *
	 */
	public Map<String, Set<String>> resolveSessionRunIdsViaSession(Set<String> sessions) throws Exception {
		int MAX_CHUNK = 20;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		// result
		TreeMap<String, Set<String>> sessionRunIdsMap = new TreeMap<String, Set<String>>();

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;



		Set<String> sSessions = new TreeSet<String>();
		sSessions.addAll(sessions);

		Set<String> sSessionsChunk = new TreeSet<String>();

		while (sSessions.size() > 0) {
			sSessionsChunk.clear();
			sql.setLength(0);



			sql.append(" select distinct bso.session_id, b.run_name ");
			sql.append(" from BROKER_SESSIONS_OPER bso, BROKER_RUNNAMES b where bso.RUNNAME_ID=b.ID and  ");


			int cnt = 0;
			for (Iterator<String> it = sSessions.iterator(); it.hasNext();) {

				String session_i = it.next();
				sSessionsChunk.add(session_i);

				if (++cnt > MAX_CHUNK) {
					break;
				}
			}
			//

			// remove added runIds
			sSessions.removeAll(sSessionsChunk);
			sql.append("( ");
			for (int i = 1; i <= sSessionsChunk.size(); i++) {
				sql.append("bso.session_id=? ");
				if (i < sSessionsChunk.size()) {
					sql.append(" or ");
				} else {
					sql.append(" ) ");
				}
			}



			// issue SQL
			cnt = 0;
			try {
				stmt = conn.prepareStatement(sql.toString());
				for (Iterator<String> it = sSessionsChunk.iterator(); it.hasNext();) {

					stmt.setString(++cnt, it.next());
				}

				ResultSet rs = stmt.executeQuery();

				while (rs.next()) {
					String session_i = rs.getString(1);
					String runid_i = rs.getString(2);


					//
					if (sessionRunIdsMap.containsKey(session_i)) {
						Set<String> runIdsSet = sessionRunIdsMap.get(session_i);
						runIdsSet.add(runid_i);
					} else {
						Set<String> runIdsSet = new TreeSet<String>();
						runIdsSet.add(runid_i);

						sessionRunIdsMap.put(session_i, runIdsSet);


					}
				}

				HtDbUtils.silentlyCloseStatement(stmt);

			} catch (SQLException sqe) {
				HtDbUtils.silentlyCloseConnection(conn);

				throw new HtException(getContext(), "resolveSessionRunIdsViaSession", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
			} catch (Throwable e) {
				HtDbUtils.silentlyCloseConnection(conn);
				throw new Exception(e);
			}
			//



		} // end of main loop

		// close statement
		HtDbUtils.silentlyCloseConnection(conn);

		return sessionRunIdsMap;
	}

	/**
	 */
	public void getBrokerDetailRunNamesDataPage(
					String run_name_filter,
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select bso.session_id 'Session ID', ");
		sql.append("b.run_name 'Run Name', ");
		sql.append("bso.alg_brk_pair 'Algorithm Broker Pair', ");
		sql.append("bso.thread 'Thread', ");
		sql.append("bso.op_time 'Operation Time', ");
		sql.append("bso.operation 'Operation' ");
		sql.append("from BROKER_SESSIONS_OPER bso, BROKER_RUNNAMES b where bso.RUNNAME_ID=b.ID  ");


		if (run_name_filter != null && run_name_filter.length() > 0) {
			if (run_name_filter.indexOf("%") >= 0) {
				sql.append(" and b.run_name like ? ");
			} else {
				sql.append(" and b.run_name = ? ");
			}
		}

		if (begDate > 0) {
			sql.append(" and bso.op_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and bso.op_time <=? ");
		}


		sql.append(" order by bso.op_time");
		sql.append(" limit ?, ? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			//
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);


			if (run_name_filter != null && run_name_filter.length() > 0) {
				stmt.setString(++cnt, run_name_filter);
			}

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}


			// bind window

			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();
			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= md.getColumnCount(); i++) {
						newRow.setValue(i, rs.getString(i));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//

				dataselector.onSelectionFinish(cntr);

			}



		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getBrokerDetailRunNamesDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// it is not selection but logically here
	public void updateRunNameTableWithUserData(
					Map<String, String> dataMap) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			sql.append("update BROKER_RUNNAMES set user_data=? where run_name=?");
			stmt = conn.prepareStatement(sql.toString());

			for (Iterator<String> it = dataMap.keySet().iterator(); it.hasNext();) {
				String runname = it.next();
				String user_data = dataMap.get(runname);

				if (user_data.length() > 512) {
					stmt.setString(1, user_data.substring(0, 511));
				} else {
					stmt.setString(1, user_data);
				}

				stmt.setString(2, runname);
				stmt.execute();
			}

			conn.commit();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "updateRunNameTableWithUserData", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public String getLastBrokerRunName(String profilename) throws Exception {
		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);
		String sql = "select run_name from broker_runnames b where id = (select run_name_id from drawable_" + partId + " where id=(select max(id) from drawable_" + partId + "))";
		PreparedStatement stmt = null;
		String result = null;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		try {
			stmt = conn.prepareStatement(sql);
			ResultSet rs = stmt.executeQuery();

			if (rs.next()) {
				result = rs.getString(1);
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getLastBrokerRunName", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

		return result;
	}

	public void getBrokerRunNamesDataPage_v2(
					long begDate,
					long endDate,
					HtRunNameObjectSelector dataselector)
					throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(
						"SELECT "
						+ "brn.id, brn.run_name, brn.op_first_time, bs.session_id, "
						+ "count_orders( bs.session_id) 'cnt_orders' , count_order_responses(  bs.session_id ) 'cnt_order_resp', "
						+ "count_close_pos(bs.session_id) 'cnt_close_pos' "
						+ "FROM "
						+ "broker_runnames as brn "
						+ "left join "
						+ "( "
						+ "select distinct b1.session_id, br1.run_name "
						+ "from BROKER_SESSIONS_OPER b1, BROKER_RUNNAMES br1 where b1.RUNNAME_ID=br1.ID "
						+ ") as bs "
						+ "on bs.run_name = brn.run_name "
						+ " where 1=1 ");


		if (begDate > 0) {
			sql.append(" and brn.op_first_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and brn.op_first_time <=? ");
		}

		sql.append(" order by brn.id ");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		long cntr = 0;
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}


			// bind window


			ResultSet rs = stmt.executeQuery();
			if (dataselector != null) {


				long prev_id = -1;
				String prev_runName = null;
				long prev_opFirstTime = -1;

				List<HtRunNameEntry.HtSessionEntry> sessionEntries = new ArrayList<HtRunNameEntry.HtSessionEntry>();
				while (rs.next()) {


					String rname = rs.getString("run_name");
					long op_first_time = rs.getLong("op_first_time");
					long id = rs.getLong("id");

					HtRunNameEntry.HtSessionEntry sentry = new HtRunNameEntry.HtSessionEntry(
									rs.getString("session_id"),
									rs.getInt("cnt_orders"),
									rs.getInt("cnt_order_resp"),
									rs.getInt("cnt_close_pos"),
									id);


					if (prev_id >= 0 && prev_id != id) {
						// create new entry
						HtRunNameEntry e = new HtRunNameEntry(prev_runName, prev_opFirstTime);

						e.sessions_m.addAll(sessionEntries);
						sessionEntries.clear();

						dataselector.onRunNameEntryArrived(e);
					}

					sessionEntries.add(sentry);

					cntr++;

					// store
					prev_id = id;
					prev_runName = rname;
					prev_opFirstTime = op_first_time;


				}

				// save last
				HtRunNameEntry e = new HtRunNameEntry(prev_runName, prev_opFirstTime);
				e.sessions_m.addAll(sessionEntries);
				sessionEntries.clear();
				dataselector.onRunNameEntryArrived(e);

				//

				dataselector.onFinish(cntr);

			}



		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getBrokerRunNamesDataPage_v2", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public void getBrokerRunNamesDataPage(
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" SELECT b.run_name 'Run Name' , b.op_first_time 'First Registration Time' , b.comment 'Comment', user_data 'User Data' ");
		sql.append(" FROM BROKER_RUNNAMES b where 1=1");

		if (begDate > 0) {
			sql.append(" and b.op_first_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and b.op_first_time <=? ");
		}

		sql.append(" order by b.op_first_time ");
		sql.append(" limit ?, ? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}


			// bind window

			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();
			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= md.getColumnCount(); i++) {
						newRow.setValue(i, rs.getString(i));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//

				dataselector.onSelectionFinish(cntr);

			}



		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getBrokerRunNamesDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public void getBackgroundJobsDataPage(
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select id 'SEQ_NUM', string_id 'ID', type_id 'Type', start_time 'Start Time' , finish_time 'Finish Time', ");
		sql.append("result_code 'Result Code' , result_string 'Result String' from background_jobs where 1=1 ");
		if (begDate > 0) {
			sql.append(" and start_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and finish_time <=? ");
		}


		sql.append(" order by start_time limit ?, ? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();
			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= md.getColumnCount(); i++) {
						newRow.setValue(i, rs.getString(i));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//

				dataselector.onSelectionFinish(cntr);

			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getBackgroundJobsDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

	}

	public void getBrokerSessionsDataPage(
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select session_id 'Session ID', ");
		sql.append("create_time 'Session Creation Time', ");
		sql.append("connect_string 'Connection String', ");
		sql.append("is_simulation 'Is Simulation' from BROKER_SESSIONS where 1=1 ");


		if (begDate > 0) {
			sql.append(" and create_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and create_time <=? ");
		}


		sql.append(" order by create_time");
		sql.append(" limit ?, ? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			//

			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}


			// bind window

			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();
			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= md.getColumnCount(); i++) {
						newRow.setValue(i, rs.getString(i));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//

				dataselector.onSelectionFinish(cntr);

			}



		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getBrokerSessionsDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// according to the filter retreives the last datetimestamp
	public long getAlertLastDateTime(
					long begDate,
					long endDate,
					String contentFilter,
					String runNameFilter,
					String sessionFilter,
					int priorityFilter) throws Exception {
		// means no rows selected
		long result = -1;

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;



		sql.append(" select max(a.alert_time)  ");
		sql.append(" from alerts a, BROKER_RUNNAMES b where a.runname_id=b.id ");

		if (begDate > 0) {
			sql.append(" and a.alert_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and a.alert_time <=? ");
		}

		if (contentFilter != null && contentFilter.length() > 0) {
			if (contentFilter.indexOf("%") >= 0) {
				sql.append(" and a.data like ? ");
			} else {
				sql.append(" and a.data = ? ");
			}

		}
		;

		if (runNameFilter != null && runNameFilter.length() > 0) {
			if (runNameFilter.indexOf("%") >= 0) {
				sql.append(" and b.run_name like ? ");
			} else {
				sql.append(" and b.run_name = ? ");
			}

		}
		;

		if (sessionFilter != null && sessionFilter.length() > 0) {
			sql.append(" and a.source = ? ");
		}
		;

		if (priorityFilter >= 0) {
			sql.append(" and a.priority <= ? ");
		}

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			stmt = conn.prepareStatement(sql.toString());

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			if (contentFilter != null && contentFilter.length() > 0) {
				stmt.setString(++cnt, contentFilter);
			}

			if (runNameFilter != null && runNameFilter.length() > 0) {
				stmt.setString(++cnt, runNameFilter);

			}

			if (sessionFilter != null && sessionFilter.length() > 0) {
				stmt.setString(++cnt, sessionFilter);

			}

			if (priorityFilter >= 0) {
				stmt.setInt(++cnt, priorityFilter);
			}

			ResultSet rs = stmt.executeQuery();

			if (rs.next()) {
				result = rs.getLong(1);
				if (rs.wasNull()) {
					result = -1;
				}
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAlertLastDateTime", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


		return result;
	}

	// it gets the alert data page
	public void getAlertsDataPage(
					long begDate,
					long endDate,
					String contentFilter,
					String runNameFilter,
					String sessionFilter,
					int priorityFilter,
					HtCommonRowSelector dataselector,
					int cutContentLength, // the maximum number of symbols to cat data column, -1 means all
					long pageNum,
					long limitWindow,
					LongParam rowCnt,
					String order_time_value,
					String final_order_time_value) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		if (rowCnt == null) {
			if (final_order_time_value != null) {
				sql.append(" select ar.tid 'Internal ID', ar.id 'Alert Id', ar.run_name 'Run Name', ar.source 'Session ID', ar.alg_brk_pair 'Algorithm Broker Pair', ");
				sql.append(" ar.thread 'Thread', ar.priority 'Priority', ar.alert_time 'Alert Time', ar.small_data 'Small Data',  ");

				if (cutContentLength >= 0) {
					// cut part
					sql.append(" substr(ar.data, 1, ");
					sql.append(cutContentLength);
					sql.append(") 'data' ");
				} else {
					sql.append(" ar.data 'data' ");
				}

				sql.append(" from ( ");
				sql.append(" select a.tid, a.id , b.run_name, a.source , a.alg_brk_pair , ");
				sql.append(" a.thread , a.priority , a.alert_time , a.small_data , a.data ");
			} else {
				sql.append(" select a.tid 'Internal ID', a.id 'Alert Id', b.run_name 'Run Name', a.source 'Session ID', a.alg_brk_pair 'Algorithm Broker Pair', ");
				sql.append(" a.thread 'Thread', a.priority 'Priority', a.alert_time 'Alert Time', a.small_data 'Small Data', ");

				if (cutContentLength >= 0) {
					// cut part
					sql.append(" substr(a.data, 1, ");
					sql.append(cutContentLength);
					sql.append(") 'data' ");
				} else {
					sql.append(" a.data 'data' ");
				}

			}
			sql.append(" from ALERTS a, BROKER_RUNNAMES b where a.runname_id=b.id ");
		} else {
			sql.append(" select count(a.id) from ALERTS a, BROKER_RUNNAMES b where a.runname_id=b.id ");
		}


		if (begDate > 0) {
			sql.append(" and a.alert_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and a.alert_time <=? ");
		}


		if (contentFilter != null && contentFilter.length() > 0) {
			if (contentFilter.indexOf("%") >= 0) {
				sql.append(" and a.data like ? ");
			} else {
				sql.append(" and a.data = ? ");
			}

		}


		if (runNameFilter != null && runNameFilter.length() > 0) {
			if (runNameFilter.indexOf("%") >= 0) {
				sql.append(" and b.run_name like ? ");
			} else {
				sql.append(" and b.run_name = ? ");
			}

		}


		if (sessionFilter != null && sessionFilter.length() > 0) {
			sql.append(" and a.source = ? ");
		}


		if (priorityFilter >= 0) {
			sql.append(" and a.priority <= ? ");
		}

		if (rowCnt == null) {
			sql.append(" order by a.tid ");

			// can be asc or desc
			if (order_time_value != null) {
				sql.append(order_time_value);
			}

			if (pageNum >= 0 && limitWindow >= 0) {
				sql.append(" limit ?, ? ");
			}

			if (final_order_time_value != null) {
				sql.append(") ar order by ar.tid ");
				sql.append(final_order_time_value);
			}
		}

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			//stmt = conn.prepareStatement(sql.toString());
			// no ned to chache as large datasets are expected
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);


			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			if (contentFilter != null && contentFilter.length() > 0) {
				stmt.setString(++cnt, contentFilter);
			}

			if (runNameFilter != null && runNameFilter.length() > 0) {
				stmt.setString(++cnt, runNameFilter);

			}

			if (sessionFilter != null && sessionFilter.length() > 0) {
				stmt.setString(++cnt, sessionFilter);

			}

			if (priorityFilter >= 0) {
				stmt.setInt(++cnt, priorityFilter);
			}

			// bind window
			if (rowCnt == null) {
				if (pageNum >= 0 && limitWindow >= 0) {
					stmt.setLong(++cnt, pageNum);
					stmt.setLong(++cnt, limitWindow);
				}
			}

			ResultSet rs = stmt.executeQuery();

			if (dataselector != null && rowCnt == null) {
				ResultSetMetaData md = rs.getMetaData();

				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}


					dataselector.newRowArrived(newRow);
					cntr++;


				} //end loop

				dataselector.onSelectionFinish(cntr);


			} else {
				// only number
				if (rs.next()) {
					rowCnt.setLong(rs.getLong(1));
				}
			}





		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAlertsDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// get one alert row
	public void getAlertDataRow(
					String idS,
					HtCommonRowSelector dataselector) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select a.id 'Alert Id', b.run_name 'Run Name', a.source 'Session ID', a.alg_brk_pair 'Algorithm Broker Pair', ");
		sql.append(" a.thread 'Thread', a.priority 'Priority', a.alert_time 'Alert Time', a.small_data 'Small Data', a.data 'Alert Data' ");
		sql.append(" from alerts a, BROKER_RUNNAMES b where a.runname_id=b.id and a.id=? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//

			stmt = conn.prepareStatement(sql.toString());

			stmt.setString(1, idS);
			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));
				}

				dataselector.newHeaderArrived(header);

				if (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					newRow.setValue(1, rs.getString(1));
					newRow.setValue(2, rs.getString(2));
					newRow.setValue(3, rs.getString(3));
					newRow.setValue(4, rs.getString(4));
					newRow.setValue(5, String.valueOf(rs.getInt(5)));
					newRow.setValue(6, String.valueOf(rs.getInt(6)));
					newRow.setValue(7, Double.valueOf(rs.getString(7)));
					newRow.setValue(8, rs.getString(8));
					newRow.setValue(9, rs.getString(9));

					dataselector.newRowArrived(newRow);
				} else {
					throw new HtException(getContext(), "getAlertDataRow", "Can't retrieve the the alert entry for id: " + idS);
				}

				//

				dataselector.onSelectionFinish(1);

			}



		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAlertDataRow", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// below functions to query event tables
	public void getCommonLogDataRow(
					long id,
					HtCommonRowSelector dataselector) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select id , tlast_created , logtype , level , thread , session , context , content ");
		sql.append(" from COMMONLOG h where id=? ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			//stmt = conn.prepareStatement(sql.toString(),java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY  );
			//stmt.setFetchSize(Integer.MIN_VALUE);


			stmt = conn.prepareStatement(sql.toString());

			stmt.setLong(1, id);
			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));
				}

				dataselector.newHeaderArrived(header);

				if (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					newRow.setValue(1, String.valueOf(rs.getLong(1)));
					newRow.setValue(2, String.valueOf(rs.getLong(2)));
					newRow.setValue(3, String.valueOf(rs.getInt(3)));
					newRow.setValue(4, String.valueOf(rs.getInt(4)));
					newRow.setValue(5, String.valueOf(rs.getInt(5)));
					newRow.setValue(6, rs.getString(6));
					newRow.setValue(7, rs.getString(7));

					Blob content = rs.getBlob(8);

					if (content.length() > 0) {
						byte[] data = content.getBytes(1, (int) content.length());
						newRow.setValue(8, new String(data, RtDatabaseManager.instance().getClientEncodingString()));
					} else {
						newRow.setValue(8, "");
					}


					dataselector.newRowArrived(newRow);
				} else {
					throw new HtException(getContext(), "getCommonLogDataRow", "Can't retrieve the the alert entry for id: " + id);
				}

				//

				dataselector.onSelectionFinish(1);

			}
			;


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getCommonLogDataRow", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// this is DO_TEXT only for better performanc
	// !!!- ZVV
	public void getDrawableObjectsDataPage_DO_TEXT(
					String profilename,
					List<HtPair<String, String>> provSymMap,
					boolean allIfMapEmpty, // if provSymMap and allIfMapEmpty==true - select all rows
					long begDate,
					long endDate,
					String shortTextFilter,
					String longTextFilter,
					String run_name_filter,
					HtCommonRowSelector dataselector,
					HtDrawableObjectSelector drawableselector,
					long pageNum,
					long limitWindow) throws Exception {
		if (drawableselector != null && dataselector != null) {
			throw new HtException(getContext(), "getDrawableObjectsDataPage", "Only one data selector is allowed");
		}

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);


		sql.append("SELECT br.run_name 'Run_Name', d.id 'Id', s.tprovider 'Provider', s.tsymbol 'Symbol', d.create_ttime 'Create_Time',  d.ttime 'Time', d.name 'Name', ");
		sql.append(" d.priority 'Text_Priority', d.short_text 'Text_Short' , d.text 'Text_Content' ");

		sql.append(" from DRAWABLE_");
		sql.append(partId);
		sql.append(" AS d straight_join BROKER_RUNNAMES AS br straight_join SYMBOLS AS s WHERE d.type=" + HtDrawableObject.DO_TEXT + " and br.id=d.run_name_id and s.tsymbol_id=d.tsymbol_id and d.id < 99999999999");


		if (begDate > 0) {
			sql.append(" and d.create_ttime >=? ");
		}

		if (endDate > 0) {
			sql.append(" and d.create_ttime <=? ");
		}

		if (shortTextFilter != null && shortTextFilter.length() > 0) {
			if (shortTextFilter.indexOf("%") >= 0) {
				sql.append(" and d.short_text like ? ");
			} else {
				sql.append(" and d.short_text = ? ");
			}
		}

		//
		if (longTextFilter != null && longTextFilter.length() > 0) {
			if (longTextFilter.indexOf("%") >= 0) {
				sql.append(" and d.text like ? ");
			} else {
				sql.append(" and d.text = ? ");
			}
		}

		if (run_name_filter != null && run_name_filter.length() > 0) {
			if (run_name_filter.indexOf("%") >= 0) {
				sql.append(" and br.run_name like ? ");
			} else {
				sql.append(" and br.run_name = ? ");
			}
		}


		if (provSymMap.size() > 0) {

			for (int i = 1; i <= provSymMap.size(); i++) {
				if (i == 1) {
					sql.append(" and ( ");
				}

				sql.append(" (s.tsymbol =? and s.tprovider = ? ) ");
				if (i < provSymMap.size()) {
					sql.append(" or ");
				}

				if (i == provSymMap.size()) {
					sql.append(") ");
				}
			}

		} else {
			if (!allIfMapEmpty) {
				sql.append(" and false ");
			}
		}


		sql.append(" order by d.id");

		// if paging is applicable
		if (pageNum >= 0 && limitWindow >= 0) {
			sql.append(" limit ?, ? ");
		}

		// HtLog.log(Level.INFO, getContext(), "getDrawableObjectsDataPage_DO_TEXT", "SQL will be executed:\n" + sql);

		// -------
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			// do not prefetch large amount -
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			if (shortTextFilter != null && shortTextFilter.length() > 0) {
				stmt.setString(++cnt, shortTextFilter);
			}

			if (longTextFilter != null && longTextFilter.length() > 0) {
				stmt.setString(++cnt, longTextFilter);
			}

			if (run_name_filter != null && run_name_filter.length() > 0) {
				stmt.setString(++cnt, run_name_filter);
			}


			for (int i = 0; i < provSymMap.size(); i++) {

				String provider_i = provSymMap.get(i).first;
				String symbol_i = provSymMap.get(i).second;

				stmt.setString(++cnt, symbol_i);
				stmt.setString(++cnt, provider_i);
			}


			if (pageNum >= 0 && limitWindow >= 0) {
				stmt.setLong(++cnt, pageNum);
				stmt.setLong(++cnt, limitWindow);
			}

			// fetch
			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i), md.getColumnType(i));
				}


				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 0; i < header.getSize(); i++) {
						newRow.setValue(i + 1, rs.getString(i + 1));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//
				dataselector.onSelectionFinish(cntr);



			} else if (drawableselector != null) {
				// specific selector
				long cntr = 0;
				while (rs.next()) {
					HtDrawableObject dobj = new HtDrawableObject();

					// common fields
					dobj.setName(rs.getString("Name"));
					dobj.setTime(rs.getLong("Time"));
					dobj.setProvider(rs.getString("Provider"));
					dobj.setSymbol(rs.getString("Symbol"));
					dobj.setRunName(rs.getString("Run_Name"));


					// assume DO_TEXT type only
					HtDrawableObject.Text text_obj = dobj.getAsText(true);

					text_obj.text_m.append(rs.getString("Text_Content"));
					text_obj.shortText_m.append(rs.getString("Text_Short"));
					text_obj.priority_m = rs.getInt("Text_Priority");


					// notify
					drawableselector.onDrawableObjectArrived(dobj);

					cntr++;

				} //next row

				drawableselector.onFinish(cntr);

			}




		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getDrawableObjectsDataPage_DO_TEXT", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public void getDrawableObjectsDataPage(
					String profilename,
					List<HtPair<String, String>> provSymMap,
					boolean allIfMapEmpty, // if provSymMap and allIfMapEmpty==true - select all rows
					long begDate,
					long endDate,
					List<Integer> type, // by or
					String shortTextFilter,
					String longTextFilter,
					String run_name_filter,
					HtCommonRowSelector dataselector,
					HtDrawableObjectSelector drawableselector,
					long pageNum,
					long limitWindow) throws Exception {
		if (drawableselector != null && dataselector != null) {
			throw new HtException(getContext(), "getDrawableObjectsDataPage", "Only one data selector is allowed");
		}

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);


		sql.append("SELECT br.run_name 'Run_Name', d.id 'Id', s.tprovider 'Provider', s.tsymbol 'Symbol', d.create_ttime 'Create_Time',  d.ttime 'Time', d.type 'Type', d.name 'Name', ");

		sql.append(" d.lineindic_name1 'LineIndic_Name1', d.lineindic_color1 'LineIndic_Color1', d.lineindic_value1 'LineIndic_Value1', ");
		sql.append(" d.lineindic_name2 'LineIndic_Name2', d.lineindic_color2 'LineIndic_Color2', d.lineindic_value2 'LineIndic_Value2', ");
		sql.append(" d.lineindic_name3 'LineIndic_Name3', d.lineindic_color3 'LineIndic_Color3', d.lineindic_value3 'LineIndic_Value3', ");
		sql.append(" d.lineindic_name4 'LineIndic_Name4', d.lineindic_color4 'LineIndic_Color4', d.lineindic_value4 'LineIndic_Value4', ");
		sql.append(" d.lineindic_name5 'LineIndic_Name5', d.lineindic_color5 'LineIndic_Color5', d.lineindic_value5 'LineIndic_Value5', ");
		sql.append(" d.lineindic_name6 'LineIndic_Name6', d.lineindic_color6 'LineIndic_Color6', d.lineindic_value6 'LineIndic_Value6', ");
		sql.append(" d.lineindic_name7 'LineIndic_Name7', d.lineindic_color7 'LineIndic_Color7', d.lineindic_value7 'LineIndic_Value7', ");

		sql.append(" d.print_bid 'Print_Bid', d.print_ask 'Print_Ask' , d.print_volume 'Print_Volume', d.print_color 'Print_Color', d.print_operation 'Print_Operation', ");
		sql.append(" d.ohlc_open 'OHLC_Open', d.ohlc_high 'OHLC_High' , d.ohlc_low 'OHLC_Low', d.ohlc_close 'OHLC_Close', d.ohlc_volume 'OHLC_Volume', ");

		sql.append(" d.priority 'Text_Priority', d.short_text 'Text_Short' , d.text 'Text_Content', ");

		// level1
		sql.append(" d.level1_best_bid_price 'Best_Bid_Price', d.level1_best_ask_price 'Best_Ask_Price' , d.level1_best_bid_volume 'Best_Bid_Volume', d.level1_best_ask_volume 'Best_Ask_Volume' ");


		sql.append(" from DRAWABLE_");
		sql.append(partId);

		// probably it will help to avoide filesort which is a nightmare!
		sql.append(" AS d straight_join BROKER_RUNNAMES AS br straight_join SYMBOLS AS s WHERE br.id=d.run_name_id and s.tsymbol_id=d.tsymbol_id and d.id < 99999999999");

		if (begDate > 0) {
			sql.append(" and d.create_ttime >=? ");
		}

		if (endDate > 0) {
			sql.append(" and d.create_ttime <=? ");
		}

		if (shortTextFilter != null && shortTextFilter.length() > 0) {
			if (shortTextFilter.indexOf("%") >= 0) {
				sql.append(" and d.short_text like ? ");
			} else {
				sql.append(" and d.short_text = ? ");
			}
		}

		//
		if (longTextFilter != null && longTextFilter.length() > 0) {
			if (longTextFilter.indexOf("%") >= 0) {
				sql.append(" and d.text like ? ");
			} else {
				sql.append(" and d.text = ? ");
			}
		}

		if (run_name_filter != null && run_name_filter.length() > 0) {
			if (run_name_filter.indexOf("%") >= 0) {
				sql.append(" and br.run_name like ? ");
			} else {
				sql.append(" and br.run_name = ? ");
			}
		}


		if (type.size() > 0) {
			sql.append(" and (");
			for (int i = 1; i <= type.size(); i++) {
				sql.append(" d.type=? ");
				if (i < type.size()) {
					sql.append("or");
				}
			}

			sql.append(") ");
		}


		if (provSymMap.size() > 0) {

			for (int i = 1; i <= provSymMap.size(); i++) {
				if (i == 1) {
					sql.append(" and ( ");
				}

				sql.append(" (s.tsymbol =? and s.tprovider = ? ) ");
				if (i < provSymMap.size()) {
					sql.append(" or ");
				}

				if (i == provSymMap.size()) {
					sql.append(") ");
				}
			}

		} else {
			if (!allIfMapEmpty) {
				sql.append(" and false ");
			}
		}


		sql.append(" order by d.id");

		// if paging is applicable
		if (pageNum >= 0 && limitWindow >= 0) {
			sql.append(" limit ?, ? ");
		}

		// HtLog.log(Level.INFO, getContext(), "getDrawableObjectsDataPage", "SQL will be executed:\n" + sql);

		// -------
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			// do not prefetch large amount -
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			if (shortTextFilter != null && shortTextFilter.length() > 0) {
				stmt.setString(++cnt, shortTextFilter);
			}

			if (longTextFilter != null && longTextFilter.length() > 0) {
				stmt.setString(++cnt, longTextFilter);
			}

			if (run_name_filter != null && run_name_filter.length() > 0) {
				stmt.setString(++cnt, run_name_filter);
			}


			if (type.size() > 0) {
				for (int i = 0; i < type.size(); i++) {
					stmt.setInt(++cnt, type.get(i));
				}
			}

			for (int i = 0; i < provSymMap.size(); i++) {

				String provider_i = provSymMap.get(i).first;
				String symbol_i = provSymMap.get(i).second;

				stmt.setString(++cnt, symbol_i);
				stmt.setString(++cnt, provider_i);
			}


			if (pageNum >= 0 && limitWindow >= 0) {
				stmt.setLong(++cnt, pageNum);
				stmt.setLong(++cnt, limitWindow);
			}

			// fetch
			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i), md.getColumnType(i));
				}


				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {

					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 0; i < header.getSize(); i++) {
						newRow.setValue(i + 1, rs.getString(i + 1));
					}

					dataselector.newRowArrived(newRow);
					cntr++;

				}


				//
				dataselector.onSelectionFinish(cntr);



			} else if (drawableselector != null) {
				// specific selector
				long cntr = 0;
				while (rs.next()) {
					HtDrawableObject dobj = new HtDrawableObject();
					int dobj_type = rs.getInt("Type");

					// common fields
					dobj.setName(rs.getString("Name"));
					dobj.setTime(rs.getLong("Time"));
					dobj.setProvider(rs.getString("Provider"));
					dobj.setSymbol(rs.getString("Symbol"));
					dobj.setRunName(rs.getString("Run_Name"));


					if (dobj_type == HtDrawableObject.DO_LEVEL1) {
						HtDrawableObject.Level1 level1_dobj = dobj.getAsLevel1(true);

						level1_dobj.best_ask_price_m = rs.getDouble("Best_Ask_Price");
						level1_dobj.best_bid_price_m = rs.getDouble("Best_Bid_Price");
						level1_dobj.best_ask_volume_m = rs.getDouble("Best_Ask_Volume");
						level1_dobj.best_bid_volume_m = rs.getDouble("Best_Bid_Volume");
					} else if (dobj_type == HtDrawableObject.DO_LINE_INDIC) {
						List<HtDrawableObject.LineIndicator> lineindic_dobj = dobj.getAsLineIndicator(true);

						String name1 = rs.getString("LineIndic_Name1");
						String name2 = rs.getString("LineIndic_Name2");
						String name3 = rs.getString("LineIndic_Name3");
						String name4 = rs.getString("LineIndic_Name4");
						String name5 = rs.getString("LineIndic_Name5");
						String name6 = rs.getString("LineIndic_Name6");
						String name7 = rs.getString("LineIndic_Name7");

						HtDrawableObject.LineIndicator lineindic_1 = new HtDrawableObject.LineIndicator(name1 != null ? name1 : "", rs.getDouble("LineIndic_Value1"), rs.getInt("LineIndic_Color1"));
						HtDrawableObject.LineIndicator lineindic_2 = new HtDrawableObject.LineIndicator(name2 != null ? name2 : "", rs.getDouble("LineIndic_Value2"), rs.getInt("LineIndic_Color2"));
						HtDrawableObject.LineIndicator lineindic_3 = new HtDrawableObject.LineIndicator(name3 != null ? name3 : "", rs.getDouble("LineIndic_Value3"), rs.getInt("LineIndic_Color3"));
						HtDrawableObject.LineIndicator lineindic_4 = new HtDrawableObject.LineIndicator(name4 != null ? name4 : "", rs.getDouble("LineIndic_Value4"), rs.getInt("LineIndic_Color4"));
						HtDrawableObject.LineIndicator lineindic_5 = new HtDrawableObject.LineIndicator(name5 != null ? name5 : "", rs.getDouble("LineIndic_Value5"), rs.getInt("LineIndic_Color5"));
						HtDrawableObject.LineIndicator lineindic_6 = new HtDrawableObject.LineIndicator(name6 != null ? name6 : "", rs.getDouble("LineIndic_Value6"), rs.getInt("LineIndic_Color6"));
						HtDrawableObject.LineIndicator lineindic_7 = new HtDrawableObject.LineIndicator(name7 != null ? name7 : "", rs.getDouble("LineIndic_Value7"), rs.getInt("LineIndic_Color7"));

						lineindic_dobj.add(lineindic_1);
						lineindic_dobj.add(lineindic_2);
						lineindic_dobj.add(lineindic_3);
						lineindic_dobj.add(lineindic_4);
						lineindic_dobj.add(lineindic_5);
						lineindic_dobj.add(lineindic_6);
						lineindic_dobj.add(lineindic_7);

					} else if (dobj_type == HtDrawableObject.DO_OHLC) {
						HtDrawableObject.OHLC ohcl_obj = dobj.getAsOHLC(true);

						ohcl_obj.close_m = rs.getDouble("OHLC_Close");
						ohcl_obj.high_m = rs.getDouble("OHLC_High");
						ohcl_obj.low_m = rs.getDouble("OHLC_Low");
						ohcl_obj.open_m = rs.getDouble("OHLC_Open");
						ohcl_obj.volume_m = rs.getDouble("OHLC_Volume");

					} else if (dobj_type == HtDrawableObject.DO_PRINT) {
						HtDrawableObject.Print print_obj = dobj.getAsPrint(true);

						print_obj.ask_m = rs.getDouble("Print_Ask");
						print_obj.bid_m = rs.getDouble("Print_Bid");
						print_obj.color_m = rs.getInt("Print_Color");
						print_obj.operation_m = rs.getInt("Print_Operation");
						print_obj.volume_m = rs.getDouble("Print_Volume");


					} else if (dobj_type == HtDrawableObject.DO_TEXT) {
						HtDrawableObject.Text text_obj = dobj.getAsText(true);

						text_obj.text_m.append(rs.getString("Text_Content"));
						text_obj.shortText_m.append(rs.getString("Text_Short"));
						text_obj.priority_m = rs.getInt("Text_Priority");
					}

					// notify
					drawableselector.onDrawableObjectArrived(dobj);

					cntr++;

				} //next row

				drawableselector.onFinish(cntr);

			}




		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getDrawableObjectsDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

	}

	public void getCommonLogDataPage(
					String contextFilter,
					String contentFilter,
					String threadFilter,
					int loglevel,
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					HtCommonLogSelection datalogselector,
					long pageNum,
					long limitWindow,
					int cutContentLength, // cut content length up to symbols, - 1means all
					LongParam rowCnt) throws Exception {

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		if (rowCnt == null) {

			sql.append("select id 'Id', tlast_created 'Created (GMT)', level 'Level', thread 'Thread', session 'Session', context 'Context',  ");
			if (cutContentLength >= 0) {
				// cut part
				sql.append(" substr(content, 1, ");
				sql.append(cutContentLength);
				sql.append(") 'content' ");
			} else {
				sql.append(" content 'content' ");
			}
			sql.append(" from COMMONLOG h where 1=1 ");
		} else {
			sql.append("select count(id) from COMMONLOG h where 1=1 ");
		}

		// context filter
		if (threadFilter != null && threadFilter.length() > 0) {
			if (threadFilter.indexOf("%") >= 0) {
				sql.append(" and thread like ? ");
			} else {
				sql.append(" and thread = ? ");
			}

		}


		// context filter
		if (contextFilter != null && contextFilter.length() > 0) {
			if (contextFilter.indexOf("%") >= 0) {
				sql.append(" and context like ? ");
			} else {
				sql.append(" and context = ? ");
			}

		}


		// content small
		if (contentFilter != null && contentFilter.length() > 0) {
			if (contentFilter.indexOf("%") >= 0) {
				sql.append(" and content like ? ");
			} else {
				sql.append(" and content = ? ");
			}

		}

		//
		if (loglevel > 0) {
			sql.append(" and level <= ?");
		}

		sql.append(" and h.logtype =? ");

		if (begDate > 0) {
			sql.append(" and h.tlast_created >=? ");
		}

		if (endDate > 0) {
			sql.append(" and h.tlast_created <=? ");
		}

		if (rowCnt == null) {
			sql.append(" order by tlast_created");

			// if paging is applicable
			if (pageNum >= 0 && limitWindow >= 0) {
				sql.append(" limit ?, ? ");
			}
		}

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;
		try {
			// do not prefetch large amount - 
			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);

			//stmt = conn.prepareStatement(sql.toString());

			if (threadFilter != null && threadFilter.length() > 0) {
				stmt.setString(++cnt, threadFilter);
			}


			if (contextFilter != null && contextFilter.length() > 0) {
				stmt.setString(++cnt, contextFilter);
			}


			// content small
			if (contentFilter != null && contentFilter.length() > 0) {
				stmt.setString(++cnt, contentFilter);
			}


			if (loglevel > 0) {
				stmt.setInt(++cnt, loglevel);
			}

			stmt.setInt(++cnt, XmlEvent.ET_CommonLog);

			//
			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			// bind window
			if (rowCnt == null) {
				if (pageNum >= 0 && limitWindow >= 0) {
					stmt.setLong(++cnt, pageNum);
					stmt.setLong(++cnt, limitWindow);
				}
			}

			ResultSet rs = stmt.executeQuery();

			// only number
			if (rowCnt != null) {

				if (rs.next()) {
					rowCnt.setLong(rs.getLong(1));
				}


			} else {

				long cntr = 0;
				if (dataselector != null) {

					ResultSetMetaData md = rs.getMetaData();
					HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
					for (int i = 1; i <= md.getColumnCount(); i++) {
						header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

					}

					dataselector.newHeaderArrived(header);

					while (rs.next()) {

						HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

						newRow.setValue(1, String.valueOf(rs.getLong(1)));
						newRow.setValue(2, String.valueOf(rs.getLong(2)));
						newRow.setValue(3, String.valueOf(rs.getInt(3)));
						newRow.setValue(4, String.valueOf(rs.getInt(4)));
						newRow.setValue(5, rs.getString(5));
						newRow.setValue(6, rs.getString(6));

						Blob content = rs.getBlob(7);

						if (content.length() > 0) {
							byte[] data = content.getBytes(1, (int) content.length());
							String datas = new String(data, RtDatabaseManager.instance().getClientEncodingString());

							newRow.setValue(7, datas);
						} else {
							newRow.setValue(7, "");
						}

						dataselector.newRowArrived(newRow);

						cntr++;
					}


					//

					dataselector.onSelectionFinish(cntr);



				} else if (datalogselector != null) {
					while (rs.next()) {
						//public CommonLog(long logThread, String session, int logLevel, long logDate, String context, 	String totalContent)

						Blob content = rs.getBlob("content");
						String contents = new String(content.getBytes(1, (int) content.length()), RtDatabaseManager.instance().getClientEncodingString());

						CommonLog logentry = new CommonLog(
										rs.getLong("Thread"),
										rs.getString("Session"),
										rs.getInt("Level"),
										rs.getLong("Created (GMT)"),
										rs.getString("Context"),
										contents);

						datalogselector.newRowArrived(logentry);
						cntr++;
					}

					datalogselector.onFinish(cntr);
				}
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getCommonLogDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	//the same as following but returns the total number of rows
	public long get_BrokerResponseOrder_Amount(
					Collection<String> sessionIdList) throws Exception {
		long result = -1;
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;
		sql.append(" SELECT count(ors.id) from ORDER_RESPONSES ors  where 1=0 ");
		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				//String sId = sessionIdList.get(i);
				sql.append(" or ors.source = ? ");

			}
		}

		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (Iterator<String> irr = sessionIdList.iterator(); irr.hasNext();) {
					stmt.setString(++cnt, irr.next());
				}
			}

			ResultSet rs = stmt.executeQuery();
			if (rs.next()) {
				result = rs.getLong(1);
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_BrokerResponseOrder_Amount", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

		return result;
	}

	// selects BrokerResponseOrder single data row
	public void get_BrokerResponses_Page(
					List<String> sessionIdList,
					Uid parentOrderUid,
					String brokerOrderId,
					String brokerPositionId,
					long begOrderDate,
					long endOrderDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" SELECT "
						+ "brks.session_id 'Session ID',  "
						+ "brks.create_time 'Session create time', "
						+ "brks.connect_string 'Session connect string', "
						+ "brks.is_simulation 'Is simulation' , "
						+ "ors.id 'Response ID', "
						+ "ors.seq_num 'Sequence Number', "
						+ "ors.op_issue_time 'Issue time', "
						+ "ors.op_time 'Operation time', "
						+ "ors.parent_id 'Parent Order ID', "
						+ "ors.broker_comment 'Broker comment', "
						+ "ors.broker_internal_code 'Broker internal code', "
						+ "ors.result_code 'Broker result code', "
						+ "ors.result_detail_code 'Broker result detail code',  "
						+ "ors.broker_position_id 'Broker position ID', "
						+ "ors.broker_order_id 'Broker Order ID', "
						+ "ors.op_price 'Operation price', "
						+ "ors.op_stop_price 'Operation STOP price', "
						+ "ors.op_tp_price 'Operation TP price', "
						+ "ors.op_sl_price 'Operation SL price',  "
						+ "ors.op_volume 'Operation volume', "
						+ "ors.op_remaining_volume 'Operation remaining volume', "
						+ "ors.provider 'RT provider', "
						+ "ors.symbol 'Symbol', "
						+ "ors.op_expiration_time 'Expiration time', "
						+ "ors.op_validity_flag 'Validity flag',  "
						+ "ors.op_pos_type 'Order Type',  "
						+ "ors.op_comission 'Comissions', "
						+ "ors.context 'Context'");

		sql.append("     FROM ORDER_RESPONSES ors, BROKER_SESSIONS brks where brks.session_id=ors.source ");

		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				//String sId = sessionIdList.get(i);
				if (i == 0) {
					sql.append(" and ( ");
				}

				sql.append(" brks.session_id = ? ");

				if (i < (sessionIdList.size() - 1)) {
					sql.append("or");
				} else if (i == (sessionIdList.size() - 1)) {
					sql.append(" ) ");
				}
			}
		}

		// parent order filter
		if (parentOrderUid != null && parentOrderUid.isValid()) {
			sql.append(" and ors.parent_id=? ");
		}

		if (brokerOrderId != null && brokerOrderId.length() > 0) {
			sql.append(" and ors.broker_order_id=? ");
		}

		if (brokerPositionId != null && brokerPositionId.length() > 0) {
			sql.append(" and ors.broker_position_id=? ");
		}



		if (begOrderDate > 0) {
			sql.append(" and ors.op_issue_time >=? ");
		}

		if (endOrderDate > 0) {
			sql.append(" and ors.op_issue_time <=? ");
		}

		sql.append(" order by ors.seq_num ");

		if (pageNum >= 0 && limitWindow >= 0) {
			sql.append(" limit ?, ? ");
		}

		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (int i = 0; i < sessionIdList.size(); i++) {
					stmt.setString(++cnt, sessionIdList.get(i));
				}

			}

			if (parentOrderUid != null && parentOrderUid.isValid()) {
				stmt.setString(++cnt, parentOrderUid.toString());
			}

			if (brokerOrderId != null && brokerOrderId.length() > 0) {
				stmt.setString(++cnt, brokerOrderId);
			}

			if (brokerPositionId != null && brokerPositionId.length() > 0) {
				stmt.setString(++cnt, brokerPositionId);
			}

			if (begOrderDate > 0) {
				stmt.setLong(++cnt, begOrderDate);
			}

			if (endOrderDate > 0) {
				stmt.setLong(++cnt, endOrderDate);
			}

			// bind window
			if (pageNum >= 0 && limitWindow >= 0) {
				stmt.setLong(++cnt, pageNum);
				stmt.setLong(++cnt, limitWindow);
			}

			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();

				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}


					dataselector.newRowArrived(newRow);
					cntr++;
				}

				dataselector.onSelectionFinish(cntr);
			}





		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_BrokerResponseOrder_Page", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// returns the amount of data retreived by the consequent function
	public long get_OrderData_Amount(
					Collection<String> sessionIdList) throws Exception {
		long result = -1;
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;
		sql.append(" SELECT count(orr.id) from ORDERS orr where 1=0 ");
		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				//String sId = sessionIdList.get(i);
				sql.append(" or orr.source = ? ");

			}
		}

		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (Iterator<String> irr = sessionIdList.iterator(); irr.hasNext();) {
					stmt.setString(++cnt, irr.next());
				}
			}

			ResultSet rs = stmt.executeQuery();
			if (rs.next()) {
				result = rs.getLong(1);
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_OrderData_Amount", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}

		return result;
	}

	/*
	 * public void get_CloseOper_Page( List<String> sessionIdList, long
	 * begOrderDate, long endOrderDate, HtCommonRowSelector dataselector, long
	 * pageNum, long limitWindow ) throws Exception { StringBuilder sql = new
	 * StringBuilder(); PreparedStatement stmt = null;
	 *
	 * sql.append(" SELECT " + "brks.session_id 'Session ID', " +
	 * "brks.create_time 'Session create time', " + "brks.connect_string 'Session
	 * connect string', " + "brks.is_simulation 'Is simulation' , " + "cp.seq_num
	 * 'Sequence Number' , " + "cp.id 'Response ID' , " + "cp.broker_position_id
	 * 'Broker position ID'," + "cp.provider 'RT Provider', " + "cp.symbol
	 * 'Symbol' , " + "cp.broker_comment 'Broker Comment', " +
	 * "cp.broker_internal_code 'Broker Internal Code'," + "cp.result_code 'Result
	 * Code', " + "cp.result_detail_Result Detail Code', " + "cp.broker_issue_time
	 * 'Issue Time', " + "cp.volume 'Volume', " + "cp.trade_direction 'Trade
	 * Direction', " + "cp.price_open 'Price Open', " + "cp.price_close 'Price
	 * Close', " + "cp.pos_open_time 'Position Open Time', " + "cp.pos_close_time
	 * 'Position Close Time', " + "cp.parent_id 'Parent ID'" );
	 *
	 *
	 *
	 *
	 *
	 * sql.append(" FROM CLOSE_POS cp , BROKER_SESSIONS brks where
	 * brks.session_id=orr.source ");
	 *
	 * if (sessionIdList != null && sessionIdList.size() > 0) { for (int i = 0; i
	 * < sessionIdList.size(); i++) { //String sId = sessionIdList.get(i); if (i
	 * == 0) { sql.append(" and ( "); }
	 *
	 * sql.append(" brks.session_id = ? ");
	 *
	 * if (i < (sessionIdList.size() - 1)) { sql.append("or"); } else if (i ==
	 * (sessionIdList.size() - 1)) { sql.append(" ) "); } } }
	 *
	 * if (begOrderDate > 0) { sql.append(" and orr.or_issue_time >=? "); }
	 *
	 * if (endOrderDate > 0) { sql.append(" and orr.or_issue_time <=? "); }
	 *
	 * sql.append(" order by orr.or_issue_time "); sql.append(" limit ?, ? ");
	 *
	 *
	 * int cnt = 0; Connection conn =
	 * RtDatabaseManager.instance().resolveSQLConnection(); try { // stmt =
	 * conn.prepareStatement(sql.toString());
	 *
	 * if (sessionIdList != null && sessionIdList.size() > 0) { for (int i = 0; i
	 * < sessionIdList.size(); i++) { stmt.setString(++cnt, sessionIdList.get(i));
	 * } }
	 *
	 * if (begOrderDate > 0) { stmt.setLong(++cnt, begOrderDate); }
	 *
	 * if (endOrderDate > 0) { stmt.setLong(++cnt, endOrderDate); }
	 *
	 * // bind window stmt.setLong(++cnt, pageNum); stmt.setLong(++cnt,
	 * limitWindow);
	 *
	 * ResultSet rs = stmt.executeQuery();
	 *
	 * if (dataselector != null) { ResultSetMetaData md = rs.getMetaData();
	 * HtCommonRowSelector.CommonHeader header = new
	 * HtCommonRowSelector.CommonHeader(); for (int i = 1; i <=
	 * md.getColumnCount(); i++) { header.addFieldId(md.getColumnName(i),
	 * md.getColumnLabel(i));
	 *
	 * }
	 *
	 * dataselector.newHeaderArrived(header);
	 *
	 * long cntr = 0; while (rs.next()) { HtCommonRowSelector.CommonRow newRow =
	 * new HtCommonRowSelector.CommonRow(header);
	 *
	 * for (int i = 1; i <= header.getSize(); i++) { newRow.setValue(i,
	 * rs.getString(i)); }
	 *
	 *
	 *
	 * dataselector.newRowArrived(newRow);
	 *
	 * }
	 *
	 * dataselector.onSelectionFinish(cntr); }
	 *
	 *
	 *
	 *
	 *
	 * } catch (SQLException sqe) { throw new HtException(getContext(),
	 * "get_CloseOper_Page", "SQL exception in \"" + sql.toString() + "\"
	 * happened: \"" + sqe.getMessage() + "\""); } catch (Throwable e) { throw new
	 * Exception(e); } finally { HtDbUtils.silentlyCloseStatement(stmt);
	 * HtDbUtils.silentlyCloseConnection(conn); } }
	 */
	public void showClosePos_Callback(
					Collection<String> sessionIdList,
					HtCommonRowSelector dataselector) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" SELECT "
						+ "c.seq_num 'Sequence Number', "
						+ "c.source 'Session ID',  "
						+ "c.ID 'ID', "
						+ "c.broker_position_id 'Broker Position ID', "
						+ "c.broker_issue_time 'Broker Issue Time' , "
						+ "c.provider 'RT Provider', "
						+ "c.symbol 'Order symbol' , "
						+ "c.volume 'Order comment', "
						+ "c.trade_direction 'Trade Direction',"
						+ "c.price_open 'Open Price', "
						+ "c.price_close 'Price Close', "
						+ "c.pos_open_time 'Position Open Time', "
						+ "c.pos_close_time 'Position Close Time', "
						+ "c.parent_id 'Parent ID', "
						+ "c.broker_comment 'Broker Comment', "
						+ "c.broker_internal_code 'Broker Internal Code', "
						+ "c.result_code 'Result Code',  "
						+ "c.result_detail_code 'Result Detail Code', "
						+ "c.broker_issue_time 'Broker Issue Time', "
						+ "c.comission_abs 'Absolute Comission', "
						+ "c.comission_pct 'Pct Comission' ");





		sql.append("     FROM CLOSE_POS c where 1=1 ");

		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {

				if (i == 0) {
					sql.append(" and ( ");
				}

				sql.append(" c.source = ? ");

				if (i < (sessionIdList.size() - 1)) {
					sql.append("or");
				} else if (i == (sessionIdList.size() - 1)) {
					sql.append(" ) ");
				}
			}
		}



		sql.append(" order by c.seq_num ");


		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());


			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (Iterator<String> itr = sessionIdList.iterator(); itr.hasNext();) {
					stmt.setString(++cnt, itr.next());
				}
			}


			// bind window

			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}



					dataselector.newRowArrived(newRow);

				}

				dataselector.onSelectionFinish(cntr);
			}





		} catch (SQLException sqe) {
			throw new HtException(getContext(), "showClosePos_Callback", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// select chunk of Order + BrokerResponseOrder data
	public void get_OrderData_Page(
					List<String> sessionIdList,
					long begOrderDate,
					long endOrderDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" SELECT "
						+ "brks.session_id 'Session ID',  "
						+ "brks.create_time 'Session create time', "
						+ "brks.connect_string 'Session connect string', "
						+ "brks.is_simulation 'Is simulation' , "
						+ "orr.id 'Order ID' , "
						+ "orr.seq_num 'Sequence Number' , "
						+ "orr.broker_position_id 'Broker position ID',"
						+ "orr.broker_order_id 'Broker order ID',"
						+ "orr.order_type 'Order type', "
						+ "orr.provider 'RT Provider', "
						+ "orr.symbol 'Order symbol' , "
						+ "orr.comment 'Order comment', "
						+ "orr.or_issue_time 'Order issue time',"
						+ "orr.or_time 'Order open time', "
						+ "orr.or_price 'Order price', "
						+ "orr.or_stop_price 'Order STOP price', "
						+ "orr.or_sl_price 'Order SL price', "
						+ "orr.or_tp_price 'Order TP price', "
						+ "orr.or_volume 'Order volume',  "
						+ "orr.or_expiration_time 'Order expiration time', "
						+ "orr.or_validity_flag 'Order validity', "
						+ "orr.or_place 'Place', "
						+ "orr.or_method 'Method', "
						+ "orr.or_additional_info 'Additional Info',"
						+ "orr.or_price_type 'Price Type', "
						+ "orr.or_stop_price_type 'STOP Price Type', "
						+ "orr.context 'Context'");


		sql.append("     FROM ORDERS orr , BROKER_SESSIONS brks where brks.session_id=orr.source  ");

		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				//String sId = sessionIdList.get(i);
				if (i == 0) {
					sql.append(" and ( ");
				}

				sql.append(" brks.session_id = ? ");

				if (i < (sessionIdList.size() - 1)) {
					sql.append("or");
				} else if (i == (sessionIdList.size() - 1)) {
					sql.append(" ) ");
				}
			}
		}

		if (begOrderDate > 0) {
			sql.append(" and orr.or_issue_time >=? ");
		}

		if (endOrderDate > 0) {
			sql.append(" and orr.or_issue_time <=? ");
		}

		sql.append(" order by orr.seq_num ");
		sql.append(" limit ?, ? ");


		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (int i = 0; i < sessionIdList.size(); i++) {
					stmt.setString(++cnt, sessionIdList.get(i));
				}
			}

			if (begOrderDate > 0) {
				stmt.setLong(++cnt, begOrderDate);
			}

			if (endOrderDate > 0) {
				stmt.setLong(++cnt, endOrderDate);
			}

			// bind window
			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}



					dataselector.newRowArrived(newRow);

				}

				dataselector.onSelectionFinish(cntr);
			}





		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_OrderData_Page", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public void get_Order_Detail_BrokerResponse_Row(
					Uid orderUid,
					HtCommonRowSelector orderDataSelection,
					HtCommonRowSelector brokerDataSelection) throws Exception {
		String sql = null;
		PreparedStatement stmt;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			// first SQL
			if (orderDataSelection != null) {
				sql = " SELECT "
								+ "brks.session_id 'Session ID',  "
								+ "brks.create_time 'Session create time', "
								+ "brks.connect_string 'Session connect string', "
								+ "brks.is_simulation 'Is simulation' , "
								+ "orr.id 'Order ID' , "
								+ "orr.seq_num 'Sequence Number' , "
								+ "orr.broker_position_id 'Broker position ID',"
								+ "orr.broker_order_id 'Broker order ID',"
								+ "orr.order_type 'Order type', "
								+ "orr.provider 'RT Provider', "
								+ "orr.symbol 'Order symbol' , "
								+ "orr.comment 'Order comment', "
								+ "orr.or_issue_time 'Order issue time',"
								+ "orr.or_time 'Order open time', "
								+ "orr.or_price 'Order price', "
								+ "orr.or_stop_price 'Order STOP price', "
								+ "orr.or_sl_price 'Order SL price', "
								+ "orr.or_tp_price 'Order TP price', "
								+ "orr.or_volume 'Order volume',  "
								+ "orr.or_expiration_time 'Order expiration time', "
								+ "orr.or_validity_flag 'Order validity', "
								+ "orr.or_place 'Place', "
								+ "orr.or_method 'Method', "
								+ "orr.or_additional_info 'Additional Info',"
								+ "orr.or_price_type 'Price Type', "
								+ "orr.or_stop_price_type 'STOP Price Type',"
								+ "orr.context 'Context' "
								+ "     FROM ORDERS orr , BROKER_SESSIONS brks where brks.session_id=orr.source and orr.id=? ";






				stmt = conn.prepareStatement(sql.toString());
				stmt.setString(1, orderUid.toString());

				ResultSet rs = stmt.executeQuery();

				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				orderDataSelection.newHeaderArrived(header);

				if (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}



					orderDataSelection.newRowArrived(newRow);
				}


				HtDbUtils.silentlyCloseStatement(stmt);
			}

			if (brokerDataSelection != null) {

				// now extract broker detail page

				sql = " SELECT "
								+ "brks.session_id 'Session ID',  "
								+ "brks.create_time 'Session create time', "
								+ "brks.connect_string 'Session connect string', "
								+ "brks.is_simulation 'Is simulation' , "
								+ "ors.id 'Response ID', "
								+ "ors.seq_num 'Sequence Number' , "
								+ "ors.op_issue_time 'Issue time', "
								+ "ors.op_time 'Operation time', "
								+ "ors.parent_id 'Parent Order ID', "
								+ "ors.broker_comment 'Broker comment', "
								+ "ors.broker_internal_code 'Broker internal code', "
								+ "ors.result_code 'Broker result code', "
								+ "ors.result_detail_code 'Broker result detail code',  "
								+ "ors.broker_position_id 'Broker position ID', "
								+ "ors.broker_order_id 'Broker Order ID', "
								+ "ors.op_price 'Operation price', "
								+ "ors.op_stop_price 'Operation STOP price', "
								+ "ors.op_tp_price 'Operation TP price', "
								+ "ors.op_sl_price 'Operation SL price',  "
								+ "ors.op_volume 'Operation volume', "
								+ "op_remaining_volume 'Operation remaining volume', "
								+ "ors.provider 'RT provider', "
								+ "ors.symbol 'Symbol', "
								+ "ors.op_expiration_time 'Expiration time', "
								+ "ors.op_validity_flag 'Validity flag',  "
								+ "ors.op_pos_type 'Order Type',  "
								+ "ors.op_comission 'Comissions', "
								+ "ors.context 'Context'"
								+ " FROM ORDER_RESPONSES ors, BROKER_SESSIONS brks where brks.session_id=ors.source and ors.parent_id=? "
								+ " order by ors.seq_num";




				stmt = conn.prepareStatement(sql.toString());
				stmt.setString(1, orderUid.toString());

				ResultSet rs = stmt.executeQuery();


				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				brokerDataSelection.newHeaderArrived(header);

				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}



					brokerDataSelection.newRowArrived(newRow);

				}

				HtDbUtils.silentlyCloseStatement(stmt);
			}




		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_Order_Detail_BrokerResponse_Row", "SQL exception in \"" + sql + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			HtDbUtils.silentlyCloseConnection(conn);
		}

	}

	public void getOrdersAndResponsesTree(
					Collection<String> sessionIdList,
					HtBrokerDialogData rdata) throws Exception 
	{

		//
		
		//
		rdata.clear();
		PreparedStatement stmt = null;
		String sql = "";

		//if (sessionIdList.isEmpty())
		//	throw new HtException(getContext(), "getOrdersAndResponsesTree", "List of sessions must be valid!");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		// first - get all orders
		sql = " SELECT "
						+ "brks.session_id,  "
						+ "orr.id, "
						+ "orr.seq_num , "
						+ "orr.order_type, "
						+ "orr.provider, "
						+ "orr.symbol, "
						+ "orr.or_issue_time,"
						+ "orr.or_time"
						+ " FROM ORDERS orr, BROKER_SESSIONS brks where brks.session_id=orr.source  ";

		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				
				if (i == 0) {
					sql += " and ( ";
				}

				sql += " brks.session_id = ? ";

				if (i < (sessionIdList.size() - 1)) {
					sql += " or";
				} else if (i == (sessionIdList.size() - 1)) {
					sql += " ) ";
				}
			}
		}

		sql += " order by orr.seq_num ";

		int cnt = 0;
		try {
			//
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for(Iterator<String> irr = sessionIdList.iterator(); irr.hasNext(); ){
					stmt.setString(++cnt, irr.next());
				}
			}

			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				rdata.addOrderEntry(
					new Uid(rs.getString("id")), 
					new Uid(rs.getString("session_id")), 
					rs.getInt("seq_num"),
					rs.getInt("order_type"), 
					rs.getString("symbol"), 
					rs.getString("provider"), 
					rs.getLong("or_issue_time"),
					rs.getLong("or_time")
				);
			}

			HtDbUtils.silentlyCloseStatement(stmt);
			////
			// now need to get all responses with valid parents

			sql = " SELECT "
							+ "brks.session_id,  "
							+ "ors.id, "
							+ "orr.seq_num 'parent_seq_num', "
							+ "ors.seq_num, "
							+ "ors.op_issue_time, "
							+ "ors.op_time, "
							+ "ors.parent_id, "
							+ "ors.provider, "
							+ "ors.symbol, "
							+ "ors.result_code, "
							+ "ors.result_detail_code, "
							+ "ors.op_pos_type 'order_type'";

			sql += "  FROM ORDER_RESPONSES ors, ORDERS orr,  BROKER_SESSIONS brks where brks.session_id=ors.source";
			sql += " and ors.parent_id = orr.id";

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (int i = 0; i < sessionIdList.size(); i++) {
				
					if (i == 0) {
						sql += " and ( ";
					}

					sql += " brks.session_id = ? ";

					if (i < (sessionIdList.size() - 1)) {
						sql += " or";
					} else if (i == (sessionIdList.size() - 1)) {
						sql += " ) ";
					}
				}
			}

			sql += " order by ors.seq_num ";
			cnt  =0;
			
			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for(Iterator<String> irr = sessionIdList.iterator(); irr.hasNext(); ){
					stmt.setString(++cnt, irr.next());
				}
			}
			
			rs = stmt.executeQuery();
			while (rs.next()) {
				
				rdata.addResponseEntryWithChildRef(
								new Uid(rs.getString("id")), 
								new Uid(rs.getString("session_id")), 
								rs.getInt("seq_num"),
								rs.getInt("order_type"), 
								rs.getString("symbol"), 
								rs.getString("provider"), 
								rs.getLong("op_issue_time"),
								rs.getLong("op_time"),
								new Uid(rs.getString("parent_id")), 
								rs.getInt("parent_seq_num"),
								rs.getInt("result_code"),
								rs.getInt("result_detail_code")
				);
				
			}
			HtDbUtils.silentlyCloseStatement(stmt);
			
			// next to get parentless order responses
			sql = " SELECT "
							+ "brks.session_id,  "
							+ "ors.id, "
							+ "ors.seq_num, "
							+ "ors.op_issue_time, "
							+ "ors.op_time, "
							+ "ors.provider, "
							+ "ors.symbol, "
							+ "ors.result_code, "
							+ "ors.result_detail_code, "
							+ "ors.op_pos_type 'order_type'";

			sql += "  FROM ORDER_RESPONSES ors, BROKER_SESSIONS brks where brks.session_id=ors.source and ors.parent_id=''";
			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (int i = 0; i < sessionIdList.size(); i++) {
					
					if (i == 0) {
						sql += " and ( ";
					}

					sql += " brks.session_id = ? ";

					if (i < (sessionIdList.size() - 1)) {
						sql += " or";
					} else if (i == (sessionIdList.size() - 1)) {
						sql += " ) ";
					}
				}
			}

			sql += " order by ors.seq_num ";
			
			cnt  =0;
			stmt = conn.prepareStatement(sql.toString());
			
			if (sessionIdList != null && sessionIdList.size() > 0) {
				for(Iterator<String> irr = sessionIdList.iterator(); irr.hasNext(); ){
					stmt.setString(++cnt, irr.next());
				}
			}
			
			rs = stmt.executeQuery();
			while (rs.next()) {
				
				rdata.addResponseEntryWithoutChildRef(
								new Uid(rs.getString("id")), new
								Uid(rs.getString("session_id")), 
								rs.getInt("seq_num"),
								rs.getInt("order_type"), 
								rs.getString("symbol"), 
								rs.getString("provider"), 
								rs.getLong("op_issue_time"),
								rs.getLong("op_time"),
								rs.getInt("result_code"),
								rs.getInt("result_detail_code")
				);
								
				
			}
			
			rdata.doFinish();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getOrdersAndResponsesTree", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public void get_BrokerResponse_Parent_Order_Row(
					Uid responseUid,
					HtCommonRowSelector orderDataSelection,
					HtCommonRowSelector brokerDataSelection) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		try {
			// first SQL
			sql.append(" SELECT "
							+ "brks.session_id 'Session ID',  "
							+ "brks.create_time 'Session create time', "
							+ "brks.connect_string 'Session connect string', "
							+ "brks.is_simulation 'Is simulation' , "
							+ "ors.id 'Response ID', "
							+ "ors.seq_num 'Sequence Number' , "
							+ "ors.op_issue_time 'Issue time', "
							+ "ors.op_time 'Operation time', "
							+ "ors.parent_id 'Parent Order ID', "
							+ "ors.broker_comment 'Broker comment', "
							+ "ors.broker_internal_code 'Broker internal code', "
							+ "ors.result_code 'Broker result code', "
							+ "ors.result_detail_code 'Broker result detail code',  "
							+ "ors.broker_position_id 'Broker position ID', "
							+ "ors.broker_order_id 'Broker Order ID', "
							+ "ors.op_price 'Operation price', "
							+ "ors.op_stop_price 'Operation STOP price', "
							+ "ors.op_tp_price 'Operation TP price', "
							+ "ors.op_sl_price 'Operation SL price',  "
							+ "ors.op_volume 'Operation volume', "
							+ "op_remaining_volume 'Operation remaining volume', "
							+ "ors.provider 'RT provider', "
							+ "ors.symbol 'Symbol', "
							+ "ors.op_expiration_time 'Expiration time', "
							+ "ors.op_validity_flag 'Validity flag',  "
							+ "ors.op_pos_type 'Order Type',  "
							+ "ors.op_comission 'Comissions', "
							+ "ors.context 'Context'");

			sql.append("     FROM ORDER_RESPONSES ors, BROKER_SESSIONS brks where brks.session_id=ors.source and ors.id=? ");

			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, responseUid.toString());

			ResultSet rs = stmt.executeQuery();
			Uid parentUid = new Uid();
			if (brokerDataSelection != null) {
				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				brokerDataSelection.newHeaderArrived(header);


				if (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					// UID
					parentUid.fromString(rs.getString(header.getFieldId("parent_id")));

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}


					brokerDataSelection.newRowArrived(newRow);
				}
			}
			// find parent order
			HtDbUtils.silentlyCloseStatement(stmt);


			// if no parent order do not extract it
			if (parentUid.isValid()) {


				//
				sql.setLength(0);
				sql.append(" SELECT "
								+ "brks.session_id 'Session ID',  "
								+ "brks.create_time 'Session create time', "
								+ "brks.connect_string 'Session connect string', "
								+ "brks.is_simulation 'Is simulation' , "
								+ "orr.id 'Order ID' , "
								+ "orr.seq_num 'Sequence Number' , "
								+ "orr.broker_position_id 'Broker position ID',"
								+ "orr.broker_order_id 'Broker order ID',"
								+ "orr.order_type 'Order type', "
								+ "orr.provider 'RT Provider', "
								+ "orr.symbol 'Order symbol' , "
								+ "orr.comment 'Order comment', "
								+ "orr.or_issue_time 'Order issue time',"
								+ "orr.or_time 'Order open time', "
								+ "orr.or_price 'Order price', "
								+ "orr.or_stop_price 'Order STOP price', "
								+ "orr.or_sl_price 'Order SL price', "
								+ "orr.or_tp_price 'Order TP price', "
								+ "orr.or_volume 'Order volume',  "
								+ "orr.or_expiration_time 'Order expiration time', "
								+ "orr.or_validity_flag 'Order validity', "
								+ "orr.or_place 'Place', "
								+ "orr.or_method 'Method', "
								+ "orr.or_additional_info 'Additional Info',"
								+ "orr.or_price_type 'Price Type', "
								+ "orr.or_stop_price_type 'STOP Price Type', "
								+ "orr.context 'Context'"
								+ " FROM ORDERS orr , BROKER_SESSIONS brks where brks.session_id=orr.source and orr.id=? ");

				stmt = conn.prepareStatement(sql.toString());
				stmt.setString(1, parentUid.toString());

				rs = stmt.executeQuery();
				if (orderDataSelection != null) {
					ResultSetMetaData md = rs.getMetaData();
					HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
					for (int i = 1; i <= md.getColumnCount(); i++) {
						header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

					}

					orderDataSelection.newHeaderArrived(header);

					if (rs.next()) {
						HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

						for (int i = 1; i <= header.getSize(); i++) {
							newRow.setValue(i, rs.getString(i));
						}



						orderDataSelection.newRowArrived(newRow);
					}
					;
				}
				;
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "get_BrokerResponse_Parent_Order_Row", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// returns historycal data
	//
	public void getHistoricalData_NoPaging(
					String profilename,
					List<HtPair<String, String>> provSymMap,
					long begDate,
					long endDate,
					HtDataSelection dataselector,
					long rowsNotifyCount,
					boolean allIfMapEmpty // if provSymMap and allIfMapEmpty==true - select all rows
					) throws Exception {

		//HtLog.log(Level.INFO, getContext(), "getHistoricalData_NoPaging", "Historical data will be read");


		//String pn = RtDatabaseManager.instance().checkProfileName(profilename);
		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" select h.ttime,");
		sql.append("			(select tsymbol from SYMBOLS where tsymbol_id=h.tsymbol_id) as tsymbol, ");
		sql.append("			(select tprovider from SYMBOLS where tsymbol_id=h.tsymbol_id) as tprovider, ");
		sql.append("h.ttype, h.aflag, ");
		sql.append("     h.tbid, h.task, ");
		sql.append("     h.topen, h.thigh, h.tlow, h.tclose, ");
		sql.append("     h.topen2, h.thigh2, h.tlow2, h.tclose2, h.tvolume, h.tid, h.color, h.operation ");
		sql.append(" from HISTORY_");
		sql.append(partId);
		sql.append(" h force index(HISTORY_").append(partId).append("_U1)");
		sql.append(" where 1=1 ");

		if (begDate > 0) {
			sql.append(" and h.ttime >=? ");
		}


		if (endDate > 0) {
			sql.append(" and h.ttime <=? ");
		}

		// symbols and providers

		if (provSymMap != null && provSymMap.size() > 0) {
			sql.append(" and h.tsymbol_id in ( select tsymbol_id from SYMBOLS s where ");

			for (int i = 1; i <= provSymMap.size(); i++) {

				sql.append(" (s.tsymbol =? and s.tprovider = ? ) ");
				if (i < provSymMap.size()) {
					sql.append(" or ");
				}
			}

			sql.append(" ) ");

		} else {

			if (!allIfMapEmpty) {
				sql.append(" and false ");
			}
		}



		sql.append(" order by h.ttime, h.tsymbol_id");

		//HtLog.log(Level.INFO, getContext(), "getHistoricalData_NoPaging", "SQL will be executed:\n" + sql);


		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {

			stmt = conn.prepareStatement(sql.toString(), java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
			stmt.setFetchSize(Integer.MIN_VALUE);



			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}



			if (provSymMap != null) {
				for (int i = 0; i < provSymMap.size(); i++) {

					String provider_i = provSymMap.get(i).first;
					String symbol_i = provSymMap.get(i).second;

					stmt.setString(++cnt, symbol_i);
					stmt.setString(++cnt, provider_i);
				}
			}



			ResultSet rs = stmt.executeQuery();


			long cntr = 0;
			if (dataselector != null) {
				while (rs.next()) {
					cntr++;

					dataselector.newRowArrived(
									rs.getLong(1),
									rs.getString(2),
									rs.getString(3),
									rs.getInt(4),
									rs.getInt(5),
									rs.getDouble(6),
									rs.getDouble(7),
									rs.getDouble(8),
									rs.getDouble(9),
									rs.getDouble(10),
									rs.getDouble(11),
									rs.getDouble(12),
									rs.getDouble(13),
									rs.getDouble(14),
									rs.getDouble(15),
									rs.getDouble(16),
									rs.getLong(17),
									rs.getInt(18),
									rs.getInt(19));

					if (rowsNotifyCount > 0) {
						if ((cntr % rowsNotifyCount) == 0) {
							dataselector.onSelectionFinish(cntr);
						}
					}



				} // end while


				dataselector.onSelectionFinish(cntr);
			}



			HtLog.log(Level.INFO, getContext(), "getHistoricalData_NoPaging", "Fetched all the rows: " + cntr);

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getHistoricalData_NoPaging", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// the same as the next fun but returns only one page
	public void getHistoricalData_Paging(
					String profilename,
					List<HtPair<String, String>> provSymMap,
					boolean allIfMapEmpty, // if provSymMap and allIfMapEmpty==true - select all rows
					long begDate,
					long endDate,
					HtDataSelection dataselector,
					long pageNum,
					long pageLimitNumber) throws Exception {
		if (pageLimitNumber <= 0) {
			throw new HtException(getContext(), "getHistoricalDataPage", "Limit window must exceed zero");
		}

		// prepare SQL
		//String pn = RtDatabaseManager.instance().checkProfileName(profilename);
		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);

		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append(" select h.ttime, ");
		sql.append("			(select tsymbol from SYMBOLS where tsymbol_id=h.tsymbol_id) as tsymbol, ");
		sql.append("			(select tprovider from SYMBOLS where tsymbol_id=h.tsymbol_id) as tprovider, ");
		sql.append("h.ttype, h.aflag, ");
		sql.append("     h.tbid, h.task, ");
		sql.append("     h.topen, h.thigh, h.tlow, h.tclose, ");
		sql.append("     h.topen2, h.thigh2, h.tlow2, h.tclose2, h.tvolume, h.tid, h.color, h.operation ");
		sql.append(" from HISTORY_");
		sql.append(partId);
		sql.append(" h force index(HISTORY_").append(partId).append("_U1)");
		sql.append(" where 1=1 ");

		if (begDate > 0) {
			sql.append(" and h.ttime >=? ");
		}
		if (endDate > 0) {
			sql.append(" and h.ttime <=? ");
		}

		// symbols and providers

		if (provSymMap != null && provSymMap.size() > 0) {

			sql.append(" and h.tsymbol_id in ( select tsymbol_id from SYMBOLS s where ");

			for (int i = 1; i <= provSymMap.size(); i++) {

				sql.append(" (s.tsymbol =? and s.tprovider = ? ) ");
				if (i < provSymMap.size()) {
					sql.append(" or ");
				}
			}

			sql.append(" ) ");

		} else {
			if (!allIfMapEmpty) {
				sql.append(" and false ");
			}
		}

		sql.append(" order by h.ttime, h.tsymbol_id ");
		sql.append(" limit ?, ? ");

		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {

			stmt = conn.prepareStatement(sql.toString());

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}


			if (provSymMap != null) {
				for (int i = 0; i < provSymMap.size(); i++) {

					String provider_i = provSymMap.get(i).first;
					String symbol_i = provSymMap.get(i).second;

					stmt.setString(++cnt, symbol_i);
					stmt.setString(++cnt, provider_i);
				}
			}


			// bind
			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, pageLimitNumber);
			//cnt-=2;

			// execute

			//HtLog.log(Level.INFO, getContext(), "getHistoricalData_Paging", "SQL will be executed: " + sql);

			ResultSet rs = stmt.executeQuery();
			long cntr = 0;
			if (dataselector != null) {
				while (rs.next()) {
					cntr++;

					dataselector.newRowArrived(
									rs.getLong(1),
									rs.getString(2),
									rs.getString(3),
									rs.getInt(4),
									rs.getInt(5),
									rs.getDouble(6),
									rs.getDouble(7),
									rs.getDouble(8),
									rs.getDouble(9),
									rs.getDouble(10),
									rs.getDouble(11),
									rs.getDouble(12),
									rs.getDouble(13),
									rs.getDouble(14),
									rs.getDouble(15),
									rs.getDouble(16),
									rs.getLong(17),
									rs.getInt(18),
									rs.getInt(19));

				} // end fetch loop

				dataselector.onSelectionFinish(cntr);
			}
			



			HtLog.log(Level.INFO, getContext(), "getHistoricalDataPage", "Fetched chunk of the rows: " + cntr);

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getHistoricalDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	public Set<String> getStoredSymbolsForProvider(String provider) throws Exception {
		if (provider == null || provider.length() <= 0) {
			throw new HtException(getContext(), "getStoredSymbolsForProvider", "Invalid provider");
		}

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		Set<String> result = new TreeSet<String>();

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("select tsymbol from SYMBOLS where tprovider=?");

		try {

			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, provider);

			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {
				result.add(rs.getString(1));
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredSymbolsForProvider", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}


		return result;
	}

	public ArrayList<ProviderSymbolInfoEntry> getStoredSymbols_ForProfile_WithDates(String profileName) throws Exception {
		ArrayList<ProviderSymbolInfoEntry> provSymMap = new ArrayList<ProviderSymbolInfoEntry>();

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profileName);

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append("select s.tsymbol, s.tprovider, b.max_t, b.min_t from symbols s, ");
		sql.append("(");
		sql.append("select max(h.ttime) 'max_t' , min(h.ttime) 'min_t', h.tsymbol_id from history_");
		sql.append(partId);
		sql.append(" h where h.tsymbol_id in ");
		sql.append("( ");
		sql.append("select distinct tsymbol_id from history_");
		sql.append(partId);
		sql.append(" ) ");
		sql.append("group by h.tsymbol_id ");
		sql.append(") b ");
		sql.append("where s.tsymbol_id = b.tsymbol_id");



		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				ProviderSymbolInfoEntry entry_i = new ProviderSymbolInfoEntry(rs.getString(2), rs.getString(1), rs.getLong(4), rs.getLong(3));
				provSymMap.add(entry_i);
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredSymbols_ForProfile_WithDates", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}



		return provSymMap;
	}

	public Set<String> getStoredProviders_ForProfile_FromHistory(String profileName) throws Exception {

		Set<String> result = new HashSet<String>();

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int partId = RtDatabaseManager.instance().getProfilePartitionId(profileName);

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append("SELECT distinct s.tprovider FROM symbols s where s.tsymbol_id in (select distinct tsymbol_id ");
		sql.append("FROM history_");
		sql.append(partId);
		sql.append(")");

		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				result.add(rs.getString(1));
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredProviders_ForProfile_FromHistory", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}



		return result;

	}

	public List<HtPair<String, String>> getStoredSymbols_ForProfile_FromHistory(String profileName) throws Exception {
		ArrayList<HtPair<String, String>> provSymMap = new ArrayList<HtPair<String, String>>();
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profileName);

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append("SELECT s.tprovider, s.tsymbol FROM symbols s where s.tsymbol_id in (select distinct tsymbol_id ");
		sql.append("FROM history_");
		sql.append(partId);
		sql.append(")");



		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				provSymMap.add(new HtPair<String, String>(rs.getString(1), rs.getString(2)));
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredSymbols_ForProfile_FromHistory", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);

			HtDbUtils.silentlyCloseConnection(conn);
		}


		return provSymMap;
	}

	public List<HtPair<String, String>> getStoredSymbols_ForProfile_FromDrawable(String profileName) throws Exception {
		ArrayList<HtPair<String, String>> provSymMap = new ArrayList<HtPair<String, String>>();
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profileName);

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append("SELECT s.tprovider, s.tsymbol FROM symbols s where s.tsymbol_id in (select distinct tsymbol_id ");
		sql.append("FROM drawable_");
		sql.append(partId);
		sql.append(")");



		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				provSymMap.add(new HtPair<String, String>(rs.getString(1), rs.getString(2)));
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredSymbols_ForProfile_FromDrawable", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


		return provSymMap;
	}

	// returns all the symbols
	public List<HtPair<String, String>> getStoredSymbols() throws Exception {

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		List<HtPair<String, String>> provSymMap = new ArrayList<HtPair<String, String>>();

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append(" select tprovider, tsymbol from SYMBOLS order by 1");


		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				provSymMap.add(new HtPair<String, String>(rs.getString(1), rs.getString(2)));
			}
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getStoredSymbols", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


		return provSymMap;

	}

	public Set<String> getAvailableProviders() throws Exception {
		Set<String> providers = new TreeSet<String>();

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append(" select distinct tprovider from SYMBOLS");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {

			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {
				providers.add(rs.getString(1));
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAvailableProviders", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


		return providers;
	}

	//
	public void getExportImportLogDataPage(
					int groupid,
					long begDate,
					long endDate,
					HtCommonRowSelector dataselector,
					long pageNum,
					long limitWindow) throws Exception {
		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;

		sql.append("select h.id, h.groupid, h.tlast_created, h.opertype , h.operstatus , h.opermessage , b.tsymbol , h.tdata ");
		sql.append(" from EXPORTIMPORTLOG h, SYMBOLS b where b.tsymbol_id=h.symbol_id ");

		sql.append(" and h.groupid =? ");

		if (begDate > 0) {
			sql.append(" and h.ttime >=? ");
		}

		if (endDate > 0) {
			sql.append(" and h.ttime <=? ");
		}

		sql.append(" order by tlast_created");
		sql.append(" limit ?, ? ");

		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {

			stmt = conn.prepareStatement(sql.toString());


			stmt.setInt(++cnt, groupid);

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}

			// bind window
			stmt.setLong(++cnt, pageNum);
			stmt.setLong(++cnt, limitWindow);

			ResultSet rs = stmt.executeQuery();

			if (dataselector != null) {

				ResultSetMetaData md = rs.getMetaData();
				HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
				for (int i = 1; i <= md.getColumnCount(); i++) {
					header.addFieldId(md.getColumnName(i), md.getColumnLabel(i));

				}

				dataselector.newHeaderArrived(header);

				long cntr = 0;
				while (rs.next()) {
					HtCommonRowSelector.CommonRow newRow = new HtCommonRowSelector.CommonRow(header);

					for (int i = 1; i <= header.getSize(); i++) {
						newRow.setValue(i, rs.getString(i));
					}

					dataselector.newRowArrived(newRow);
					cntr++;
				}


				//

				dataselector.onSelectionFinish(cntr);

			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getExportImportLogDataPage", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
	}

	// returns the first and the last dates in DB
	public void getLoadedMinMaxDates(
					String profilename,
					String symbol,
					String provider,
					LongParam minDate,
					LongParam maxDate) throws Exception {
		minDate.setLong(-1);
		maxDate.setLong(-1);

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		int partId = RtDatabaseManager.instance().getProfilePartitionId(profilename);

		sql.append(" select hp2.ttime ");
		sql.append("  from HISTORY_");
		sql.append(partId);
		sql.append(" hp2, SYMBOLS sp2 ");
		sql.append("  where hp2.tsymbol_id = sp2.tsymbol_id and ");
		sql.append("  sp2.tsymbol = ? and ");
		sql.append("  sp2.tprovider = ? and ");
		sql.append("  hp2.ttime = ( ");
		sql.append("    select max(hp.ttime) from HISTORY_");
		sql.append(partId);
		sql.append(" hp ");
		sql.append(" 	where hp.tsymbol_id = ( select sp.tsymbol_id from SYMBOLS sp where ");
		sql.append("    sp.tsymbol = ? and sp.tprovider = ? )");
		sql.append(" ) ");
		sql.append(" union all ");
		sql.append(" select hp2.ttime ");
		sql.append("  from HISTORY_");
		sql.append(partId);
		sql.append(" hp2, SYMBOLS sp2 ");
		sql.append("  where hp2.tsymbol_id = sp2.tsymbol_id and ");
		sql.append("  sp2.tsymbol = ? and ");
		sql.append("  sp2.tprovider = ? and ");
		sql.append("  hp2.ttime = ( ");
		sql.append("    select min(hp.ttime) from HISTORY_");
		sql.append(partId);
		sql.append(" hp ");
		sql.append(" 	where hp.tsymbol_id = ( select sp.tsymbol_id from SYMBOLS sp where ");
		sql.append("    sp.tsymbol = ? and sp.tprovider = ? )");
		sql.append(" ) ");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {


			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, symbol);
			stmt.setString(2, provider);

			stmt.setString(3, symbol);
			stmt.setString(4, provider);

			stmt.setString(5, symbol);
			stmt.setString(6, provider);

			stmt.setString(7, symbol);
			stmt.setString(8, provider);

			ResultSet rs = stmt.executeQuery();

			if (rs.next()) {
				maxDate.setLong(rs.getLong(1));
			}

			if (rs.next()) {
				minDate.setLong(rs.getLong(1));
			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getLoadedMinMaxDates", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}



	}

	// session - BrokerMetaInfo
	public Map<String, Set<BrokerMetaInfo>> getAvailableTradingMetainfo(
					Set<String> sessions) throws Exception {
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		TreeMap<String, Set<BrokerMetaInfo>> metainfo = new TreeMap<String, Set<BrokerMetaInfo>>();

		sql.append("select session_id, symbol, sign_digits, point_value, spread from broker_symbols_metainfo where 1=1 ");


		if (sessions != null && sessions.size() > 0) {

			for (int i = 0; i < sessions.size(); i++) {


				if (i == 0) {
					sql.append(" and ( ");
				}

				sql.append(" session_id = ? ");

				if (i < (sessions.size() - 1)) {
					sql.append("or");
				} else if (i == (sessions.size() - 1)) {
					sql.append(" ) ");
				}


			}

		}

		sql.append(" order by session_id");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;

		try {

			stmt = conn.prepareStatement(sql.toString());

			if (sessions != null && sessions.size() > 0) {
				for (Iterator<String> it = sessions.iterator(); it.hasNext();) {
					stmt.setString(++cnt, it.next());
				}

			}


			//
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {
				BrokerMetaInfo bmi = new BrokerMetaInfo();
				bmi.pointValue_m = rs.getDouble("point_value");
				bmi.signDigits_m = rs.getInt("sign_digits");
				bmi.spread_m = rs.getDouble("spread");
				bmi.symbol_m.append(rs.getString("symbol"));


				String session = rs.getString("session_id");


				if (metainfo.containsKey(session)) {
					Set<BrokerMetaInfo> metaInfoSet = metainfo.get(session);
					metaInfoSet.add(bmi);

				} else {
					Set<BrokerMetaInfo> metaInfoSet = new TreeSet<BrokerMetaInfo>();
					metaInfoSet.add(bmi);

					metainfo.put(session, metaInfoSet);
				}




			}
			;


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAvailableTradingMetainfo", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


		return metainfo;
	}

	// this extracts available providers and symbols
	public void getAvailableTradingReportSymbols(
					List<String> sessionIdList, // list of sessions
					long begDate,
					long endDate,
					Map<String, List<HtPair<String, String>>> provNumDenomSymbols) throws Exception {
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		provNumDenomSymbols.clear();

		sql.append("select distinct provider, symbol_num, symbol_denom from ORDER_RESPONSES ors ");
		sql.append("where ors.result_code = ");
		sql.append(OperationResult.PT_ORDER_ESTABLISHED);

		if (sessionIdList != null && sessionIdList.size() > 0) {
			for (int i = 0; i < sessionIdList.size(); i++) {
				//String sId = sessionIdList.get(i);
				if (i == 0) {
					sql.append(" and ( ");
				}

				sql.append(" ors.source = ? ");

				if (i < (sessionIdList.size() - 1)) {
					sql.append("or");
				} else if (i == (sessionIdList.size() - 1)) {
					sql.append(" ) ");
				}
			}
		}

		if (begDate > 0) {
			sql.append(" and ors.op_issue_time >=? ");
		}

		if (endDate > 0) {
			sql.append(" and ors.op_issue_time <=? ");
		}

		sql.append(" order by provider, symbol_num, symbol_denom ");
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		int cnt = 0;

		try {

			stmt = conn.prepareStatement(sql.toString());

			if (sessionIdList != null && sessionIdList.size() > 0) {
				for (int i = 0; i < sessionIdList.size(); i++) {
					stmt.setString(++cnt, sessionIdList.get(i));
				}

			}

			if (begDate > 0) {
				stmt.setLong(++cnt, begDate);
			}

			if (endDate > 0) {
				stmt.setLong(++cnt, endDate);
			}
			//
			ResultSet rs = stmt.executeQuery();

			while (rs.next()) {

				String provider = rs.getString("provider");
				String symbolNum = rs.getString("symbol_num");
				String SymbolDenom = rs.getString("symbol_denom");

				provider = (provider != null ? provider : "");
				symbolNum = (symbolNum != null ? symbolNum : "");
				SymbolDenom = (SymbolDenom != null ? SymbolDenom : "");

				if (provNumDenomSymbols.containsKey(provider)) {
					List<HtPair<String, String>> symbols = provNumDenomSymbols.get(provider);
					symbols.add(new HtPair(symbolNum, SymbolDenom));

				} else {
					List<HtPair<String, String>> symbols = new ArrayList<HtPair<String, String>>();

					symbols.add(new HtPair(symbolNum, SymbolDenom));
					provNumDenomSymbols.put(provider, symbols);
				}


			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getAvailableTradingReportSymbols", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}


	}

	/*
	 * public void investigateHolesInDataFeed( long begDate, long endDate, String
	 * profilename, String provider, String symbol, int aggrPeriod, int
	 * multFactor, int histogrumNumber, Vector<HistogramEntry> histogram) throws
	 * Exception { histogram.clear();
	 *
	 * // millisecs long time_threshold = -1; if (aggrPeriod ==
	 * TSAggregationPeriod.AP_Hour) { time_threshold = 60 * 60 * multFactor *
	 * 1000; } else if (aggrPeriod == TSAggregationPeriod.AP_Minute) {
	 * time_threshold = 60 * multFactor * 1000; } else if (aggrPeriod ==
	 * TSAggregationPeriod.AP_Day) { time_threshold = 60 * 60 * 24 * multFactor *
	 * 1000; } else { throw new HtException(getContext(),
	 * "investigateHolesInDataFeed", "Invalid date-time parameters"); }
	 *
	 * for (int i = 1; i <= histogrumNumber; i++) { HistogramEntry entry_i = new
	 * HistogramEntry();
	 *
	 *
	 *
	 * if (i < histogrumNumber) { entry_i.firstValue = time_threshold * i;
	 * entry_i.secondValue = time_threshold * (i + 1); } else { entry_i.firstValue
	 * = time_threshold * i; }
	 *
	 * histogram.add(entry_i);
	 *
	 *
	 * }
	 *
	 * Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
	 *
	 * //String pn = RtDatabaseManager.instance().checkProfileName(profilename);
	 * int partId =
	 * RtDatabaseManager.instance().getProfilePartitionId(profilename);
	 *
	 * StringBuilder sql = new StringBuilder(); PreparedStatement stmt = null;
	 *
	 * int cnt = 0; try {
	 *
	 * sql.append(" select h.ttime "); sql.append(" from HISTORY_");
	 * sql.append(partId); sql.append(" h, SYMBOLS s "); sql.append(" where ");
	 * sql.append(" h.tsymbol_id = s.tsymbol_id "); if (begDate > 0) {
	 * sql.append(" and h.ttime >=? "); } if (endDate > 0) { sql.append(" and
	 * h.ttime <=? "); }
	 *
	 * sql.append(" order by ttime ");
	 *
	 *
	 * stmt = conn.prepareStatement(sql.toString(),
	 * java.sql.ResultSet.TYPE_FORWARD_ONLY, java.sql.ResultSet.CONCUR_READ_ONLY);
	 * stmt.setFetchSize(Integer.MIN_VALUE);
	 *
	 * if (begDate > 0) { stmt.setLong(++cnt, begDate); }
	 *
	 * if (endDate > 0) { stmt.setLong(++cnt, endDate); }
	 *
	 * ResultSet rs = stmt.executeQuery(); long time_cur = -1; long time_prev =
	 * -1; while (rs.next()) { time_cur = rs.getLong(1);
	 *
	 * if (time_prev > 0 && time_cur > 0) { long dist_milisec = time_cur -
	 * time_prev;
	 *
	 * // find range for (int i = 1; i <= histogrumNumber; i++) { }
	 *
	 * }
	 *
	 * // if (time_cur > 0) { time_prev = time_cur; }
	 *
	 *
	 *
	 * }
	 *
	 *
	 *
	 *
	 *
	 * } catch (SQLException sqe) { throw new HtException(getContext(),
	 * "get_OrderData_Page", "SQL exception in \"" + sql.toString() + "\"
	 * happened: \"" + sqe.getMessage() + "\""); } catch (Throwable e) { throw new
	 * Exception(e); } finally { HtDbUtils.silentlyCloseStatement(stmt);
	 * HtDbUtils.silentlyCloseConnection(conn); } }
	 *
	 * // this function queries for some important information public List<String>
	 * queryDatabaseInfo() throws Exception { List<String> result = new
	 * ArrayList<String>();
	 *
	 * PreparedStatement stmt = null; StringBuilder sql = new StringBuilder();
	 *
	 * Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
	 * sql.append("");
	 *
	 * try {
	 *
	 * stmt = conn.prepareStatement(sql.toString());
	 *
	 * } catch (SQLException sqe) { throw new HtException(getContext(),
	 * "queryDatabaseInfo", "SQL exception in \"" + sql.toString() + "\" happened:
	 * \"" + sqe.getMessage() + "\""); } catch (Throwable e) { throw new
	 * Exception(e); } finally { HtDbUtils.silentlyCloseStatement(stmt);
	 * HtDbUtils.silentlyCloseConnection(conn); }
	 *
	 *
	 * return result; }
	 *
	 */
	// --------------------------------------------
	// functions to work with user session tables
	/**
	 *
	 */
	@Override
	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}
}
