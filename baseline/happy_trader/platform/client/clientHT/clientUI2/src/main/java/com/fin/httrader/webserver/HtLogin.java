/*
 * HtLogin.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtSecurityProxy;
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
public class HtLogin extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtLogin */
	public HtLogin() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=utf-8");
		forceNoCache(res);

		try {
			this.generateUniquePageId();
			String operation = getStringParameter(req, "operation", false);

			// store operation
			setStringSessionValue(getUniquePageId(), req, "operation", operation);

			if (operation.equalsIgnoreCase("logout")) {

				Cookie sessCookie = HtWebUtils.findSessionIdCookie(req);
				if (sessCookie != null) {
					HtCommandProcessor.instance().get_HtSecurityProxy().remote_stopUserSession(sessCookie.getValue());
				}

				HtWebUtils.clearSessionCookie(res);
				setStringSessionValue(getUniquePageId(), req, "hint", "Thank you for using Happy Trader");
			} else if (operation.equalsIgnoreCase("new_login")) {
				// n/a
				setStringSessionValue(getUniquePageId(), req, "hint", "Welcome to Happy Trader console");
			} else if (operation.equalsIgnoreCase("session_invalid")) {
				setStringSessionValue(getUniquePageId(), req, "hint", "The session has expired, please re-login");
			} else {
				throw new HtException(getContext(), "initialize_Get", "Invalid operation");
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		
		return true;

	}
	; //

	// only post here
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			String userName = getStringParameter(req, "user_id", false);
			String password = getStringParameter(req, "pass_id", false);

			String userIp = req.getRemoteHost();

			String sessionId = HtCommandProcessor.instance().get_HtSecurityProxy().remote_startUserSession(userName, password, userIp);

			if (sessionId != null) {
				// redirect to main page
				HtWebUtils.setUpSessionCookie(res, sessionId);
				res.sendRedirect("/htIndex.jsp");
			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid user name or password!");
			}


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	; //
}
