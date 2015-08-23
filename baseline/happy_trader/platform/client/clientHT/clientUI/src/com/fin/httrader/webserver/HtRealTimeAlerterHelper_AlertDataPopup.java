/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRealTimeAlerterHelper_AlertDataPopup extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		internal_do(req, res);
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		internal_do(req, res);
	}

	// ----------------------------------

	// this is a single instance but the calls may come from different threads
	private void internal_do(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			StringBuilder data = new StringBuilder();

			// id of 
			String id = getStringParameter(req, "id", false);

			HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonRow row = new HtCommonRowSelector.CommonRow(header);

			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showAlertDataRow(id, header, row);

			res.getOutputStream().print("<html>");
			res.getOutputStream().print("<head>");
			res.getOutputStream().print("<meta http-equiv='Content-Type' content='text/html; charset=windows-1251'>");
			res.getOutputStream().print("<meta http-equiv='Cache-Control' content='No-cache'>");
			res.getOutputStream().print("<link href='css/console.css' rel='stylesheet' type='text/css'/>");
			res.getOutputStream().print("</head>");
			res.getOutputStream().print("<body class=x8>");

			//res.getOutputStream().print("<table cellpadding='2' cellspacing='2'>");
			//res.getOutputStream().print("<tr>");
			//res.getOutputStream().print("<td align=left valign=top class='x8'>");
			res.getOutputStream().print("<div ");
			res.getOutputStream().print(" id='");
			res.getOutputStream().print(id);
			res.getOutputStream().print("'");
			res.getOutputStream().print(" align='left' style='font-size: smaller; height:630px; width:600px; overflow:scroll; border: 1px solid black'>");

			res.getOutputStream().print(row.getFormattedValue(header.getFieldId("data")));

			res.getOutputStream().print("</div>");
			//res.getOutputStream().print("</td>");
			//res.getOutputStream().print("</tr>");

			//res.getOutputStream().print("</table>");

			res.getOutputStream().print("</body>");
			res.getOutputStream().print("</html>");
			res.flushBuffer();
			res.getOutputStream().close();


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
		}
	}
}
