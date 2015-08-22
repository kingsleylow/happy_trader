/*
 * CommonLog.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;





import com.fin.httrader.utils.HtDateTimeUtils;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;


/**
 *
 * @author Administrator
 * Class incapsulating log information
 */
public class CommonLog {

	public static final int LL_FATAL = 1;
	public static final int LL_ERROR = 2;
	public static final int LL_WARN = 3;
	public static final int LL_INFO = 4;
	public static final int LL_DEBUG = 5;
	public static final int LL_DEBUGDETAIL = 6;

	public static final String[] LogLevels_m = {"N/A", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "DEBUG DETAIL"};
	private static final TreeSet<Integer> availableLevels_m = new TreeSet<Integer>
	(
			Arrays.asList	(	new Integer[]{ LL_FATAL, LL_ERROR, LL_WARN, LL_INFO, LL_DEBUG, LL_DEBUGDETAIL	}	)
	);

	private static final TreeMap<Integer, String> availableLevelsMap_m = new TreeMap<Integer, String>();

	static {

		for(Iterator<Integer> it = availableLevels_m.iterator(); it.hasNext();) {
			Integer ll = it.next();
			availableLevelsMap_m.put(ll, LogLevels_m[ll]);
		}

	}

	public static Set<Integer> getAvailableLogLevels() {
		return availableLevels_m;
	}

	public static Map<Integer, String> getAvailableLogLevelsMap() {
		return availableLevelsMap_m;
	}

	public static String getLogLevelName(int loglevel) {
		if (loglevel >= LL_FATAL && loglevel <= LL_DEBUGDETAIL)
			return LogLevels_m[loglevel];
		else
			return "";
	}

	/** Creates a new instance of CommonLog */
	public CommonLog() {
		logDate_m = HtDateTimeUtils.getCurGmtTime();
		logLevel_m = LL_INFO;
	}

	public CommonLog(long logThread, String session, int logLevel, long logDate, String context, 	String totalContent)
	{
		logThread_m = logThread;

		session_m.append(session);

		logLevel_m = logLevel;
		logDate_m = logDate;

		context_m.append(context);
		content_m.append(totalContent);


		
	}

	// create a deep copy
	public void create(CommonLog src) {
		logThread_m = src.logThread_m;

		logLevel_m = src.logLevel_m;

		context_m.setLength(0);
		context_m.append(src.context_m);

		logDate_m = src.logDate_m;

		content_m.setLength(0);
		content_m.append(src.content_m);

		session_m.setLength(0);
		session_m.append(src.session_m);


		
	}

	public void setContext(String context) {
		context_m.setLength(0);
		context_m.append(context);
	}

	public String getContext() {
		return context_m.toString();
	}

	public void setContent(String context) {
		content_m.setLength(0);
		content_m.append(context);
	}

	public String getContent() {
		return content_m.toString();
	}

	// just add sto the content
	public void addContentedAttribute(String attribute)
	{
		content_m.append("\r\n");
		if (attribute != null)
			content_m.append(attribute);
	}

	public void setSession(String context) {
		session_m.setLength(0);
		session_m.append(context);
	}

	public String getSession() {
		return session_m.toString();
	}

	public void setLogLevel(int loglevel) {
		logLevel_m = loglevel;
	}

	public int getLogLevel() {
		return logLevel_m;
	}

	

	public void setLogDate(long logdatemilis) {
		logDate_m = logdatemilis;
	}

	public long getLogDate() {
		return logDate_m;
	}

	public void setLogThread(long logThread) {
		logThread_m = logThread;
	}

	public long getLogThread() {
		return logThread_m;
	}

	
	public static void createXmlEventCommonLog(CommonLog commonlog, XmlEvent event) {
		event.clear();


		event.getAsLog(true).create(commonlog);
		event.setEventType(XmlEvent.ET_CommonLog);
	}

	public String toSmallDebugString()
	{
		StringBuilder out = new StringBuilder("Log entry: [ ");
		out.append( getSession() );
		out.append(	" ] [ ");
		out.append(getLogLevelName(getLogLevel()));
		out.append(" ] - " );
		out.append(getContext());
		out.append(" ] ");

		return out.toString();
	}
	//
	private long logThread_m = -1;
	private int logLevel_m;
	private StringBuilder context_m = new StringBuilder();
	private long logDate_m;
	private StringBuilder content_m = new StringBuilder();
	private StringBuilder session_m = new StringBuilder();

};
