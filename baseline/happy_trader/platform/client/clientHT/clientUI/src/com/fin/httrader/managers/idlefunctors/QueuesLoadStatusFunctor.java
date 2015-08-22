/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.managers.idlefunctors;

import com.fin.httrader.interfaces.RtIdleManagerFunctor;
import com.fin.httrader.interfaces.SingleReaderQueueInterface;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtCriticalErrorManager;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.hlpstruct.CriticalErrorEntry;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class QueuesLoadStatusFunctor extends RtIdleManagerFunctor {

	private final Set<SingleReaderQueueInterface> queues_m = new HashSet<SingleReaderQueueInterface>();


	// -----------

	// signleton only
	private final static QueuesLoadStatusFunctor instance_m = new QueuesLoadStatusFunctor();

	private String getContext()
	{
		return QueuesLoadStatusFunctor.class.getSimpleName();
	}

	public static QueuesLoadStatusFunctor getInstance()
	{
		return instance_m;
	}

	public QueuesLoadStatusFunctor()
	{
		super("Queues load status", RtConfigurationManager.StartUpConst.IDLE_THREAD_SECS_CHECK_QUEUE_STATUS_PERIOD);
	}

	@Override
	public void execute() throws Exception {
		synchronized (queues_m) {
			for(Iterator<SingleReaderQueueInterface> it = queues_m.iterator(); it.hasNext(); ) {
				SingleReaderQueueInterface q = it.next();


				int qsz = q.getQueueSize();
				if (qsz > RtConfigurationManager.StartUpConst.WARN_ELEMENTS_IN_QUEUE ) {
					RtCriticalErrorManager.instance().signalCriticalError(getContext(), "", "The queue: [ " + q.getName() + " ] exceeds " + RtConfigurationManager.StartUpConst.WARN_ELEMENTS_IN_QUEUE + " elements: " + qsz, CriticalErrorEntry.URGENCY_FLAG_EXCEPT);
				}
				else {
					//System.out.println("[ " + HtUtils.time2String_Gmt( HtUtils.getCurGmtTime() )  + " ] - the queue: [ " + q.getName() + " ] has size: " + qsz);
				}
			}
		}
	}

	public void registerQueue(SingleReaderQueueInterface q) {
		synchronized (queues_m) {
			queues_m.add(q);
		}

		HtLog.log(Level.INFO, getContext(), "registerQueue", "Queue registered for monotoring: " + q.getName());

	}

	public void removeQueue(SingleReaderQueueInterface q) {
		synchronized (queues_m) {
			queues_m.remove(q);

		}

		HtLog.log(Level.INFO, getContext(), "registerQueue", "Queue removed for monotoring: " + q.getName());
	}


}
