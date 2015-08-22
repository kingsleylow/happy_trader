/*
 * HtRTProviderManager.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import com.fin.httrader.webserver.GridDescriptor.GridHelper;
import java.io.IOException;
import java.util.Iterator;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtRTProviderManager extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtRTProviderManager */
	public HtRTProviderManager() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {
			generateUniquePageId();
			setStringSessionValue(getUniquePageId(), req, "provider_list", "");

			readProviderData(req);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	//
	// this is initialization function for dialog
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);

			if (operation.equals("start_provider")) {
				startRtProvider(req);
			} else if (operation.equals("stop_provider")) {
				stopRtProvider(req);
			} else if (operation.equals("refresh")) {
				// no-op
			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}

			readProviderData(req);

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	; //

	private void startRtProvider(HttpServletRequest req) throws Exception {
		String provider_id = getStringParameter(req, "provider_id", false);

		HtRTProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(provider_id);

		if (prop.isAlien()) {
			throw new HtException(getContext(), "startRtProvider", "This is an alien provider: '" + provider_id + "', can't be started from here");
		}

		HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_startProvider(
						provider_id,
						prop.getProviderClass(),
						prop.getParameters(),
						prop.getHourShift());
	}

	private void stopRtProvider(HttpServletRequest req) throws Exception {
		String provider_id = getStringParameter(req, "provider_id", false);
		HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_stopProvider(provider_id);
	}

	private void readProviderData(HttpServletRequest req) throws Exception {

		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("RT Provider"),
							new GridFieldDescriptor("Status"),
							new GridFieldDescriptor("Action"),
							new GridFieldDescriptor("Check", "ro", "na", "center"),
							new GridFieldDescriptor("Status Code"),
							new GridFieldDescriptor("Status String")
						
				});

		

		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();

		int i = 0;
		for (Iterator<String> it = providers.iterator(); it.hasNext();) {
			String provider_id_i = it.next();

			dataset.addRow();
			dataset.setRowValue(i, 0, provider_id_i);

		
			BackgroundJobStatus providerStatus = null;

			boolean isRunning = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_queryProviderRunning(provider_id_i);
			if (isRunning) {
				providerStatus = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_getProviderStatus(provider_id_i);
			}

			HtRTProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(provider_id_i);

		
			if (!prop.isAlien()) {
				dataset.setRowValue(i, 1, isRunning ? "Running" : "Idle");
			} else {
				dataset.setRowValue(i, 1, "alien");
			}

		

			if (!prop.isAlien()) {
				if (!isRunning) {
					StringBuilder out = new StringBuilder();
					out.append("<a href='#' onclick='do_start_provider(\"");
					out.append(provider_id_i);
					out.append("\");'>Start</a>");
					dataset.setRowValue(i, 2, out.toString());

				} else {
					StringBuilder out = new StringBuilder();
					out.append("<a href='#' onclick='do_stop_provider(\"");
					out.append(provider_id_i);
					out.append("\");'>Stop</a>");
					dataset.setRowValue(i, 2, out.toString());
				}

			} else {

				dataset.setRowValue(i, 2, "alien");
			}

		

			if ((isRunning && (providerStatus.getStatusCode()==BackgroundJobStatus.STATUS_OK)) || prop.isAlien()) {

				dataset.setRowValue(i, 3, GridHelper.CHECKBOX_ENTRY_UNCHECKED);
			}
			else {
				dataset.setRowValue(i, 3, GridHelper.CHECKBOX_ENTRY_UNCHECKED_DISABLED);
			}

			if (isRunning) {
				dataset.setRowValue(i, 4, BackgroundJobStatus.getStatusString(providerStatus.getStatusCode()));
				dataset.setRowValue(i, 5, providerStatus.getStatusReason());
			}
			else {
				dataset.setRowValue(i, 4, "");
				dataset.setRowValue(i, 5, "");
			}
			
		
			i++;
		}
		
		

		setStringSessionValue(getUniquePageId(), req, "provider_list", HtWebUtils.createHTMLForDhtmlGrid(dataset));
	}
}

