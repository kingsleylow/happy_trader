/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.interfaces.ReturnValueEnterface;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import com.fin.httrader.webserver.GridDescriptor.GridHelper;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 */
public class HtExternalApiRun_v2 extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			readAllData(req, res);
			readTemplates(req, res);



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		HtAjaxCaller ajaxCaller = new HtAjaxCaller();
		ajaxCaller.setOk();

		try {
			// set page UID
			this.setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("remove_entries")) {
				removeEntries(ajaxCaller, req);
			} else if (operation.equalsIgnoreCase("delete_template")) {
				deleteTemplate(ajaxCaller, req);
			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			ajaxCaller.setError(-1, e.getMessage());
			
		}

		ajaxCaller.serializeToXml(res);
		return true;
	}

	// ----------------------
	// helpers
	private void deleteTemplate(HtAjaxCaller ajaxCaller, HttpServletRequest req) throws Exception {
		String template_name = getStringParameter(req, "template_name", false);

		HtCommandProcessor.instance().get_HtExternalApiProxy().remote_removeTemplateJar(template_name);

		ajaxCaller.setData("Template successfully removed");
	}

	private void removeEntries(HtAjaxCaller ajaxCaller, HttpServletRequest req) throws Exception {
		String total_data = getStringParameter(req, "total_data", false);
		XmlParameter request_param = XmlHandler.deserializeParameterStatic(total_data);

		Set<String> keys = request_param.getKeys();
		for (Iterator<String> it = keys.iterator(); it.hasNext();) {
			String uid_s = request_param.getString(it.next());
			Uid uid = new Uid(uid_s);

			HtCommandProcessor.instance().get_HtExternalApiProxy().remote_removeEntry(uid);
		}

		ajaxCaller.setData("Successfully removed");

	}

	private void readAllData(HttpServletRequest req, HttpServletResponse res) throws Exception {
		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("ID"),
							new GridFieldDescriptor("Package Name"),
							new GridFieldDescriptor("Status"),
							new GridFieldDescriptor("Start Time"),
							new GridFieldDescriptor("Finish Time"),
							new GridFieldDescriptor("Remove", "ro", "na", "center"),});

		List<ExternalScriptStructure> slist = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_queryScriptRuns();
		for (int i = 0; i < slist.size(); i++) {

			ExternalScriptStructure s_i = slist.get(i);
			dataset.addRow();

			dataset.setRowValue(i, 0, s_i.getUid().toString());
			dataset.setRowValue(i, 1, "<a href='#' onclick='load_jar_file(\"" + s_i.getUid().toString() + "\",\"" + s_i.getName() + "\")'>" + s_i.getName() + "</a>");
			dataset.setRowValue(i, 2, "<a href='#' onclick='open_log(\"" + s_i.getUid().toString() + "\",\"" + s_i.getName() + "\", \"" + ExternalScriptStructure.getStatusName(s_i.getStatus()) + "\")'>" + ExternalScriptStructure.getStatusName(s_i.getStatus()) + "</a>");
			dataset.setRowValue(i, 3, s_i.getBeginTime() >= 0 ? HtDateTimeUtils.time2SimpleString_Gmt(s_i.getBeginTime()) : "N/A");
			dataset.setRowValue(i, 4, s_i.getEndTime() >= 0 ? HtDateTimeUtils.time2SimpleString_Gmt(s_i.getEndTime()) : "N/A");
			dataset.setRowValue(i, 5, GridHelper.CHECKBOX_ENTRY_UNCHECKED);

		}


		// just create a table with all data
		setStringSessionValue(getUniquePageId(), req, "external_run_packages", HtWebUtils.createHTMLForDhtmlGrid(dataset));
	}

	private void readTemplates(HttpServletRequest req, HttpServletResponse res) throws Exception {
		final Set<File> templjars = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_getListOfTemplateJars();
		setStringSessionValue(getUniquePageId(), req, "template_jars", 
						HtWebUtils.createOptionListWithAttributes(null, templjars, new String[]{"_file_name"}, new ReturnValueEnterface() {

			@Override
				public Object getValue(Object key) {
					File f = (File) key;
					
					StringBuilder val = new StringBuilder(f.getName()).append(" [ ").append(HtDateTimeUtils.time2SimpleString_Gmt(f.lastModified())).append(" ]");
					return val.toString();
				}

			@Override
			public Object getAttributeValue(String attributeName, Object key) {
				if (attributeName.equalsIgnoreCase("_file_name")) {
					File f = (File) key;
					return f.getName();
				}
				
				return "";
			}
			})
		);


	}
}
