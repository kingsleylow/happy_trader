/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller;

import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import java.util.List;
import java.util.Map;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

@Controller
@RequestMapping("/accdt")
public class JoomlaBridgeController {
	@Autowired
	private AccountService accountService_m;
	
	@RequestMapping("/bridge/{user_name}/do.html")
	public String initialize( @PathVariable("user_name") String userName, Map<String, Object> map) throws Exception {
		createUserPageContext(userName, map);
		return "joomla_bridge";
	}
	
	/**
	 * 
	 * helpers
	 */
	void createUserPageContext(String userName, Map<String, Object> map) throws Exception
	{
		
		
		User user = accountService_m.resolveUserByName(userName);
		if (user == null)
			throw new Exception("Invalid user: " + userName);
		
		map.put("userName", user.getUserName());
		
		List<Account> accounts = user.getAccounts();
		map.put("accounts", accounts);
		
		
	}
}
