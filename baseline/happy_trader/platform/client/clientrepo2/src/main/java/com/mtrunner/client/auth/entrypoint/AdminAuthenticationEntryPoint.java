/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth.entrypoint;

import java.io.IOException;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.web.AuthenticationEntryPoint;
import org.springframework.stereotype.Component;

/**
 *
 * @author Administrator
 */
@Component("adminAuthenticationEntryPoint")
public class AdminAuthenticationEntryPoint implements AuthenticationEntryPoint {
	
	
	
	private String errorPage;
	
	
 
	public AdminAuthenticationEntryPoint() {
	}
 
	public AdminAuthenticationEntryPoint(String errorPage) {
		this.errorPage = errorPage;
	}
 
	public String getErrorPage() {
		return errorPage;
	}
 
	public void setErrorPage(String errorPage) {
		this.errorPage = errorPage;
	}
 
	
	@Override
	public void commence(HttpServletRequest request, HttpServletResponse response,
					AuthenticationException authException) throws IOException {
		
		
		
		String navigate = request.getContextPath() + "/" + errorPage;
		
		//response.sendError(HttpServletResponse.SC_UNAUTHORIZED, "Unauthorized");
		response.sendRedirect(navigate);
	}
}
