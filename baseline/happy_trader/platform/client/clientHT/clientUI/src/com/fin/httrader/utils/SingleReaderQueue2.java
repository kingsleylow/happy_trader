/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import com.fin.httrader.managers.idlefunctors.QueuesLoadStatusFunctor;
import com.fin.httrader.interfaces.SingleReaderQueueInterface;
import java.util.Collection;
import java.util.LinkedList;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import jsync.Queue;

/**
 *
 * @author Victor_Zoubok New implementation
 */
public class SingleReaderQueue2 implements SingleReaderQueueInterface{

	
		private int maxElements_m = -1;
		private QueuesLoadStatusFunctor monitor_m = null;
		private StringBuilder name_m = new StringBuilder();
		
		private final LinkedList queue_m = new LinkedList();
		
		private final Lock lock_m = new ReentrantLock();
    private final Condition notEmpty_m = lock_m.newCondition();

		public SingleReaderQueue2(String name, boolean monitored, int maxelements) {
				maxElements_m = maxelements;

				if (name != null) {
						name_m.append(name);
				} else {
						name_m.append("Unknown: " + this.hashCode());
				}

				if (monitored) {
						monitor_m = QueuesLoadStatusFunctor.getInstance();
						monitor_m.registerQueue(this);
				}
				
				// make this fair so far
			
		}

		@Override
		public int getQueueSize() {
				int sz = -1;
				
				lock_m.lock();
				try {
						sz = queue_m.size();
				}
				finally
				{
						lock_m.unlock();
				}
				return sz;
				
		}

		@Override
		public String getName() {
				return name_m.toString();
		}

		@Override
		public void get(Collection list, int msecs) {
				
				list.clear();
		
				lock_m.lock();
        try {
						
						try {
								while(queue_m.isEmpty()) {
										notEmpty_m.await(msecs, TimeUnit.MILLISECONDS);
								}
						}
						catch(InterruptedException e)
						{
								
						}
						
							
						list.addAll(queue_m);
						queue_m.clear();
						
						

            
        } finally {
            lock_m.unlock();
        }
		
		}

		@Override
		public void get(Collection list, int maxsize, int msecs) {
				list.clear();
		
				lock_m.lock();
        try {
						
						try {
								while(queue_m.isEmpty()) {
										notEmpty_m.await(msecs, TimeUnit.MILLISECONDS);
								}
						}
						catch(InterruptedException e)
						{
								
						}
						
						// size to copy
						int msize  = (maxsize > queue_m.size() ? queue_m.size() : maxsize );
							
						int idx = 0;
						while(!queue_m.isEmpty() ) {

							Object o_i = queue_m.remove();
							list.add(o_i);

							if (++idx >= msize)
								break;
						}
			
						

            
        } finally {
            lock_m.unlock();
        }
				
				
		}

		@Override
		public Object get(int msecs) {
				Object item = null;
				lock_m.lock();
        try {
						
						try {
								while(queue_m.isEmpty()) {
										notEmpty_m.await(msecs, TimeUnit.MILLISECONDS);
								}
						}
						catch(InterruptedException e)
						{
								
						}
						
						if (!queue_m.isEmpty()) {
								item = queue_m.remove();
							
								
						}

            
        } finally {
            lock_m.unlock();
        }
				
				return item;
		}

		@Override
		public void get(Collection list) {
				
				list.clear();
		
				lock_m.lock();
        try {
						
						try {
								while(queue_m.isEmpty()) {
										notEmpty_m.await();
								}
						}
						catch(InterruptedException e)
						{
								
						}
						
							
						list.addAll(queue_m);
						queue_m.clear();
					
						

            
        } finally {
            lock_m.unlock();
        }
		}

		@Override
		public void getNoWait(Collection list) {
				
				list.clear();
				
				lock_m.lock();
				try {
						list.addAll(queue_m);
				}
				finally
				{
						lock_m.unlock();
				}
			
				
		}

		@Override
		public void releaseMonitoring() {
				throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
		}

		@Override
		public void put(Object o) throws SingleReaderQueueExceedLimit {
				
				lock_m.lock();
        try {
						
						if (maxElements_m > 0 && queue_m.size() >= maxElements_m) {
										throw new SingleReaderQueueExceedLimit("The number of elements is more than " + queue_m.size() + " assume it's a stuck, object is: " + o.toString());
						}
						
						queue_m.add(o);
						notEmpty_m.signal(); 
            
        } finally {
            lock_m.unlock();
        }
		
		}

		@Override
		public void shutdown() {
				
				// interruped exception
				// enforce other queues to throw this exception
				
				
		}
}
