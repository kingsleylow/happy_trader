/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.interfaces.BackgroundJobFunctor;
import com.fin.httrader.managers.RtBackgroundJobManager;
import com.fin.httrader.managers.RtBackgroundJobManager.BackgroundJobThread;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBackgroundJobProxy extends HtProxyBase {
	
	public HtBackgroundJobProxy()
	{
	}
	
	public void remote_startBackgroundJob(BackgroundJobFunctor bckFunctor, String strId, boolean isExclusive, String typeId) throws Exception
	{
		RtBackgroundJobManager.instance().startBackgroundJob(bckFunctor, strId, isExclusive, typeId);
	}
	
	public Map<String, BackgroundJobThread> remote_getAllRunningJobs() throws Exception
	{
		return RtBackgroundJobManager.instance().getAllRunningJobs();
	}
	
	public List<RtBackgroundJobManager.BackgroundJobEntry> remote_getAllRunningJobsDescriptors() throws Exception
	{
		return RtBackgroundJobManager.instance().getAllRunningJobsDescriptors();
	}
	
	
	public BackgroundJobThread remote_getJob(String jobId) throws Exception
	{
		return RtBackgroundJobManager.instance().getJob(jobId);
	}
	
	@Override
	public String getProxyName() {
		return "bck_job_proxy";
	}
	
}
