/*
 * HtExtLogger.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import java.io.File;
import java.io.RandomAccessFile;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class HtExtScriptLogger extends HtExtBaseLogger {
	//

	File file_m = null;
	RandomAccessFile raccess_m = null;

	// ----------------------

	public abstract void addStringEntry_Derived(String data) throws Exception;

	public abstract void addErrorEntry_Derived(String data) throws Exception;

	// ----------------------
	public HtExtScriptLogger(Uid runUid) throws Exception {
		
		file_m = HtCommandProcessor.instance().get_HtExternalApiProxy().remote_resolveHtmlLogFilePath(runUid, false);
		raccess_m = new RandomAccessFile(file_m, "rw");



	}

	@Override
	public void addStringEntry(String data) throws HtExtException {

		String dataf = createAppropriateDataFormatting(addDataInfo(data));

		try {

			writeToFile(dataf);
			addStringEntry_Derived(dataf);
		} catch (Throwable e) {
			writeToFile(createAppropriateErrorFormatting("Exception on making output: " + e.getMessage()));
		}


	}

	@Override
	public void addErrorEntry(String data) throws HtExtException {
		String dataf = createAppropriateErrorFormatting(addDataInfo(data));

		try {

			writeToFile(dataf);
			addErrorEntry_Derived(dataf);

		} catch (Throwable e) {
			writeToFile(createAppropriateErrorFormatting("Exception on making error output: " + e.getMessage()));
		}





	}

	public void finishLogWriting() {
		writeToFile(addDataInfo("End of output"));
		try {
			FileChannel channel = raccess_m.getChannel();

			if (channel != null) {
				channel.close();
			}

			if (raccess_m != null) {
				raccess_m.close();
				raccess_m = null;
			}
		} catch (Throwable e) {
			// ignore
		}
	}

	/*
	 * HELPERS
	 */
	
	private void writeToFile(String data) {
		FileChannel channel = raccess_m.getChannel();
		FileLock lock = null;

		try {
			lock = channel.lock();

			raccess_m.seek(raccess_m.length());
			raccess_m.writeBytes(data);

		} catch (Throwable e) {
		} finally {
			if (lock != null) {
				try {
					lock.release();
				} catch (Throwable e) {
				}
			}
		}
	}

	private String addDataInfo(String data) {
		StringBuilder out = new StringBuilder();
		out.append("[");
		out.append(HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()));
		out.append("] - ( ");
		out.append(Thread.currentThread().getId());
		out.append(" ) - ");

		// adopt data
		out.append(data);

		return out.toString();
	}

	// this creates HTML formatting
	private String createAppropriateErrorFormatting(String data) {
		String[] st = data.split("[\n\r]");

		StringBuilder line = new StringBuilder();
		for (int i = 0; i < st.length; i++) {
			String w = st[i].replaceAll("\\\\", "\\\\\\\\");
			w = w.replaceAll("[']", "\\\\'");


			line.append("<div style=\"background-color:darkorange;\">");
			line.append(w);
			line.append("</div>");

		}

		return line.toString();
	}

	private String createAppropriateDataFormatting(String data) {
		String[] st = data.split("[\n\r]");

		StringBuilder line = new StringBuilder();
		for (int i = 0; i < st.length; i++) {
			//replacement.replaceAll("\\\\", "\\\\\\\\");
			String w = st[i].replaceAll("\\\\", "\\\\\\\\");
			w = w.replaceAll("[']", "\\\\'");

			line.append(w);
			line.append("<br>");

		}

		return line.toString();
	}
}
