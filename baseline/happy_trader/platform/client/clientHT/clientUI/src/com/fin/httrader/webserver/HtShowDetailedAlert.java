/*
 * HtShowDetailedAlert.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtUtils;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowDetailedAlert extends HtServletsBase {

	/** Creates a new instance of HtShowDetailedAlert */
	public HtShowDetailedAlert() {
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


			setStringSessionValue(getUniquePageId(), req, "id", "");
			setStringSessionValue(getUniquePageId(), req, "source", "");
			setStringSessionValue(getUniquePageId(), req, "alg_brk_pair", "");
			setStringSessionValue(getUniquePageId(), req, "thread", "");
			setStringSessionValue(getUniquePageId(), req, "priority", "");
			setStringSessionValue(getUniquePageId(), req, "alert_time", "");
			setStringSessionValue(getUniquePageId(), req, "data", "");
			setStringSessionValue(getUniquePageId(), req, "run_name", "");

			String id_str = getStringParameter(req, "ids", false);
			HtCommonRowSelector.CommonHeader header = new HtCommonRowSelector.CommonHeader();
			HtCommonRowSelector.CommonRow row = new HtCommonRowSelector.CommonRow(header);

			// fetch
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showAlertDataRow(id_str, header, row);

			for (int i = 1; i <= header.getSize(); i++) {
				if (header.getFieldName(i).equalsIgnoreCase("priority")) {
					header.addFormatter(i,
									new HtCommonRowSelector.HtCommonFieldFormatter() {

										public String format(HtCommonRowSelector.CommonRow row, String sval) {
											return AlertPriorities.getAlertpriorityByName(HtUtils.parseInt(sval));
										}
									});
				}



				//
				if (header.getFieldName(i).equalsIgnoreCase("alert_time")) {
					header.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
				}


			} // end loop


			// initialize
			setStringSessionValue(getUniquePageId(), req, "id", row.getFormattedValue(header.getFieldId("id")));
			setStringSessionValue(getUniquePageId(), req, "source", row.getFormattedValue(header.getFieldId("source")));
			setStringSessionValue(getUniquePageId(), req, "alg_brk_pair", row.getFormattedValue(header.getFieldId("alg_brk_pair")));
			setStringSessionValue(getUniquePageId(), req, "thread", row.getFormattedValue(header.getFieldId("thread")));
			setStringSessionValue(getUniquePageId(), req, "priority", row.getFormattedValue(header.getFieldId("priority")));
			setStringSessionValue(getUniquePageId(), req, "alert_time", row.getFormattedValue(header.getFieldId("alert_time")));
			setStringSessionValue(getUniquePageId(), req, "data", row.getFormattedValue(header.getFieldId("data")));
			setStringSessionValue(getUniquePageId(), req, "run_name", row.getFormattedValue(header.getFieldId("run_name")));
			setStringSessionValue(getUniquePageId(), req, "small_data", row.getFormattedValue(header.getFieldId("small_data")));


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;

	}
	
	
}
