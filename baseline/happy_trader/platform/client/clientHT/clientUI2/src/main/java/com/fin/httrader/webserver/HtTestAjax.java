/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtTestAjax extends HtServletsBase {
	
	private static String TEST_STR = "Тест Тест абвгдеёжзиклмнопрстуфхцчщэюя";

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			String req_encoding = getStringParameter(req, "req_encoding", false).toUpperCase();
			boolean as_xml = getBooleanParameter(req, "as_xml", true);
			String xml_encoding = getStringParameter(req, "xml_encoding", true).toUpperCase();
			res.setCharacterEncoding(req_encoding);

			if (as_xml) {
				res.getOutputStream().print("<?xml version=\"1.0\" encoding=\""+xml_encoding+"\"?>");
				res.getOutputStream().print("<data>");

			}
			res.getOutputStream().println("Text print->");
			res.getOutputStream().print(TEST_STR);
			res.getOutputStream().println();
			res.getOutputStream().println("Byte print->");
			res.getOutputStream().write(TEST_STR.getBytes(req_encoding));
			res.getOutputStream().println();
			res.getOutputStream().print("Finished.");

			if (as_xml) {
				res.getOutputStream().print("</data>");
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		
		return true;
		
	}
}
