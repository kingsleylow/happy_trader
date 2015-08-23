/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import com.fin.httrader.utils.ThreadLocalFormats.HtThreadLocalDecimalFormat;
import com.fin.httrader.utils.ThreadLocalFormats.HtThreadLocalSimpleDateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.TimeZone;


/**
 *
 * @author DanilinS
 */
public class HtDateTimeUtils {

	private static HtThreadLocalSimpleDateFormat gmtSimpleFormat_m = new HtThreadLocalSimpleDateFormat("dd-MM-yyyy HH:mm:ss", "GMT-0");
	private static HtThreadLocalSimpleDateFormat gmtFormat_m = new HtThreadLocalSimpleDateFormat("dd-MM-yyyy'T'HH:mm:ss.S", "GMT-0");
	//private static HtThreadLocalDecimalFormat nf_m = new HtThreadLocalDecimalFormat('.', 20);


	

	private HtDateTimeUtils()
	{
	}
	
	public static String getContext() {
		return HtDateTimeUtils.class.getSimpleName();
	}

	public static double convertUnixTimeMillisec2MatlabTime(double utimemillisec) {
		return ((utimemillisec / 86400000.0) + 719529.0);
	}

	public static double convertUnixTimeMillisec2MatlabTime(long utimemillisec) {
		return ((utimemillisec / 86400000.0) + 719529.0);
	}

	// parsed date entered from web page
	// adjust local to GMT if necessary

	public static long parseDateTimeParameter_ToGmt(String parameter, String format) throws Exception {

		long dateValue = -1;

		SimpleDateFormat gmtFormat = new SimpleDateFormat(format);
		// use GMT only
		gmtFormat.setTimeZone(TimeZone.getTimeZone("GMT-0"));
		


		// need to parse to GMT time only to avoid any doubts
		if (parameter != null && parameter.length() > 0) {
			dateValue = gmtFormat.parse(parameter).getTime();
		}


		return dateValue;

	}

	public static long parseDateTimeParameter(String parameter, boolean adjustFromLocal) throws Exception {

		long dateValue = -1;

		SimpleDateFormat ascFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm");
		int hour_shift = 0;

		if (adjustFromLocal) {
			TimeZone dtz = TimeZone.getDefault();
			ascFormat.setTimeZone(dtz);

			hour_shift = dtz.getDSTSavings();

		} else {
			// use GMT only
			ascFormat.setTimeZone(TimeZone.getTimeZone("GMT-0"));
		}


		// need to parse to GMT time only to avoid any doubts
		if (parameter != null && parameter.length() > 0) {
			dateValue = ascFormat.parse(parameter).getTime() + hour_shift;
		}


		return dateValue;

	}
	
	//
	public static String time2SimpleString_Local(long val) {

		if (val < 0) {
			return "";
		}

		GregorianCalendar tm = new GregorianCalendar();
		tm.setTimeInMillis(val);
		return String.format("%1$td-%1$tm-%1$tY %1$tH:%1$tM:%1$tS", tm);

	}

	public static String time2SimpleString_Gmt(long val) {
		if (val < 0) {
			return "";
		}

		return gmtSimpleFormat_m.getFormatThreadCopy().format(new Date(val));
	}

	public static String time2SimpleString_Gmt(double val) {
		return time2SimpleString_Gmt((long) val);
	}

	public static String time2String_Local(long val) {

		if (val < 0) {
			return "";
		}

		GregorianCalendar tm = new GregorianCalendar();
		tm.setTimeInMillis(val);

		return String.format("%1$td-%1$tm-%1$tYT%1$tH:%1$tM:%1$tS.%1$tL", tm);
	}

	// converts these values to time
	// it accept actual month !!!
	// it also accepts Calendar reference if necessary
	public static long yearMonthDay2Time_Gmt(int year, int month, int day) {
		if (year <= 0 || month < 1 || day < 1) {
			return -1;
		}

		Calendar tm = new GregorianCalendar();

		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		tm.setTimeZone(tzone);



		tm.set(year, month - 1, day, 0, 0, 0);
		tm.set(Calendar.MILLISECOND, 0);

		return tm.getTimeInMillis();

	}

	// converts these values to time
	// it accept actual month !!!
	// it also accepts Calendar reference if necessary
	public static long yearMonthDayHourMinSec2Time_Gmt(int year, int month, int day, int hour, int minute, int sec) {
		if (year <= 0 || month < 1 || day < 1) {
			return -1;
		}

		Calendar tm = new GregorianCalendar();

		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		tm.setTimeZone(tzone);



		tm.set(year, month - 1, day, hour, minute, sec);
		tm.set(Calendar.MILLISECOND, 0);

		return tm.getTimeInMillis();

	}

	// another version
	// it allows re-use of Calendar instance
	public static Calendar yearMonthDay2Time_ReuseCalendar_Gmt(int year, int month, int day, Calendar tmc, LongParam result) {
		if (year <= 0 || month < 1 || day < 1) {
			return null;
		}

		Calendar tm = null;

		if (tmc == null) {
			// init
			tm = new GregorianCalendar();
			TimeZone tzone = TimeZone.getTimeZone("GMT-0");
			tm.setTimeZone(tzone);
		} else {
			tm = tmc;
		}

		tm.set(year, month - 1, day, 0, 0, 0);
		tm.set(Calendar.MILLISECOND, 0);

		result.setLong(tm.getTimeInMillis());

		// return the instance
		return tm;
	}

	// another version
	// it allows re-use of Calendar instance
	public static Calendar yearMonthDayHourMinSec2Time_ReuseCalendar_Gmt(int year, int month, int day, int hour, int minute, int sec, Calendar tmc, LongParam result) {
		if (year <= 0 || month < 1 || day < 1) {
			return null;
		}

		Calendar tm = null;

		if (tmc == null) {
			// init
			tm = new GregorianCalendar();
			TimeZone tzone = TimeZone.getTimeZone("GMT-0");
			tm.setTimeZone(tzone);
		} else {
			tm = tmc;
		}


		tm.set(year, month - 1, day, hour, minute, sec);
		tm.set(Calendar.MILLISECOND, 0);

		result.setLong(tm.getTimeInMillis());

		// return the instance
		return tm;
	}

	public static void time2YearMonthDay_Gmt(long val, LongParam year, LongParam month, LongParam day) {
		if (val < 0) {
			return;
		}

		GregorianCalendar tm = new GregorianCalendar();
		tm.setTimeInMillis(val);

		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		tm.setTimeZone(tzone);


		year.setLong(tm.get(Calendar.YEAR));
		month.setLong(tm.get(Calendar.MONTH) + 1);
		day.setLong(tm.get(Calendar.DAY_OF_MONTH));

	}

	public static void time2YearMonthDayHourMinSec_Gmt(
					long val,
					LongParam year, LongParam month, LongParam day, LongParam hour, LongParam minute, LongParam secs) {
		if (val < 0) {
			return;
		}

		GregorianCalendar tm = new GregorianCalendar();
		tm.setTimeInMillis(val);

		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		tm.setTimeZone(tzone);


		year.setLong(tm.get(Calendar.YEAR));
		month.setLong(tm.get(Calendar.MONTH) + 1);
		day.setLong(tm.get(Calendar.DAY_OF_MONTH));
		hour.setLong(tm.get(Calendar.HOUR_OF_DAY));
		minute.setLong(tm.get(Calendar.MINUTE));
		secs.setLong(tm.get(Calendar.SECOND));

	}

	public static String time2String_Gmt(long val) {

		if (val < 0) {
			return "";
		}

		return gmtFormat_m.getFormatThreadCopy().format(new Date(val));
	}

	public static long getCurGmtTime() {
		return System.currentTimeMillis();
	}

	// this gets the current date and round to the beginning of that day
	public static long getCurGmtTime_DayBegin() {

		Calendar dCal = Calendar.getInstance();

		//Sets the values for the fields YEAR, MONTH, DAY_OF_MONTH, HOUR, MINUTE, and SECOND.
		// because we perform recalculation here we need to set up the zone correctly
		// pure UTC

		TimeZone tzone = TimeZone.getTimeZone("GMT-0");
		dCal.setTimeZone(tzone);

		int year = dCal.get(Calendar.YEAR);
		int month = dCal.get(Calendar.MONTH);
		int day = dCal.get(Calendar.DAY_OF_MONTH);



		//dCal.set(Calendar.DST_OFFSET, 0);
		//dCal.set(Calendar.ZONE_OFFSET, 0);




		dCal.set(year, month, day, 0, 0, 0);
		dCal.set(Calendar.MILLISECOND, 0);


		return dCal.getTimeInMillis();


	}
	
	public static long convertUinxSecToUnixMsec(long unixSecs)
	{
			if (unixSecs < 0)
					return -1;
			else if (unixSecs == 0)
					return 0;
			else 
					return unixSecs * 1000;
					
	}
	
	public static double convertUinxMsecToUnixSec(long unixSecs)
	{
			if (unixSecs < 0)
					return -1;
			else if (unixSecs == 0)
					return 0;
			else 
					return (double)unixSecs / 1000.0;
					
	}
}
