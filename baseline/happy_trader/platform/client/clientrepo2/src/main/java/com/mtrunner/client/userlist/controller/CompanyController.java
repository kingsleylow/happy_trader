/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.controller;

import com.mtrunner.client.userlist.controller.bind.GeneralDateEditorSupport;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import java.util.Date;
import java.util.List;
import java.util.Map;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.InitBinder;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

/**
 *
 * @author Administrator
 */
@Controller
@RequestMapping("/company")
public class CompanyController {
	@Autowired
	private AccountService accountService_m;
	
	@Autowired
	private GeneralDateEditorSupport dateEditor_m;

	
	@InitBinder
	public void initBinder(WebDataBinder binder) {
		binder.registerCustomEditor(Date.class, "created", this.dateEditor_m);
	}
	
	@RequestMapping("/companies")
	public String listCompanies(Map<String, Object> map) {
		
		createCompanyPageContext(map);
		
		return "companies";
	}
	
	@RequestMapping("/disable_company/{id}/do.html")
	public String disableCompany(@PathVariable("id") Integer companyId, Map<String, Object> map) {
		accountService_m.enableCompany(companyId, false);
	
		createCompanyPageContext(map);
		
		return "companies";
	}

	@RequestMapping("/restore_company/{id}/do.html")
	public String restoreCompany(@PathVariable("id") Integer companyId, Map<String, Object> map) {
		accountService_m.enableCompany(companyId, true);
	
		createCompanyPageContext(map);
		
		return "companies";
	}
	
	@RequestMapping(params = "add", value = "/add_company.html", method = RequestMethod.POST)
	public String addCompany(@ModelAttribute("company") Company company, BindingResult result, Map<String, Object> map) {
		// Should init the AutoPopulatingList
		accountService_m.addCompany(company);
		createCompanyPageContext(map);
		
		return "companies";
	}
	
	@RequestMapping(params = "update", value = "/add_company.html", method = RequestMethod.POST)
	public String updateCompany(@ModelAttribute("company") Company company, BindingResult result, Map<String, Object> map) {
		// Should init the AutoPopulatingList
		accountService_m.updateCompany(company);
		createCompanyPageContext(map);
		
		return "companies";
	}
	
	@RequestMapping(value = "/edit_company/{id}/do.html", method = RequestMethod.GET)
	public String editCompany(@PathVariable("id") Integer companyId, Map<String, Object> map) {

		// edit account
		Company companyObj = accountService_m.resolveCompany(companyId);
		map.put("companyObj", companyObj);
		
		createCompanyPageContext(map);
	
		return "companies";
	}
	
	/**
	 * 
	 * helpers
	 */
	void createCompanyPageContext(Map<String, Object> map)
	{
		map.put("company", new Company());
		
		List<Company> companies = accountService_m.listCompanies();
		map.put("companies_list", companies);
	}
	
}
