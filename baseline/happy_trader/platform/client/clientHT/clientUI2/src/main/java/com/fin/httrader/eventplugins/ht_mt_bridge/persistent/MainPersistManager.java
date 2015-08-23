/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.persistent;

import com.fin.httrader.configprops.HtStartupConstants;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperAccountMetainfoChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperBalanceChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionChange;
import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db.*;
import com.fin.httrader.hlpstruct.PosState;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDbUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.mysql.jdbc.ConnectionImpl;

import java.io.File;
import java.math.BigInteger;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import org.hibernate.Hibernate;

import org.hibernate.Query;
import org.hibernate.SQLQuery;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.cfg.Configuration;
import org.hibernate.cfg.Environment;
import org.hibernate.cfg.Mappings;
import org.hibernate.engine.spi.SessionFactoryImplementor;
import org.hibernate.service.ServiceRegistryBuilder;
import org.hibernate.service.jdbc.connections.spi.ConnectionProvider;
import org.hibernate.type.BigIntegerType;
import org.hibernate.type.LongType;
import org.hibernate.type.StringType;
import org.jboss.logging.Logger;

/**
 *
 * @author Victor_Zoubok
 */
public class MainPersistManager {

		private SessionFactory sessFactory_m = null;

		public static final int QUERY_ACCOUNT_DEAFULT = 0x0000;
		public static final int QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY = 0x0001;
		public static final int QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY = 0x0002;

		public static final int QUERY_POSITION_DO_NOT_INCLUDE_CHANGE_HISTORY = 0x0001;
		public static final int QUERY_POSITION_INCLUDE_CHANGE_HISTORY = 0x0002;

		// what type of positions to select
		public static final int QUERY_POSITION_ONLY_HISTORY = 1;
		public static final int QUERY_POSITION_ONLY_OPEN = 2;
		public static final int QUERY_POSITION_ONLY_BOTH = 3;
		
		private static final String SQL_CREATE_FILE = "mtrunner_create.sql";
		private static final String SQL_CREATE_FILE_FORCE = "mtrunner_create_force.sql";
		
		private static final String DB_SCHEME = "mtrunner";
		
		

		public MainPersistManager() {
		}

		private String getContext() {
				return getClass().getSimpleName();
		}

		public void createDBTables( boolean force) throws Exception {

				TransactionManager trx = new TransactionManager(getSessionFactory().getCurrentSession());
					
				// if force - recreates
				trx.startTransaction();
				try {
						
						String createScript = null;
						
						if (!force)
								createScript = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory() +
										File.separatorChar+	SQL_CREATE_FILE;
						else
								createScript = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory() +
										File.separatorChar+	SQL_CREATE_FILE_FORCE;
						
						File createScript_f  =new File(createScript);
						if (!createScript_f.exists())
								throw new HtException(getContext(), "createDBTables", "File does not exist");
						
						Session sess = trx.getSession();
						
						SessionFactoryImplementor sfi = (SessionFactoryImplementor) sess.getSessionFactory();
						ConnectionProvider cp = sfi.getConnectionProvider();
						Connection conn = cp.getConnection();
				
						
						
						StringBuilder sql = new StringBuilder();
						HtFileUtils.simpleReadFile(createScript_f, sql, null);
						
						if (sql.length() <=0)
									throw new HtException(getContext(), "createDBTables", "Nothing read from file: " + createScript_f.getAbsolutePath());
						
						String[] splitted = sql.toString().split(";");
						if (splitted != null) {
								for (String splitted_sql : splitted) {
										splitted_sql = splitted_sql.replace(";", "").trim();
										
										if (HtUtils.nvl(splitted_sql))
												continue;
										
										Statement stmt = null;
										try {
												stmt=  conn.createStatement();
												stmt.execute(splitted_sql);
										}
										catch(Exception e)
										{
												
												HtDbUtils.silentlyCloseConnection(conn);
												throw new HtException(getContext(), "createDBTables", "Exception on executing SQL: " + splitted_sql +
													" - " + e.getMessage() );
										}
										finally
										{
												HtDbUtils.silentlyCloseStatement(stmt);
										}
								}
								
						}
						
						
						HtDbUtils.silentlyCloseConnection(conn);
						
						
				} catch (Throwable e) {
						trx.rollBackTransaction();
						throw new HtException(getContext(), "createDBTables", "createDBTables(...) error: " + e.getMessage() + ", rolling back...");
				} finally {
						trx.commitTransaction();
				}
		}

		public void clearAllData() throws Exception {

				TransactionManager trx = new TransactionManager(getSessionFactory().getCurrentSession());
				
				trx.startTransaction();
				try {

						Session sess = trx.getSession();

						Query query = sess.createSQLQuery("truncate table account_metainfo");
						query.executeUpdate();

						query = sess.createSQLQuery("truncate table account_update");
						query.executeUpdate();

						query = sess.createSQLQuery("truncate table balance_update");
						query.executeUpdate();

						query = sess.createSQLQuery("truncate table company_metainfo");
						query.executeUpdate();

						query = sess.createSQLQuery("truncate table position");
						query.executeUpdate();

						query = sess.createSQLQuery("truncate table position_update");
						query.executeUpdate();

				} catch (Throwable e) {
						trx.rollBackTransaction();
						throw new HtException(getContext(), "clearAllData", "Error on clearig data: " + e.getMessage() + ", rolling back...");
				} finally {
						trx.commitTransaction();
				}
		}

		public void queryPositions(
			TransactionManager trx,
			HtMtEventWrapperPositionsHistory r,
			String accountId,
			String companyName,
			int flag_state,
			long openTimeFilter,
			long closeTimeFilter,
			int detail_flag
		) throws Exception {
				// query positions
				r.getPositions().clear();

				if (detail_flag == MainPersistManager.QUERY_POSITION_INCLUDE_CHANGE_HISTORY) {
						// another SQL - include change history
						queryPositions_withDetails(trx, r, accountId, flag_state, openTimeFilter, closeTimeFilter, companyName);
				} else if (detail_flag == MainPersistManager.QUERY_POSITION_DO_NOT_INCLUDE_CHANGE_HISTORY) {
						queryPositions_noDetails(trx, r, accountId, flag_state, openTimeFilter, closeTimeFilter, companyName);
				} else {
						throw new HtException(getContext(), "queryPositions", "Invalid detail flag");
				}

		}

		public void queryAccountDetails(
			TransactionManager trx,
			HtMtEventWrapperAccountHistory r,
			String accountId,
			String companyName,
			int detail_flag
		) throws Exception {
				// first get metainfo
				r.getBalanceChange().clear();
				r.getAccountChange().clear();

				r.setAccountChangesFlag(MainPersistManager.QUERY_ACCOUNT_DEAFULT);

				// sellect account metainfo
				Session sess = trx.getSession();
				SQLQuery query = sess.createSQLQuery("SELECT ami.id, cmi.company_name,  ami.account_name, ami.server, ami.holder_name, ami.currency, ami.leverage, "
					+ "balpdt.balance "
					+ "from {h-schema}company_metainfo cmi, {h-schema}account_metainfo ami, {h-schema}balance_update balpdt "
					+ " where  "
					+ "ami.company_minfo_id = cmi.id "
					+ "and balpdt.account_minfo_id = ami.id "
					+ "and balpdt.id= "
					+ "    (select   "
					+ "	max(b.id) from  "
					+ "	{h-schema}balance_update b where "
					+ "	b.account_minfo_id = ami.id )  "
					+ "and ami.account_name= :account_name_p  "
					+ "and cmi.company_name=:company_name_p "
					+ "union all "
					+ "SELECT ami.id, cmi.company_name, ami.account_name, ami.server, ami.holder_name, ami.currency, ami.leverage, "
					+ "null "
					+ "from {h-schema}company_metainfo cmi, {h-schema}account_metainfo ami "
					+ "where  "
					+ "ami.company_minfo_id = cmi.id "
					+ "and not exists "
					+ "( "
					+ "   select 1 "
					+ "    from {h-schema}balance_update balpdt "
					+ "	where balpdt.account_minfo_id = ami.id "
					+ ") "
					+ "and ami.account_name= :account_name_p  "
					+ "and cmi.company_name=:company_name_p");

				query.addScalar("id", BigIntegerType.INSTANCE).
					addScalar("company_name", StringType.INSTANCE).
					addScalar("account_name", StringType.INSTANCE).
					addScalar("server", StringType.INSTANCE).
					addScalar("holder_name", StringType.INSTANCE).
					addScalar("currency", StringType.INSTANCE).
					addScalar("leverage", BigIntegerType.INSTANCE).
					addScalar("balance", BigIntegerType.INSTANCE).
					setParameter("company_name_p", companyName).
					setParameter("account_name_p", accountId);

				Object[] result = this.<Object[]>getOnlyOneItem(query.list());
				if (result == null) {
						// now rows
						return;
				}
						
				r.setInit();
				long accminfo_id = ((BigInteger) result[0]).longValue();

				r.setAccountId((String) result[2]);
				r.setCurrency((String) result[5]);
				r.setHolderName((String) result[4]);
				r.setLeverage(((BigInteger) result[6]).intValue());
				r.setCompanyName((String) result[1]);
				r.setLastBalance(((BigInteger) result[7]).intValue());
				r.setServer((String) result[3]);

				if ((detail_flag & MainPersistManager.QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY) > 0) {

						r.setAccountChangesFlag(MainPersistManager.QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY);

						// get history balance
						query = (SQLQuery) sess.createSQLQuery("select balance from {h-schema}balance_update where account_minfo_id= :accminfoid").
							addScalar("balance", BigIntegerType.INSTANCE).
							setParameter("accminfoid", accminfo_id);

						List<BigInteger> res = query.list();
						for (int i = 0; i < res.size(); i++) {
								HtMtEventWrapperAccountHistory.BalanceChange bce = new HtMtEventWrapperAccountHistory.BalanceChange();
								bce.setBalance(res.get(i).longValue());

								r.getBalanceChange().add(bce);
						}
				}

				if ((detail_flag & MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY) > 0) {

						r.setAccountChangesFlag(MainPersistManager.QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY);
						// get account history
						query = (SQLQuery) sess.createSQLQuery("select * from {h-schema}account_update where account_minfo_id= :accminfoid").
							addEntity(HtMtAccountUpdate.class).
							setParameter("accminfoid", accminfo_id);

						List<HtMtAccountUpdate> res_accupd = query.list();
						for (int i = 0; i < res_accupd.size(); i++) {
								HtMtAccountUpdate accupd_e = res_accupd.get(i);

								HtMtEventWrapperAccountHistory.AccountChange acc = new HtMtEventWrapperAccountHistory.AccountChange();

								acc.setEquity(accupd_e.getEquity());
								acc.setFreeMargine(accupd_e.getFreeMargine());
								acc.setFreeMargineMode(accupd_e.getFreeMargineMode());
								acc.setStopOutLevel(accupd_e.getStopOutLevel());
								acc.setStopOutMode(accupd_e.getStopOutMode());

								r.getAccountChange().add(acc);

						}
				}

		}

		public void initialize() throws Exception {
				try {
						if (sessFactory_m != null) 
								return;
						

						Configuration configuration = new Configuration();
						// we create some configuration fil dynamically
						
						String xmlConfContent = initHibernateXMLConfiguration();
						
						File tmpf = File.createTempFile("hbconf", "");
						HtFileUtils.writeContentToFile(tmpf, xmlConfContent);
						
						/*
						String configfile = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getConfigDirectory()
							+ File.separatorChar + "hibernate.cfg.xml";

						File f_configfile = new File(configfile);
						if (!f_configfile.exists()) {
								throw new HtException(getContext(), "initialize", "Configuration file does not exist");
						}
						*/

						initHibernateConfiguration(configuration);
						configuration.configure(tmpf);
						
						tmpf.delete();
						
						ServiceRegistryBuilder serviceRegistryBuilder = new ServiceRegistryBuilder().applySettings(configuration.getProperties());
						sessFactory_m = configuration.buildSessionFactory(serviceRegistryBuilder.buildServiceRegistry());
						
						// create tables if not created
						createDBTables(false);
						XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_INFO, "Creates tables with non-force flag");
						

				} catch (Throwable me) {

						throw new HtException(getContext(), "initialize: \n", HtException.getFullErrorString(me));
				}

		}

		public void deinitialize() throws Exception {
				if (sessFactory_m != null) {
						sessFactory_m.close();
						sessFactory_m = null;
				}
		}

		public SessionFactory getSessionFactory() {
				return sessFactory_m;
		}

		public HtMtEventWrapperPositionChange save_HtMtPosition(
			TransactionManager trx, 
			HtMtPosition new_pos, 
			HtMtAccountMetainfo accminfo, 
			HtMtCompanyMetainfo compminfo
		) throws Exception {

				// event is here - can generate
				Session sess = trx.getSession();
				new_pos.setAccountMetainfoUid(accminfo.getAccountMetanfoUid());

				// query existing records from position
				Query query = sess.createSQLQuery("select p.* from {h-schema}position p where p.account_minfo_id=:accmid and p.ticket = :tckt").
					addEntity(HtMtPosition.class).
					setParameter("accmid", accminfo.getAccountMetanfoUid()).
					setParameter("tckt", new_pos.getTicket());

				List<HtMtPosition> positions = query.list();
				HtMtPosition positions_read_result = getOnlyOneItem(positions);

				if (positions_read_result == null) {
						// just insert and exit - we don't need position update
						sess.save(new_pos);
						sess.flush();

						// need to create initial update
						// sl, tp, profit
						HtMtPositionUpdate initial_update_object = new_pos.createUpdateObject();

						// save
						HtMtEventWrapperPositionChange e = new HtMtEventWrapperPositionChange();
						e.setAccountId(accminfo.getAccountId());
						e.setCompanyName(compminfo.getCompanyName());

						insertOrUpdate_HtMtPositionUpdate(trx, initial_update_object);
						e.initializeFromPos(new_pos);

						// send event
						return e;

				} else // update
				{
						new_pos.setPositionUid(positions_read_result.getPositionUid());
				}

				if (positions_read_result.equalTo(new_pos)) {
						// nothing to chnage
						// just update PK
						new_pos.setPositionUid(positions_read_result.getPositionUid());
						return null;
				}

				// 1) check if we have an update
				HtMtPositionUpdate new_pos_update = positions_read_result.doWeHaveUpdateObject(new_pos);

				if (new_pos_update != null) {
						// go update position update record if necessary
						insertOrUpdate_HtMtPositionUpdate(trx, new_pos_update);
				}

				// update if we have differencse
				// remove old obj
				//sess.evict(positions_read_result);
				// update
				//sess.update(new_pos);
				// update in cache
				positions_read_result.assign(new_pos);
				sess.flush();

				//send
				HtMtEventWrapperPositionChange e = new HtMtEventWrapperPositionChange();
				e.setAccountId(accminfo.getAccountId());
				e.setCompanyName(compminfo.getCompanyName());

				e.initializeFromPos(new_pos);
				return e;

		}

		private void insertOrUpdate_HtMtPositionUpdate(TransactionManager trx, HtMtPositionUpdate new_posupdate) throws Exception {
				// if we need to send HtMtPositionUpdate event
				Session sess = trx.getSession();

				// look for max ro based on FK - position_id
				Query queryPos_update = sess.createSQLQuery("select * from {h-schema}position_update where position_id=:posid and"
					+ " id=( select max(id) from {h-schema}position_update where position_id=:posid )").
					addEntity(HtMtPositionUpdate.class).
					setParameter("posid", new_posupdate.getPosUpdateUid());

				// one row expected
				HtMtPositionUpdate positions_update_read_result = this.<HtMtPositionUpdate>getOnlyOneItem(queryPos_update.list());

				if (positions_update_read_result == null) {
						// insert only
						sess.save(new_posupdate);
						sess.flush();

				} else {
						// check if we have the same row
						if (!positions_update_read_result.compareTo(new_posupdate)) {
								// insert new row 
								sess.save(new_posupdate);
								sess.flush();

						} else {
								// just update PK
								new_posupdate.setPosUpdateUid(positions_update_read_result.getPosUpdateUid());
						}

				}

		}

		public HtMtEventWrapperAccountChange save_HtMtAccountUpdate(TransactionManager trx, HtMtAccountUpdate accupdt, HtMtAccountMetainfo accminfo,HtMtCompanyMetainfo compminfo) throws Exception {
				Session sess = trx.getSession();
				accupdt.setAccountMetainfoUid(accminfo.getAccountMetanfoUid());
				

				// select existing row
				Query query = sess.createSQLQuery("select id, account_minfo_id, "
					+ "equity, free_margin, free_margin_mode, stop_out, stop_out_mode from "
					+ "{h-schema}account_update where account_minfo_id=:accmid and id= ( select max(id) from {h-schema}account_update where "
					+ "account_minfo_id=:accmid)").
					addEntity(HtMtAccountUpdate.class).
					setParameter("accmid", accupdt.getAccountMetainfoUid());

				List<HtMtAccountUpdate> res = query.list();
				HtMtAccountUpdate account_update_result = getOnlyOneItem(res);

				boolean event_to_send = false;
				// compare only with the last row
				if (account_update_result == null) {
						// insert update
						sess.save(accupdt);
						sess.flush();

						// send update event
						event_to_send = true;
				} else if (!account_update_result.equalTo(accupdt)) {
						// insert and send update event
						sess.save(accupdt);
						sess.flush();

						event_to_send = true;
				}

				if (event_to_send) {
						HtMtEventWrapperAccountChange e = new HtMtEventWrapperAccountChange();

						e.setAccountId(accminfo.getAccountId());
						e.setEquity(accupdt.getEquity());
						e.setFreeMargine(accupdt.getFreeMargine());
						e.setFreeMargineMode(accupdt.getFreeMargineMode());
						e.setStopOutLevel(accupdt.getStopOutLevel());
						e.setStopOutMode(accupdt.getStopOutMode());
						e.setServer(accminfo.getServer());
						e.setCompanyName(compminfo.getCompanyName());

						return e;
				}

				// just read the PK
				accupdt.setAccountUpdateUid(account_update_result.getAccountUpdateUid());

				return null;

		}

		public HtMtEventWrapperBalanceChange save_HtBalanceUpdate(
			TransactionManager trx, 
			HtMtBalanceUpdate balupd, 
			HtMtAccountMetainfo accminfo,
			HtMtCompanyMetainfo compminfo
		) throws Exception {
				Session sess = trx.getSession();
				balupd.setAccountMetainfoUid(accminfo.getAccountMetanfoUid());

				// select existing row
				Query query = sess.createSQLQuery("select id, account_minfo_id, balance from "
					+ "{h-schema}balance_update where account_minfo_id=:accmid and id= ( select max(id) from {h-schema}balance_update where "
					+ "account_minfo_id=:accmid)").
					addEntity(HtMtBalanceUpdate.class).
					setParameter("accmid", balupd.getAccountMetainfoUid());

				List<HtMtBalanceUpdate> res = query.list();
				HtMtBalanceUpdate balance_update_result = getOnlyOneItem(res);

				boolean event_to_send = false;
				// compare only with the last row
				if (balance_update_result == null) {
						// insert update
						sess.save(balupd);
						sess.flush();

						event_to_send = true;
						// send update event
				} else if (!balance_update_result.equalTo(balupd)) {
						// insert and send update event
						sess.save(balupd);
						sess.flush();

						event_to_send = true;

				}

				if (event_to_send) {
						HtMtEventWrapperBalanceChange e = new HtMtEventWrapperBalanceChange();

						e.setAccountId(accminfo.getAccountId());
						e.setBalance(balupd.getBalance());
						e.setServer(accminfo.getServer());
						e.setCompanyName(compminfo.getCompanyName());

						return e;
				}

				// just read the PK
				balupd.setBalanceUpdateUid(balance_update_result.getBalanceUpdateUid());
				return null;
		}

		public boolean save_HtMtAccountMetainfo(TransactionManager trx, HtMtCompanyMetainfo compminfo, HtMtAccountMetainfo accminfo) throws Exception {
				Session sess = trx.getSession();

				accminfo.setCompanyMetanfoUid(compminfo.getCompanyMetanfoUid());

				// resolve company metainfo first
				Query query = sess.createSQLQuery("select id from {h-schema}account_metainfo where company_minfo_id = :cmf and account_name = :anm").
					addScalar("id", BigIntegerType.INSTANCE).
					setParameter("cmf", compminfo.getCompanyMetanfoUid()).
					setParameter("anm", accminfo.getAccountId());

				List<BigInteger> res = query.list();
				BigInteger id_result = getOnlyOneItem(res);

				if (id_result == null) {
						sess.save(accminfo);
						sess.flush();

						// 
						return true;

				}

				accminfo.setAccountMetanfoUid(id_result.longValue());
				return false;
		}

		// returns true if we need to push an event
		public boolean save_HtMtCompanyMetainfo(TransactionManager trx, HtMtCompanyMetainfo compminfo) throws Exception {
				Session sess = trx.getSession();

				Query query = sess.createSQLQuery("select id from {h-schema}company_metainfo where company_name = :cn").
					addScalar("id", BigIntegerType.INSTANCE).
					setParameter("cn", compminfo.getCompanyName());

				List<BigInteger> res = query.list();
				BigInteger id_result = getOnlyOneItem(res);

				if (id_result == null) {
						sess.save(compminfo);
						sess.flush();

						// send update event
						return true;
				}

				compminfo.setCompanyMetanfoUid(id_result.longValue());
				return false;

		}

		// helper to return a single item
		private <T> T getOnlyOneItem(List<T> datalst) throws Exception {
				if (datalst.isEmpty()) {
						return null;
				}

				if (datalst.size() != 1) {
						throw new HtException(getContext(), "getOnlyOneItem", "List must have only one single record");
				}

				return datalst.get(0);
		}

		private void queryPositions_withDetails(
			TransactionManager trx,
			HtMtEventWrapperPositionsHistory r,
			String accountId,
			int flag_state,
			long openTimeFilter,
			long closeTimeFilter,
			String companyName
		) throws Exception {
				Session sess = trx.getSession();
				r.setPositionChangesProvided(true);

				SQLQuery query = null;

				String sql_filter = "";
				if (openTimeFilter >= 0) {
						sql_filter = " and open_time >= :open_time_p";
				}

				if (closeTimeFilter >= 0) {
						sql_filter = " and open_time >= :open_time_p";
				}

				if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_OPEN || flag_state == MainPersistManager.QUERY_POSITION_ONLY_HISTORY) {

						query = sess.createSQLQuery("select {p.*}, {pup.*} from "
							+ " {h-schema}position p, {h-schema}account_metainfo am, {h-schema}position_update pup, {h-schema}company_metainfo cmi where "
							+ " p.account_minfo_id= am.id and pup.position_id=p.id and am.account_name= :account_name_p "
							+ " and pos_state = :poststate_p and cmi.id=am.company_minfo_id and cmi.company_name=:company_name_p"
							+ sql_filter
							+ " order by p.id asc,  pup.position_id asc ");

						if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_OPEN) {
								query.setParameter("poststate_p", PosState.STATE_OPEN);
						} else {
								query.setParameter("poststate_p", PosState.STATE_HISTORY);
						}

				} else if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_BOTH) {
						query = sess.createSQLQuery("select {p.*}, {pup.*} from "
							+ " {h-schema}position p, {h-schema}account_metainfo am, {h-schema}position_update pup, {h-schema}company_metainfo cmi where "
							+ " p.account_minfo_id= am.id and pup.position_id=p.id and am.account_name= :account_name_p "
							+ " and cmi.id=am.company_minfo_id and cmi.company_name=:company_name_p "
							+ sql_filter
							+ " order by p.id asc,  pup.position_id asc ");
				} else {
						throw new HtException(getContext(), "queryPositions_noDetails", "Invalid state flag");
				}

				query.setParameter("account_name_p", accountId);
				query.setParameter("company_name_p", companyName);

				query.addEntity("p", HtMtPosition.class);
				query.addEntity("pup", HtMtPositionUpdate.class);

				if (openTimeFilter >= 0) {
						query.setParameter("open_time_p", openTimeFilter);
				}
				if (closeTimeFilter >= 0) {
						query.setParameter("close_time_p", closeTimeFilter);
				}

				// execute
				List<Object[]> positions = query.list();
				long current_pos_id = -1;

				List<HtMtEventWrapperPositionsHistory.PositionChange> pos_updates = new ArrayList<HtMtEventWrapperPositionsHistory.PositionChange>();
				for (int i = 0; i < positions.size(); i++) {
						HtMtPosition pos = (HtMtPosition) positions.get(i)[0];
						HtMtPositionUpdate pos_update = (HtMtPositionUpdate) positions.get(i)[1];

						HtMtEventWrapperPositionsHistory.PositionChange pos_chnage = new HtMtEventWrapperPositionsHistory.PositionChange();
						pos_chnage.setProfit(pos_update.getProfit());
						pos_chnage.setStopLoss(pos_update.getStopLoss());
						pos_chnage.setTakeProfit(pos_update.getTakeProfit());

						pos_updates.add(pos_chnage);

						if (current_pos_id == -1) {
								current_pos_id = pos.getPositionUid();
								continue;
						}

						if (pos.getPositionUid() != current_pos_id) {

								HtMtEventWrapperPositionsHistory.PositionEntry pose = new HtMtEventWrapperPositionsHistory.PositionEntry();
								pose.getHtMtEventWrapperPositionChange().initializeFromPos(pos);
								pose.getChanges().addAll(pos_updates);
								pose.getHtMtEventWrapperPositionChange().setAccountId(accountId);

								// add entry
								r.getPositions().put(pos.getPositionUid(), pose);

								// switch
								pos_updates.clear();
								current_pos_id = pos.getPositionUid();
						}

						// they are duplicated
				}

		}

		private void queryPositions_noDetails(
			TransactionManager trx,
			HtMtEventWrapperPositionsHistory r,
			String accountId,
			int flag_state,
			long openTimeFilter,
			long closeTimeFilter,
			String companyName
		) throws Exception {
				Session sess = trx.getSession();
				r.setPositionChangesProvided(false);

				SQLQuery query = null;

				String sql_filter = "";
				if (openTimeFilter >= 0) {
						sql_filter = " and open_time >= :open_time_p";
				}

				if (closeTimeFilter >= 0) {
						sql_filter = " and open_time >= :open_time_p";
				}

				if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_OPEN || flag_state == MainPersistManager.QUERY_POSITION_ONLY_HISTORY) {

						query = sess.createSQLQuery("select p.* from {h-schema}position p, {h-schema}account_metainfo am, {h-schema}company_metainfo cmi where "
							+ " p.account_minfo_id= am.id and am.account_name= :account_name_p and pos_state = :poststate_p "
							+ " and cmi.id=am.company_minfo_id and cmi.company_name=:company_name_p "
							+ sql_filter
							+ "order by p.id asc");

						if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_OPEN) {
								query.setParameter("poststate_p", PosState.STATE_OPEN);
						} else {
								query.setParameter("poststate_p", PosState.STATE_HISTORY);
						}

				} else if (flag_state == MainPersistManager.QUERY_POSITION_ONLY_BOTH) {
						query = sess.createSQLQuery("select p.* from {h-schema}position p, {h-schema}account_metainfo am, {h-schema}company_metainfo cmi where "
							+ " p.account_minfo_id=am.id and am.account_name=:account_name_p "
							+ " and cmi.id=am.company_minfo_id and cmi.company_name=:company_name_p "
							+ sql_filter
							+ " order by p.id asc");
				} else {
						throw new HtException(getContext(), "queryPositions_noDetails", "Invalid state flag");
				}

				query.addEntity(HtMtPosition.class);
				query.setParameter("account_name_p", accountId);
				query.setParameter("company_name_p", companyName);

				if (openTimeFilter >= 0) {
						query.setParameter("open_time_p", openTimeFilter);
				}
				if (closeTimeFilter >= 0) {
						query.setParameter("close_time_p", closeTimeFilter);
				}

				// execute
				List<HtMtPosition> positions = query.list();
				for (int i = 0; i < positions.size(); i++) {
						HtMtPosition pos = positions.get(i);

						HtMtEventWrapperPositionsHistory.PositionEntry pose = new HtMtEventWrapperPositionsHistory.PositionEntry();
						pose.getHtMtEventWrapperPositionChange().initializeFromPos(pos);
						pose.getHtMtEventWrapperPositionChange().setAccountId(accountId);

						// add entry
						r.getPositions().put(pos.getPositionUid(), pose);

				}

				// that's it!
		}
		
		// create mapping
		private String initHibernateXMLConfiguration() throws Exception {
				StringBuilder s = new StringBuilder();
				
				s.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
				s.append("<!DOCTYPE hibernate-configuration SYSTEM \"http://www.hibernate.org/dtd/hibernate-configuration-3.0.dtd\">");
				s.append("<hibernate-configuration>");
				s.append("<session-factory>");
				s.append("<mapping class=\"").append(HtMtCompanyMetainfo.class.getCanonicalName()).append("\" />");
				s.append("<mapping class=\"").append(HtMtAccountMetainfo.class.getCanonicalName()).append("\" />");
				s.append("<mapping class=\"").append(HtMtAccountUpdate.class.getCanonicalName()).append("\" />");
				s.append("<mapping class=\"").append(HtMtPosition.class.getCanonicalName()).append("\" />");
				s.append("<mapping class=\"").append(HtMtPositionUpdate.class.getCanonicalName()).append("\" />");
				s.append("<mapping class=\"").append(HtMtBalanceUpdate.class.getCanonicalName()).append("\" />");
				s.append("</session-factory>");
				s.append("</hibernate-configuration>");
				
				return s.toString();
		}
		
		private void initHibernateConfiguration(Configuration config) throws Exception {
		
				
			
			Connection conn  = RtDatabaseManager.instance().resolveSQLConnection();
			Map<String, String> props = RtDatabaseManager.instance().getDbConnectData(conn);
			
			//String user = props.get("db_user");
			//String password = props.get("db_password");
			//String url = "jdbc:mysql://localhost:"+ props.get("db_port") + "/" + DB_SCHEME;
			
			//System.setProperty("hibernate.connection.url",url);
			System.setProperty("hibernate.connection.datasource",HtStartupConstants.JNDI_CONNECT_POOL_LOCATION);
			//System.setProperty("hibernate.default_schema", MainPersistManager.DB_SCHEME );
			
			//System.setProperty("hibernate.connection.password",password);
			//System.setProperty("hibernate.connection.username",user);
			//System.setProperty("hibernate.connection.driver_class", "com.mysql.jdbc.Driver");
			//System.setProperty("hibernate.connection.pool_size", "1");
			
			System.setProperty("hibernate.dialect", "org.hibernate.dialect.MySQLInnoDBDialect");
			System.setProperty("hibernate.current_session_context_class", "thread");
			System.setProperty("hibernate.show_sql", "false");
			System.setProperty("hibernate.cache.use_second_level_cache", "false");
			//System.setProperty("hibernate.hbm2ddl.auto", "false");
			
			// mapping
			
			config.setProperties(System.getProperties());
			config.setProperty(Environment.DEFAULT_SCHEMA, MainPersistManager.DB_SCHEME);
		
}

};
