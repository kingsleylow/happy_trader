/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.controller;

import com.mtrunner.client.userlist.controller.bind.CompanyEditorSupport;
import com.mtrunner.client.userlist.controller.bind.GeneralBooleanEditorSupport;
import com.mtrunner.client.userlist.controller.bind.GeneralDateEditorSupport;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import java.util.Date;
import java.util.Formatter;
import java.util.List;
import java.util.Map;
import javax.servlet.http.HttpServletRequest;
import org.apache.log4j.Logger;
import org.apache.log4j.Priority;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.InitBinder;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.servlet.ModelAndView;  

@Controller
@RequestMapping("/server")
public class ServerControler {
	
	static Logger log_m = Logger.getLogger(ServerControler.class.getName());
	
	@Autowired
	private CompanyEditorSupport companyEditor_m;
	
	@Autowired
	private GeneralBooleanEditorSupport boolEditor_m;
	
	@Autowired
	private GeneralDateEditorSupport dateEditor_m;


	
	@Autowired
	private AccountService accountService_m;
	
	@InitBinder
	public void initBinder(WebDataBinder binder) {
		binder.registerCustomEditor(Company.class, this.companyEditor_m);
		binder.registerCustomEditor(Boolean.class, "demo", this.boolEditor_m);
		binder.registerCustomEditor(Date.class, "created", this.dateEditor_m);
	}
	
	@RequestMapping("/servers")
	public String  listServers(Map<String, Object> map) throws Exception
	{
		createServerPageContext(map);
		
		return "servers";
	}
	
	@RequestMapping("/disable_server/{id}/do.html")
	public String disableServer(@PathVariable("id") Integer serverId, Map<String, Object> map) {
		accountService_m.enableServer(serverId, false);
	
		createServerPageContext(map);
		
		return "servers";
	}

	@RequestMapping("/restore_server/{id}/do.html")
	public String restoreServer(@PathVariable("id") Integer serverId, Map<String, Object> map) {
		accountService_m.enableServer(serverId, true);
	
		createServerPageContext(map);
		
		return "servers";
	}

	@RequestMapping(params = "add", value = "/add_server.html", method = RequestMethod.POST)
	public String addServer(@ModelAttribute("server") Server server, BindingResult result, Map<String, Object> map) {
		
		log_m.info( "Adding server entry: " + server.getServer_name());
		
		
		// Should init the AutoPopulatingList
		accountService_m.addServer(server);
		createServerPageContext(map);
		
		return "servers";
	}
	
	@RequestMapping(params = "update", value = "/add_server.html", method = RequestMethod.POST)
	public String updateServer(@ModelAttribute("server") Server server, BindingResult result, Map<String, Object> map) {
		// Should init the AutoPopulatingList
		accountService_m.updateServer(server);
		createServerPageContext(map);
		
		return "servers";
	}

	@RequestMapping(value = "/edit_server/{id}/do.html", method = RequestMethod.GET)
	public String editServer(@PathVariable("id") Integer serverId, Map<String, Object> map) {

		// edit account
		Server serverObj = accountService_m.resolveServer(serverId);
		map.put("serverObj", serverObj);
		
		createServerPageContext(map);
	
		return "servers";
	}
	

	/*
	@ExceptionHandler(Exception.class)
  public ModelAndView handleError(HttpServletRequest req, Exception exception) {
    log_m.error("Request: " + req.getRequestURL() + " raised " + exception.toString());

    ModelAndView mav = new ModelAndView();
    mav.addObject("exception", exception);
    mav.addObject("url", req.getRequestURL());
    mav.setViewName("error");
    return mav;
  }
	*/
	
	
	/**
	 * 
	 * helpers
	 */
	void createServerPageContext(Map<String, Object> map)
	{
		map.put("server", new Server());
		
		List<Server> servers = accountService_m.listServers();
		map.put("servers_list", servers);
		
		map.put("companies_list", accountService_m.listCompanies());
	}
	
	
}
