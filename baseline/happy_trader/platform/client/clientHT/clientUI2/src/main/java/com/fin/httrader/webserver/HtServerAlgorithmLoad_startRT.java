/*
 * HtServerAlgorithmLoad_startRT.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import java.io.IOException;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerAlgorithmLoad_startRT extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtServerAlgorithmLoad_startRT */
	public HtServerAlgorithmLoad_startRT() {
	}

	// ----------------------------
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// new UID
			this.generateUniquePageId();

			// init simulation profiles
			Set<String> simprofiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileList();
			setStringSessionValue(getUniquePageId(), req, "sim_profile_list_only", HtWebUtils.createOptionList(null, simprofiles, false));

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}
	
}
