/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author DanilinS
 */
public class HtPropertyTable_v2 extends HtServletsBase {

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {

		forceNoCache(res);


		

		try {
			setUniquePageId(getStringParameter(req, "page_uid", false));
			clearErrorXmlInfo(req);

			String type = getStringParameter(req, "type", false);

			if (type.equalsIgnoreCase("EXPORT_CSV")) {
				String propId = getStringParameter(req, "prop_id", false);

				res.setHeader("Content-disposition", "attachment; filename=" + URLUTF8Encoder.encode(propId) + ".csv");
				res.setHeader("Content-Type", "application/octet-stream");

				String table_data_xml = getStringParameter(req, "table_xml_data", true);

				XmlHandler hndl2 = new XmlHandler();
				XmlParameter xmlparameter = new XmlParameter();

				hndl2.deserializeParameter(table_data_xml, xmlparameter);
				if (!xmlparameter.getCommandName().equalsIgnoreCase("xrid_serialization")) {
					throw new HtException(getContext(), "doPost", "Invalid table_data_xml data type");
				}

				

				res.getOutputStream().print(makeCSVOutput(xmlparameter));
				res.getOutputStream().close();



			} else if (type.equalsIgnoreCase("IMPORT_CSV")) {

				// xml is expected to be an output
				res.setContentType("text/xml; charset=windows-1251");


				String table_csv_data = getStringParameter(req, "table_csv_data", true);
				res.getOutputStream().print(convertFromCSV(table_csv_data));
				res.getOutputStream().close();

			} else {
				throw new HtException(getContext(), "doPost", "Invalid command type: " + type);
			}



		} catch (Throwable e) {

			// error to be provided by 2ways
			initErrorXmlInfo(req, e);

			res.setContentType("text/html; charset=windows-1251");
			res.setHeader("Content-disposition", "");

			res.getOutputStream().print(HtWebUtils.writeHtmlErrorToString(e));
			res.getOutputStream().close();
		}
	}

	// -------------------------
	// helpers
	String makeCSVOutput(XmlParameter xmlparameter) throws Exception {
		StringBuilder out = new StringBuilder();

		List<String> columns = xmlparameter.getStringList("columns");

		XmlParameter value_data_2 = xmlparameter.getXmlParameter("data");
		List<XmlParameter> rows_data = value_data_2.getXmlParameterList("rows");

		// header
		for (int i = 0; i < columns.size(); i++) {
			out.append("\"");
			out.append(columns.get(i));
			out.append("\"");

			if (i == (columns.size() - 1)) {
				out.append("\r\n");
			} else {
				out.append(",");
			}
		}

		// read a row

		for (int i = 0; i < rows_data.size(); i++) {
			XmlParameter row_i = rows_data.get(i);

			for (int k = 0; k < columns.size(); k++) {


				XmlParameter cell_i = row_i.getXmlParameter(columns.get(k));

				int column_type_i = cell_i.getType("value");

				if (column_type_i == XmlParameter.VT_String) {
					String value_i = cell_i.getString("value");

					out.append("\"");
					out.append(value_i);
					out.append("\"");

					if (k == (columns.size() - 1)) {
						out.append("\r\n");
					} else {
						out.append(",");
					}


				}
			}

		} // rows


		return out.toString();
	}

	//
	String convertFromCSV(String table_csv_data) throws Exception {
		// need to parse table_csv_data
		// and create xGrid XML

		StringBuilder out = new StringBuilder("<?xml version=\"1.0\"?>");
		out.append("<rows>");

		

		CSVReader reader = new CSVReader(new StringReader(table_csv_data), ',', '"', '\0');
		String[] nextLine;

		int rows = 0;
		while ((nextLine = reader.readNext()) != null) {


			if (rows++ == 0) {
				// header
				out.append("<head>");

				for (int i = 0; i < nextLine.length; i++) {
					out.append("<column>");
					out.append("<![CDATA[");

					out.append(nextLine[ i ]);

					out.append("]]>");
					out.append("</column>");
				}

				out.append("</head>");
				continue;
			}

			out.append("<row id=\"");
			out.append(rows);
			out.append("\">");

			for (int i = 0; i < nextLine.length; i++) {

				out.append("<cell>");
				out.append("<![CDATA[");
				out.append(nextLine[ i ]);
				out.append("]]>");

				out.append("</cell>");

			}
			out.append("</row>");


		}


		reader.close();
		out.append("</rows>");
		return out.toString();
	}
	
} // end class
