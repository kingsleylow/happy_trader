/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.IOException;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtStartStopServer_SelectSymbolsForProfile extends HtServletsBase  {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			// new UID always

			generateUniquePageId();
			setStringSessionValue(getUniquePageId(), req, "provider_symbols", "");

			String operation = getStringParameter(req, "operation", true);
			if (operation.equalsIgnoreCase("do_read_for_profile")) {
				String profile_id = getStringParameter(req, "profile_id", false);
				List<HtPair<String, String>> providerSymbolList = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory( profile_id );

				StringBuilder out = new StringBuilder();

				for(int i = 0; i < providerSymbolList.size(); i++) {
					out.append(providerSymbolList.get(i).first);
					out.append(",");
					out.append(providerSymbolList.get(i).second);

					if (i<(providerSymbolList.size()-1))
						out.append("\n");

				}
				
				setStringSessionValue(getUniquePageId(), req, "provider_symbols", out.toString());
			}


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		return true;

	} //

	

	// -----------------------
	// helpers
	
}
