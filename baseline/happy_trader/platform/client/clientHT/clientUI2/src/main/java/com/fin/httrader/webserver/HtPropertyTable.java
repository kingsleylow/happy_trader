/*
 * HtPropertyTable.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author Victor_Zoubok
 * Special class intended to get property table and provide export file in XML form
 */
public class HtPropertyTable extends HtServletsBase {

	/** Creates a new instance of HtPropertyTable */
	public HtPropertyTable() {
	}

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		// this is only intended to return session variables
		try {

			String operation = getStringParameter(req, "operation", false);

			if (operation.equals("get_property_table")) {
				getXml(req, res);
			} else if (operation.equals("set_property_table")) {
				setXml(req, res);
			} else {
				throw new HtException(getContext(), "doPost", "Invalid operation:" + operation);
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);

		}


	}

	private void setXml(HttpServletRequest req, HttpServletResponse res) throws Exception {
		// parse file data - what we want to upload
		XmlHandler xmlhandler = new XmlHandler();

		// current properties
		List<HtPair<String, String>> cur_properties = getStringPairListParameter(req, "cur_properties", true);

		String file_properties_content = "";
		//file_properties_content = getStringParameter(req, "file_properties", false);


		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(1024 * 1024);
		ServletFileUpload upload = new ServletFileUpload(factory);

		List items = upload.parseRequest(req);
		Iterator iter = items.iterator();


		while (iter.hasNext()) {

			FileItem item = (FileItem) iter.next();
			//String name = item.getFieldName();
			//String contentType = item.getContentType();
			//boolean isInMemory = item.isInMemory();
			//long sizeInBytes = item.getSize();

			if (item.getFieldName().equalsIgnoreCase("file_properties")) {
				file_properties_content = item.getString();
				break;
			}
		}
		;


		// file properties
		//String file_properties_content = getStringParameter(req, "file_properties", false);
		XmlParameter xmlparameter = new XmlParameter();
		xmlhandler.deserializeParameter(file_properties_content, xmlparameter);

		List<String> keys = null;
		List<String> values = null;

		if (xmlparameter.getKeys().contains("keys")) {
			keys = xmlparameter.getStringList("keys");
		} else {
			keys = new ArrayList<String>();
		}

		if (xmlparameter.getKeys().contains("values")) {
			values = xmlparameter.getStringList("values");
		} else {
			values = new ArrayList<String>();
		}

		res.setHeader("Content-disposition", "attachment; filename=property.xml");
		res.setHeader("Content-Type", "application/octet-stream");
		forceNoCache(res);

		// result
		LinkedHashMap<String, String> resultParams = new LinkedHashMap<String, String>();

		for (int i = 0; i < cur_properties.size(); i++) {
			HtPair<String, String> cur_prop_i = cur_properties.get(i);
			resultParams.put(cur_prop_i.first, cur_prop_i.second);
		}

		// add imported params substituting the same results
		for (int i = 0; i < keys.size(); i++) {
			String key = keys.get(i);
			String value = values.get(i);

			resultParams.put(key, value);
		}

		StringBuilder out = new StringBuilder();
		out.append("<?xml version=\"1.0\"?>");
		out.append("<rows>");

		int cnt = 0;
		for (Iterator<String> it = resultParams.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			String value = resultParams.get(key);


			out.append("<row id=\"").append(cnt++).append("\">");

			out.append("<cell>");
			out.append(key);
			out.append("</cell>");

			out.append("<cell>");
			out.append(value);
			out.append("</cell>");


			out.append("</row>");
		}

		out.append("</rows>");

		res.getOutputStream().print(out.toString());
		res.getOutputStream().close();
	}

	// this downloads XML report
	private void getXml(HttpServletRequest req, HttpServletResponse res) throws Exception {

		// parameters
		List<HtPair<String, String>> properties = getStringPairListParameter(req, "properties", true);
		String propId = getStringParameter(req, "prop_id", false);


		res.setHeader("Content-disposition", "attachment; filename=" + propId + "_property.xml");
		res.setHeader("Content-Type", "application/octet-stream");
		forceNoCache(res);
		StringBuilder out = new StringBuilder();


		XmlHandler xmlhandler = new XmlHandler();
		XmlParameter xmlparameter = new XmlParameter();

		Vector<String> keys = new Vector<String>();
		Vector<String> values = new Vector<String>();



		for (int i = 0; i < properties.size(); i++) {
			HtPair<String, String> prop_i = properties.get(i);
			keys.add(prop_i.first);
			values.add(prop_i.second);
		}

		xmlparameter.setStringList("keys", keys);
		xmlparameter.setStringList("values", values);


		xmlhandler.serializeParameter(xmlparameter, out);

		res.getOutputStream().print(out.toString());
		res.getOutputStream().close();
	}
}
