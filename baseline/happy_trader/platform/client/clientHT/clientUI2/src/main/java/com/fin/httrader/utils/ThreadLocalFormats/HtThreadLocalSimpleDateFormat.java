/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.ThreadLocalFormats;

import java.text.SimpleDateFormat;
import java.util.TimeZone;

/**
 *
 * @author DanilinS
 * This class provide access to SimpleDate format separate for each thread
 * to avoid mutexing as we are not greedy 
 */
public class HtThreadLocalSimpleDateFormat {

		String format_m = null;
		String timeZoneString_m = null;
		public HtThreadLocalSimpleDateFormat(String format, String timeZoneString)
		{
			format_m = format;
			timeZoneString_m = timeZoneString;
		}

    private final ThreadLocal local_thread_m = new ThreadLocal() {
				@Override
        protected Object initialValue() {
            SimpleDateFormat df = new SimpleDateFormat(format_m);
						df.setTimeZone(TimeZone.getTimeZone(timeZoneString_m));
						return df;

        }
    };

		public SimpleDateFormat getFormatThreadCopy()
		{
			return (SimpleDateFormat)local_thread_m.get();
		}

}
