/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.ctags;

import com.mtrunner.client.utils.GeneralUtils;
import java.io.IOException;
import java.io.StringWriter;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.PageContext;
import javax.servlet.jsp.tagext.SimpleTagSupport;

/**
 *
 * @author Administrator
 */
public class JQueryInclusionTag extends SimpleTagSupport {

	private String base;
	private boolean up;
	private static final String BASE= "resources/lib/";
	
	
	

	public void setBase(String base) {
		this.base = base;
	}

	public void setUp(boolean up) {
		this.up = up;
	}

	@Override
	public void doTag() throws JspException, IOException {

		JspWriter out = getJspContext().getOut();
		PageContext pageContext = (PageContext) getJspContext();
		String basePath = GeneralUtils.createFullBasePath(base, pageContext );
		
		out.print(includeJQueryString(basePath, up));
	}

	private String includeJQueryString(String fullBaseHref, boolean goUp) {
		StringBuilder r = new StringBuilder();
		String fullBaseHref_s = fullBaseHref != null ? fullBaseHref : "";

		String goUpStr = goUp ? "../" : "/";

		/*
		r.append(" <link rel=\"stylesheet\" href=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/ui/css/ui-lightness/jquery-ui-1.9.2.custom.min.css\"/> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery-1.8.3.min.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery.base64.min.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery.form.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery-ui-1.9.2.custom.min.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery.blockUI.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery.md5.js\"></script> \n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append("resources/lib/jquery/jquery.csv-0.71.min.js\"></script> \n");
		*/
		r.append("\n");
		r.append(" <link rel=\"stylesheet\" href=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-ui-1.11.2/jquery-ui.min.css\"/>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-ui-1.11.2/external/jquery/jquery.js\"></script>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-ui-1.11.2/jquery-ui.min.js\"></script>\n");
		// external
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-external/jquery.base64.min.js\"></script>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-external/jquery.form.js\"></script>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-external/jquery.blockUI.min.js\"></script>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-external/jquery.md5.js\"></script>\n");
		r.append(" <script type=\"text/javascript\" src=\"").append(fullBaseHref_s).append(goUpStr).append(BASE).append("jquery-external/jquery.csv-0.71.min.js\"></script>\n");
		r.append("\n");
		
		return r.toString();
	}

}
