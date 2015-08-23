/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth.filters;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.mtrunner.client.auth.JoomlaDBUtil;
import com.mtrunner.client.utils.GeneralUtils;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.log4j.Logger;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;

/**
 *
 * @author Administrator
 */
public class AdminUsernamePasswordAuthenticationFilter extends UsernamePasswordAuthenticationFilter {

	private static final String COOKIE = "php_current_user_1DDF749B834644EBA78FAAC0D7CCC42B";
	private static final String PARAM = "user";
	private static final String DUMMY_PASS = "dummy";
	
	static Logger log_m = Logger.getLogger(AdminUsernamePasswordAuthenticationFilter.class.getName());

	@Override
	protected boolean requiresAuthentication(HttpServletRequest request, HttpServletResponse response) {

		
		// first check user name
		String username = this.extractSecurityParam(request);
		log_m.debug("Username as parameter detected: "+username);

		if (username == null) {
			
			// get as cookie
			Cookie security = extractSecurityCookie(request);
			log_m.debug("Security cookie is: "+( security != null ? security.toString() : "N/A" ));
		
			if (security != null) {
				username = security.getValue();
				
				if (GeneralUtils.nvl(username))
					return false;
			}
			else
				return false;
			
			
		}
		else {
		
			// add new cookie
			addNewSecurityCookie(username, response);
			log_m.debug("Added new security cookie with user: " + username);
		}
		
		
		if (!GeneralUtils.nvl(username)) {
			UsernamePasswordAuthenticationToken authRequest = new UsernamePasswordAuthenticationToken(username, DUMMY_PASS);

			try {
				Authentication authResult = this.getAuthenticationManager().authenticate(authRequest);
				allOk(request, response, authResult);
			} catch (AuthenticationException e) {
				failed(request, response, e);
			}

		}
						
		
		return false;
	}
	
	private void addNewSecurityCookie(String user, HttpServletResponse response) {
		Cookie c = new Cookie(COOKIE, user);
		c.setPath("/");
		c.setMaxAge((int)(HtDateTimeUtils.getCurGmtTime() / 1000) + 3600);
		
    response.addCookie(c);
	}

	private Cookie extractSecurityCookie(HttpServletRequest request) {
		Cookie[] cookies = request.getCookies();
		if (cookies != null) {
			for (Cookie cookie : cookies) {
				if (cookie.getName().equalsIgnoreCase(COOKIE)) {
					return cookie;
				}
			}
		}

		return null;
	}
	
	private String extractSecurityParam(HttpServletRequest request) {

		if (!request.getMethod().equalsIgnoreCase("get"))
			return null;

		String user = request.getParameter(PARAM);
		if (!GeneralUtils.nvl(user)) {
			return user;
		}

		return null;
	}

	private void allOk(HttpServletRequest request, HttpServletResponse response, Authentication authResult) {
		try {
			successfulAuthentication(request, response, authResult);
		} catch (Exception e) {
		}
	}

	private void failed(HttpServletRequest request, HttpServletResponse response, AuthenticationException authFailed) {
		try {
			unsuccessfulAuthentication(request, response, authFailed);
		} catch (Exception e) {
		}
	}

}
