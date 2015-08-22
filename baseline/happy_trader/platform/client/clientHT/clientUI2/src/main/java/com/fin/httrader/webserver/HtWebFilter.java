/*
 * HtWebFilter.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.io.IOException;
import java.util.Enumeration;
import java.util.logging.Level;
import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtWebFilter implements Filter {

	private static final boolean NAVIGATE_ONLY_WEBPUGINS = false;
	// RtConfigurationManager.StartUpConst.WEB_PLUGIN_DIR
		
	private static final String JSP_PAGE_LOGIN = "/htLoginHelper.jsp";
	
	private static final String [] NAVIGATE_ONLY_WEBPUGINS_EXCLUDE = {"js", "HtMTBridgeProcessorHistory"};
	
	private static final String[] EXCEPT_CHEK_SECURITY={
			"/htNotIE.jsp", 
			"/htLoginHelper.jsp", 
			"/htLogin.jsp", 
			"/htWL4RTProviderService.jsp", 
			"/htHTTPEventPropagator.jsp", 
			"/htHTTPEventPropagator.jsp",
			"/htHTTPCommandServlet.jsp", 
			"/htWebStartHelper.jsp"
	};


	private static final String[] JSP_PAGE_NO_NEED_TO_REDIRECT = {
		"/HtRealTimeAlerterHelper_AlertDataPopup.jsp",
		"/HtReturnSessionVar_v2.jsp"
	};

	private String getContext() {
		return getClass().getSimpleName();
	}

	/** Creates a new instance of HtWebFilter */
	public HtWebFilter() {
	}

	
	
	@Override
	public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain)
					throws IOException, ServletException {


		HttpServletResponse httpresp = (HttpServletResponse) servletResponse;
		HttpServletRequest httpreq = (HttpServletRequest) servletRequest;

		try {

			String uri_lower = httpreq.getRequestURI().toLowerCase();
			String uri_normal = httpreq.getRequestURI();
			if (NAVIGATE_ONLY_WEBPUGINS) {
				
				// no jsp at all
				if (uri_lower.indexOf(".jsp") == -1) {
						
						if (is_NAVIGATE_ONLY_WEBPUGINS_EXCLUDE(uri_lower) || 
							(uri_lower.indexOf(RtConfigurationManager.StartUpConst.WEB_PLUGIN_DIR) != -1)
						) {

										// filter chain
										filterChain.doFilter(servletRequest, servletResponse);

										return;

						}
				}
				httpresp.getOutputStream().print("<html>Not supported</html>");
				return;
						
			}
			
			if (!isSelfNavigating(uri_normal)) {
				//checkIE(httpresp, httpreq);
				checkSecurity(httpresp, httpreq);
			}

			// filter chain
			filterChain.doFilter(servletRequest, servletResponse);

		} catch (Throwable e) {
			HtWebUtils.writeHtmlErrorToOutput(httpresp, e);
		}


	}
	

	@Override
	public void init(FilterConfig filterConfig) throws ServletException {
	}

	@Override
	public void destroy() {
	}

	// ---------------------------------
	/*
	private void checkIE(HttpServletResponse httpresp, HttpServletRequest httpreq) throws IOException {
		// do far ignore

		String user_agent = ((String) httpreq.getHeader("User-Agent")).toUpperCase();
		if (user_agent.indexOf("MSIE") < 0) {
			// redirect to special page
			httpresp.sendRedirect(JSP_PAGE_NOT_IE);
		}


	}
	*/
	
	private void checkSecurity(HttpServletResponse httpresp, HttpServletRequest httpreq) throws Exception {
		//boolean success = false;
		Cookie sessCookie = HtWebUtils.findSessionIdCookie(httpreq);

		// get cookie - check session
		String sessionId = (sessCookie != null ? sessCookie.getValue() : "");

		// empty cleared session
		if (sessionId.equalsIgnoreCase(Uid.INVALID_UID) || sessionId.length() == 0) {

			if (!ifNoRedirect(httpreq)) {
				httpresp.sendRedirect(JSP_PAGE_LOGIN + "?operation=new_login");
				return;
			} else {
				// exception
				throw new HtException(getContext(), "checkSecurity", "Please login!");
			}

		}

		boolean status = HtCommandProcessor.instance().get_HtSecurityProxy().remote_verifyUserSession(sessionId, RtConfigurationManager.StartUpConst.WEB_SESSION_TIMEOUT_MIN * 60 * 1000);

		if (!status) {
			if (!ifNoRedirect(httpreq)) {
				// send redirect
				httpresp.sendRedirect(JSP_PAGE_LOGIN + "?operation=session_invalid");

			} else {
				// exception
				throw new HtException(getContext(), "checkSecurity", "Session invalid, please re-login!");
			}

		}



	}

	private boolean ifNoRedirect(HttpServletRequest httpreq) throws Exception {
		String uri = httpreq.getRequestURI();
			for (String v : JSP_PAGE_NO_NEED_TO_REDIRECT) {
					if (uri.indexOf(v) >= 0) {
							return true;
					}
			}

		return false;
	}

	private boolean is_NAVIGATE_ONLY_WEBPUGINS_EXCLUDE(String uri) {
		for (String v: NAVIGATE_ONLY_WEBPUGINS_EXCLUDE) {
					if (uri.indexOf(v.toLowerCase()) != -1) {
							return true;
					}
		}

		return false;
	}
	
	private boolean isSelfNavigating(String uri) {
		for (String v: EXCEPT_CHEK_SECURITY) {
					if (uri.indexOf(v) != -1) {
							return true;
					}
		}

		return false;
	}
}
