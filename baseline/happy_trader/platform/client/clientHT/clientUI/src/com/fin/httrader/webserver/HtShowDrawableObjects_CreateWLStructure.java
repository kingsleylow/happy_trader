/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtDrawableObjectSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;
import java.util.zip.ZipOutputStream;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author DanilinS
 * This will create WL Developer WS structure to see this
 */
public class HtShowDrawableObjects_CreateWLStructure extends HtServletsBase {

	public static final String BEGIN_SEP_TAG	= "<B";
	public static final String END_SEP_TAG		= "E>";

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		

		try {

			// set this page UID
			setUniquePageId(getStringParameter(req, "page_uid", false));


			String begDateStr = getStringParameter(req, "bdate", true);
			String endDateStr = getStringParameter(req, "edate", true);

			String shortTextFilter = getStringParameter(req, "short_text_filter", true);
			String longTextFilter = getStringParameter(req, "long_text_filter", true);
			String runNameFilter = getStringParameter(req, "run_name_filter", true);


			// if provide milliseconds
			final boolean time_in_millisec = getBooleanParameter(req, "time_in_millisec", true);
			boolean allIfMapEmpty = getBooleanParameter(req, "all_if_no_symb", true);

			long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
			long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);

			// paging
			// -1 means all
			long page_num = getIntParameter(req, "read_page_num", false);
			long read_rows_per_page = getIntParameter(req, "read_rows_per_page", false);



			String profile = getStringParameter(req, "profile", true);
			List<HtPair<String, String>> provSymbMapPage = getStringPairListParameter(req, "ps", true);
			List<Integer> type = getIntListParameter(req, "type", true);

			final SimpleDateFormat dtFormat = new SimpleDateFormat("dd-MM-yyyy,HH:mm:ss");
			dtFormat.setTimeZone(TimeZone.getTimeZone("GMT"));

			
			// load history data
			//final HttpServletResponse fres = res;
			final ServletOutputStream ostrm = res.getOutputStream();
			final XmlHandler hndl = new XmlHandler();
			final StringBuilder chunk = new StringBuilder();
			final DecimalFormat dcf = new DecimalFormat("0000000000");

			//XmlParameter out_param = new XmlParameter();
			
			//final List<XmlParameter> drowableObjList = new ArrayList<XmlParameter>();

			HtDrawableObjectSelector dataselector = new HtDrawableObjectSelector() {

				public void onDrawableObjectArrived(HtDrawableObject dobj) throws Exception
				{
					XmlParameter param = new XmlParameter();
					dobj.drawableObjectToParameter(dobj, param);

					//drowableObjList.add(param);

					hndl.serializeParameter(param, chunk);
					
					ostrm.print( dcf.format(chunk.length()) );
					ostrm.print(chunk.toString());
				
					
				
				};

				@Override
				public void onFinish(long cntr) throws Exception {
					
				}

				
			};

			StringBuilder hd_cont= new StringBuilder();
			hd_cont.append("attachment; filename=");
			hd_cont.append("data_");
			hd_cont.append(HtDateTimeUtils.getCurGmtTime());
			hd_cont.append(".dat");

			res.setHeader("Content-disposition", hd_cont.toString());
			res.setHeader("Content-Type", "application/octet-stream");


			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showDrawableObjects_Callback(
							profile,
							provSymbMapPage,
							allIfMapEmpty,
							begDate,
							endDate,
							type,
							shortTextFilter,
							longTextFilter,
							runNameFilter,
							null,
							dataselector,
							page_num,
							read_rows_per_page);

			/*
			out_param.setXmlParameterList("drawable_objects", drowableObjList);
			StringBuilder out = new StringBuilder();
			hndl.serializeParameter(out_param, out);
			 */

			
			//ostrm.print(out.toString());

			ostrm.close();

			//drowableObjList.clear();

		// output conte

		} catch (Throwable e) {
			res.setContentType("text/html; charset=windows-1251");
			forceNoCache(res);

			writeHtmlErrorToOutput(res, e);

		}
	}

	/*
	 * Helpers
	 */
	void writeContentToFiles(Map<String, StringBuilder> filesContent, String extention, String tmpFileDir) throws Exception {
		for (Iterator<String> it = filesContent.keySet().iterator(); it.hasNext();) {
			String key_i = it.next();
			StringBuilder content_i = filesContent.get(key_i);

			StringBuilder tmpFileName = new StringBuilder(tmpFileDir);
			tmpFileName.append(File.separatorChar);
			tmpFileName.append(key_i);
			tmpFileName.append(".");
			tmpFileName.append(extention);

		
			FileWriter writer = new FileWriter(tmpFileName.toString());
			writer.write(content_i.toString());
			writer.close();

		}
	}

	void createZipFromTempDir(String tmpDir, ServletOutputStream ostrm) throws Exception {

		ZipOutputStream out = new ZipOutputStream(new BufferedOutputStream(ostrm));
		HtFileUtils.zipDir(tmpDir, out);
	}

	String resolveTempDirectory() {
		StringBuilder tmpFileDir = new StringBuilder(HtSystem.getSystemTempPath());
		tmpFileDir.append(File.separatorChar);
		tmpFileDir.append("ht.tmp");
		tmpFileDir.append(Uid.generateNewStringUid());
		File fdir = new File(tmpFileDir.toString());
		fdir.mkdirs();

		return fdir.getAbsolutePath();
	}

	void deleteTempDirectory(String tmpDir) {
		(new File(tmpDir)).delete();
	}
}
