/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;




/**
 *
 * @author Victor_Zoubok
 */
public class HtExternalApiRun_v2_Helper extends HtServletsBase {

	private StringBuilder packageName_m = new StringBuilder();
	//private StringBuilder packageLoadShortPath_m = new StringBuilder();
	private StringBuilder runUid_m = new StringBuilder();
	


	// ------------------------------------
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {


			String operation = getStringParameter(req, "operation", false);

			
			// here we upload file for execution
			StringBuilder fullPath = new StringBuilder();
			Uid runUid = new Uid();
			runUid.generate();
			
			String baseName = runUid.toString() + ".jar";
			runUid_m.append(runUid);

			if (operation.equalsIgnoreCase("load_locally")) {

				// upload file and set status to pending
				
				HtWebUtils.uploadToTempFile(req, res,
							baseName, HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory(),
							fullPath, packageName_m);

				ExternalScriptStructure estruct = new ExternalScriptStructure(runUid, ExternalScriptStructure.STATUS_PENDING, HtDateTimeUtils.getCurGmtTime(), -1, packageName_m.toString(), "");
				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_insertNewEntry(estruct);
			}
			else if (operation.equalsIgnoreCase("load_as_template")) {
				packageName_m.append( getStringParameter(req, "template_name", false));

				// just copy
				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_copyTemplateToWorkingDirectory(runUid, packageName_m.toString());

				// update
				ExternalScriptStructure estruct = new ExternalScriptStructure(runUid, ExternalScriptStructure.STATUS_PENDING, HtDateTimeUtils.getCurGmtTime(), -1, packageName_m.toString(), "");
				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_insertNewEntry(estruct);


			}
			else {
			  throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}

	public String getOriginalPackageName()
	{
		return packageName_m.toString();
	}


	/*
	public String getShortTempJarName()
	{
		return packageLoadShortPath_m.toString();
	}
	 */

	public String getRunUid()
	{
		return runUid_m.toString();
	}

	// ------------------------------------
	
}
