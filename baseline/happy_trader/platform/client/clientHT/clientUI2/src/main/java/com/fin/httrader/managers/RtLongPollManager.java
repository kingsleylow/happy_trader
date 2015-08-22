/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.interfaces.HtLongPollManagerEventFilter;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import jsync.Event;

/**
 * This class is helper for long polling HTTP servlets TODO - event priority
 * will be 30 o the On request it must subscribe relevant events and push them
 * to the queuely Must explicitely clean up pushes would go from another
 * thread(s) Must be periodically checked and if last access time exceeds timout
 * clean this queue
 */
public class RtLongPollManager extends Thread implements RtManagerBase, HtEventListener {

		public static final int QUEUE_PRIORITY_SUBSCR_NUM = 30;
		public static final int PERIODICAL_SLEEP_MSEC = 5 * 1000;
		public static final String LONG_POLL_MNGR_LISTENER_NAME = "Long Poll Manager";

		private Event toShutdown_m = new Event();
		private Event started_m = new Event();

		// exception - queue not valid
		public static class  QueueNotValid  extends Exception {

			
				public QueueNotValid(String message) {
						super(message);
				}

				private QueueNotValid() {
						super();
				}

				private QueueNotValid(String message, Throwable cause) {
						super(message, cause);
				}

				private QueueNotValid(Throwable cause) {
						super(cause);
				}
		};

		public static class PollingEntry {

				// use array cause more efficient to iterate
				// do not expect much elements in the queue
				// so restrict this to some reasonable limit
				private ArrayBlockingQueue<XmlEvent> queue_m = new ArrayBlockingQueue(RtConfigurationManager.StartUpConst.WARN_ELEMENTS_IN_QUEUE);
				private long lastAccessTime_m = HtDateTimeUtils.getCurGmtTime();

				// this is initialized after queue is created
				private int maxDelayAfterLastAccessMsec_m = -1;
				private RtLongPollManager base_m = null;
				private HtLongPollManagerEventFilter filter_m = null;

				public PollingEntry(HtLongPollManagerEventFilter filter, RtLongPollManager base, int max_delay_afeter_queue_access_sec, int event_type_to_ubscribe) {
						maxDelayAfterLastAccessMsec_m = max_delay_afeter_queue_access_sec * 1000; // msec
						base_m = base;
						filter_m = filter;
						
					

						// subscribe
						RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).
							subscribeForEvent(event_type_to_ubscribe, QUEUE_PRIORITY_SUBSCR_NUM, base_m);
				}

				public long getLastAccessTime() {
						return lastAccessTime_m;
				}
				
				public void setLastAccessTime(long lastAccessTime)
				{
						lastAccessTime_m = lastAccessTime;
				}

				public HtLongPollManagerEventFilter getFilter()
				{
						return filter_m;
				}
				
				public long getMaxDelayTime() {
						return maxDelayAfterLastAccessMsec_m;
				}

				public ArrayBlockingQueue<XmlEvent> getQueue() {
						return queue_m;
				}

				public void clear() {
						lastAccessTime_m = -1;
						queue_m.clear();

						RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForAllEvents(QUEUE_PRIORITY_SUBSCR_NUM, base_m);
				}

		};

		private final ConcurrentHashMap<Integer, PollingEntry> baseQueue_m = new ConcurrentHashMap<Integer, PollingEntry>();

		static private RtLongPollManager instance_m = null;

		// ------------------------------------
		protected String getContext() {
				return this.getClass().getSimpleName();
		}

		static public RtLongPollManager create() throws Exception {
				if (instance_m == null) {
						instance_m = new RtLongPollManager();
				}

				return instance_m;
		}

		static public RtLongPollManager instance() {
				return instance_m;
		}

		// ----------------
		public RtLongPollManager() throws Exception {
				toShutdown_m.reset();
				started_m.reset();

				start();

				if (!started_m.waitEvent(5000)) {
						throw new HtException(getContext(), getContext(), "Cannot start poll manager manager thread");
				}
				HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
		}

		// returns NULL if nothing found and tout=-1
		// otherwise wait
		public XmlEvent popQueueElement(int cookie, int timeout_msec) throws Exception {

				if (toShutdown_m.waitEvent(0))
							throw new HtException(getContext(), getContext(), "We are in shutdown state");
				
				PollingEntry pe = getEventQueue(cookie);
				pe.setLastAccessTime( HtDateTimeUtils.getCurGmtTime() );
				
				HtLog.log(Level.FINE, getContext(), "popQueueElement", "Accessing queue element for queue with ID: " +cookie );

				// returns null if queue is emtpty
				return pe.queue_m.poll(timeout_msec, TimeUnit.MILLISECONDS);
		}
		
		public int getQueuePendingSize(int cookie) throws Exception
		{
				if (toShutdown_m.waitEvent(0))
						throw new HtException(getContext(), getContext(), "We are in shutdown state");
				
				PollingEntry pe = getEventQueue(cookie);
				
				return pe.getQueue().size();
		}

		public void pushQueueElement(XmlEvent xml_event) {
				try {
						
						// no more elements while we are in shotdown
						if (toShutdown_m.waitEvent(0))
								return;
						
						// propagate to all queues
						for (Iterator<Integer> it = baseQueue_m.keySet().iterator(); it.hasNext();) {
								int cookie = it.next();
								PollingEntry pe = getEventQueue(cookie);
								HtLongPollManagerEventFilter fltr = pe.getFilter();
								
								// push if filter is null or if it matches
								if (fltr == null ) {
										HtLog.log(Level.FINE, getContext(), "pushQueueElement", "Pushing event: " + xml_event.toString()+ " to queue with cookie: " +cookie );
										pe.getQueue().add(xml_event);
								}
								else if (fltr.match(xml_event)) {
										HtLog.log(Level.FINE, getContext(), "pushQueueElement", "Pushing event: " + xml_event.toString()+ " to queue with cookie: " +cookie);
										pe.getQueue().add(xml_event);
								}
										
						}
				} catch (Throwable e) {
						// might be severe
						HtLog.log(Level.WARNING, getContext(), "pushQueueElement", "Cannot push event to long poll queues: " + e.getMessage());
				}

		}

		public boolean isQueueValid(int cookie) {
				return baseQueue_m.containsKey(cookie);
		}

		// synch as must be atomic
		public void initEventQueue(
			int cookie,
			int event_type_to_subscribe,
			int max_delay_afeter_queue_access_sec,
			HtLongPollManagerEventFilter filter
		) throws Exception {
				
				if (toShutdown_m.waitEvent(0)) {
						throw new HtException(getContext(), "initEventQueue", "We are in shutdown phase");
				}

				// create relevant entry and subscribe for events
				if (baseQueue_m.containsKey(cookie)) {
						throw new HtException(getContext(), "initEventQueue", "Queue already initialized: " + cookie);
				}

				if (max_delay_afeter_queue_access_sec <= 0) {
						throw new HtException(getContext(), "initEventQueue", "Select the number of seconds this queue is valid");
				}

				PollingEntry pentry = new PollingEntry(filter, this, max_delay_afeter_queue_access_sec, event_type_to_subscribe);

				baseQueue_m.put(cookie, pentry);
				HtLog.log(Level.INFO, getContext(), "initEventQueue", "Event queue initialized with cookie: " + cookie + 
					", maximum live time is: " + max_delay_afeter_queue_access_sec + 
					", event type to subscribe: " + XmlEvent.getEventTypeName(event_type_to_subscribe));

		}

		// synch as must be atomic
		public void removeEventQueue(int cookie) throws Exception {

				PollingEntry pentry = getEventQueue(cookie);
				pentry.clear();

				baseQueue_m.remove(cookie);
				HtLog.log(Level.INFO, getContext(), "initEventQueue", "Event queue removed with cookie: " + cookie);

		}

		public void totalCleanUp() {
				// totally clean all queue
				for (Iterator<Integer> it = baseQueue_m.keySet().iterator(); it.hasNext();) {
						PollingEntry pentry = baseQueue_m.get(it.next());
						pentry.clear();
				}

				baseQueue_m.clear();
				HtLog.log(Level.INFO, getContext(), "totalCleanUp", "Total cleanup done");
		}

		@Override
		public void run() {
				try {
						started_m.signal();
						
						while (true) {
								if (toShutdown_m.waitEvent(PERIODICAL_SLEEP_MSEC)) {
										break;
								}

								HtDateTimeUtils.getCurGmtTime();

								// no need to synch as using special queue
								for (Iterator<Integer> it = baseQueue_m.keySet().iterator(); it.hasNext();) {
										int cookie = it.next();
										PollingEntry pentry = baseQueue_m.get(cookie);

										long elapsed_msec = HtDateTimeUtils.getCurGmtTime() - pentry.getLastAccessTime();
										if (elapsed_msec > pentry.getMaxDelayTime()) {
												// clean
												pentry.clear();
												baseQueue_m.remove(cookie);

												HtLog.log(Level.FINE, getContext(), "run", 
													"Event queue was removed automatically with cookie: " + cookie + " after: " + elapsed_msec + " msecs" );
										}

								}

								//HtLog.log(Level.INFO, getContext(), "run", "Proceeded with cleanup");
						}

				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "run", "Exception happened in garbage collector thread: " + e.getMessage());

				}

				HtLog.log(Level.INFO, getContext(), "run", "Garbage collector thread finishing...", null);
		}

		@Override
		public void shutdown() {
				toShutdown_m.signal();

				try {
						join();
				} catch (Throwable e) {
				}
				
				totalCleanUp();

				HtLog.log(Level.INFO, getContext(), "shutdown", "shutdown");
		}

		@Override
		public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {
				pushQueueElement(event);
		}

		@Override
		public String getListenerName() {
				return LONG_POLL_MNGR_LISTENER_NAME;
		}

		/**
		 * Helpers
		 */
		private PollingEntry getEventQueue(int cookie) throws QueueNotValid {
				if (!baseQueue_m.containsKey(cookie)) {
						throw new QueueNotValid("Queue not found found via cookie: " + cookie);
				}

				return baseQueue_m.get(cookie);
		}
}
