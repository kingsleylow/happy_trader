/*
 * HtWebUtils.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.interfaces.ReturnValueEnterface;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import it.sauronsoftware.base64.Base64;
import java.io.File;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;
import java.util.logging.Level;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author Victor_Zoubok
 * Class is intended to contain some utilities functions
 */
public class HtWebUtils {

	public static final DateFormat HTTP_DF = new SimpleDateFormat("EEE, dd MMM yyyy HH:mm:ss z", Locale.US);
	public static final DateFormat VERSION0_DF = new SimpleDateFormat("EEE, dd-MMM-yy HH:mm:ss z", Locale.US);

	static {
		HTTP_DF.setTimeZone(TimeZone.getTimeZone("GMT"));
		VERSION0_DF.setTimeZone(TimeZone.getTimeZone("GMT"));
	}

	;

	public static String getContext() {
		return HtWebUtils.class.getSimpleName();
	}

	static Cookie findSessionIdCookie(HttpServletRequest req) {
		//Cookie result = null;

		Cookie[] cookies = req.getCookies();

		if (cookies != null) {
			for (int i = 0; i < cookies.length; i++) {
				if (cookies[i].getName().equals(RtSecurityManager.HT_SEC_COOKIE_NAME)) {
					return cookies[i];
				}
			}
		}

		return null;
	}

	static void clearSessionCookie(HttpServletResponse res) {
		Cookie invalidCookie = new Cookie(RtSecurityManager.HT_SEC_COOKIE_NAME, Uid.INVALID_UID);
		res.addCookie(invalidCookie);
	}

	static void setUpSessionCookie(HttpServletResponse res, String sessionID) {
		Cookie securityCookie = new Cookie(RtSecurityManager.HT_SEC_COOKIE_NAME, sessionID);
		int maxtime = (int) (HtDateTimeUtils.getCurGmtTime() / 1000) + RtConfigurationManager.StartUpConst.WEB_SESSION_TIMEOUT_MIN * 60;
		securityCookie.setMaxAge(maxtime);
		res.addCookie(securityCookie);
	}
	// ------------------------------------------

	// this creates option list
	static String createOptionList(Object def, Collection<?> objectCollection, boolean valueAttrib) {
		StringBuilder out = new StringBuilder();
		for (Iterator<?> it = objectCollection.iterator(); it.hasNext();) {
			Object value = it.next();
			if (def != null && def.equals(value)) {
				out.append("<option selected ");
			} else {
				out.append("<option ");
			}

			if (valueAttrib) {
				out.append(" value=\"");
				out.append(value);
				out.append("\">");
				out.append(value);
				out.append("</option>");
			} else {
				out.append(">");
				out.append(value);
				out.append("</option>");
			}



		}

		return out.toString();
	}

	static String createOptionList(Object def, Object[] objectArr) {
		StringBuilder out = new StringBuilder();
		for (int i = 0; i < objectArr.length; i++) {
			Object value = objectArr[i];
			if (def != null && def.equals(value)) {
				out.append("<option selected>").append(value).append("</option>");
			} else {
				out.append("<option>").append(value).append("</option>");
			}

		}

		return out.toString();
	}

	static String createOptionMappedList(Object def, Map<?, ?> objectMap) {
		StringBuilder out = new StringBuilder();
		for (Iterator<?> it = objectMap.keySet().iterator(); it.hasNext();) {

			Object key = it.next();
			Object value = objectMap.get(key);

			if (def != null && def.equals(key)) {
				out.append("<option value='");
				out.append(key);
				out.append("' selected>");
				out.append(value);
				out.append("</option>");
			} else {
				out.append("<option value='");
				out.append(key);
				out.append("'>");
				out.append(value);
				out.append("</option>");
			}

		}

		return out.toString();
	}

	static String createOptionList(Object def, Object[] keyArr, ReturnValueEnterface rvi) {
		StringBuilder out = new StringBuilder();

		for (int i = 0; i < keyArr.length; i++) {
			Object key = keyArr[i];
			if (def != null && def.equals(key)) {
				out.append("<option value='");
				out.append(key);
				out.append("' selected>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			} else {
				out.append("<option value='");
				out.append(key);
				out.append("'>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			}

		}

		return out.toString();
	}

	static String createOptionList(Object def, Collection keyArr, ReturnValueEnterface rvi) {
		StringBuilder out = new StringBuilder();

		for (Iterator it = keyArr.iterator(); it.hasNext(); ) {

			Object key = it.next();
			
			if (def != null && def.equals(key)) {
				out.append("<option value='");
				out.append(key);
				out.append("' selected>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			} else {
				out.append("<option value='");
				out.append(key);
				out.append("'>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			}

		}

		return out.toString();
	}
	
	static String createOptionListWithAttributes(Object def, Collection keyArr, String[] attributeNames, ReturnValueEnterface rvi) {
		StringBuilder out = new StringBuilder();

		for (Iterator it = keyArr.iterator(); it.hasNext(); ) {

			Object key = it.next();
			
			StringBuilder attributeValues = new StringBuilder();
			if (attributeNames != null) {
				for(int i = 0; i < attributeNames.length; i++) {
					attributeValues.append(" ").append(attributeNames[i]).append("='").
									append(rvi.getAttributeValue(attributeNames[i], key)).append("' ");
				}
			}
			
			if (def != null && def.equals(key)) {
				out.append("<option").append(attributeValues.toString()).append(" value='");
				out.append(key);
				out.append("' selected>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			} else {
				out.append("<option").append(attributeValues.toString()).append(" value='");
				out.append(key);
				out.append("'>");
				out.append(rvi.getValue(key));
				out.append("</option>");
			}

		}

		return out.toString();
	}

	static String createJScriptList(List<?> objectList, boolean addEval) {

		StringBuilder var = new StringBuilder();
		var.append("tmp_var_");
		var.append(Uid.generateNewStringUid());

		StringBuilder out = new StringBuilder();
		if (addEval) {
			out.append("eval(\" ");
		}
		out.append(var.toString());
		out.append(" = new Array();");

		for (int i = 0; i < objectList.size(); i++) {
			Object value = objectList.get(i);

			out.append(var);
			out.append("[");
			out.append(i);
			out.append("] = '");
			out.append(value);
			out.append("';");
		}

		out.append(var.toString());
		out.append(";");
		if (addEval) {
			out.append("\");");
		}

		return out.toString();
	}

	static String createJScriptMappedList(Map<?, ?> objectMap, boolean addEval) {

		StringBuilder var = new StringBuilder();
		var.append("tmp_var_");
		var.append(Uid.generateNewStringUid());

		StringBuilder out = new StringBuilder();
		if (addEval) {
			out.append("eval(\" ");
		}
		out.append(var.toString());
		out.append(" = new Array();");

		for (Iterator<?> it = objectMap.keySet().iterator(); it.hasNext();) {
			Object key = it.next();
			Object value = objectMap.get(key);

			out.append(var);
			out.append("['");
			out.append(key);
			out.append("'] = '");
			out.append(value);
			out.append("';");
		}

		out.append(var.toString());
		out.append(";");
		if (addEval) {
			out.append("\");");
		}

		return out.toString();
	}

	// very specific function
	static String createJScriptForProfileProviderMap(
					Map<String, List<HtPair<String, String>>> profProvSymbMap) {
		StringBuilder var = new StringBuilder();
		var.append("tmp_var_");
		var.append(Uid.generateNewStringUid());

		StringBuilder out = new StringBuilder("eval(\" ");
		out.append(var.toString());
		out.append(" = new Array();");

		for (Iterator<String> it = profProvSymbMap.keySet().iterator(); it.hasNext();) {
			String prof_i = it.next();
			out.append(var.toString());
			out.append("['");
			out.append(prof_i);
			out.append("']=");

			List<HtPair<String, String>> provSymb_i = profProvSymbMap.get(prof_i);

			out.append(" new Array( ");
			for (int i = 0; i < provSymb_i.size(); i++) {
				HtPair<String, String> pair_i = provSymb_i.get(i);

				String provider_i = pair_i.first;
				String symbol_i = pair_i.second;

				//

				out.append("'");
				out.append(provider_i);
				out.append("', '");
				out.append(symbol_i);
				out.append("',");

			}

			out.setLength(out.length() - 1);
			out.append("); ");
		}

		out.append(var.toString());
		out.append("; \");");

		return out.toString();
	}

	static String createEmptyXGridRow() {
		StringBuilder out = new StringBuilder();
		out.append("<?xml version=\"1.0\"?>");
		out.append("<rows>");
		out.append("</rows>");
		return out.toString();
	}

	// --------------------------------------------
	// creates option list from range
	static String createOptionListFromRange(int bidx, int eidx, int defaultval) {
		StringBuilder out = new StringBuilder();

		for (int i = bidx; i <= eidx; i++) {
			if (i == defaultval) {
				out.append("<option selected>");
				out.append(i);
				out.append("</option>");
			} else {
				out.append("<option>");
				out.append(i);
				out.append("</option>");
			}

		}

		return out.toString();
	}
	// --------------------------------------------

	static String createXmlParametersTable(Map<String, String> parameters) {
		StringBuilder out = new StringBuilder("<?xml version=\"1.0\"?><rows>");
		int cnt = 0;


		for (Iterator<String> it = parameters.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			String value = parameters.get(key);
			out.append("<row id=\"").append(cnt++).append("\">");

			out.append("<cell>");
			out.append(HtUtils.prepareValidXml(key));
			out.append("</cell>");

			out.append("<cell>");
			out.append(HtUtils.prepareValidXml(value));
			out.append("</cell>");

			out.append("</row>");

		}

		out.append("</rows>");

		return out.toString();
	}

	// ------------------------------------------
	static String createHtmlParametersTable(Map<String, String> parameters) {
		StringBuilder out = new StringBuilder();
		int cnt = 0;

		out.append("<tr NOWRAP=true>");
		out.append("<td type='ed'>Name</td>");
		out.append("<td type='ed'>Value</td>");
		out.append("</tr>");

		for (Iterator<String> it = parameters.keySet().iterator(); it.hasNext();) {
			String key_i = it.next();
			String value_i = parameters.get(key_i);

			out.append("<tr NOWRAP=true id='");
			out.append(++cnt);
			out.append("'>");

			out.append("<td>");
			out.append(key_i);
			out.append("</td>");

			out.append("<td>");
			out.append(value_i);
			out.append("</td>");

			out.append("</tr>");

		}


		return out.toString();
	}

	// ------------------------------------------
	static public String generateErrorHtml(String errorMsg) {
		String sMsg = null;

		if (errorMsg == null || errorMsg.length() <= 0) {
			sMsg = "Error description is not available";
		} else {
		
			sMsg = errorMsg.replaceAll("[<]", "&lt;").replaceAll("[>]", "&gt;").replaceAll("[\n]", "<br>");

		}


		StringBuilder errHTML = new StringBuilder();
		//errHTML.append("<html>");
		//errHTML.append("<head>");
		//errHTML.append("<meta http-equiv='Content-Type' content='text/html; charset=windows-1251'>");
		//errHTML.append("<meta http-equiv='Cache-Control' content='No-cache'>");
		errHTML.append("<link href='/css/console.css' rel='stylesheet' type='text/css'/></head> ");
		//errHTML.append("</head>");
		//errHTML.append("<body>");

		errHTML.append(" <table cellspacing='0' cellpadding='0' border='0' width='80%'>  ");
		errHTML.append(" <tr> ");
		errHTML.append(" <td width='5%' bgcolor='#cccc99'>&nbsp;</td> ");
		errHTML.append(" <td width='90%' bgcolor='#cccc99'> ");
		errHTML.append("         <table cellspacing='0' cellpadding='0' border='0' width='100%'> ");
		errHTML.append("           <tr> ");
		errHTML.append("             <td>&nbsp;</td> ");
		errHTML.append("            </tr> ");
		errHTML.append("            <tr> ");
		errHTML.append("               <td class='OraErrorHeader' ><img src='/imgs/errorl.gif'/>Error ");
		errHTML.append("                 <hr noshade size='1'/> ");
		errHTML.append("               </td> ");
		errHTML.append("            </tr> ");
		errHTML.append("            <tr> ");
		errHTML.append("               <td class='OraErrorText'> ");
		errHTML.append("                 &nbsp; ");
		errHTML.append("               </td> ");
		errHTML.append("            </tr> ");
		errHTML.append("            <tr> ");
		errHTML.append("               <td class='OraErrorText'> ");
		errHTML.append(sMsg);
		errHTML.append("               </td> ");
		errHTML.append("            </tr> ");
		errHTML.append("            <tr> ");
		errHTML.append("               <td>&nbsp;</td> ");
		errHTML.append("            </tr>  ");
		errHTML.append("         </table>  ");
		errHTML.append("     </td>  ");
		errHTML.append("     <td width='5%' bgcolor='#cccc99'>&nbsp;</td> ");
		errHTML.append("   </tr>  ");
		errHTML.append("   </table> ");

		//errHTML.append("	</body>");
		//errHTML.append("	</html>");

		return errHTML.toString();
	}

	// this is helper to avoid use sessiion variables
	static public void writeHtmlErrorToOutput(HttpServletResponse res, Throwable e) {
		try {
			StringBuilder out = new StringBuilder();
			out.append(e.toString());
			out.append("\n\n");
			out.append("Stack trace as follows:");
			out.append("\n");
			out.append(HtException.getErrorStack(e));
			out.append("\n");

			res.getOutputStream().print(HtWebUtils.generateErrorHtml(out.toString()));

			//res.flushBuffer();
			res.getOutputStream().close();
		} catch (Throwable e2) {
			HtLog.log(Level.WARNING, getContext(), "writeHtmlErrorToOutput", "Cannot write error HTML because of: " + e2.getMessage());
		}
	}

	static public String writeHtmlErrorToString(Throwable e) {

		StringBuilder out = new StringBuilder();

		out.append(e.getMessage());
		out.append("<hr>");
		out.append("Stack trace as follows:");
		out.append("\n");
		out.append(HtException.getErrorStack(e));
		out.append("\n");

		return HtWebUtils.generateErrorHtml(out.toString());
	}

	// this creates error entry and put this to session variable
	static public String createErrorInfoEntryForXGrid(Throwable e) {
		StringBuilder out = new StringBuilder();
		out.append("<?xml version=\"1.0\"?>");
		out.append("<errorinfo>");
		out.append("<entry>");

		out.append(HtUtils.wrapToCDataTags(HtWebUtils.writeHtmlErrorToString(e)));

		out.append("</entry>");
		out.append("</errorinfo>");

		return out.toString();
	}

	// ------------------------------------------------------

	public static void uploadToFile(
					HttpServletRequest req,
					HttpServletResponse res,
					String targetDirPath,
					String targetName, // if not null - change this name
					StringBuilder passedFileName
				) throws Exception
	{

		passedFileName.setLength(0);

		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(1024 * 1024);
		ServletFileUpload upload = new ServletFileUpload(factory);


		List /* FileItem */ items = upload.parseRequest(req);
		Iterator iter = items.iterator();


		File tmpF = null;

		while (iter.hasNext()) {
			FileItem item = (FileItem) iter.next();

			String name = item.getFieldName();
			//String contentType = item.getContentType();
			//boolean isInMemory = item.isInMemory();
			//long sizeInBytes = item.getSize();


			if (name.equalsIgnoreCase("package_data")) {

				if (tmpF == null) {
					throw new HtException(getContext(), "uploadToTempFile", "Invalid file name passed");
				}


				item.write(tmpF);

			} else if (name.equalsIgnoreCase("package_data_file_path")) {
				String package_data_file_path = item.getString();
				File f = new File(package_data_file_path);


				// extention
				passedFileName.append(f.getName());


				File f1 = new File(targetDirPath);
				f1.mkdirs();

				if (targetName != null && targetName.length() >0)
				  tmpF = new File( f1.getAbsolutePath() + File.separatorChar + targetName);
				else
				  tmpF = new File( f1.getAbsolutePath() + File.separatorChar + f.getName());

				
				
			}
		}

	}

	// ------------------------------------------------------
	// this uploads to temporary file
	public static void uploadToTempFile(
					HttpServletRequest req,
					HttpServletResponse res,
					StringBuilder packageLoadFullPath,
					StringBuilder packageLoadShortPath,
					StringBuilder packageName) throws Exception {

		packageName.setLength(0);
		packageLoadShortPath.setLength(0);
		packageLoadFullPath.setLength(0);


		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(1024 * 1024);
		ServletFileUpload upload = new ServletFileUpload(factory);


		List /* FileItem */ items = upload.parseRequest(req);
		Iterator iter = items.iterator();


		File tmpF = null;

		while (iter.hasNext()) {
			FileItem item = (FileItem) iter.next();

			String name = item.getFieldName();
			//String contentType = item.getContentType();
			//boolean isInMemory = item.isInMemory();
			//long sizeInBytes = item.getSize();


			if (name.equalsIgnoreCase("package_data")) {

				if (tmpF == null) {
					throw new HtException(getContext(), "uploadToTempFile", "Invalid file name passed");
				}


				item.write(tmpF);

			} else if (name.equalsIgnoreCase("package_data_file_path")) {
				String package_data_file_path = item.getString();
				File f = new File(package_data_file_path);


				// extention
				packageName.append(f.getName());

				int idx = packageName.toString().indexOf('.');
				if (idx < 0) {
					throw new HtException(getContext(), "uploadToTempFile", "Invalid file name: " + packageName);
				}

				String ext = packageName.substring(idx);
				String body = packageName.substring(0, idx);

				tmpF = File.createTempFile(body, ext);

				packageLoadShortPath.append(tmpF.getName());
				packageLoadFullPath.append(tmpF.getAbsolutePath());

			}
		}

	}

	public static void uploadToTempFile(
					HttpServletRequest req,
					HttpServletResponse res,
					String baseName,
					String basePath,
					StringBuilder packageLoadFullPath,
					StringBuilder packageName) throws Exception {

		packageName.setLength(0);
		packageLoadFullPath.setLength(0);


		DiskFileItemFactory factory = new DiskFileItemFactory();
		factory.setSizeThreshold(1024 * 1024);
		ServletFileUpload upload = new ServletFileUpload(factory);


		List /* FileItem */ items = upload.parseRequest(req);
		Iterator iter = items.iterator();


		File tmpF = null;

		while (iter.hasNext()) {
			FileItem item = (FileItem) iter.next();

			String name = item.getFieldName();
			//String contentType = item.getContentType();
			//boolean isInMemory = item.isInMemory();
			//long sizeInBytes = item.getSize();


			if (name.equalsIgnoreCase("package_data")) {

				if (tmpF == null) {
					throw new HtException(getContext(), "uploadToTempFile", "Invalid file name passed");
				}


				item.write(tmpF);

			} else if (name.equalsIgnoreCase("package_data_file_path")) {
				String package_data_file_path = item.getString();
				File f = new File(package_data_file_path);


				// extention
				packageName.append(f.getName());

				int idx = packageName.toString().indexOf('.');
				if (idx < 0) {
					throw new HtException(getContext(), "uploadToTempFile", "Invalid file name: " + packageName);
				}

				//String ext = packageName.substring(idx);
				//String body = packageName.substring(0, idx);

				tmpF = new File(basePath);
				// make dirs
				tmpF.mkdirs();

				if (!tmpF.isDirectory()) {
					throw new HtException(getContext(), "uploadToTempFile", basePath + " is not a directory");
				}


				tmpF = new File(tmpF.getAbsolutePath() + File.separatorChar + baseName);

				packageLoadFullPath.append(tmpF.getAbsolutePath());

			}
		}

	}

	public static String createXMLForDhtmlGrid(GridDataSet dataset) throws Exception {
		StringBuilder out = new StringBuilder();

		out.append("<?xml version=\"1.0\"?>");
		out.append("<rows>");

		for (int i = 0; i < dataset.getRowsNumber(); i++) {


			// values
			out.append("<row id=\"");
			out.append(i);
			out.append("\">");

			for (int k = 0; k < dataset.getColumnsNumber(); k++) {

				out.append("<cell>");
				out.append("<![CDATA[");
				out.append(dataset.getRowValue(i, k));
				out.append("]]>");
				out.append("</cell>");
			}

			out.append("</row>");

		}

		out.append("</rows>");

		return out.toString();
	}

	// this creates HTML
	// for using with DHTML XGRID
	public static String createHTMLForDhtmlGrid(GridDataSet dataset) throws Exception {
		StringBuilder out = new StringBuilder();

		// header
		out.append("<tr>");
		for (int k = 0; k < dataset.getColumnsNumber(); k++) {
			GridFieldDescriptor decsriptor = dataset.getDescriptor(k);

			out.append("<td");

			if (decsriptor.getFieldType().length() > 0) {
				out.append(" type='");
				out.append(decsriptor.getFieldType());
				out.append("'");
			}

			if (decsriptor.getFieldSort().length() > 0) {
				out.append(" sort='");
				out.append(decsriptor.getFieldSort());
				out.append("'");
			}

			if (decsriptor.getFieldAlign().length() > 0) {
				out.append(" align='");
				out.append(decsriptor.getFieldAlign());
				out.append("'");
			}


			if (!dataset.isHeaderWrapped()) {
				out.append(" NOWRAP=true ");
			}
			

			out.append(">");
			out.append(decsriptor.getFieldName());
			out.append("</td>");

		}
		out.append("</tr>\n");

		// body
		for (int i = 0; i < dataset.getRowsNumber(); i++) {

			// values
			out.append("<tr>");

			for (int k = 0; k < dataset.getColumnsNumber(); k++) {

				out.append("<td>");
				out.append(dataset.getRowValue(i, k));
				out.append("</td>");

			}
			out.append("</tr>\n");
		}


		return out.toString();
	}

	public static String serializeForPassingAsUrl_Map(Map<String, String> params) {
		DecimalFormat dcf = new DecimalFormat("000");


		StringBuilder out = new StringBuilder();
		out.append(dcf.format(params.size()));
		for (Iterator<String> it = params.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			String value = params.get(key);

			// <total length> <string length key> <key value> <string length value> <key value>
			// 000 the length - I think enough
			String key_encoded = URLUTF8Encoder.encode(key);
			String value_encoded = URLUTF8Encoder.encode(value);
			out.append(dcf.format(key_encoded.length()));
			out.append(key_encoded);

			out.append(dcf.format(value_encoded.length()));
			out.append(value_encoded);

		}

		return out.toString();
	}

	public static Map<String, String> deserializeForPassingAsUrl_Map(String encodedstr) throws Exception {
		TreeMap<String, String> result = new TreeMap<String, String>();

		if (encodedstr == null || encodedstr.length() <= 0) {
			return result;
		}

		DecimalFormat dcf = new DecimalFormat("000");

		StringReader streader = new StringReader(encodedstr);
		char[] numbuf = new char[3];

		if (streader.read(numbuf) == -1) {
			throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read total map length");
		}

		int totallength = dcf.parse(new String(numbuf)).intValue();
		for (int i = 0; i < totallength; i++) {

			if (streader.read(numbuf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read key length");
			}
			int keylength = dcf.parse(new String(numbuf)).intValue();

			char[] key_buf = new char[keylength];
			if (streader.read(key_buf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read key");
			}

			if (streader.read(numbuf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read value length");
			}
			int valuelength = dcf.parse(new String(numbuf)).intValue();

			char[] value_buf = new char[valuelength];
			if (streader.read(value_buf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read value");
			}

			result.put(URLUTF8Encoder.unescape(new String(key_buf)), URLUTF8Encoder.unescape(new String(value_buf)));

		}

		return result;
	}

	// -----
	public static String serializeForPassingAsUrl_List(List<String> params) {
		DecimalFormat dcf = new DecimalFormat("000");


		StringBuilder out = new StringBuilder();
		out.append(dcf.format(params.size()));
		for (int i = 0; i < params.size(); i++) {


			// <total length> <string length key> <key value> <string length value> <key value>
			// 000 the length - I think enough

			String value_encoded = URLUTF8Encoder.encode(params.get(i));
			out.append(dcf.format(value_encoded.length()));
			out.append(value_encoded);

		}

		return out.toString();
	}

	public static List<String> deserializeForPassingAsUrl_List(String encodedstr) throws Exception {
		List<String> result = new ArrayList<String>();
		DecimalFormat dcf = new DecimalFormat("000");

		StringReader streader = new StringReader(encodedstr);
		char[] numbuf = new char[3];

		if (streader.read(numbuf) == -1) {
			throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read total list length");
		}

		int totallength = dcf.parse(new String(numbuf)).intValue();
		for (int i = 0; i < totallength; i++) {

			if (streader.read(numbuf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read value length");
			}
			int keylength = dcf.parse(new String(numbuf)).intValue();

			char[] key_buf = new char[keylength];
			if (streader.read(key_buf) == -1) {
				throw new HtException(getContext(), "deserializeForPassingAsUrl", "Cannot read value");
			}


			result.add(URLUTF8Encoder.unescape(new String(key_buf)));

		}

		return result;
	}

	//
	/**
	 * Decodes the passed UTF-8 String using an algorithm that's compatible with
	 * JavaScript's <code>decodeURIComponent</code> function. Returns
	 * <code>null</code> if the String is <code>null</code>.
	 *
	 * @param s The UTF-8 encoded String to be decoded
	 * @return the decoded String
	 */
	public static String decodeURIComponent(String s) {
		if (s == null) {
			return null;
		}

		String result = null;

		try {
			result = URLDecoder.decode(s, "UTF-8");
		} // This exception should never occur.
		catch (UnsupportedEncodingException e) {
			return null;
		}

		return result;
	}

	/**
	 * Encodes the passed String as UTF-8 using an algorithm that's compatible
	 * with JavaScript's <code>encodeURIComponent</code> function. Returns
	 * <code>null</code> if the String is <code>null</code>.
	 *
	 * @param s The String to be encoded
	 * @return the encoded String
	 */
	public static String encodeURIComponent(String s) {
		String result = null;

		try {
			result = URLEncoder.encode(s, "UTF-8").replaceAll("\\+", "%20").replaceAll("\\%21", "!").replaceAll("\\%27", "'").replaceAll("\\%28", "(").replaceAll("\\%29", ")").replaceAll("\\%7E", "~");
		} // This exception should never occur.
		catch (UnsupportedEncodingException e) {
			return null;
		}

		return result;
	}

	public static <T> String createURLListParameterHelper(T[] values) {
		StringBuilder out = new StringBuilder();
		if (values != null) {


			for (int i = 0; i < values.length; i++) {
				out.append(String.valueOf(values[i]));
				out.append(",");

			}
		}

		return out.toString();
	}
	
	public static String createClickHtml(String onClickParam, String caption)
	{
		StringBuilder data = new StringBuilder();
		
		data.append("<div ").append("style='cursor:hand;text-decoration:underline' ").append("onclick='").append(onClickParam).append("' >");
		data.append(caption);
		data.append("</div>");
		
		return data.toString();
	}
}
