//-< Semaphore.java >------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Simple semaphore with wait() signal() operations
//-------------------------------------------------------------------*--------*

package jsync;

/** Classical Dijkstra semaphore with <code>wait()</code> and 
 *  <code>signal()</code> operations.
 */
public class Semaphore {
    /** Wait for non-zero value of counter.
     */
    public synchronized void waitSemaphore() { 
        while (counter == 0) { 
            try { 
                wait();
	    } catch(InterruptedException ex) { 
	        // It is possible for a thread to be interrupted after 
	        // being notified but before returning from the wait() 
	        // call. To prevent lost of notification notify()
	        // is invoked. 
	        notify(); 
	        throw new InterruptedError(); 
	    }
	} 
	counter -= 1;
    }

    /** Wait at most <code>timeout</code> miliseconds for non-zero value 
     *  of counter.
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return <code>true</code> if counter is not zero, <code>false</code>
     *  if <code>wait()</code> was terminated due to timeout expiration.
     */
    public synchronized boolean waitSemaphore(long timeout) { 
        if (counter == 0) { 
            long startTime = System.currentTimeMillis();      
            do { 
		long currentTime = System.currentTimeMillis();
                if (currentTime < startTime) { 
                    currentTime = startTime;
                }
		if (currentTime - startTime >= timeout) { 
		    return false;
		}
		try { 
		    wait(timeout - currentTime + startTime);
		} catch(InterruptedException ex) { 
		    // It is possible for a thread to be interrupted after 
		    // being notified but before returning from the wait() 
		    // call. To prevent lost of notification notify()
		    // is invoked. 
		    notify(); 
		    throw new InterruptedError();
		}
	    } while (counter == 0);  
	}
	counter -= 1;
	return true;
    }

    /** Increment value of the counter. If there are waiting threads, exactly 
     *  one of them will be awaken.
     */
    public synchronized void signal() { 
        counter += 1;
        notify();
    }

    /** Create semaphore with zero counter value.
     */
    public Semaphore() { counter = 0; }

    /** Create semaphore with specified non-negative counter value.
     * 
     * @param initValue initial value of semaphore counter
     */
    public Semaphore(int initValue) { 
        Assert.that(initValue >= 0);
        counter = initValue; 
    }

    protected int counter;
} 


