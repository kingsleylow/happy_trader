/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtMathUtils;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 * bean
 */
public class HtWebStartHelper extends HtServletsBase {

	@Override
	public String getContext() {
		return HtWebStartHelper.class.getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		forceNoCache(res);


		try {
			this.generateUniquePageId();
			String application_name = this.getStringParameter(req, "application_name", false);

			setStringSessionValue(getUniquePageId(), req, "url_to_load", "");
			setStringSessionValue(getUniquePageId(), req, "application_name", application_name);

			String url = null;
			

			//


			// include authentication via session cookie
			Cookie sessCookie = HtWebUtils.findSessionIdCookie(req);


			if (sessCookie != null) {
				String sessionId = sessCookie.getValue();
				RtSecurityManager.UserStructure uf = HtCommandProcessor.instance().get_HtSecurityProxy().remote_resolveUserViaSessionId(sessionId);

				if (uf == null) {
					throw new HtException(getContext(), "initialize_Get", "Invalid session");
				}

				url = createWebAppUrl(application_name, uf.userName_m.toString(), uf.hashedPassword_m.toString() );
				
			} else {
				String user = this.getStringParameter(req, "user", false);
				String password = this.getStringParameter(req, "password", false);
				String userIp = req.getRemoteHost();

				String sessionId = HtCommandProcessor.instance().get_HtSecurityProxy().remote_startUserSession(user, password, userIp);
				if (sessionId == null) {
					throw new HtException(getContext(), "initialize_Get", "Invalid user or password");
				}
				HtCommandProcessor.instance().get_HtSecurityProxy().remote_stopUserSession(sessionId);

				// ok authentication
				url = createWebAppUrl(application_name, user, HtMathUtils.md5HashString(password) );

			}

			setStringSessionValue(getUniquePageId(), req, "url_to_load", url.toString());




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
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
