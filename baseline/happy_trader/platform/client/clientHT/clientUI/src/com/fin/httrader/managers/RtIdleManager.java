/*
 * RtIdleManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.HtMain;
import com.fin.httrader.interfaces.RtIdleManagerFunctor;
import com.fin.httrader.managers.RtManagerBase;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Level;
import jsync.Event;

/**
 *
 *
 * @author victor_zoubok
 * This class perfrom any kind of work in background mode
 */
public class RtIdleManager extends Thread implements RtManagerBase {

	private Event toShutdown_m = new Event();
	private Event started_m = new Event();
	private boolean shutDownFlag_m = false;
	private final Set<RtIdleManagerFunctor> functors_m = new HashSet<RtIdleManagerFunctor>();
	static private RtIdleManager instance_m = null;


	//
	static public RtIdleManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtIdleManager();
		}

		return instance_m;
	}

	static public RtIdleManager instance() {

		return instance_m;
	}

	protected String getContext() {
		return RtIdleManager.class.getSimpleName();
	}

	/** Creates a new instance of RtIdleManager */
	private RtIdleManager() throws Exception {
		// init

		toShutdown_m.reset();
		started_m.reset();

		start();

		if (!started_m.waitEvent(5000)) {
			throw new HtException(getContext(), getContext(), "Cannot start idle manager thread");
		}

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	public void startShutDown() {

		setShutdownInProgress();

		HtLog.log(Level.INFO, getContext(), "startShutDown", "Started shutdown process");

	}

	public void registerFunctor(RtIdleManagerFunctor functor) {
		synchronized (functors_m) {
			functors_m.add(functor);
			HtLog.log(Level.INFO, getContext(), "run", "The functor registered: " + functor.getFunctorName() );
		}
	}

	public void removeFunctor(RtIdleManagerFunctor functor) {
		synchronized (functors_m) {
			functors_m.remove(functor);
			HtLog.log(Level.INFO, getContext(), "run", "The functor removed: " + functor.getFunctorName() );
		}
	}

	public void run() {
		try {

			int wait_msec = RtConfigurationManager.StartUpConst.IDLE_THREAD_SECS_TIMEOUT * 1000;
			
			started_m.signal();

			// lower a bit priority
			Thread.currentThread().setPriority(Thread.NORM_PRIORITY -1);
			while (true) {

				// wait a number of msces
				if (toShutdown_m.waitEvent(wait_msec)) {
					break;
				}


				if (!getShutdownInProgressStatus()) {


					synchronized (functors_m)
					{
						for (Iterator<RtIdleManagerFunctor> it = functors_m.iterator(); it.hasNext(); ) {

							RtIdleManagerFunctor f = it.next();

							// ----
							// executing functor
							try {

								if (f.checkIfExecuting()) {
									
									f.execute();
									HtLog.log(Level.FINE, getContext(), "run", "The functor executed: " + f.getFunctorName() );

								}
				
								// her we can execute
							} catch (Throwable e) {
								// in case of exception kill resources
								HtLog.log(Level.WARNING, getContext(), "run", "Exception happened on executing the functor: " + f.getFunctorName() + " - " + e.getMessage());
							}

							//
						}
					}

				}

				// seems the time of shutdown
				if (toShutdown_m.waitEvent(0)) {
					break;
				}

			} // end main loop



		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "run", "Exception happened in the idle thread: " + e.getMessage());
			
			RtCriticalErrorManager.instance().signalCriticalError("Idle Manager", "", e, CriticalErrorEntry.URGENCY_FLAG_WARN);
		}


		HtLog.log(Level.INFO, getContext(), "run", "Idle thread is finishing...", null);
	}

	@Override
	public void shutdown() {
		toShutdown_m.signal();

		try {
			join();
		} catch (Throwable e) {
		}

		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// ----------------
	public synchronized void setShutdownInProgress() {
		shutDownFlag_m = true;
	}

	public synchronized boolean getShutdownInProgressStatus() {
		return shutDownFlag_m;
	}
}
