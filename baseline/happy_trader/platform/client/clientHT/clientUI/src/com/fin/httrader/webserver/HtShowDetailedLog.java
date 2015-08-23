/*
 * HtShowDetailedLog.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Locale;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author victor_zoubok
 */
public class HtShowDetailedLog extends HtServletsBase {

	/**
	 * Creates a new instance of HtShowDetailedLog
	 */
	public HtShowDetailedLog() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new Uid and set initial values
			generateUniquePageId();


			//! what we are goint to pass to the page
			setStringSessionValue(getUniquePageId(), req, "id_val", "");
			setStringSessionValue(getUniquePageId(), req, "tlast_created_val", "");
			setStringSessionValue(getUniquePageId(), req, "level_val", "");
			setStringSessionValue(getUniquePageId(), req, "thread_val", "");
			setStringSessionValue(getUniquePageId(), req, "session_val", "");
			setStringSessionValue(getUniquePageId(), req, "context_val", "");
			setStringSessionValue(getUniquePageId(), req, "content", "");



			//! input parameters
			// operation
			String id_str = getStringParameter(req, "id", false);
			long id = Long.valueOf(id_str);

			HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonRow row = new HtCommonRowSelector.CommonRow(header);
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showCompleteLogRow(id, header, row);



			setStringSessionValue(getUniquePageId(), req, "id_val", row.getFormattedValue(header.getFieldId("id")));
			setStringSessionValue(getUniquePageId(), req, "tlast_created_val", HtDateTimeUtils.time2String_Gmt(Long.valueOf(row.getFormattedValue(header.getFieldId("tlast_created")))));
			setStringSessionValue(getUniquePageId(), req, "level_val", row.getFormattedValue(header.getFieldId("level")));
			setStringSessionValue(getUniquePageId(), req, "thread_val", row.getFormattedValue(header.getFieldId("thread")));
			setStringSessionValue(getUniquePageId(), req, "session_val", row.getFormattedValue(header.getFieldId("session")));
			setStringSessionValue(getUniquePageId(), req, "context_val", row.getFormattedValue(header.getFieldId("context")));


			// this must be a table
			StringBuilder row_content = new StringBuilder();
			String raw_content_data = row.getFormattedValue(header.getFieldId("content"));
			row_content.append("<table width=100% class=x8><tr><td align=left>");

			
			setStringSessionValue(getUniquePageId(), req, "content", raw_content_data.toString());



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;

	}
	
};
