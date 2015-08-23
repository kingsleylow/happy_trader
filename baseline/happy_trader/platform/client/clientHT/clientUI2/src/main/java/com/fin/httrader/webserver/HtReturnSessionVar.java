/*
 * HtReturnSessionVar.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtException;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtReturnSessionVar extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		// this is only intended to return session variables
			
		try {

			String cookie = getStringParameter(req, "page_uid", false);
			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("get_session_var")) {

				if (!processXmlXGridRequest(cookie, req, res)) {
					throw new HtException(getContext(), "doGet", "cannot process XGrid request");
				}
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);

		}
	}
}
