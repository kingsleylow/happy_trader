
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.service;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.mtrunner.client.HappyTraderTransport.CachedRequestHelper;
import com.mtrunner.client.userlist.dao.AccountDAO;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;

import java.io.BufferedOutputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.hibernate.Criteria;
import org.hibernate.Hibernate;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.criterion.Restrictions;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

/**
 *
 * @author Administrator
 */
@Service
public class AccountServiceImpl implements AccountService {

	@Autowired
	private AccountDAO accDAO_m;
	
	

	@Transactional
	@Override
	public void addAccount(Account account) {
		Date now = new Date(System.currentTimeMillis());
		account.setCreated(now);
		account.setEnabled(1);
		account.setLastUpdated(now);

		accDAO_m.getSessionFactory().getCurrentSession().save(account);
	}

	@Transactional
	@Override
	public void updateAccount(Account account) {
		Date now = new Date(System.currentTimeMillis());
		account.setLastUpdated(now);
		account.setEnabled(1);
		accDAO_m.getSessionFactory().getCurrentSession().update(account);
	}

	@Transactional
	@Override
	public User resolveUser(int userId) {
		if (userId < 0) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		User user = (User) sess.get(User.class, userId);

		return user;
	}
	
	@Transactional
	@Override
	public User resolveUserByName(String userName) {
		if (userName  == null) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		
		String hql = "from User where UserName = :cn";
		Query query = sess.createQuery(hql).setParameter("cn", userName);
		
		List<User> res = query.list();
		if (res.size() != 1)
			return null;

		User user= res.get(0);
		Hibernate.initialize(user.getAccounts());
		
		return user;
	}
	
	@Transactional
	@Override
	public void removeDisabledAccounts()
	{
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		String hql = "delete from Account where enabled=0";
		
		Query query = sess.createQuery(hql);
		query.executeUpdate();
		
	}

	
	@Transactional
	@Override
	public Company resolveCompany(int companyId) {
		if (companyId < 0) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		Company company = (Company) sess.get(Company.class, companyId);

		return company;
	}
	
	@Transactional
	@Override
	public Company resolveCompanyByName(String companyName) 
	{
		if (companyName == null) {
			return null;
		}
		
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		
		String hql = "from Company where CompanyName = :cn";
		Query query = sess.createQuery(hql).setParameter("cn", companyName);
		
		List<Company> res = query.list();
		if (res.size() == 1)
			return res.get(0);
		
		return null;
	}
	
	@Transactional
	@Override
	public Company resolveCompanyFetchEnabledAccounts(int companyId)
	{
		if (companyId < 0) {
			return null;
		}
		
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		//Company company = (Company) sess.get(Company.class, companyId);
		
		
			
		String hql = "from   Company c  where c.id= :company_id and exists  (   "
						+ "from Account acc  where acc.company = c and acc.enabled = :enabled)	";
		Query query = sess.createQuery(hql).setParameter("company_id", companyId).setParameter("enabled", 1);
		
		List<Company> res = query.list();
		if (res.isEmpty()) {
			return (Company) sess.get(Company.class, companyId);
		}
		
		if (res.size() != 1)
			throw new RuntimeException("Should not happen");
		Company comp = res.get(0);
		Hibernate.initialize(comp.getAccounts());

		return comp;
	}
	
	@Transactional
	@Override
	public Company resolveCompanyFetchAccounts(int companyId) {
		if (companyId < 0) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		Company company = (Company) sess.get(Company.class, companyId);
		
		Hibernate.initialize(company.getAccounts());

		return company;
	}
	

	@Transactional
	@Override
	public Account resolveAccount(int accountId) {
		if (accountId < 0) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		Account account = (Account) sess.get(Account.class, accountId);

		return account;
	}
	
	@Transactional
	@Override
	public Server resolveServer(int serverId)
	{
		if (serverId < 0) {
			return null;
		}

		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		Server server = (Server) sess.get(Server.class, serverId);

		return server;
	}

	@Transactional
	@Override
	public List<Account> listAccounts() {
		return accDAO_m.getSessionFactory().getCurrentSession().createQuery("from Account").list();
	}

	@Transactional
	@Override
	public List<User> listUsers() {
		return accDAO_m.getSessionFactory().getCurrentSession().createQuery("from User").list();
	}

	@Transactional
	@Override
	public List<Company> listCompaniesFetchAccountsServers()
	{
		List<Company> companies =  accDAO_m.getSessionFactory().getCurrentSession().createQuery("from Company").list();
		
		for(int i = 0; i < companies.size(); i++) {
				Company company = companies.get(i);
				Hibernate.initialize(company.getAccounts());
				Hibernate.initialize(company.getServers());
		}
		
		return companies;
	}
	
	@Transactional
	@Override
	public List<Company> listCompanies() {
		List<Company> companies =  accDAO_m.getSessionFactory().getCurrentSession().createQuery("from Company").list();
		
		return companies;
	}
	
	@Transactional
	@Override
	public List<Server> listServers()
	{
		return accDAO_m.getSessionFactory().getCurrentSession().createQuery("from Server").list();
	}
	
	@Transactional
	@Override
	public List<Server> listServers(int companyId)
	{
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();
		
		Criteria criteria= sess.createCriteria(Server.class);
		
		return criteria.createAlias("company", "c").add(Restrictions.eq("c.id", companyId)).list();
	}

	@Transactional
	@Override
	public void enableAccount(int id, boolean enable) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Account account = (Account) sess.load(Account.class, id);
		if (null != account) {

			account.setEnabled(enable ? 1 : 0);
			account.setLastUpdated(new Date(System.currentTimeMillis()));
			sess.save(account);
		}

	}

	@Transactional
	@Override
	public void enableUser(int id, boolean enable) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		User user = (User) sess.load(User.class, id);
		if (null != user) {

			user.setEnabled(enable);
			user.setLastUpdated(new Date(System.currentTimeMillis()));
			sess.save(user);
		}
	}

	@Transactional
	@Override
	public void addUser(User user) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());
		user.setCreated(now);
		user.setLastUpdated(now);
		user.setEnabled(true);

		sess.save(user);
	}

	@Transactional
	@Override
	public void updateUser(User user) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());

		user.setLastUpdated(now);
		user.setEnabled(true);

		sess.update(user);
	}

	@Transactional
	@Override
	public void addCompany(Company company) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());

		company.setLastUpdated(now);
		company.setCreated(now);
		company.setEnabled(1);

		sess.save(company);
	}

	@Transactional
	@Override
	public void updateCompany(Company company) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());

		company.setLastUpdated(now);
		company.setEnabled(1);

		sess.update(company);
	}

	@Transactional
	@Override
	public void enableCompany(int id, boolean enable) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Company company = (Company) sess.load(Company.class, id);
		if (null != company) {

			company.setEnabled(enable ? 1 : 0);
			company.setLastUpdated(new Date(System.currentTimeMillis()));
			sess.save(company);
		}
	}

	@Transactional
	@Override
	public void addServer(Server server) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());

		server.setLastUpdated(now);
		server.setCreated(now);
		server.setEnabled(1);

		sess.save(server);
	}

	@Transactional
	@Override
	public void updateServer(Server server) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Date now = new Date(System.currentTimeMillis());

		server.setLastUpdated(now);
		server.setEnabled(1);

		sess.update(server);
	}

	@Transactional
	@Override
	public void enableServer(int id, boolean enable) {
		Session sess = accDAO_m.getSessionFactory().getCurrentSession();

		Server server = (Server) sess.load(Server.class, id);
		if (null != server) {

			server.setEnabled(enable ? 1 : 0);
			Date now = new Date(System.currentTimeMillis());
			
			server.setLastUpdated(now);
			sess.save(server);
		}
	}

	@Override
	public boolean isAccountEnabled(Account account)
	{
		
		if (account.getUser().isEnabled() && account.isEnabled() == 1 & account.getCompany().isEnabled() == 1)
			return true;
		
		return false;
	}
	
	@Transactional
	@Override
	public void createSetupFile(OutputStream out) throws Exception {
		
		
		BufferedOutputStream bout = new BufferedOutputStream(out);
		// read all companies
		List<Account> accounts = listAccounts();
		for(Account acc: accounts) {
			if (isAccountEnabled(acc) ) {
				String line = acc.getAccount_id() +"\t" + acc.getInvest_password() + "\t" + acc.getServer().getServer_name() + "\t-1\n";
				bout.write(line.getBytes());
			}
			
			
		}
		
		bout.flush();
	}
	
	

}
