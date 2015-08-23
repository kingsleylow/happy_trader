/*
 * RtGlobalEventManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtManagerBase;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.TreeMap;

import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 * This class is intended to be global event manager
 * propagating of XmlEvent data
 */
public class RtGlobalEventManager implements RtManagerBase {

	public static int ALL_EVENT_TYPES = -1;
	// constat showing that event processing will be propagated in main thread (inside pushCommonEvent )
	public static int MAIN_THREAD = -1;

	// helper class incapsulating ListenerThread
	public class ListenerThread /* extends Thread */ {

		private HtEventListener priority0Listener_m = null;
		private boolean isMainThread_m = false;
		private final Object mtx_m = new Object();

		//private Event threadStarted_m = new Event();
		//private Queue dataQueue_m = new Queue();
		//SingleReaderQueue dataQueue_m = new SingleReaderQueue("Global Event Manager Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);
		// propagation map, the first key is priority
		// the second map is event type vs list of event Listeners
		TreeMap<Integer, HashMap<Integer, HashSet<HtEventListener>>> propagationList_m = new TreeMap<Integer, HashMap<Integer, HashSet<HtEventListener>>>();
		// special kind of queue to organize delays
		//private HashMap<HtEventListener, Queue> delayQueues_m = new HashMap<HtEventListener, Queue>();
		private final HashMap<HtEventListener, SingleReaderQueue> delayQueues_m = new HashMap<HtEventListener, SingleReaderQueue>();

		//
		public ListenerThread(boolean mainThread) throws Exception {
			isMainThread_m = mainThread;

		
		}

	
		public void subscribePriority0Listener(HtEventListener listener) {
			synchronized (mtx_m) {
				priority0Listener_m = listener;
				HtLog.log(Level.INFO, "ListenerThread", "subscribePriority0Listener", "Subscribed priority 0 listener: " + listener.getListenerName() );
			}
		}

		public void unsubscribePriority0Listener(HtEventListener listener) {
			synchronized (mtx_m) {
				priority0Listener_m = null;
				HtLog.log(Level.INFO, "ListenerThread", "unsubscribePriority0Listener", "Unsubscribed priority 0 listener: " + listener.getListenerName() );

			}
		}

		// eventType = -1 means ALL events
		public void subscribeForEvent(int eventType, int priority, HtEventListener listener)  {
			synchronized (mtx_m) {
				//TreeMap<Integer, HashMap<Integer, LinkedList<HtEventListener> > > propagationList_m

				HashMap<Integer, HashSet<HtEventListener>> evListList = null;
				HashSet<HtEventListener> events = null;

				if (propagationList_m.containsKey(priority)) {
					evListList = propagationList_m.get(priority);

					//
					if (evListList.containsKey(eventType)) {
						events = evListList.get(eventType);

						// if resolved. no need to sign two times
						if (!events.contains(listener)) // populate
						{
							events.add(listener);
						}
					} else {
						events = new HashSet<HtEventListener>();

						// populate
						events.add(listener);
						evListList.put(eventType, events);
					}

				//

				} else {
					evListList = new HashMap<Integer, HashSet<HtEventListener>>();
					events = new HashSet<HtEventListener>();

					// populate
					events.add(listener);
					evListList.put(eventType, events);
					propagationList_m.put(priority, evListList);
				}


			}



		}

		// eventType = -1 means ALL eventss
		public void unsubscribeForEvent(int eventType, int priority, HtEventListener listener)  {
			synchronized (mtx_m) {

				// iterate through all prioroties

				if (propagationList_m.containsKey(priority)) {
					HashMap<Integer, HashSet<HtEventListener>> evListList = propagationList_m.get(priority);


					// through event types
					if (evListList.containsKey(eventType)) {
						HashSet<HtEventListener> evList = evListList.get(eventType);

						// look for event
						evList.remove(listener);

					}

				} else {
					HtLog.log(Level.WARNING, getContext(), "unsubscribeForEvent", "Note that for that event type: " +
									XmlEvent.getEventTypeName(eventType) + " and the priority: " + priority + " listener not found - check subscrbeForEvent(...) - priority, listener name: " + listener.getListenerName());
				}
			}
		}

		// whether it is subscribed for the type of event
		public boolean isSubscribedForEvent(int eventType, int priority, HtEventListener listener) {
			synchronized (mtx_m) {
				if (propagationList_m.containsKey(priority)) {
					HashMap<Integer, HashSet<HtEventListener>> evListList = propagationList_m.get(priority);

					if (evListList.containsKey(eventType)) {
						HashSet<HtEventListener> evList = evListList.get(eventType);
						return evList.contains(listener);

					}

				}
			}

			return false;
		}

		// unsubscribe listener from all available events for defined priority
		public void unsubscribeForAllEvents(int priority, HtEventListener listener) {
			synchronized (mtx_m) {
				if (propagationList_m.containsKey(priority)) {
					HashMap<Integer, HashSet<HtEventListener>> evListList = propagationList_m.get(priority);


					for (Iterator<Integer> it2 = evListList.keySet().iterator(); it2.hasNext();) {
						HashSet<HtEventListener> evList = evListList.get(it2.next());

						// unsubscribe
						evList.remove(listener);


					}

				}
			}
		}

		// unsubscribe listener from all available events for all priorities
		public void unsubscribeForAllEvents(HtEventListener listener) {
			synchronized (mtx_m) {
				for (Iterator<Integer> it = propagationList_m.keySet().iterator(); it.hasNext();) {

					Integer priority = it.next();

					HashMap<Integer, HashSet<HtEventListener>> evListList = propagationList_m.get(priority);


					for (Iterator<Integer> it2 = evListList.keySet().iterator(); it2.hasNext();) {
						HashSet<HtEventListener> evList = evListList.get(it2.next());

						// unsubscribe
						evList.remove(listener);


					}

				}
			}
		}

		/*
		public void pushCommonEvent(XmlEvent event) throws Exception {

		// PERFORMANCE BUG - 25-06-2009 - do not keep a copy but orig reference here
		//dataQueue_m.put(new XmlEvent(event));
		dataQueue_m.put(event);

		}
		 */

		// this doesn not pass to the queue but propagate events in the calle thread
		public void pushCommonEventInTheCalleeThread(XmlEvent event) {

			try {
				// first priority listener
				if (priority0Listener_m != null) {
					synchronized (delayQueues_m) {
						if (delayQueues_m.containsKey(priority0Listener_m)) {
							SingleReaderQueue queue = delayQueues_m.get(priority0Listener_m);

							
							
							try {
								queue.put(event);
							} catch (Throwable e) {
								throw new HtException("ListenerThread", "pushCommonEventInTheCalleeThread", "Exception when propagating to DELAYED queue for priority 0 listener: " + priority0Listener_m.getListenerName() + " - " + e.getMessage());
							}
							
						}
						else {
							try {
								priority0Listener_m.onEventArrived(event);
							} catch (Throwable e) {
								throw new HtException("ListenerThread", "pushCommonEventInTheCalleeThread", "Exception when propagating to the priority 0 listener: " + priority0Listener_m.getListenerName() + " - " + e.getMessage());
							}
						}

					}

				}
			} catch (Throwable e) {
				HtLog.log(Level.WARNING, "ListenerThread", "pushCommonEventInTheCalleeThread", "Internal exception on (onEventArrived) for priority 0 listener: \"" + e.getMessage() + "\"");
			}


			// other events
			List<HtEventListener> evListSubscr = getSubscribedListeners(event.getEventType());


			for (int i = 0; i < evListSubscr.size(); i++) {
				try {
					HtEventListener listener = evListSubscr.get(i);
					synchronized (delayQueues_m) {

						if (delayQueues_m.containsKey(listener)) {

							SingleReaderQueue queue = delayQueues_m.get(listener);

							try {
								queue.put(event);
							} catch (Throwable e) {
								throw new HtException("ListenerThread", "pushCommonEventInTheCalleeThread", "Exception when propagating to DELAYED queue: " + listener.getListenerName() + " - " + e.getMessage());
							}

						} else {

							try {
								listener.onEventArrived(event);
							} catch (Throwable e) {
								throw new HtException("ListenerThread", "pushCommonEventInTheCalleeThread", "Exception when propagating to the listener: " + listener.getListenerName() + " - " + e.getMessage());
							}
						}

					}
				} catch (Throwable e) {
					HtLog.log(Level.WARNING, "ListenerThread", "pushCommonEventInTheCalleeThread", "Internal exception on (onEventArrived): \"" + e.getMessage() + "\"");
				}
			}

		}

		public void forceDelayForSubscriber(HtEventListener listener) {
			synchronized (delayQueues_m) {
				if (!delayQueues_m.containsKey(listener)) {

					delayQueues_m.put(listener, new SingleReaderQueue("Temp Delay Queue", false, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE));
					HtLog.log(Level.INFO, "ListenerThread", "forceDelayForSubscriber", "Event propagation suspended for listener: " + listener.getListenerName());
				}
			}
		}

		public long switchOffDelayForSubscriber(HtEventListener listener) {
			//Queue queue = null;
			SingleReaderQueue queue = null;
			synchronized (delayQueues_m) {
				if (delayQueues_m.containsKey(listener)) {
					queue = delayQueues_m.get(listener);
					delayQueues_m.remove(listener);
				} else {
					HtLog.log(Level.WARNING, "ListenerThread", "switchOffDelayForSubscriber", "Delayed queue was not found for that listener: " + listener.getListenerName());
				}
			}

			// propagate what has left
			long size = 0;

			if (queue != null) {
				ArrayList list = new ArrayList();

				XmlEvent event = null;

				// wait 0 secs
				queue.getNoWait(list);

				for (Iterator it = list.iterator(); it.hasNext();) {
					//event = (XmlEvent) queue.get();

					event = (XmlEvent) it.next();

					if (isMainThread_m) {
						try {
							listener.onEventArrived(event);
						} catch (Throwable e) {
							HtLog.log(Level.WARNING, "ListenerThread", "switchOffDelayForSubscriber", "When propagating DELAYED events internal exception on (onEventArrived): \"" + e.getMessage() +
											"for listener: " + listener.getListenerName());
						}
					}
					/*else {
					// not implemented so far
					Assert.that("Not implemented", false);
					}
					 */

					size++;

					event = null;
				}

				HtLog.log(Level.INFO, "ListenerThread", "switchOffDelayForSubscriber", "Event propagation resumed, listener removed: " + listener.getListenerName() + " number of delayed events: " + size);


			}

			return size;
		}

		//  helpers
		private List<HtEventListener> getSubscribedListeners(int dataEventType) {

			ArrayList<HtEventListener> evListSubscr = new ArrayList<HtEventListener>();

			synchronized (mtx_m) {
				for (Iterator<Integer> it = propagationList_m.keySet().iterator(); it.hasNext();) {

					Integer priority_i = it.next();

					// resolve
					HashMap<Integer, HashSet<HtEventListener>> evListList = propagationList_m.get(priority_i);

					for (Iterator<Integer> it2 = evListList.keySet().iterator(); it2.hasNext();) {
						int eventType = it2.next();

						HashSet<HtEventListener> evList = evListList.get(eventType);
						for (Iterator<HtEventListener> it3 = evList.iterator(); it3.hasNext();) {

							// propagate - either matching type or all types
							HtEventListener listener = it3.next();
							if ((eventType >= 0 && dataEventType == eventType) || eventType == ALL_EVENT_TYPES) {
								evListSubscr.add(listener);
							}

						}

					}
				} // end of mail loop

			}


			return evListSubscr;
		}
	}; // end of class
	/**
	Main class
	 */
	private Object mtx_m = new Object();

	//private HashMap<Long, ListenerThread> engineThreads_m = new HashMap<Long, ListenerThread>();
	private ListenerThread mainThread_m = null;
	static private RtGlobalEventManager instance_m = null;

	static public RtGlobalEventManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtGlobalEventManager();
		}

		return instance_m;
	}

	static public RtGlobalEventManager instance() {
		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	// creates event manager
	private RtGlobalEventManager() throws Exception {
		// create main thread structures
		mainThread_m = new ListenerThread(true);

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	// this pushes event to the queue
	// forcing all the listeners to retreive this event
	// must not block
	public void pushCommonEvent(XmlEvent event) {

		// delegates to all the threads
		// we must guarantee the different objects here to be sure that recepients may not copy them

		// NOT USED AT ALL SO FAR
		/*
		for (Iterator<Long> it = engineThreads_m.keySet().iterator(); it.hasNext();) {
		try {
		resolveListenerThread(it.next()).pushCommonEvent(event);
		} catch (Throwable e) {
		HtLog.log(Level.WARNING, getContext(), "pushCommonEvent", "Internal exception on (pushCommonEvent): \"" + e.getMessage() + "\"");
		}
		}
		 */

		// process main thread events
		mainThread_m.pushCommonEventInTheCalleeThread(event);


	}

	// NOT USED AT ALL
	/*
	public long startListenerThread() throws Exception {
	synchronized (mtx_m) {

	ListenerThread listenThread = new ListenerThread(false);
	long id = listenThread.getId();
	engineThreads_m.put(id, listenThread);
	return id;
	}


	}
	 *

	public void stopListenerThread(long tId) throws Exception {
	synchronized (mtx_m) {
	// stop thread
	resolveListenerThread(tId).waitForFinish();

	// remove from the list
	engineThreads_m.remove(tId);
	}
	}
	 */

	// no need to raise exceptions
	public ListenerThread resolveListenerThread(long tId) {
		if (tId == MAIN_THREAD) {
			return mainThread_m;
		}

		return null;

	// NOY USED AT ALL
		/*
	synchronized (mtx_m) {
	if (!engineThreads_m.containsKey(tId)) {
	//throw new HtException(getContext(), "resolveListenerThread", "Invalid thread: " + tId);
	Assert.that("Invalid thread ID");
	return null;
	}

	return engineThreads_m.get(tId);

	}
	 */
	}

	public void shutdown() {

		// NOT USED AT ALL
		/*
		for (Iterator<Long> it = engineThreads_m.keySet().iterator(); it.hasNext();) {
		try {

		resolveListenerThread(it.next()).waitForFinish();
		} catch (Throwable e) {
		HtLog.log(Level.WARNING, getContext(), "shutdown", "Exception on shut down event manager: " + e.getMessage(), null);
		}
		}
		 */

		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");

	}
}
