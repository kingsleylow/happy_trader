/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.io.File;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtReadFile extends HtServletsBase {

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		
		try {

			// what read
			String target_read = getStringParameter(req, "target_read", false);

			// allowed only to read from 
			if (target_read.equalsIgnoreCase("cpp_server")) {
				forceNoCache(res);
				res.setContentType("text/plain; charset=windows-1251");
				
				String server_id = getStringParameter(req, "server_id", false);
				String file_name = HtCommandProcessor.instance().get_HtServerProxy().remote_getLogFileName(server_id);

				File rf = new File(file_name);
				if (!rf.exists()) {
					throw new HtException(getContext(), "doGet", "Cpp internal log file does not exist: \"" + file_name + "\"");
				}

				HtFileUtils.simpleReadFile(rf, res.getOutputStream());
			} else if (target_read.equalsIgnoreCase("java_server")) {
				forceNoCache(res);
				res.setContentType("text/plain; charset=windows-1251");
				
				File rf = new File(HtLog.getLogFileName());

				if (!rf.exists()) {
					throw new HtException(getContext(), "doGet", "Java log file does not exist: \"" + HtLog.getLogFileName() + "\"");
				}

				HtFileUtils.simpleReadFile(rf, res.getOutputStream());

			}
			else if (target_read.equalsIgnoreCase("external_package")) {
				forceNoCache(res);
				res.setContentType("text/html; charset=windows-1251");

				String uid_s = getStringParameter(req, "package_uid", false);
				Uid package_uid = new Uid(uid_s);

				// resolve file name
				//ExternalScriptStructure ss = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_queryScriptRun(package_uid);

				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_copyLogFileToStream( res.getOutputStream(), package_uid );

								
			}
			else if (target_read.equalsIgnoreCase("external_package_jar")) {
								
				res.setDateHeader("Expires", 1);
				String package_uid_s = getStringParameter(req, "package_uid", false);
				Uid package_uid = new Uid(package_uid_s);

				// resolve file name
				ExternalScriptStructure ss = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_queryScriptRun(package_uid);
				
				res.setContentType("application/octet-stream");
				res.setHeader("Content-disposition", "attachment; filename="+ss.getName()+".jar");


				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_copyJarFileToStream( res.getOutputStream(), package_uid );

				
			}
			else
				throw new HtException(getContext(), "doGet", "Invalid target_read parameter");

			// flush
			res.flushBuffer();
			res.getOutputStream().close();

		} catch (Throwable e) {
			res.setContentType("text/html; charset=windows-1251");
			writeHtmlErrorToOutput(res, e);
		}

	}
}
