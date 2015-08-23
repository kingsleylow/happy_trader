//-< Concurrent.java >-----------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Concurrent execution of several tasks and collecting their results
//-------------------------------------------------------------------*--------*

package jsync;

/** Class initiating parallel execution of several threads  and collecting
 *  their results. Two models of execution are supported - wait completion
 *  of all activities or wait until some thread is terminated and stop all
 *  other threads.
 *
 * @see jsync.Activity
 */
public class Concurrent {
    /** Constructor of <code>Concurrent</code> class. This constructor creates 
     *  a thread object for each activity.
     * 
     * @param activities array of activities to be done.
     */
    public Concurrent(Activity[] activities) { 
	nActivities = activities.length;
        threads = new Thread[nActivities];
	for (int i = nActivities; --i >= 0;) { 
	    threads[i] = new Task(i, activities[i], this);
	}
    }

    /** Start execution of activity threads.
     */
    public synchronized void start() {
        nTerminated = 0;
	results = new Object[nActivities];
        for (int i = nActivities; --i >= 0;) { 
	    threads[i].start();
	}
    }

    /** Wait until one of the started threads finish it's execution. 
     *  After it all other threads are stopped and result of
     *  finished thread is returned.
     * 
     * @return result returned by thread first finished it's execution.
     */
    public synchronized Object waitOne() { 
        Object result = null;
        while (nTerminated == 0) { 
	    try { 
	        wait();
	    } catch(InterruptedException ex) { 
	        throw new InterruptedError();
	    }
	}
	for (int i = nActivities; --i >= 0;) { 
	     if (results[i] != null) { 
	         result = results[i];
	     } else { 
	         //threads[i].stop();
	         threads[i].interrupt();
	     }
	}
	return result;
    } 

    /** Wait during specified period of time until one of started threads 
     *  finish it's execution.  After it all other threads are stopped 
     *  and result of finished thread is returned.
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return result returned by thread first finished it's execution or
     *  <code>null</code> if timeout is expired before any thread finish it's
     *  execution.
     */
    public synchronized Object waitOne(long timeout) { 
        Object result = null;
        while (nTerminated == 0) { 
	    try { 
	        wait(timeout);
	    } catch(InterruptedException ex) {
	        throw new InterruptedError();
	    }
	}
	for (int i = nActivities; --i >= 0;) { 
	     if (results[i] != null) { 
	         result = results[i];
	     } else { 
	         //threads[i].stop();
	         threads[i].interrupt();
	     }
	}
	return result;
    } 

    /** Wait until all started threads finish their execution. 
     * 
     * @return array containing results returned by each of the threads.
     */
    public synchronized Object[] waitAll() { 
        while (nTerminated < nActivities) { 
	    try { 
	        wait();
	    } catch(InterruptedException ex) {
	        throw new InterruptedError();
	    }
	}
        return results;
    } 

    /** Wait at most <code>timeout</code> miliseconds until all started 
     *  threads finish their execution. 
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return array containing results returned by each of the threads or 
     *  <code>null</code> if timeout is expired before all threads finish 
     *  execution.
     */
    public synchronized Object[] waitAll(long timeout) { 
        long startTime = System.currentTimeMillis();      
        while (nTerminated < nActivities) { 
	    long currentTime = System.currentTimeMillis();
            if (currentTime < startTime) { 
                currentTime = startTime;
            }
	    if (currentTime - startTime >= timeout) { 
	        for (int i = nActivities; --i >= 0;) { 
		    //threads[i].stop();
		    threads[i].interrupt();
	        }
	        return null;
	    }
	    try { 
	        wait(timeout - currentTime + startTime);
	    } catch(InterruptedException ex) {
	        throw new InterruptedError();
	    }
	}
        return results;
    } 



    protected synchronized void setResult(int threadNo, Object result) { 
        results[threadNo] = result;
	nTerminated += 1;
	notify();
    }
	
    protected Thread[] threads;
    protected Object[] results;
    protected int      nTerminated;
    protected int      nActivities;
}


class Task extends Thread { 
    Activity   activity;
    Concurrent referee;
    int        id;
  
    public void run() { 
        Object result;
	try { 
  	    result = activity.run();
	} catch(Exception ex) { 
	    result = null;
	}
        referee.setResult(id, result);
    } 

    Task(int id, Activity activity, Concurrent referee) { 
        this.id = id;
        this.activity = activity;
        this.referee = referee;
    }
}
