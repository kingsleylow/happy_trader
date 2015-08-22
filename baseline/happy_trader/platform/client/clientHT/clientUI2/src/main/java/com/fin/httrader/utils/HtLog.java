/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.io.File;
import java.util.Calendar;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.fin.httrader.model.HtCommandProcessor;

/**
 *
 * @author DanilinS
 */
public class HtLog {

	private static HtLog instance_m = null;
	private static Logger log_m = null;
	StringBuilder logname_m = new StringBuilder();


	// ----------------------------
	// ctor
	private HtLog(Level level) throws Exception {
		if (log_m == null) {
			log_m = Logger.getLogger("");
			Handler[] handlers = log_m.getHandlers();
			for (Handler handler : handlers) {
				log_m.removeHandler(handler);
			}


			Calendar cld = Calendar.getInstance();

			// resolve log path



			logname_m.append(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getLogDirectory());
			logname_m.append(File.separator);
			logname_m.append( "httrader_");

			logname_m.append(String.format("%1$td-%1$tm-%1$tY_%1$tH-%1$tM-%1$tS", cld.getTimeInMillis()));
			logname_m.append(".log");


			FileHandler filehandler = new FileHandler(logname_m.toString());

			HtLogFormatter fmt = new HtLogFormatter();

			filehandler.setFormatter(fmt);

			log_m.addHandler(filehandler);
			log_m.setLevel(level);
		}

	}

	// ---------------------
	// public
	// functions

	public static void create(Level level) throws Exception {
		instance_m = new HtLog(level);
	}

	public static String getLogFileName()
	{
		if (instance_m != null) {
			return instance_m.logname_m.toString();
		}

		return "";
	}

	public static Logger getLogging() {
		return log_m;
	}

	public static void log(Level level, String sourceClass, String sourceMethod, String msg, Object[] params) {
		if (log_m == null) {
			return;
		}

		log_m.logrb(level, sourceClass, sourceMethod, "", msg, params);

		if (level == Level.SEVERE) {
			System.out.println("FATAL exception happened! \"" + msg + "\". Aborting... ");
			System.exit(-24);
		}
	}
	

	public static void log(Level level, String sourceClass, String sourceMethod, String msg) {
		log(level, sourceClass, sourceMethod, msg, null);
	}

}
