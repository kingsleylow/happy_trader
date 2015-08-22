/*
 * RtDatabaseSchemaManager.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.utils.HtDbUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok This class manages creation/deletion of global tables
 */
public class RtDatabaseSchemaManager implements RtManagerBase {

	static private RtDatabaseSchemaManager instance_m = null;

	/**
	 */
	static public RtDatabaseSchemaManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtDatabaseSchemaManager();
		}

		return instance_m;
	}

	static public RtDatabaseSchemaManager instance() {
		return instance_m;
	}

	static String getContext() {
		return RtDatabaseSchemaManager.class.getSimpleName();
	}

	/**
	 * Creates a new instance of RtDatabaseSchemaManager
	 */
	private RtDatabaseSchemaManager() {

		HtLog.log(Level.INFO, getContext(), getContext(), "Database Schema Manager started");
	}

	/**
	 */
	
	public synchronized boolean checkProfileTablesExist(int partitionId) throws Exception
	{
	  
		
		PreparedStatement stmt = null;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		
	
	 

		String sql= "select count(1) from information_schema.TABLES where " +
		  "upper(TABLE_SCHEMA) = ? and " +
		  "(upper(TABLE_NAME) = ? or upper(TABLE_NAME) = ? or upper(TABLE_NAME) = ?) ";

		int cnt = 0;

		try {
			stmt = conn.prepareStatement(sql.toString());
			String schema = RtDatabaseManager.instance().getDbConnectData(conn).get("db_dbname");

			if (schema == null || schema.length() <= 0) {
				throw new HtException(getContext(), "checkProfileTablesExist", "Invalid schema name");
			}

			stmt.setString(1, schema.toUpperCase());
			stmt.setString(2, "HISTORY_"+partitionId);
			stmt.setString(3, "LEVEL1_"+partitionId);
			stmt.setString(4, "DRAWABLE_"+partitionId);
			
			ResultSet rs = stmt.executeQuery();
			
			while (rs.next()) {
			  cnt = rs.getInt(1);
			}
			
			
		
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "checkProfileTablesExist", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);
		}
		
		if (cnt == 3)
		  return true;
		
		return false;

	}
	
	public synchronized void createNewProfile(int partitionId, List<String> errorMessages) {
		// create list of tables necessary for functioning
		//String pn = RtDatabaseManager.instance().checkProfileName(profilename);
		StringBuilder sql = new StringBuilder();
		Statement stmt = null;
		Connection conn = null;
		
		try {
			conn = RtDatabaseManager.instance().resolveSQLConnection();

		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
			return;
		} 
		
		try {
			stmt = conn.createStatement();


			// main history table
			sql.append("create table  HISTORY_");
			sql.append(partitionId);
			sql.append(" (");
			sql.append("tid INT AUTO_INCREMENT PRIMARY KEY,");
			sql.append("tsymbol_id SMALLINT not null,");
			sql.append("ttime bigint not null,");
			sql.append("topen double,");
			sql.append("tclose double,");
			sql.append("thigh double,");
			sql.append("tlow double,");

			sql.append("topen2 double,");
			sql.append("tclose2 double,");
			sql.append("thigh2 double,");
			sql.append("tlow2 double,");

			sql.append("tvolume double,");
			sql.append("tbid double,");
			sql.append("task double,");
			sql.append("ttype tinyint,");
			sql.append("aflag tinyint,");
			sql.append("color tinyint, ");
			sql.append("operation tinyint");
			sql.append(") ");

			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		try {

			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append("create unique index HISTORY_");
			sql.append(partitionId);
			sql.append("_U1 on HISTORY_");
			sql.append(partitionId);
			sql.append("(ttime, tsymbol_id);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index HISTORY_");
			sql.append(partitionId);
			sql.append("_I0 on HISTORY_");
			sql.append(partitionId);
			sql.append("(tsymbol_id);");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}
		
		// level 1
		try {
			stmt = conn.createStatement();

			sql.setLength(0);
			// main level1 table
			sql.append("create table  LEVEL1_");
			sql.append(partitionId);
			sql.append(" (");
			sql.append("tid INT AUTO_INCREMENT PRIMARY KEY,");
			sql.append("tsymbol_id SMALLINT not null,");
			sql.append("ttime bigint not null,");
			sql.append("bid double,");
			sql.append("bidsize double,");
			sql.append("ask double,");
			sql.append("asksize double,");
			sql.append("openint double )");
			
			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		try {

			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append("create unique index LEVEL1_");
			sql.append(partitionId);
			sql.append("_U1 on LEVEL1_");
			sql.append(partitionId);
			sql.append("(ttime, tsymbol_id);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index LEVEL1_");
			sql.append(partitionId);
			sql.append("_I0 on LEVEL1_");
			sql.append(partitionId);
			sql.append("(tsymbol_id);");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}
		
		
		
		// ----------------------
		try {
			// create stored function
			stmt = conn.createStatement();
			sql.setLength(0);

			sql.append(" CREATE FUNCTION get_maxDate_");
			sql.append(partitionId);
			sql.append(" (p_max_minutes double, p_exact_time double, p_provider varchar(32), p_symbol varchar(32)) RETURNS double ");
			sql.append(" DETERMINISTIC READS SQL DATA ");
			sql.append(" BEGIN ");
			sql.append(" DECLARE not_found INT DEFAULT 0;	");
			sql.append(" DECLARE p_maxdate double;");
			sql.append(" DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET not_found = 1;");
			sql.append(" select   max(hst2.ttime)  into  p_maxdate");
			sql.append(" 	from HISTORY_");
			sql.append(partitionId);
			sql.append(" hst2, symbols smb2");
			sql.append(" 		where smb2.tsymbol_id=hst2.tsymbol_id");
			sql.append(" 		and smb2.tprovider=p_provider  and smb2.tsymbol=p_symbol");
			sql.append(" 		and hst2.ttime <=p_exact_time  and hst2.ttime >= (p_exact_time - 1000 * p_max_minutes  * 60);");
			sql.append(" if  (not_found =1) then");
			sql.append(" 	set p_maxdate = -1;	");
			sql.append(" end if;");
			sql.append(" return  p_maxdate;	");
			sql.append(" END ");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		try {

			stmt = conn.createStatement();

			// create drawable objects table
			sql.setLength(0);
			sql.append("create table  DRAWABLE_");
			sql.append(partitionId);
			sql.append(" (");
			sql.append("id INT AUTO_INCREMENT PRIMARY KEY,");
			sql.append("tsymbol_id SMALLINT not null,");
			sql.append("create_ttime bigint not null,");
			sql.append("ttime bigint not null,");
			sql.append("type SMALLINT,");
			sql.append("name varchar(64),");
			sql.append("run_name_id integer,");

			// vector<LineIndic>
			sql.append("lineindic_name1 varchar(32),");
			sql.append("lineindic_color1 SMALLINT,");
			sql.append("lineindic_value1 double,");

			sql.append("lineindic_name2 varchar(32),");
			sql.append("lineindic_color2 SMALLINT,");
			sql.append("lineindic_value2 double,");

			sql.append("lineindic_name3 varchar(32),");
			sql.append("lineindic_color3 SMALLINT,");
			sql.append("lineindic_value3 double,");

			sql.append("lineindic_name4 varchar(32),");
			sql.append("lineindic_color4 SMALLINT,");
			sql.append("lineindic_value4 double,");

			sql.append("lineindic_name5 varchar(32),");
			sql.append("lineindic_color5 SMALLINT,");
			sql.append("lineindic_value5 double,");

			sql.append("lineindic_name6 varchar(32),");
			sql.append("lineindic_color6 SMALLINT,");
			sql.append("lineindic_value6 double,");

			sql.append("lineindic_name7 varchar(32),");
			sql.append("lineindic_color7 SMALLINT,");
			sql.append("lineindic_value7 double,");

			// Print
			sql.append("print_bid double,");
			sql.append("print_ask double,");
			sql.append("print_volume double,");
			sql.append("print_color SMALLINT,");
			sql.append("print_operation SMALLINT,");

			// OHLC
			sql.append("ohlc_open double,");
			sql.append("ohlc_high double,");
			sql.append("ohlc_low double,");
			sql.append("ohlc_close double,");
			sql.append("ohlc_volume double,");

			// Text
			sql.append("priority SMALLINT,");
			sql.append("short_text varchar(32),");
			sql.append("text varchar(8192), ");

			// level1
			sql.append("level1_best_bid_price double,");
			sql.append("level1_best_ask_price double,");
			sql.append("level1_best_bid_volume double,");
			sql.append("level1_best_ask_volume double ");

			sql.append(" ) ");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		try {

			stmt = conn.createStatement();

			// index
			sql.setLength(0);
			sql.append("create index DRAWABLE_");
			sql.append(partitionId);
			sql.append("_I1 on DRAWABLE_");
			sql.append(partitionId);
			sql.append("(tsymbol_id, ttime);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index DRAWABLE_");
			sql.append(partitionId);
			sql.append("_I2 on DRAWABLE_");
			sql.append(partitionId);
			sql.append("(name, ttime);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index DRAWABLE_");
			sql.append(partitionId);
			sql.append("_I3 on DRAWABLE_");
			sql.append(partitionId);
			sql.append("(short_text);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index DRAWABLE_");
			sql.append(partitionId);
			sql.append("_I4 on DRAWABLE_");
			sql.append(partitionId);
			sql.append("(run_name_id);");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append("create index DRAWABLE_");
			sql.append(partitionId);
			sql.append("_I5 on DRAWABLE_");
			sql.append(partitionId);
			sql.append("(create_ttime);");
			stmt.execute(sql.toString());


		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		HtDbUtils.silentlyCloseConnection(conn);


	}

	public synchronized void removeProfile(int partitionId, List<String> errorMessages)  {
		// remove tables
		//String pn = RtDatabaseManager.instance().checkProfileName(profilename);
		
		StringBuilder sql = new StringBuilder();
		Statement stmt = null;
		
		Connection conn = null;
		
		// resolve connection
		try {
			conn = RtDatabaseManager.instance().resolveSQLConnection();
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
			
			return;
		}

		try {
			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append("drop table HISTORY_").append(partitionId).append(";");
			
			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}
		
		try {
			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append("drop table LEVEL1_").append(partitionId).append(";");
			
			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}


		try {
			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append(" DROP FUNCTION get_maxDate_").append(partitionId);
		
			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}

		// drop drawable
		try {
			stmt = conn.createStatement();

			sql.setLength(0);
			sql.append(" DROP table DRAWABLE_").append(partitionId);
			
			stmt.execute(sql.toString());

		} catch (SQLException sqe) {
			errorMessages.add(new StringBuilder("SQL exception in \"").append(sql.toString()).append("\" happened: \"").append(sqe.getMessage()).append("\"\n").toString());
		} catch (Throwable e) {
			errorMessages.add(e.getMessage());
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}


		// delete connection
		HtDbUtils.silentlyCloseConnection(conn);



	}

	public synchronized boolean isGlobalsCreated() throws Exception {
		// read from dictionary
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();

		sql.append("select count(1) from information_schema.TABLES where ");
		sql.append("TABLE_SCHEMA = ? and ");
		sql.append("(TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ? ");
		sql.append("or TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ?  or TABLE_NAME = ? or TABLE_NAME=? ");
		sql.append("or TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ? or TABLE_NAME = ? ) ");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();

		try {
			stmt = conn.prepareStatement(sql.toString());
			String schema = RtDatabaseManager.instance().getDbConnectData(conn).get("db_dbname");

			if (schema == null || schema.length() <= 0) {
				throw new HtException(getContext(), "isGlobalsCreated", "Invalid schema name");
			}

			stmt.setString(1, schema);
			stmt.setString(2, "COMMONLOG");
			stmt.setString(3, "POSITIONS");
			stmt.setString(4, "SYMBOLS");
			stmt.setString(5, "EXPORTIMPORTLOG");
			stmt.setString(6, "ORDERS");
			stmt.setString(7, "ORDER_RESPONSES");
			stmt.setString(8, "BROKER_SESSIONS");
			stmt.setString(9, "BROKER_SESSIONS_OPER");
			stmt.setString(10, "ALERTS");
			stmt.setString(11, "BROKER_RUNNAMES");
			stmt.setString(12, "BROKER_SYMBOLS_METAINFO");
			stmt.setString(13, "USER_SESSIONS");
			stmt.setString(14, "USERS");
			stmt.setString(15, "SCRIPTS_RUNS");
			stmt.setString(16, "CLOSE_POS");



			ResultSet rs = stmt.executeQuery();
			int cnt = 0;
			while (rs.next()) {
				if (rs.getInt(1) == 15) {
					return true;
				}
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "isGlobalsCreated", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);

		}

		return false;
	}

	public synchronized void clearGlobalTables() throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		Statement stmt = conn.createStatement();
		StringBuilder sql = new StringBuilder();

		try {
			sql.append("delete from SCRIPTS_RUNS;");
			stmt.execute(sql.toString());
			conn.commit();

			sql.append("delete from COMMONLOG;");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append("delete from POSITIONS;");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from SYMBOLS");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from EXPORTIMPORTLOG");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from ORDERS");
			stmt.execute(sql.toString());

			sql.setLength(0);
			sql.append(" delete from ORDER_RESPONSES");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from ALERTS");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from BROKER_SESSIONS_OPER");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from BROKER_RUNNAMES");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from BROKER_SYMBOLS_METAINFO");
			stmt.execute(sql.toString());
			conn.commit();


			sql.setLength(0);
			sql.append(" delete from USERS");
			stmt.execute(sql.toString());
			conn.commit();

			sql.setLength(0);
			sql.append(" delete from USER_SESSIONS");
			stmt.execute(sql.toString());
			conn.commit();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
			HtDbUtils.silentlyCloseConnection(conn);

		}
	}

//
	public static void createSchemeIfNecessary(Connection conn) throws Exception {


		StringBuilder sql = new StringBuilder();
		PreparedStatement stmt = null;
		boolean found_scheme = false;
		sql.setLength(0);
		try {
			sql.append("SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?");
			stmt = conn.prepareStatement(sql.toString());
			ResultSet rs = stmt.executeQuery();

			String s;
			if ((s = rs.getString(1)) != null) {
				if (s.equalsIgnoreCase(HtStartupConstants.DB_SCHEMA)) {
					// done
					found_scheme = true;
				}
			}
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "createSchemeIfNecessary", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		} finally {
			HtDbUtils.silentlyCloseStatement(stmt);
		}



		if (!found_scheme) {
			sql.setLength(0);
			sql.append("CREATE SCHEMA IF NOT EXISTS " + HtStartupConstants.DB_SCHEMA + " CHARACTER SET=UTF8 ");

			Statement stmt2 = null;

			try {
				stmt2 = conn.createStatement();
				stmt2.execute(sql.toString());

			} catch (SQLException sqe) {
				HtLog.log(Level.WARNING, getContext(), "createSchemeIfNecessary", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
			} finally {
				HtDbUtils.silentlyCloseStatement(stmt2);
			}

			HtLog.log(Level.INFO, getContext(), "createSchemeIfNecessary", "Create schema: " + HtStartupConstants.DB_SCHEMA);
		}



	}

	public synchronized void removeGlobalTables() throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		Statement stmt = conn.createStatement();
		StringBuilder sql = new StringBuilder();

		/*
		 * try { sql.append("drop table HISTORY;"); stmt.execute(sql.toString()); }
		 * catch (SQLException sqe) { HtLog.log(Level.WARNING, getContext(),
		 * "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\"
		 * happened: \"" + sqe.getMessage() + "\""); }
		 *
		 */

		sql.setLength(0);
		try {
			sql.append("drop table COMMONLOG;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table POSITIONS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table SYMBOLS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table EXPORTIMPORTLOG;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table ORDERS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table ORDER_RESPONSES;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table CLOSE_POS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table BROKER_SESSIONS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table BROKER_SESSIONS_OPER;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table BROKER_RUNNAMES;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table ALERTS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table BROKER_SYMBOLS_METAINFO;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}


		sql.setLength(0);
		try {
			sql.append("drop table USERS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table USER_SESSIONS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table SCRIPTS_RUNS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("drop table BACKGROUND_JOBS;");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("DROP FUNCTION IF EXISTS count_order_responses");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}

		sql.setLength(0);
		try {
			sql.append("DROP FUNCTION IF EXISTS count_orders");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}
		
		sql.setLength(0);
		try {
			sql.append("DROP FUNCTION IF EXISTS count_close_pos");
			stmt.execute(sql.toString());
		} catch (SQLException sqe) {
			HtLog.log(Level.WARNING, getContext(), "removeGlobalTables", "SQL exception in \"" + sql.toString() + "\" happened: \"" + sqe.getMessage() + "\"");
		}


		HtDbUtils.silentlyCloseStatement(stmt);
		HtDbUtils.silentlyCloseConnection(conn);


	}

	public synchronized void createGlobalTables() throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		Statement stmt = conn.createStatement();
		StringBuilder sql = new StringBuilder();

		// table for holding any log messages
		// and other object
		sql.append("create table COMMONLOG (");
		sql.append(" id INT AUTO_INCREMENT PRIMARY KEY,");
		sql.append(" tlast_created bigint not null, ");
		sql.append(" logtype int not null, ");
		sql.append(" level int not null,   ");
		sql.append(" thread int,   ");
		sql.append(" session varchar(64),  ");
		sql.append(" context varchar(64),  ");
		sql.append(" content MEDIUMBLOB ");
		sql.append(");");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			int errcode = e.getErrorCode();
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getErrorCode() + " - " + e.getMessage() + "\"", null);

			// check whether we actually connected to our user
			if (errcode == 1046) {
				// NO database selected
				throw new HtException("RtdatabaseManager", "createGlobalTables", "Seems you are not logged to necessary schema!");
			}
		}

		sql.setLength(0);
		sql.append("create index COMMONLOG_I1 on COMMONLOG (tlast_created, logtype);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"", null);
		}

		sql.setLength(0);
		sql.append("create index COMMONLOG_I2 on COMMONLOG (context);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"", null);
		}

		sql.setLength(0);
		sql.append("create index COMMONLOG_I3 on COMMONLOG (thread);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"", null);
		}

		// positions still opened and closed

		sql.setLength(0);
		sql.append("create table POSITIONS (");
		sql.append(" id INT AUTO_INCREMENT PRIMARY KEY,");
		sql.append(" tlast_created DATETIME not null, ");
		sql.append(" tlast_updated DATETIME not null, ");
		sql.append(" broker_position_id varchar(32), ");
		sql.append(" symbol_id SMALLINT, ");
		sql.append(" open_time double, ");
		sql.append(" close_time double, ");
		sql.append(" volume double, ");
		sql.append(" state SMALLINT, ");  // opened , closed or not available
		sql.append(" broker_id varchar(64), "); // broker position ID
		sql.append(" open_price double, ");
		sql.append(" close_price double, ");
		sql.append(" profit_trade_currency double, ");
		sql.append(" trade_currency varchar(6) ");

		sql.append(" );");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create index POSITIONS_I1 on POSITIONS (tlast_created, symbol_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index POSITIONS_I2 on POSITIONS (broker_position_id, symbol_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// the table containg symbols reference
		sql.setLength(0);
		sql.append("create table SYMBOLS (");
		sql.append("tprovider varchar(32) not null,");
		sql.append("tsymbol varchar(32) not null,");
		sql.append("tsymbol_id SMALLINT AUTO_INCREMENT PRIMARY KEY ");
		sql.append(");");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create unique index SYMBOLS_U1 on SYMBOLS(tprovider, tsymbol );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// create export / import log table as containing huge amount of data
		sql.setLength(0);

		sql.append("create table EXPORTIMPORTLOG (");
		sql.append(" id INT AUTO_INCREMENT PRIMARY KEY,");
		sql.append(" groupid varchar(32) not null,"); // reference to common log
		sql.append(" tlast_created bigint not null,");
		sql.append(" opertype SMALLINT,"); // import or export
		sql.append(" operstatus SMALLINT,"); // code of operation
		sql.append(" opermessage varchar(4096),"); // message releted operation
		sql.append(" symbol_id SMALLINT, ");
		sql.append(" tdata varchar(4096)"); // HtRtData.toString()
		sql.append(" );");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index EXPORTIMPORTLOG_I1 on EXPORTIMPORTLOG(opertype,groupid);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index EXPORTIMPORTLOG_I2 on EXPORTIMPORTLOG(tlast_created);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// TABLE CLOSE_POS
		sql.setLength(0);
		sql.append("create table CLOSE_POS( ");
		sql.append(" source varchar(32), ");
		sql.append(" id varchar(32), ");
		sql.append(" broker_position_id varchar(32), ");
		sql.append(" broker_issue_time double, ");
		sql.append(" provider varchar(32), ");
		sql.append(" symbol varchar(32), ");
		sql.append(" volume double, ");
		sql.append(" trade_direction smallint, ");
		sql.append(" price_open double, ");
		sql.append(" price_close double, ");
		sql.append(" pos_open_time double, ");
		sql.append(" pos_close_time double, ");
		sql.append(" parent_id varchar(32), ");
		sql.append(" broker_comment varchar(2048), ");
		sql.append(" broker_internal_code bigint, ");
		sql.append(" result_code bigint, ");
		sql.append(" result_detail_code bigint,  ");
		sql.append(" comission_abs double, ");
		sql.append(" comission_pct double, ");
		sql.append(" seq_num int AUTO_INCREMENT PRIMARY KEY) ");
		

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index CLOSE_POS_I0 on CLOSE_POS( source);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}
		
		sql.setLength(0);
		sql.append("create unique index CLOSE_POS_U0 on CLOSE_POS(ID);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		// TRADING DIALOG table
		// this contains
		sql.setLength(0);
		sql.append("create table ORDERS( ");
		sql.append(" source varchar(32), ");
		sql.append(" id varchar(32), ");
		sql.append(" broker_position_id varchar(32), ");
		sql.append(" broker_order_id varchar(32), ");
		sql.append(" order_type bigint, ");
		sql.append(" provider varchar(32), ");
		sql.append(" symbol varchar(32), ");
		sql.append(" context varchar(128), ");

		sql.append(" comment varchar(2048), ");
		sql.append(" or_issue_time double, ");
		sql.append(" or_time double, ");
		sql.append(" or_price double, ");
		sql.append(" or_stop_price double, ");
		sql.append(" or_sl_price double, ");
		sql.append(" or_tp_price double, ");
		sql.append(" or_volume double, ");

		sql.append(" or_expiration_time double, ");
		sql.append(" or_validity_flag int, ");

		sql.append(" or_place varchar(32), ");
		sql.append(" or_method varchar(32), ");
		sql.append(" or_additional_info varchar(64), ");
		sql.append(" or_stop_price_type smallint, ");
		sql.append(" or_price_type smallint, ");
		sql.append(" seq_num int AUTO_INCREMENT PRIMARY KEY ) ");


		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDERS_I0 on ORDERS( or_issue_time);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDERS_I1 on ORDERS(broker_position_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDERS_I2 on ORDERS(order_type);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create index ORDERS_I3 on ORDERS(source);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}
		
		sql.setLength(0);
		sql.append("create unique index ORDERS_U0 on ORDERS(ID);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		//
		sql.setLength(0);
		sql.append("create table ORDER_RESPONSES( ");
		sql.append(" source varchar(32),  ");
		sql.append(" id varchar(32), ");
		sql.append(" op_issue_time double,  ");
		sql.append(" parent_id varchar(32), ");
		sql.append(" broker_comment varchar(2048), ");
		sql.append(" broker_internal_code bigint, ");
		sql.append(" result_code bigint, ");
		sql.append(" result_detail_code bigint, ");
		sql.append(" context varchar(128), ");

		sql.append(" broker_position_id varchar(32), ");
		sql.append(" broker_order_id varchar(32), ");
		sql.append(" op_price double, ");
		sql.append(" op_stop_price double, ");

		sql.append(" op_tp_price double, ");
		sql.append(" op_sl_price double, ");
		sql.append(" op_volume double, ");
		sql.append(" op_remaining_volume double, ");


		sql.append(" provider varchar(32), ");
		sql.append(" symbol varchar(32), ");

		sql.append(" op_expiration_time double, ");
		sql.append(" op_validity_flag int, ");
		sql.append(" op_pos_type int, ");

		sql.append(" op_time double, ");
		sql.append(" op_comission double, ");
		sql.append(" seq_num int AUTO_INCREMENT PRIMARY KEY ) ");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I0 on ORDER_RESPONSES( op_issue_time);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I1 on ORDER_RESPONSES( parent_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I2 on ORDER_RESPONSES(result_code, result_detail_code);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I3 on ORDER_RESPONSES( broker_internal_code);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I4 on ORDER_RESPONSES( broker_position_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ORDER_RESPONSES_I5 on ORDER_RESPONSES(source);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}
		
		sql.setLength(0);
		sql.append("create unique index ORDER_RESPONSES_U0 on ORDER_RESPONSES(ID);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		// broker session table
		sql.setLength(0);
		sql.append(" create table BROKER_SESSIONS ( ");
		sql.append(" session_id varchar(32) PRIMARY KEY,  ");
		sql.append(" create_time double, ");
		sql.append(" is_simulation smallint , ");
		sql.append(" connect_string varchar(256) )");



		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// metainfo broker table
		sql.setLength(0);
		sql.append(" create table BROKER_SYMBOLS_METAINFO ( ");
		sql.append(" session_id varchar(32),  ");
		sql.append(" symbol varchar(32), ");
		sql.append(" sign_digits smallint, ");
		sql.append(" point_value double, ");
		sql.append(" spread double )");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create index BROKER_SYMBOLS_METAINFO_I1 on BROKER_SYMBOLS_METAINFO( session_id, symbol);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}





		//
		// broker session table
		sql.setLength(0);
		sql.append(" create table BROKER_SESSIONS_OPER ( ");
		sql.append(" id int AUTO_INCREMENT PRIMARY KEY,  ");
		sql.append(" session_id varchar(32),  ");
		sql.append(" alg_brk_pair varchar(64), ");
		sql.append(" thread int,  ");
		//sql.append(" run_name varchar(256), ");
		sql.append(" runname_id int not null, ");
		sql.append(" op_time double, ");
		sql.append(" operation smallint ) ");



		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}



		////
		sql.setLength(0);
		sql.append("create index BROKER_SESSIONS_OPER_I0 on BROKER_SESSIONS_OPER( session_id );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create index BROKER_SESSIONS_OPER_I1 on BROKER_SESSIONS_OPER( runname_id );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index BROKER_SESSIONS_OPER_I2 on BROKER_SESSIONS_OPER( op_time, thread, alg_brk_pair );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}



		// this table contains alerts
		// source - reference to broker sessions if applicable
		// alg_brk_pair contains algorithm broker pair (if session is not applicable)
		sql.setLength(0);
		sql.append("create table ALERTS( ");
		sql.append(" id varchar(32) PRIMARY KEY,  ");
		sql.append(" source varchar(32), ");
		sql.append(" alg_brk_pair varchar(64), ");
		sql.append(" thread int, ");
		sql.append(" priority smallint,  ");
		sql.append(" alert_time double, ");
		sql.append(" data varchar(16384), ");
		sql.append(" small_data varchar(150), ");
		sql.append(" runname_id int, ");
		sql.append(" tid int ");
		sql.append(" )");


		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ALERTS_I0 on ALERTS( alert_time, source);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ALERTS_I1 on ALERTS( runname_id);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index ALERTS_I2 on ALERTS( small_data );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("ALTER TABLE ALERTS MODIFY COLUMN tid INTEGER UNSIGNED NOT NULL DEFAULT NULL AUTO_INCREMENT, ");
		sql.append(" ADD UNIQUE INDEX ALERTS_U3 USING BTREE(tid);");

		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		// -------------------------------------
		// BROKER_RUNNAMES
		sql.setLength(0);
		sql.append("create table BROKER_RUNNAMES( ");
		sql.append(" id int AUTO_INCREMENT PRIMARY KEY,  ");
		sql.append(" run_name varchar(250), ");
		sql.append(" comment varchar(4096), ");
		sql.append(" op_first_time double, ");
		sql.append(" user_data varchar(512) ");
		sql.append(" )");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		sql.setLength(0);
		sql.append("create unique index BROKER_RUNNAMES_U0 on BROKER_RUNNAMES( run_name);");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index BROKER_RUNNAMES_I1 on BROKER_RUNNAMES( op_first_time );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// -------------------------------------------
		// user table
		sql.setLength(0);
		sql.append("create table USERS( ");
		sql.append("id int AUTO_INCREMENT PRIMARY KEY,");
		sql.append("username varchar(64) not null,");
		sql.append("password varchar(64) not null,");
		sql.append("disabled smallint not null");
		sql.append(" )");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create unique index USERS_U1 on USERS( username );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// -------------------------------------------
		// session table
		sql.setLength(0);
		sql.append("create table USER_SESSIONS( ");
		sql.append("id int AUTO_INCREMENT PRIMARY KEY,");
		sql.append("user_id int not null,");
		sql.append("session_id varchar(32) not null,");
		sql.append("last_login double not null,");
		sql.append("disabled smallint not null,");
		sql.append("user_ip varchar(32)");
		sql.append(" )");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create index USER_SESSIONS_I1 on USER_SESSIONS( user_id );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		sql.setLength(0);
		sql.append("create unique index USER_SESSIONS_U2 on USER_SESSIONS( session_id );");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// table scripts runs
		sql.setLength(0);
		sql.append("create table SCRIPTS_RUNS( ");
		sql.append("run_id varchar(32) PRIMARY KEY not null,");
		sql.append("name varchar(128) not null,");
		sql.append("run_status int not null,");
		sql.append("start_time double,");
		sql.append("finish_time double,");
		sql.append("custom_result_string varchar(2048)");
		sql.append(" )");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}

		// -----------------------

		sql.setLength(0);

		sql.append("create table BACKGROUND_JOBS (");
		sql.append(" id INT AUTO_INCREMENT PRIMARY KEY,");
		sql.append("string_id varchar(128),");
		sql.append("type_id varchar(128),");
		sql.append("start_time double,");
		sql.append("finish_time double,");
		sql.append("result_code int,");
		sql.append("result_string varchar(1024)");
		sql.append(" )");
		try {
			stmt.execute(sql.toString());
		} catch (SQLException e) {
			HtLog.log(Level.WARNING, getContext(), "createGlobalTables", "Exception in \"" + sql.toString() + "\" happened: \"" + e.getMessage() + "\"");
		}


		// -----------------------------------

		sql.setLength(0);
		sql.append(" CREATE FUNCTION  count_order_responses( src varchar(32)) RETURNS int(11) ");
		sql.append(" DETERMINISTIC READS SQL DATA ");
		sql.append(" begin ");
		sql.append(" declare cnt_res integer; ");
		sql.append(" select count(id) into cnt_res from order_responses where source=src; ");
		sql.append(" return cnt_res; ");
		sql.append(" end");
		stmt.execute(sql.toString());


		// -----------------------------------

		sql.setLength(0);
		sql.append(" CREATE FUNCTION  count_orders( src varchar(32)) RETURNS int(11) ");
		sql.append(" DETERMINISTIC READS SQL DATA ");
		sql.append(" begin ");
		sql.append(" declare cnt_res integer; ");

		sql.append(" select count(id) into cnt_res from orders where source=src; ");
		sql.append(" return cnt_res; ");
		sql.append(" end");

		stmt.execute(sql.toString());
		
		// -----------------------------------
		
		sql.setLength(0);
		sql.append(" CREATE FUNCTION  count_close_pos( src varchar(32)) RETURNS int(11) ");
		sql.append(" DETERMINISTIC READS SQL DATA ");
		sql.append(" begin ");
		sql.append(" declare cnt_res integer; ");

		sql.append(" select count(id) into cnt_res from close_pos where source=src; ");
		sql.append(" return cnt_res; ");
		sql.append(" end");

		stmt.execute(sql.toString());



		/*
		 * // ----------------------------------- // new addendum // a single
		 * clasterizide history table sql.setLength(0); sql.append("create table
		 * HISTORY ("); sql.append("profile_id INT,"); sql.append("tid INT
		 * AUTO_INCREMENT,"); sql.append("tsymbol_id SMALLINT,"); sql.append("ttime
		 * bigint not null,"); sql.append("topen double,"); sql.append("tclose
		 * double,"); sql.append("thigh double,"); sql.append("tlow double,");
		 *
		 * sql.append("topen2 double,"); sql.append("tclose2 double,");
		 * sql.append("thigh2 double,"); sql.append("tlow2 double,");
		 *
		 * sql.append("tvolume double,"); sql.append("tbid double,");
		 * sql.append("task double,"); sql.append("ttype tinyint,");
		 * sql.append("aflag tinyint, color tinyint, operation tinyint ");
		 * sql.append(" PRIMARY KEY (tid, profile_id)" ); sql.append(" ) PARTITION
		 * BY LIST(profile_id) ( ");
		 *
		 * // partitions for(int i = 1; i <=
		 * HtStartupConstants.MYSQL_MAX_PARTITIONS; i++) { sql.append(" PARTITION "+
		 * "p"+i+ " VALUES IN (" + i+ "),"); }
		 *
		 * sql.setLength(sql.length()-1); sql.append(" )" );
		 *
		 * //ALTER TABLE `htrader`.`history` MODIFY COLUMN `tid` INTEGER NOT NULL
		 * DEFAULT NULL AUTO_INCREMENT; stmt.execute(sql.toString());
		 *
		 * sql.setLength(0); sql.append("create unique index HISTORY_U1 on HISTORY
		 * (profile_id, tsymbol_id, ttime);"); stmt.execute(sql.toString());
		 *
		 * // sql.setLength(0);
		 *
		 * sql.append(" CREATE FUNCTION get_maxDate(p_max_minutes double,
		 * p_exact_time double, p_provider varchar(32), p_symbol varchar(32))
		 * RETURNS double"); sql.append(" BEGIN "); sql.append(" DECLARE not_found
		 * INT DEFAULT 0;	"); sql.append(" DECLARE p_maxdate double;"); sql.append("
		 * DECLARE CONTINUE HANDLER FOR SQLSTATE '02000' SET not_found = 1;");
		 * sql.append(" select max(hst2.ttime) into p_maxdate"); sql.append(" from
		 * HISTORY hst2, symbols smb2"); sql.append(" where
		 * smb2.tsymbol_id=hst2.tsymbol_id"); sql.append(" and
		 * smb2.tprovider=p_provider and smb2.tsymbol=p_symbol"); sql.append(" and
		 * hst2.ttime <=p_exact_time and hst2.ttime >= (p_exact_time - 1000 *
		 * p_max_minutes * 60);"); sql.append(" if (not_found =1) then");
		 * sql.append(" set p_maxdate = -1;	"); sql.append(" end if;"); sql.append("
		 * return p_maxdate;	"); sql.append(" END "); stmt.execute(sql.toString());
		 */

		HtDbUtils.silentlyCloseStatement(stmt);
		HtDbUtils.silentlyCloseConnection(conn);



		HtLog.log(Level.INFO, getContext(), "createGlobalTables", "Created global tables successfully", null);

	}

	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}
}
