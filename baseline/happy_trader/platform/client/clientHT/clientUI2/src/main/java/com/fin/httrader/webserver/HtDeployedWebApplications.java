/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.configprops.helper.PluginPageDescriptor;
import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtDeployedWebApplications extends HtServletsBase {
	
	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public HtDeployedWebApplications()
	{
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new UID
			this.generateUniquePageId();

			Set<String> webapps = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebApplications();

			GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Group ID"),
							new GridFieldDescriptor("Application"),
							new GridFieldDescriptor("Type"),
							new GridFieldDescriptor("Description")
							
							
				}
			);

			int idx = 0;
			StringBuilder user = new StringBuilder();
			StringBuilder hashed_password = new StringBuilder();
			
			resolveUserAndPassword(req, user, hashed_password);
			
			
			for(Iterator<String> it = webapps.iterator(); it.hasNext(); ) {
				dataset.addRow();

				String name_i = it.next();
				String url = createWebAppUrl(name_i, user.toString(), hashed_password.toString());
				//dataset.setRowValue(idx++, 0, "<a href=\"#\" onclick=\"start_webapp('"+name_i+"')\">"+name_i+"</a>");
				
				dataset.setRowValue(idx, 0, "");
				dataset.setRowValue(idx, 1, HtWebUtils.createClickHtml("launch_app(\"" + url + "\")", name_i));
				dataset.setRowValue(idx, 2, "Web Application"); 
				dataset.setRowValue(idx, 3, ""); 
				idx++;
				
			}
			
			
			// need to add web pages
			Map<String, List< PluginPageDescriptor > > web_page_plugins = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getPluginPages();
			for(Iterator<String> it = web_page_plugins.keySet().iterator(); it.hasNext(); ) {
				

				String name_i = it.next();
				List< PluginPageDescriptor > url_list = web_page_plugins.get(name_i);
				
				for(int i = 0; i < url_list.size(); i++) {
				  
				  dataset.addRow();
				  
				  PluginPageDescriptor descr = url_list.get(i);
				 
				  dataset.setRowValue(idx, 0, String.valueOf(descr.id) ); 
				  dataset.setRowValue(idx, 1, HtWebUtils.createClickHtml("launch_page_url(\"" + descr.url + "\")", name_i));
				  dataset.setRowValue(idx, 2, "Page");
				  dataset.setRowValue(idx, 3, descr.description); 
				  
				  idx++;
				}
			}


			// list of web apps
			setStringSessionValue(getUniquePageId(), req, "webapp_list_data", HtWebUtils.createHTMLForDhtmlGrid(dataset));


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	/**
	 * Helpers
	 */
	
	private void resolveUserAndPassword(HttpServletRequest req, StringBuilder user, StringBuilder hashed_password) throws Exception
	{
		user.setLength(0);
		hashed_password.setLength(0);
		Cookie sessCookie = HtWebUtils.findSessionIdCookie(req);
		
		if (sessCookie != null) {
			String sessionId = sessCookie.getValue();
			
			RtSecurityManager.UserStructure uf = HtCommandProcessor.instance().get_HtSecurityProxy().remote_resolveUserViaSessionId(sessionId);

			if (uf == null) {
					throw new HtException(getContext(), "resolveUserAndPassword", "Invalid session");
			}

			user.append(uf.userName_m.toString());
			hashed_password.append(uf.hashedPassword_m.toString());
			
			
			
		}
		else
			throw new HtException(getContext(), "resolveUserAndPassword", "You are not logged in");
		
	}
	
	private String createWebAppUrl(String application_name, String user, String hashed_password) {
		StringBuilder url = new StringBuilder("/loadjar/");
		url.append(application_name);
		url.append("/");
		url.append(user);
		url.append("/");
		url.append(hashed_password);
		url.append("/");
		url.append("launch.jnlp");

		return url.toString();
	}

}
