/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.controller;

import com.mtrunner.client.userlist.controller.bind.GeneralDateEditorSupport;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
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
@RequestMapping("/user")
public class UserController {
	@Autowired
	private AccountService accountService_m;
	
	@Autowired
	private GeneralDateEditorSupport dateEditor_m;

	@InitBinder
	public void initBinder(WebDataBinder binder) {
		binder.registerCustomEditor(Date.class, "created", this.dateEditor_m);
	}
	
	
	@RequestMapping("/users")
	public String listUsers(Map<String, Object> map) {
		
		createUserPageContext(map);
		
		return "users";
	}
	
	
	@RequestMapping("/disable_user/{id}/do.html")
	public String disableUser(@PathVariable("id") Integer userId, Map<String, Object> map) {
		accountService_m.enableUser(userId, false);
	
		createUserPageContext(map);
		
		return "users";
	}

	@RequestMapping("/restore_user/{id}/do.html")
	public String restoreUser(@PathVariable("id") Integer userId, Map<String, Object> map) {
		accountService_m.enableUser(userId, true);
	
		createUserPageContext(map);
		
		return "users";
	}

	@RequestMapping(params = "add", value = "/add_user.html", method = RequestMethod.POST)
	public String addUser(@ModelAttribute("user") User user, BindingResult result, Map<String, Object> map) {
		// Should init the AutoPopulatingList
		accountService_m.addUser(user);
		createUserPageContext(map);
		
		return "users";
	}
	
	@RequestMapping(params = "update", value = "/add_user.html", method = RequestMethod.POST)
	public String updateUser(@ModelAttribute("user") User user, BindingResult result, Map<String, Object> map) {
		// Should init the AutoPopulatingList
		accountService_m.updateUser(user);
		createUserPageContext(map);
		
		return "users";
	}

	@RequestMapping(value = "/edit_user/{id}/do.html", method = RequestMethod.GET)
	public String editUser(@PathVariable("id") Integer userId, Map<String, Object> map) {

		// edit account
		User userObj = accountService_m.resolveUser(userId);
		map.put("userObj", userObj);
		
		createUserPageContext(map);
	
		return "users";
	}
	
	/**
	 * 
	 * helpers
	 */
	void createUserPageContext(Map<String, Object> map)
	{
		map.put("user", new User());
		
		List<User> users = accountService_m.listUsers();
		map.put("users_list", users);
	}

}
