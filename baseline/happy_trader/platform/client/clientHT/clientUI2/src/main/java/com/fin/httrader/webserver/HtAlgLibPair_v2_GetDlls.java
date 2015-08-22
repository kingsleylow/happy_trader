/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.webserver.HtServletsBase;
import com.google.gson.Gson;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtAlgLibPair_v2_GetDlls extends HtServletsBase {
	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}
	
	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		

		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);

		HtAjaxCaller ajaxCaller = new HtAjaxCaller();
		ajaxCaller.setOk();
		
		Gson gson = new Gson();
		
		try {
			String operation= getStringParameter(req, "operation", false);
			Set<File> available_libs = null;
			
			if (operation.equalsIgnoreCase("alg_dll_list")) {
				available_libs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllAlgorithmDlls();
			}
			else if (operation.equalsIgnoreCase("brk_dll_list")) {
				available_libs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllBrokerDlls();
			}
			else
				throw new HtException(getContext(), "doGet", "Invalid operation type");
			
		
			if (available_libs != null) {
				List<String> libs = new ArrayList<String>();
				for(Iterator<File> it=  available_libs.iterator(); it.hasNext(); ) {
					libs.add( it.next().getAbsolutePath());
				}
				
				ajaxCaller.setData( gson.toJson(libs) );
			}
		
			
		}
		catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
		}
		
		ajaxCaller.serializeToXml(res);
	}
}
