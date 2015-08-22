/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httraderdevelop.utils;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httraderdevelop.HtVolumeAnalizerView;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class HtFrameLogger {
	public static void log(Level level, String sourceClass, String sourceMethod, String msg)
	{
		StringBuilder entry = new StringBuilder();
		entry.append(HtDateTimeUtils.time2SimpleString_Local(HtDateTimeUtils.getCurGmtTime()));
		entry.append(" - [ ");
		entry.append(Thread.currentThread().getId());
		entry.append(" ] - ");
		entry.append(msg);
		HtVolumeAnalizerView.getInstance().addLoggingEntry(entry.toString());
	}
}
