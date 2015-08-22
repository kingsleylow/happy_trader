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
import org.springframework.web.portlet.ModelAndView;

/**
 *
 * @author Administrator
 */
@Controller
@RequestMapping("/service")
public class Admin403Controller {
	
	// for 403 access denied page
	@RequestMapping(value = "/admin403", method = RequestMethod.GET)
	public String accesssDenied(Principal user, Map<String, Object> map) {
 

		if (user != null) {
			map.put("msg", "Hi " + user.getName() + ", you do not have permission to access this page!");
		} else {
			map.put("msg", "You do not have permission to access this page!");
		}
 
		return "admin403";
 
	}
}
