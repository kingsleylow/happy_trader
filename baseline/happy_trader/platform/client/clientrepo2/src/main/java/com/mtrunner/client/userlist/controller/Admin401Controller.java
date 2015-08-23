/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller;

import java.security.Principal;
import java.util.Map;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

/**
 *
 * @author Administrator
 */
@Controller
@RequestMapping("/service")
public class Admin401Controller {
	// for 403 access denied page
	@RequestMapping(value = "/admin401", method = RequestMethod.GET)
	public String authenticationFailed(Principal user, Map<String, Object> map) {
 

		if (user != null) {
			map.put("msg", "Hi " + user.getName() + ", authentication failed!");
		} else {
			map.put("msg", "Authentication failed");
		}
 
		return "admin401";
 
	}
}
