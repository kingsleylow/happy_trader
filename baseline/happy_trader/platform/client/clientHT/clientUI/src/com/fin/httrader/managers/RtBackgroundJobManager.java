/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.BackgroundJobFunctor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.Uid;

import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 * @author Victor_Zoubok
 * This class must implement any background job 
 */
public class RtBackgroundJobManager implements RtManagerBase{

	// this is just a descriptor
	static public class BackgroundJobEntry implements Comparable<BackgroundJobEntry>
	{
		private boolean exclusive_m = false;
		String strId_m = null;
		String typeId_m = null;
		long started_m = -1;
		long finished_m = -1;
		BackgroundJobStatus status_m = new BackgroundJobStatus();
		boolean isStarted_m = false;
		
		BackgroundJobEntry(
				boolean exclusive,
				String strId,
				String typeId,
				long started,
				long finished,
				BackgroundJobStatus status,
				boolean isStarted
		)
		{
			exclusive_m = exclusive;
			strId_m = strId;
			typeId_m = typeId;
			started_m = started;
			finished_m = finished;
			status_m.setUp(status.getStatusCode(), status.getStatusReason());
			isStarted_m = isStarted;
		}
		
		public long getStartTime()
		{
			
				return finished_m;
			
		}
		
		public long getFinishTime()
		{
			
				return started_m;
			
		}
		
		public BackgroundJobStatus getStatus()
		{
			return status_m;
		}
		
		public boolean isStarted()
		{
			return isStarted_m;
		}
		
		public boolean ifExclusiveLock()
		{
			return exclusive_m;
		}
		
		public String getStringId()
		{
			return strId_m.toString();
		}
		
		public String getTypeId()
		{
			return typeId_m.toString();
		}

		
		@Override
		public int compareTo(BackgroundJobEntry o) {
			return Long.valueOf(o.getStartTime()).compareTo(Long.valueOf(o.getStartTime()));
		}
		
	};
	
	static public class BackgroundJobThread extends Thread
	{
	
		// databse ID
		private long dbId_m = -1;		
		
		// with the sane type ID- can be shared or exclusevely locked
		private boolean exclusive_m = false;
		
		// type ID  - any string identigying type - like "IMPORT" or "EXPORT"
		private StringBuilder typeId_m = new StringBuilder();
		
		// unique string ID
		private StringBuilder strId_m = new StringBuilder();
		
		private BackgroundJobFunctor functor_m = null;
		private Event threadStarted_m = new Event();
		private RtBackgroundJobManager base_m = null;
		
		private long started_m = -1;
		private long finished_m = -1;
		
		// multithread
		private BackgroundJobStatus status_m = new BackgroundJobStatus();
	
		
		private String getContext() {
			return BackgroundJobThread.class.getSimpleName() + " " + this.hashCode();
		}
		
		public BackgroundJobThread(long dbId, RtBackgroundJobManager base,BackgroundJobFunctor bckFunctor, boolean isExclusive, String strId, String typeId) throws Exception
		{
			dbId_m = dbId;
			exclusive_m = isExclusive;
			strId_m.append(strId);
			typeId_m.append(typeId);
			base_m = base;
			functor_m = bckFunctor;
			status_m.initializeOk();
			
			start();
			
		
			if (!threadStarted_m.waitEvent(1000)) {
				throw new HtException(getContext(), getContext(), "Working thread cannot be started");
			}
			
		}
		
		public long getStartTime()
		{
			synchronized(this) {
				return started_m;
			}
		}
		
		public long getFinishTime()
		{
			synchronized(this) {
				return finished_m;
			}
		}
		
		public BackgroundJobStatus getStatus()
		{
			return status_m;
		}
		
		public boolean isStarted()
		{
			return threadStarted_m.waitEvent(0);
		}
		
		public boolean ifExclusiveLock()
		{
			return exclusive_m;
		}
						
		public BackgroundJobFunctor getFunctor()
		{
			return functor_m;
		}
		
		public String getStringId()
		{
			return strId_m.toString();
		}
		
		public String getTypeId()
		{
			return typeId_m.toString();
		}
		
		
		
		@Override
		public void run()
		{
			threadStarted_m.signal();
			synchronized(this) {
				started_m = HtDateTimeUtils.getCurGmtTime();
			}
			
			HtLog.log(Level.INFO, getContext(), "run", "Bck Job thread started at: " + HtDateTimeUtils.time2SimpleString_Gmt(started_m));
			
			// update in Db
			try {
				RtDatabaseManager.instance().updateBackgroundJobEntry(dbId_m, getStringId(), getTypeId(), getStatus().getStatusCode(), getStatus().getStatusReason(), started_m, -1);
			}
			catch(Throwable e)
			{
				HtLog.log(Level.WARNING, getContext(), "run", "Error writing background info to DB: " + e.getMessage());
			}
			
			// CALL
			try {
				// here client can change status
				functor_m.execute(status_m);
			}
			catch(Throwable e)
			{
				status_m.setUp(-1, e.getMessage());
				HtLog.log(Level.INFO, getContext(), "run", "Bck Job exception: " + e.getMessage());
				XmlEvent.createSimpleLog("", getContext(), CommonLog.LL_ERROR, "Exception on executing background job: " + getStringId() + " - " + e.getMessage());
			}
			
			//
							
			synchronized(this) {
				finished_m = HtDateTimeUtils.getCurGmtTime();
			}
			
			
			// deregister
			try {
				base_m.removeRegisteredJob(getStringId());
			}
			catch(Throwable e)
			{
				HtLog.log(Level.WARNING, getContext(), "run", "Cannot deregister Bck Job entry with ID: " + e.getMessage() + ", ignoring...");
			}
			
			try {
				// update in DB
				RtDatabaseManager.instance().updateBackgroundJobEntry(
								dbId_m,
								getStringId(), 
								getTypeId(), 
								getStatus().getStatusCode(), 
								getStatus().getStatusReason(), 
								getStartTime(),
								getFinishTime()
				);
			}
			catch(Throwable e2)
			{
				HtLog.log(Level.WARNING, getContext(), "run", "Error writing background info to DB: " + e2.getMessage());
			}
			
			
			HtLog.log(Level.INFO, getContext(), "run", "Bck Job thread finishing at: " + HtDateTimeUtils.time2SimpleString_Gmt(finished_m) + 
							" with status: " + status_m.getStatusReason());
			
		}
		
	
		
	}
	
	// ----------------------
	
	private final HashMap<String, BackgroundJobThread> jobs_m = new HashMap<String, BackgroundJobThread>();
	private final HashMap<String, String> jobLocks_m = new HashMap<String, String>();
	private Event shutDownInitiated_m = new Event();
					
	static final int SHUTDOWN_WAIT_SEC = 10;
	static private RtBackgroundJobManager instance_m = null;

	static public RtBackgroundJobManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtBackgroundJobManager();
		}

		return instance_m;
	}

	static public RtBackgroundJobManager instance() {
		return instance_m;
	}
	
	protected String getContext() {
			return RtBackgroundJobManager.class.getSimpleName();
	}
	
	
	public RtBackgroundJobManager()
	{
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}
	
	
	
	/**
	 * starts background job. Finishes automatically
	 */
	public void startBackgroundJob(BackgroundJobFunctor bckFunctor, String strId, boolean isExclusive, String typeId) throws Exception
	{
				
		if (shutDownInitiated_m.waitEvent(0))
			throw new HtException(getContext(), "startBackgroundJob", "Cannot start new background jobs while in shutdown" ); 
		
		// register
		synchronized(jobs_m) {
			
			if (typeId== null || typeId.length() <=0)
				throw new HtException(getContext(), "startBackgroundJob", "Invalid Type ID" );
			
			
			if (strId== null || strId.length() <=0)
				throw new HtException(getContext(), "startBackgroundJob", "Invalid Job ID" );
			
					
			if (jobs_m.containsKey(strId))
				throw new HtException(getContext(), "startBackgroundJob", "Job with ID already registered: " + strId );
		
			
			if (isExclusive) {
				if (!jobLocks_m.containsKey(typeId)) {
					jobLocks_m.put(typeId, strId);
				}
				else
					throw new HtException(getContext(), "startBackgroundJob", "Job with ID: "+ strId + 
								" cannot be started because exclusive lock exists for operation type: " +typeId+" is required");
		
			}
			
			// insert in DB
			long dbId = RtDatabaseManager.instance().insertBackgroundJobEntry(strId, typeId);
			
			// create job
			BackgroundJobThread th = new BackgroundJobThread(dbId, this, bckFunctor, isExclusive, strId, typeId);
			
			// register				
			jobs_m.put(th.getStringId(), th);
			
			HtLog.log(Level.INFO, getContext(), "startBackgroundJob", "Job is registered: " + strId + " is exclusive lock: " + isExclusive + " job type: " + typeId );
		}
	
	}
	
	public Map<String, BackgroundJobThread> getAllRunningJobs() throws Exception
	{
		synchronized(jobs_m) {
			HashMap<String, BackgroundJobThread> result = new HashMap<String, BackgroundJobThread>();
			result.putAll(jobs_m);
		
			return result;
		}
	}
	
	public List<BackgroundJobEntry> getAllRunningJobsDescriptors() throws Exception
	{
		Map<String, BackgroundJobThread> data = getAllRunningJobs();
		List<BackgroundJobEntry> result = new ArrayList<BackgroundJobEntry>();
		
		for(Iterator<String> it = data.keySet().iterator(); it.hasNext(); ) {
			BackgroundJobThread th_i = data.get( it.next() );
			BackgroundJobEntry je_i = new BackgroundJobEntry(
					th_i.ifExclusiveLock(),
					th_i.getStringId(),
					th_i.getTypeId(),
					th_i.getStartTime(),
					th_i.getFinishTime(),
					th_i.getStatus(),
					th_i.isStarted()
			);
			
			result.add(je_i);
		}
		
		Collections.sort(result);
		return result;
		
	}
	
	public BackgroundJobThread getJob(String jobId) throws Exception
	{
		synchronized(jobs_m) {
			if (!jobs_m.containsKey(jobId))
				throw new HtException(getContext(), "getJob", "Job is not registered via its UID: " + jobId);
			
			return jobs_m.get(jobId);
		}
	}
		
	public void removeRegisteredJob(String jobId) throws Exception
	{
		synchronized(jobs_m) {
			if (!jobs_m.containsKey(jobId))
				throw new HtException(getContext(), "removeRegisteredJob", "Job is not registered via its UID: " + jobId);
			
			BackgroundJobThread th = jobs_m.get(jobId);
			if (th.ifExclusiveLock()) {
				jobLocks_m.remove(th.getTypeId());
			}
			
			jobs_m.remove(jobId);

			
		}
	}
	
	
	@Override
	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), getContext(), "Start finishing...");
		shutDownInitiated_m.signal();
		
		try {
			// need to complete pending jobs
			Map<String, BackgroundJobThread> allJobs = getAllRunningJobs();
			for(Iterator<String> it = allJobs.keySet().iterator(); it.hasNext(); ) {
				String strId = it.next();
				
				try {
					BackgroundJobThread th_i = getJob(strId);
					
									
					
					th_i.join(SHUTDOWN_WAIT_SEC * 1000);
					if (th_i.isAlive()) {
						HtLog.log(Level.WARNING, getContext(), "shutdown", "Background job: " + th_i.getStringId() + " cannot be stopped for " + SHUTDOWN_WAIT_SEC+" secs, ignoring...");
					}
				}
				catch(Throwable e2)
				{
					HtLog.log(Level.WARNING, getContext(), "shutdown", "Exception on shutdown background job: " + e2.getMessage() + ", ignoring...");
				}
			}
		}
		catch(Throwable e)
		{
			HtLog.log(Level.WARNING, getContext(), "shutdown", "Exception on shutdown: " + e.getMessage());
		}
						
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}
	
	/*
	 * Helpers
	 */
	
	
	
	
	
	
	
}
