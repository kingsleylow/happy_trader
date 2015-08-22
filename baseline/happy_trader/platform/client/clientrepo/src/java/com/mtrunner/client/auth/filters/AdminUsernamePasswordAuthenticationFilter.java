/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth.filters;

import com.mtrunner.client.utils.GeneralUtils;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
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
	private static final String DUMMY_PASS = "dummy";

	@Override
	protected boolean requiresAuthentication(HttpServletRequest request, HttpServletResponse response) {

		
		
		Cookie security = extractSecurityCookie(request);
		
		if (security == null) {
			//failed(request, response, new BadCredentialsException("Invalid cookie"));
			return false;
		}
		
		String username = security.getValue();
		
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
