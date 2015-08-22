/*
 * HtCriticalErrors.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtCriticalErrors extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtCriticalErrors */
	public HtCriticalErrors() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);



		try {

			// generate new UID
			this.generateUniquePageId();

			readData(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}; //

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);



		try {

			// generate new UID
			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);
			if (operation.equalsIgnoreCase("clear_errors")) {
				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_clearCriticalErrors();

				readData(req);
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	; //

	//
	private void readData(HttpServletRequest req) throws Exception {
		

		List<CriticalErrorEntry> errors = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getCriticalErrors();


			GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("Time"),
							new GridFieldDescriptor("Is Fatal"),
							new GridFieldDescriptor("Context"),
							new GridFieldDescriptor("Session"),
							new GridFieldDescriptor("Reason"),
							new GridFieldDescriptor("Detailed Info")
						
				});
			
		
			
		

		for (int i = 0; i < errors.size(); i++) {
			CriticalErrorEntry error_i = errors.get(i);
			
			dataset.addRow();
			dataset.setRowValue(i, 0, HtDateTimeUtils.time2SimpleString_Gmt(error_i.getTime()));
			dataset.setRowValue(i, 1, error_i.getUrgencyFlagString());
			dataset.setRowValue(i, 2, error_i.getContext());
			dataset.setRowValue(i, 3, error_i.getSession());
			dataset.setRowValue(i, 4, error_i.getReason());
			dataset.setRowValue(i, 5, error_i.getCallStack());

		}
		
		setStringSessionValue(getUniquePageId(), req, "error_data", HtWebUtils.createHTMLForDhtmlGrid(dataset));
	}
}
