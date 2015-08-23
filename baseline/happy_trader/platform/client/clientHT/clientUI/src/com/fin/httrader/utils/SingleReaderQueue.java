/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import com.fin.httrader.interfaces.SingleReaderQueueInterface;
import com.fin.httrader.managers.idlefunctors.QueuesLoadStatusFunctor;
import java.util.ArrayList;
import java.util.Collection;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 * This is a simple queue
 * one writer and one reader
 * The purpose is that when put and read are blocking but due to small number of objects
 * read happens as an array
 */
public class SingleReaderQueue implements SingleReaderQueueInterface {

	// -----------------------

	

	// -----------------------

	//private Event event_m = new Event();
	private final ArrayList list_m = new ArrayList();

	private int maxElements_m = -1;

	private final StringBuilder name_m = new StringBuilder();

	private QueuesLoadStatusFunctor monitor_m = null;

	// if monitored
	// the instance is registered in special functor who periodically will check all queues size
	public SingleReaderQueue(String name, boolean monitored, int maxelements) {
		maxElements_m = maxelements;

		if (name != null)
			name_m.append(name);
		else
			name_m.append("Unknown: " + this.hashCode());

		if (monitored) {
			monitor_m = QueuesLoadStatusFunctor.getInstance();
			monitor_m.registerQueue(this);
		}
	}

	@Override
	public String getName()
	{
		return name_m.toString();
	}

	@Override
	public void releaseMonitoring()
	{
		if (monitor_m != null) {
			monitor_m.removeQueue(this);
		}
	}

	@Override
	public void put(Object o) throws SingleReaderQueueExceedLimit {
		synchronized (list_m) {

			if (maxElements_m > 0 && list_m.size() >= maxElements_m) {
				throw new SingleReaderQueueExceedLimit("The number of elements is more than " + list_m.size() + " assume it's a stuck, object is: " + o.toString());
			}
			

			list_m.add(o);
			// use notify all
			list_m.notifyAll();

	
		}
	}

	
	@Override
	public void getNoWait(Collection list) {
		list.clear();
		
		synchronized (list_m) {
			list.addAll(list_m);
			list_m.clear();
		}
	}

	// returns a copy
	@Override
	public void get(Collection list, int msecs) {

		// wait
		list.clear();

		synchronized (list_m) {
			if (list_m.isEmpty()) {

				try {
					list_m.wait(msecs);
				} catch (InterruptedException ignored) {
					// ignore
				}
				
			}

			// get data
			list.addAll(list_m);
			list_m.clear();

		} // end synchronized
	

	}

	// returns a copy
	@Override
	public void get(Collection list, int maxsize, int msecs) {

		// wait
		list.clear();

		synchronized (list_m) {
			if (list_m.isEmpty()) {

				try {
					list_m.wait(msecs);
				} catch (InterruptedException ignored) {
					// ignore
				}

			}

			// get data
			int msize  = (maxsize > list_m.size() ? list_m.size() : maxsize );
			//int idxb = list_m.size() - msize;
			
			int idx = 0;
			while(!list_m.isEmpty() ) {

				Object o_i = list_m.get(0);
				list.add(o_i);
				
				list_m.remove(0);

				if (++idx >= msize)
					break;
			}
			

		} // end synchronized


	}

	// return single object
	@Override
	public Object get( int msecs) {

		synchronized (list_m) {
			if (list_m.isEmpty()) {

				try {
					list_m.wait(msecs);
				} catch (InterruptedException ignored) {
					// ignore
				}

			}

			// get data
		
			if (!list_m.isEmpty()) {
				int lastidx = list_m.size() - 1;
				Object o = list_m.get(lastidx);

				list_m.remove(lastidx);
				return o;
				
			}

			return null;

			

		} // end synchronized
	}
	

	// returns a copy
	@Override
	public void get(Collection list) {

		list.clear();

		synchronized (list_m) {
			if (list_m.isEmpty()) {

				try {
					list_m.wait();
				} catch (InterruptedException ignored) {
					// ignore
				}


			}

			// get data
			list.addAll(list_m);
			list_m.clear();

		} // end synchronized

	
	}

	@Override
	public int getQueueSize()
	{
		synchronized (list_m) {
			return list_m.size();
		}
	}

		@Override
		public void shutdown() {
				// N/A
				// only 2 threads working with that queue so notify is enough here
				synchronized (list_m) {
						list_m.notifyAll();
				}
		}
}
