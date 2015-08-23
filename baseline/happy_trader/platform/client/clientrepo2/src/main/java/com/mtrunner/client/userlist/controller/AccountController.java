/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller;

import com.mtrunner.client.userlist.controller.bind.UserEditorSupport;
import com.mtrunner.client.userlist.controller.bind.ServerEditorSupport;
import com.mtrunner.client.userlist.controller.bind.CompanyEditorSupport;
import com.mtrunner.client.userlist.controller.bind.GeneralDateEditorSupport;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import com.mtrunner.client.utils.GeneralUtils;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;


import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.ui.ModelMap;
import org.springframework.util.AutoPopulatingList;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.InitBinder;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;

@Controller
@RequestMapping("/account")
public class AccountController {

	@Autowired
	private AccountService accountService_m;

	@Autowired
	private UserEditorSupport userEditor_m;

	@Autowired
	private CompanyEditorSupport companyEditor_m;
	
	@Autowired
	private ServerEditorSupport serverEditor_m;
	
	@Autowired
	private GeneralDateEditorSupport dateEditor_m;

	@InitBinder
	public void initBinder(WebDataBinder binder) {
		binder.registerCustomEditor(User.class, this.userEditor_m);
		binder.registerCustomEditor(Company.class, this.companyEditor_m);
		binder.registerCustomEditor(Server.class, this.serverEditor_m);
		binder.registerCustomEditor(Date.class, "created", this.dateEditor_m);
	}
	
	
	@RequestMapping("/user_accounts.html")
	public String listAccountsInitial(Map<String, Object> map) throws Exception
	{
		
		createUserAccountDefaultPageContext(map, -1);
		return "user_accounts";
	}
	
	@RequestMapping("/user_accounts/{company_id}/do.html")
	public String listAccounts(@PathVariable("company_id") Integer companyId, Map<String, Object> map) throws Exception
	{
		
		// this is special stuff for company navigation
		if (companyId == null || companyId < 0)
			createUserAccountDefaultPageContext(map, -1);
		else
			createUserAccountDefaultPageContext(map, companyId);
		
		
		return "user_accounts";
	}
	
	@RequestMapping("/user_accounts/{company_id}/{account_id}/do.html")
	public String listAccountsKeepAccout(
					@PathVariable("company_id") Integer companyId, 
					@PathVariable("account_id") Integer accountId, 
					Map<String, Object> map
	) throws Exception
	{
		
		// this is special stuff for company navigation
		if (companyId == null || companyId < 0)
			createUserAccountDefaultPageContext(map, -1);
		else
			createUserAccountDefaultPageContext(map, companyId);
		
		Account resolvedAccount = accountService_m.resolveAccount(accountId);
		if (resolvedAccount==null)
			throw new Exception("Invalid account id: " + accountId );
			
		map.put("accountObj", resolvedAccount);
		
		
		return "user_accounts";
	}

	@RequestMapping("/disable_user_account/{id}/do.html")
	public String disableUserAccount(@PathVariable("id") Integer accId, Map<String, Object> map) throws Exception
	{
		accountService_m.enableAccount(accId, false);
		createUserAccountDefaultPageContext(map, -1);
		
		return "user_accounts";
	}

	@RequestMapping("/restore_user_account/{id}/do.html")
	public String restoreuserAccount(@PathVariable("id") Integer accId, Map<String, Object> map) throws Exception
	{
		accountService_m.enableAccount(accId, true);
		createUserAccountDefaultPageContext(map, -1);
		
		return "user_accounts";
	}

	@RequestMapping(params = "add", value = "/add_account.html", method = RequestMethod.POST)
	public String addAccount( @ModelAttribute("account") Account account, BindingResult result, Map<String, Object> map) throws Exception
	{
		accountService_m.addAccount(account);
		// after we added account
		
		
		createUserAccountDefaultPageContext(map, -1);
		return "user_accounts";
	}
	
	@RequestMapping(params = "update", value = "/add_account.html", method = RequestMethod.POST)
	public String updateAccount( @ModelAttribute("account") Account account, BindingResult result, Map<String, Object> map) throws Exception
	{
		
		accountService_m.updateAccount(account);
		createUserAccountDefaultPageContext(map, account.getCompany().getId());
		return "user_accounts";
	}
	
	@RequestMapping(params = "remove", value = "/add_account.html", method = RequestMethod.POST)
	public String removeAccount( @ModelAttribute("account") Account account, BindingResult result, Map<String, Object> map) throws Exception
	{
		
		// this removes disabled accounst
		accountService_m.removeDisabledAccounts();
		createUserAccountDefaultPageContext(map, -1);
		return "user_accounts";
	}
	

	@RequestMapping(value = "/edit_account/{id}/do.html", method = RequestMethod.GET)
	public String editAccount(@PathVariable("id") Integer accId, Map<String, Object> map) throws Exception
	{
		
		// this just populate fields
		
		// edit account
		Account resolvedAccount = accountService_m.resolveAccount(accId);
		if (resolvedAccount==null)
			throw new Exception("Invalid account id: " + accId );
			
		map.put("accountObj", resolvedAccount);
		createUserAccountDefaultPageContext(map, resolvedAccount.getCompany().getId());

		//return "redirect:/account/user_accounts";
		return "user_accounts";
	}
	
	/**
	 * Helpers
	 */
	
	private void createUserAccountDefaultPageContext(Map<String, Object> map, int companyId ) throws Exception
	{
		map.put("account", new Account());

		List<Company> allCompanies = accountService_m.listCompanies();
		map.put("users_list", accountService_m.listUsers());
		map.put("companies_list", allCompanies);
		map.put("accountsList", accountService_m.listAccounts());
		
		List<Server> serversList = null;
		// find only relevant servers
		if (companyId >=0) {
			serversList = accountService_m.listServers(companyId);
			map.put("companyCurrentId", companyId);
		}
		else {
			if (allCompanies.size() > 0) {
				int companydefaultId = allCompanies.get(0).getId();
				serversList =  accountService_m.listServers(companydefaultId);
				map.put("companyCurrentId", companydefaultId);
			}
			else {
				throw new Exception("No companies in the system");
			}
				
		}
		
		if (serversList.size() <=0) {
			Company company = accountService_m.resolveCompany(companyId);
			throw new Exception("No servers registered for the company: " + company.getCompanyName());
		}
		
		map.put("servers_list", serversList);
		
	}

	
}
