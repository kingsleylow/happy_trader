//-< Mutex.java >----------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Mutual threads exclusion
//-------------------------------------------------------------------*--------*

package jsync;

/** Class for providing mutual exclusion of threads. Only one thread can enter
 *  critical section guarded by mutex (but can do it several times).
 */
public class Mutex {
    /** Lock mutex. This method should be called before entering critical 
     *  section.
     */ 
    public synchronized void enter() { 
        Thread self = Thread.currentThread();
        if (owner != null || nBlocked != 0) { 
	    if (owner == self) { 
	        nested += 1;
		return;
	    }
	    do { 
	        try {  
	            nBlocked += 1;
		    wait();
		    nBlocked -= 1;
		} catch(InterruptedException ex) { 
		    // It is possible for a thread to be interrupted after 
		    // being notified but before returning from the wait() 
		    // call. To prevent lost of notification notify()
		    // is invoked. 
		    notify(); 
		    nBlocked -= 1;
		    throw new InterruptedError(); 
		}
	    } while (owner != null);
	}
	Assert.that(nested == 0);
	owner = self;
	nested = 1;
    }

    /** Try to lock mutex within specified period of time. This method 
     *  should be called before entering critical section.
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return <code>true</code> if mutex is successfully locked, 
     *  <code>false</code> if <code>enter()</code> was terminated 
     *  due to timeout expiration.
     */
    public synchronized boolean enter(long timeout) { 
        Thread self = Thread.currentThread();
        if (owner != null || nBlocked != 0) { 
	    if (owner == self) { 
	        nested += 1;
		return true;
	    }  
	    if (timeout == 0) { 
		return false;
	    }
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
		    nBlocked += 1;
		    wait(timeout - currentTime + startTime);
		    nBlocked -= 1;
		} catch(InterruptedException ex) { 
		    // It is possible for a thread to be interrupted after 
		    // being notified but before returning from the wait() 
		    // call. To prevent lost of notification notify()
		    // is invoked. 
		    notify(); 
		    nBlocked -= 1;
		    throw new InterruptedError();
		}
	    } while (owner != null);
	}
	Assert.that(nested == 0);
	owner = self;
	nested = 1;
	return true;
    }

    /** Release mutex. This method should be called after exit from critical 
     *  section. Mutex will be unlocked only if number of <code>leave()</code>
     *  invocations is equal to the number of <code>enter()</code> invocations.
     */
    public synchronized void leave() { 
        if (Thread.currentThread() != owner) { 
	    throw new NotOwnerError();
	}
        Assert.that(nested > 0);
        if (--nested == 0) { 
	    owner = null;
	    if (nBlocked != 0) { 
	        notify();
	    }
	}
    }

    protected Thread  owner;
    protected int     nested;
    protected int     nBlocked;
} 
