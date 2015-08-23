
package com.mtrunner.client.userlist.controller;

import com.fin.httrader.eventplugins.ht_mt_bridge.delivery.HtMtEventWrapperPositionsHistory;
import com.fin.httrader.eventplugins.ht_mt_bridge.persistent.MainPersistManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetAccountHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.request.HtGetPositionHistoryReq;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetAccountHistoryResp;
import com.fin.httrader.webserver.web_plugins.ht_mt_bridge.dialog.response.HtGetPositionHistoryResp;
import com.mtrunner.client.HappyTraderTransport.CachedRequestHelper;
import com.mtrunner.client.HappyTraderTransport.RequestHelper;

import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.userlist.controller.bind.UserEditorSupport;
import com.mtrunner.client.userlist.controller.bind.ServerEditorSupport;
import com.mtrunner.client.userlist.controller.bind.CompanyEditorSupport;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import com.mtrunner.client.utils.GeneralUtils;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import org.apache.commons.lang3.StringEscapeUtils;
import org.apache.commons.lang3.time.DateUtils;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;


import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.access.annotation.Secured;
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
@RequestMapping("/accdt")
public class AccDetailsController {
	
	//@Autowired
	//private SettingsManager settings_m;
					
	@Autowired
	private AccountService accountService_m;
	
	@InitBinder
	public void initBinder(WebDataBinder binder)
	{
	}
	
	
	@RequestMapping("/account_details")
	public String listAccountsDefault(@PathVariable("account_id") Integer accId, Map<String, Object> map) throws Exception
	{
		
		throw new Exception("Direct navigation not allowed");
	}
	
	//@Secured({"ROLE_ADMIN"})
	@RequestMapping("/account_details/{account_id}/do.html")
	public String listAccountsInitial(@PathVariable("account_id") Integer accId, Map<String, Object> map) throws Exception
	{
		initPageContext(map, accId);
		return "account_details";
	}
	
	@RequestMapping("/account_details/{user_name}/{account_id}/do.html")
	public String listAccountsInitial(
					@PathVariable("user_name") String userName, 
					@PathVariable("account_id") Integer accId, 
					Map<String, Object> map
	) throws Exception
	{
		// user_name is ignored - tis is for security level
		initPageContext(map, accId);
		return "account_details";
	}
	
	
	
	// helpers
	private void initPageContext(Map<String, Object> map, int accountId) throws Exception
	{
		Account acc = accountService_m.resolveAccount(accountId);
		if (acc == null)
			throw new Exception("Invalid accound id: " + accountId);
		
		User user = accountService_m.resolveUser(acc.getUser().getId());
		if (user == null)
			throw new Exception("Cannot resolve user");
		
		map.put("accountObj", acc);
		map.put("userObj", user);
		map.put("baseUrl", SettingsManager.getInstance().getHappyTraderUrl());
	
		
		// acc history request
		CachedRequestHelper rh = new CachedRequestHelper();
		HtGetAccountHistoryResp accr =rh.parseAccountHistoryRequest(rh.accountHistoryRequest(acc.getCompany().getCompanyName(), acc.getAccount_id()));
		//map.put("accHistRespObj", accr);
		map.put("lastBalance", accr.getAccHistoryObject().getLastBalance());
		map.put("currency", accr.getAccHistoryObject().getCurrency());
		map.put("currency", accr.getAccHistoryObject().getCurrency());
		map.put("holderName", accr.getAccHistoryObject().getHolderName());
		
		
		
		
		
		/*
		Date beginOfDay = DateUtils.truncate(new Date(), Calendar.DATE);
		
		HtGetPositionHistoryResp r = rh.positionHistoryRequestHistoryPositions(
						acc.getCompany().getCompanyName(), 
						acc.getAccount_id(), beginOfDay.getTime(), false, 1);
	
		map.put("posHistRespObj", r);
		map.put("curHistPage", 1);
		*/
		
		
	}
	
	
}
