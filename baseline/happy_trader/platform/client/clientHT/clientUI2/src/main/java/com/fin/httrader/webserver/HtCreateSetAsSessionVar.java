/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;

import java.util.HashSet;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 * This creates session variable in set
 */
public class HtCreateSetAsSessionVar extends HtServletsBase {
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

	
		try {

			String cookie = getStringParameter(req, "cookie", false);
			String set_name = getStringParameter(req, "set_name", false);
			String operation = getStringParameter(req, "operation", false);

			Object obj = getObjectSessionValue(cookie, req, set_name);

			if (operation.equalsIgnoreCase("add_value")) {
				String set_value = getStringParameter(req, "set_value", false);
				HashSet<String> sobj = null;
				if (obj==null) {
					sobj = new HashSet();
					setObjectSessionValue(cookie, req, set_name, sobj);
				}
				else {
					sobj = (HashSet<String>)obj;
				}

				// add
				sobj.add(set_value);
				
			}
			else if (operation.equalsIgnoreCase("remove_value")) {
				String set_value = getStringParameter(req, "set_value", false);

				if (obj != null) {
					HashSet<String> sobj = (HashSet<String>)obj;
					sobj.remove(set_value);
	
				}
				
			}
			else if (operation.equalsIgnoreCase("add_value_list")) {
				List<String> value_list = getStringListParameter(req, "set_value_list", false);
				HashSet<String> sobj = null;
				if (obj==null) {
					sobj = new HashSet();
				}
				else {
					sobj = (HashSet<String>)obj;
				}

				sobj.addAll(value_list);
				
				
			}
			else if (operation.equalsIgnoreCase("remove_value_list")) {
				List<String> value_list = getStringListParameter(req, "set_value_list", false);

				if (obj != null) {
					HashSet<String> sobj = (HashSet<String>)obj;
					sobj.removeAll(value_list);

					

				}
				
			}
			else if (operation.equalsIgnoreCase("clear_all")) {
				if (obj != null) {
					HashSet<String> sobj = (HashSet<String>)obj;
					sobj.clear();
					obj = null;

					setObjectSessionValue(cookie, req, set_name, null);
				}
			}
			else
				throw new HtException(getContext(), "doGet", "Invalid operation: " + operation);


			forceNoCache(res);

			


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "doGet", e.getMessage());
		}
	}

	
}
