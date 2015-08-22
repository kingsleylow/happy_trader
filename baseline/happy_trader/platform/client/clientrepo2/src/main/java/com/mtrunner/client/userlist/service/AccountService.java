/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.service;

import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;
import java.io.OutputStream;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Administrator
 */
public interface AccountService {

	public void addAccount(Account account);

	public void updateAccount(Account account);
	
	public void removeDisabledAccounts();

	public List<Account> listAccounts();

	public List<User> listUsers();

	public List<Company> listCompanies();
	
	public List<Company> listCompaniesFetchAccountsServers();

	public List<Server> listServers();

	public List<Server> listServers(int companyId);

	public User resolveUser(int userId);
	
	public User resolveUserByName(String userName);

	public Company resolveCompany(int companyId);
	
	public Company resolveCompanyByName(String companyName);
	
	public Company resolveCompanyFetchAccounts(int companyId);
	
	public Company resolveCompanyFetchEnabledAccounts(int companyId);

	public Account resolveAccount(int accountId);

	public Server resolveServer(int serverId);

	public void enableAccount(int id, boolean enable);

	public void enableUser(int id, boolean enable);

	public void addUser(User user);

	public void updateUser(User user);

	public void addCompany(Company company);

	public void updateCompany(Company company);

	public void enableCompany(int id, boolean enable);

	public void addServer(Server server);

	public void updateServer(Server server);

	public void enableServer(int id, boolean enable);

	public void createSetupFile(OutputStream out) throws Exception;
	
	public boolean isAccountEnabled(Account account);
	
	
	
}
