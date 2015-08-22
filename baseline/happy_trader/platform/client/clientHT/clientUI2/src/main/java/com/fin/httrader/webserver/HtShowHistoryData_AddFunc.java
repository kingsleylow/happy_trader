/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
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
public class HtShowHistoryData_AddFunc extends HtServletsBase {
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		res.setContentType("text/xml; charset=UTF-8");
		forceNoCache(res);


		HtAjaxCaller ajaxCaller = new HtAjaxCaller();

		try {
			
				// set this page UID
				setUniquePageId(getStringParameter(req, "page_uid", false));

				List<HtPair<String, String>> provSymbMapPage = getStringPairListParameter(req, "ps", true);
				String sim_profile_name = getStringParameter(req, "sim_profile_name", false);
				HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_createDummyProfileProperty(provSymbMapPage);

				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setSimulationProfileProperty(sim_profile_name, prop);
				ajaxCaller.setOk();
				ajaxCaller.setData("Simulation profile created");

									

		} catch (Throwable e) {
				ajaxCaller.setError(-1, e.getMessage());
		}

		
		ajaxCaller.serializeToXml(res);


	}


}
