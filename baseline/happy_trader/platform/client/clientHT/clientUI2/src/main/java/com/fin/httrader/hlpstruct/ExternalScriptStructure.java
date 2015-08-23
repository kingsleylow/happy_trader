/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.utils.HtEnumeratorDescriptor;
import com.fin.httrader.utils.Uid;

/**
 *
 * @author DanilinS
 */
public class ExternalScriptStructure {

	public static final int STATUS_DUMMY = 0;
	public static final int STATUS_PENDING = 1;
	public static final int STATUS_RUN = 2;
	public static final int STATUS_FINISHED = 3;

	private static HtEnumeratorDescriptor descriptor_m = new HtEnumeratorDescriptor("com.fin.httrader.hlpstruct.ExternalScriptStructure");

	public static String getStatusName(int pt) {
		return descriptor_m.getNameById_StrictValue(pt);
	}


	// members

	public ExternalScriptStructure()
	{
	}

	public ExternalScriptStructure(Uid run_uid, int status, double start_time, double end_time, String name, String customResultString)
	{
		status_m = status;
		start_time_m = start_time;
		end_time_m = end_time;
		run_uid_m.fromUid(run_uid);

		name_m.append(name);
		customResultString_m.append(customResultString);

	}

	public int getStatus()
	{
		return status_m;
	}

	public void setStatus(int status)
	{
		status_m = status;
	}

	public void setEndTime(double end_time)
	{
		end_time_m = end_time;
	}

	public void setBeginTime(double start_time)
	{
		start_time_m = start_time;
	}

	public String getName()
	{
		return name_m.toString();
	}

	public double getBeginTime()
	{
		return start_time_m;
	}

	public double getEndTime()
	{
		return end_time_m;
	}

	public Uid getUid()
	{
		return run_uid_m;
	}
	
	public String getCustomresultString()
	{
	  return customResultString_m.toString(); 
	}
	
	public void setCustomresultString(String resultString)
	{
	  customResultString_m.setLength(0);
	  customResultString_m.append(resultString);
	}


	private int status_m = STATUS_DUMMY;
	private StringBuilder name_m = new StringBuilder();
	private double start_time_m = -1;
	private double end_time_m = -1;
	private Uid run_uid_m = new Uid();
	private StringBuilder customResultString_m = new StringBuilder();

}
