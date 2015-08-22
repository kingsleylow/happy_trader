/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.ctags;

import com.mtrunner.client.utils.GeneralUtils;
import java.io.IOException;
import javax.servlet.jsp.JspException;
import javax.servlet.jsp.JspWriter;
import javax.servlet.jsp.PageContext;
import javax.servlet.jsp.tagext.SimpleTagSupport;

/**
 *
 * @author Administrator
 */
public class BasePathTag extends SimpleTagSupport {

	private String base;
	
	public void setBase(String base) {
		this.base = base;
	}
	
	@Override
	public void doTag() throws JspException, IOException {

		JspWriter out = getJspContext().getOut();
		PageContext pageContext = (PageContext) getJspContext();
	
		out.print(GeneralUtils.createFullBasePath(base,pageContext ));
	}
	
}
