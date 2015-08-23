/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.servlets;

import static com.mtrunner.client.servlets.SendMessageServlet_old.log_m;
import com.mtrunner.client.utils.GeneralUtils;
import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Administrator
 */
//@WebServlet(name = "ResourceDispatcherServlet", urlPatterns = "/resourceDispatcher")
@WebServlet(name = "ResourceDispatcherServlet")
public class ResourceDispatcherServlet extends HttpServlet {

	@Override
	protected void doGet(HttpServletRequest httpServletRequest, HttpServletResponse httpServletResponse) throws ServletException, IOException {
		//String servletPath = httpServletRequest.getServletPath();   // /resource
		String pathInfo = httpServletRequest.getPathInfo();

		String url = "/WEB-INF/resources" + pathInfo;

		try {
			
			String tp = getMimeType(url);
			if (tp != null)
				httpServletResponse.setContentType(tp);
			
			RequestDispatcher dispatcher = httpServletRequest.getRequestDispatcher(url);
			dispatcher.include(httpServletRequest, httpServletResponse);
			
			

		} catch (Exception e) {
			if (!httpServletResponse.isCommitted()) {
				httpServletResponse.sendError(500);
			}
		}

	}
	
	String getMimeType(String pathInfo)
	{
			if (GeneralUtils.nvl(pathInfo))
				return null;
			
			if (pathInfo.indexOf("css") != -1)
				return "text/css";
			else if (pathInfo.indexOf("js") != -1)
				return "text/javascript";
			else if (pathInfo.indexOf("html") != -1 || pathInfo.indexOf("htm") != -1)
				return "text/html";
			else if (pathInfo.indexOf("xml") != -1)
				return "text/xml";
			else if (pathInfo.indexOf("png") != -1)
				return "image/png";
			else if (pathInfo.indexOf("jpg") != -1)
				return "image/jpeg";
			else
				return null;
	}

}
