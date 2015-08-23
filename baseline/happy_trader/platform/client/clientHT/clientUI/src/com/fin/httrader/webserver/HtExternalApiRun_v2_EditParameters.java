/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.externalrunstruct.RunPackageDescriptor;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExternalApiRun_v2_EditParameters extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			if (1 == 1) {
				throw new HtException(getContext(), "initialize_Get", "Direct navigation not allowed!");
			}
		} catch (Throwable e) {

			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		try {

			generateUniquePageId();

			setStringSessionValue(getUniquePageId(), req, "xml", "");
			setStringSessionValue(getUniquePageId(), req, "work_uid", "");
			setStringSessionValue(getUniquePageId(), req, "package_load_short_path", "");
			setStringSessionValue(getUniquePageId(), req, "package_name", "");
			setStringSessionValue(getUniquePageId(), req, "is_template", "");
			setStringSessionValue(getUniquePageId(), req, "version", "");
			

			String operation = getStringParameter(req, "operation", false);

			Uid workUid = new Uid();
			workUid.generate();

			setStringSessionValue(getUniquePageId(), req, "work_uid", workUid.toString());

			if (operation.equalsIgnoreCase("initial_load")) {

				res.setContentType("text/html; charset=windows-1251");
				forceNoCache(res);


				// upload file and set status to pending

				StringBuilder packageName = new StringBuilder();

				HtWebUtils.uploadToFile(req, res,
								HtSystem.getSystemTempPath(),
								resolveTemporaryFile(workUid).getName(),
								packageName);

				setStringSessionValue(getUniquePageId(), req, "package_name", packageName.toString());

				// read this file descriptor
				RunPackageDescriptor package_descr =  HtCommandProcessor.instance().get_HtExternalApiProxy().remote_resolveDescriptorFromJar(resolveTemporaryFile(workUid).getCanonicalPath());
				setStringSessionValue(getUniquePageId(), req, "xml", package_descr.xmlDescriptor_m.toString());
				setStringSessionValue(getUniquePageId(), req, "is_template", "false");
				setStringSessionValue(getUniquePageId(), req, "version", package_descr.moduleVersion_m.toString());
				
			} else if (operation.equalsIgnoreCase("initial_load_template")) {

				res.setContentType("text/html; charset=windows-1251");
				forceNoCache(res);

				// copy from template
				String template_name = getStringParameter(req, "template_name", false);

				String dst = resolveTemporaryFile(workUid).getAbsolutePath();
				String src = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_resolveTemplateJarFilePath(template_name).getAbsolutePath();

				HtFileUtils.copyFile(src, dst);


				// copy
				setStringSessionValue(getUniquePageId(), req, "package_name", template_name);
				RunPackageDescriptor package_descr =  HtCommandProcessor.instance().get_HtExternalApiProxy().remote_resolveDescriptorFromJar(dst);
				
				
				setStringSessionValue(getUniquePageId(), req, "xml", package_descr.xmlDescriptor_m.toString());
				setStringSessionValue(getUniquePageId(), req, "is_template", "true");
				setStringSessionValue(getUniquePageId(), req, "version", package_descr.moduleVersion_m.toString());


			} else if (operation.equalsIgnoreCase("download")) {
				res.setDateHeader("Expires", 1);


				Uid workUid_passed = new Uid(getStringParameter(req, "work_uid", false));

				if (!workUid_passed.isValid()) {
					throw new HtException(getContext(), "initialize_Post", "Invalid passed work UID");
				}

				String package_name = getStringParameter(req, "package_name", false);
				String new_xml_content = getStringParameter(req, "xml_content", false);



				String copiedFile = resolveTemporaryCopiedFile(workUid_passed).getAbsolutePath();
				HtFileUtils.copyFile(resolveTemporaryFile(workUid_passed).getAbsolutePath(), copiedFile);

				File rf_copy = new File(copiedFile);
				if (!rf_copy.exists()) {
					throw new HtException(getContext(), "initialize_Post", "The file does not exist :\"" + rf_copy.getAbsolutePath() + "\" doesn't exist");
				}


				// edit this file and add new sontent
				HtCommandProcessor.instance().get_HtExternalApiProxy().remote_updateXmlDescriptorFromJar(rf_copy.getAbsolutePath(), new_xml_content);

				//need to create this file

				res.setContentType("application/octet-stream");
				res.setHeader("Content-disposition", "attachment; filename=" + package_name);
				res.setContentLength((int) rf_copy.length());

				HtFileUtils.simpleReadFile(rf_copy, res.getOutputStream());

				rf_copy.delete();


			} else if (operation.equalsIgnoreCase("save_to_template")) {
				
				// this reply is done via ajax - so need to take care about it in JSP
				
				HtAjaxCaller ajaxCaller = new HtAjaxCaller();
				try {

					ajaxCaller.setOk();

					String total_data = getStringParameter(req, "total_data", false);
					XmlParameter request_param = XmlHandler.deserializeParameterStatic(total_data);

					Uid workUid_passed = new Uid(request_param.getString("work_uid"));
					if (!workUid_passed.isValid()) {
						throw new HtException(getContext(), "initialize_Post", "Invalid passed work UID");
					}

					String package_name = request_param.getString("package_name");
					String new_xml_content = request_param.getString("xml_content");

					// need to update current template with that file
					File template_file = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_resolveTemplateJarFilePath(package_name);

					HtCommandProcessor.instance().get_HtExternalApiProxy().remote_updateXmlDescriptorFromJar(template_file.getAbsolutePath(), new_xml_content);
				} catch (Throwable e) {
					ajaxCaller.setError(-1, e.getMessage());
				}
				
				res.setContentType("text/xml; charset=UTF-8");
				forceNoCache(res);
		
				ajaxCaller.serializeToXml(res);

			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			res.setContentType("text/html; charset=windows-1251");
			forceNoCache(res);

			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}

	/**
	 * Helpers
	 */
	private File resolveTemporaryFile(Uid uid) throws Exception {
		return new File(HtSystem.getSystemTempPath() + File.separatorChar + uid.toString() + ".jar");

	}

	private File resolveTemporaryCopiedFile(Uid uid) throws Exception {
		return new File(HtSystem.getSystemTempPath() + File.separatorChar + uid.toString() + "_copy.jar");

	}
}
