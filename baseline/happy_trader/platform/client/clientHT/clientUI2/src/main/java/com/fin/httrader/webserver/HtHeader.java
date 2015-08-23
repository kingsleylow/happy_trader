/*
 * HtHeader.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtHeader extends HtServletsBase {

	private StringBuilder userName_m = new StringBuilder();

	/** Creates a new instance of HtHeader */
	public HtHeader() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean areCriticalErrors() {
		return HtCommandProcessor.instance().get_HtConfigurationProxy().remote_doWeHaveCriticalErrors();
	}

	public String getUserName() {
		return userName_m.toString();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			userName_m.setLength(0);
			Cookie sessCookie = HtWebUtils.findSessionIdCookie(req);


			if (sessCookie != null) {
				// get cookie - check session
				String sessionId = sessCookie.getValue();
				userName_m.append(HtCommandProcessor.instance().get_HtSecurityProxy().remote_resolveUserViaSessionId(sessionId).userName_m);

			} else {
				throw new HtException(getContext(), "initialize_Get", "Invalid session cookie");
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		
		return true;

	}
	; //
}
