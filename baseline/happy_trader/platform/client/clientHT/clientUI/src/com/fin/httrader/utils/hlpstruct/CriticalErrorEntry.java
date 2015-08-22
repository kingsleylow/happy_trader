/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;

/**
 *
 * @author DanilinS
 * Entry describing critical error/warn entry
 */
public class CriticalErrorEntry {

	public static int URGENCY_FLAG_EXCEPT = 1;
	public static int URGENCY_FLAG_WARN = 2;
	public static int URGENCY_FLAG_INFO = 3;
	
	
	public CriticalErrorEntry(String context, String srvSessionName, String reason, String callstack, int urgentFlag) {
		context_m.append(context);
		session_m.append(srvSessionName);
		reason_m.append(reason);
		time_m = HtDateTimeUtils.getCurGmtTime();
		urgentFlag_m = urgentFlag;
		callstack_m.append(callstack);
		threadId_m = Thread.currentThread().getId();
	};

	public String getContext()
	{
		return context_m.toString();
	}

	public String getSession()
	{
		return session_m.toString();
	}
	
	public String getCallStack()
	{
		return callstack_m.toString();
	}

	public String getReason()
	{
		return reason_m.toString();
	}

	public long getTime()
	{
		return time_m;
	}
	
	public long getThreadId()
	{
		return threadId_m;
	}

	public int getUrgencyFlag()
	{
		return urgentFlag_m;
	}
	
	public String getUrgencyFlagString()
	{
		if (getUrgencyFlag() == URGENCY_FLAG_EXCEPT) {
				return "EXCEPTION";
		}
		else if (getUrgencyFlag() == URGENCY_FLAG_WARN) {
				return "WARN";
		}
		else if (getUrgencyFlag() == URGENCY_FLAG_INFO) {
				return "INFO";
		}
		
		return "N/A";
	}

	@Override
	public String toString()
	{
		StringBuilder out = new StringBuilder();
		
		out.append("\n------------------------------------------------------------\n");
		out.append("[ ");
		out.append(HtDateTimeUtils.time2String_Gmt(getTime()));
		out.append(" ] - ");
		out.append("[ ").append(getThreadId()).append(" ] - ");
		if (getUrgencyFlag() == URGENCY_FLAG_EXCEPT) {
				out.append("EXCEPTION: ( " );
		}
		else if (getUrgencyFlag() == URGENCY_FLAG_WARN) {
				out.append("WARN: ( " );
		}
		else if (getUrgencyFlag() == URGENCY_FLAG_INFO) {
				out.append("INFO: ( " );
		}
	
		out.append(getContext());
		out.append(" ) ( ");
		out.append(getSession());
		out.append(" ) - \"");
		out.append(getReason()).append("\"");
		if (callstack_m.length() > 0)
			out.append("\n").append(callstack_m).append("\n");
		
		
		return out.toString();
	}
	// ------------

	private final StringBuilder context_m = new StringBuilder();

	private final StringBuilder session_m = new StringBuilder();

	private final StringBuilder reason_m = new StringBuilder();
	
	private final StringBuilder callstack_m = new StringBuilder();

	private long time_m = -1;

	// if this entry can be ignored (like warning)
	private int urgentFlag_m = URGENCY_FLAG_INFO;
	
	long threadId_m;
}
