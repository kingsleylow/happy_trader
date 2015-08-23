/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;

import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtReturnSessionVar_v2 extends HtServletsBase {

	
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	
	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		// this is only intended to return a session variables
		try {

			String cookie = getStringParameter(req, "cookie", false);
			String response_type = getStringParameter(req, "response_type", false);
			String session_var = getStringParameter(req, "session_var", false);

			forceNoCache(res);

			if (response_type.equalsIgnoreCase("text")) {
				res.setContentType("text/html; charset=windows-1251");
			} else if (response_type.equalsIgnoreCase("xml")) {
				res.setContentType("text/xml; charset=windows-1251");
			} else if (response_type.equalsIgnoreCase("file")) {
				res.setHeader("Content-disposition", "attachment; filename=data.xml");
				res.setHeader("Content-Type", "application/octet-stream");

			} else {
				res.setContentType("text/xml; charset=windows-1251");
			}

			res.getOutputStream().print(getStringSessionValue(cookie, req, session_var));
			res.getOutputStream().close();


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "doGet", e.getMessage());
		}
	}
}
