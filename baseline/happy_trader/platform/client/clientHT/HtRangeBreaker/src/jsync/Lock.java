//-< Lock.java >-----------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Class supporting shared and exclusive locks
//-------------------------------------------------------------------*--------*

package jsync;

/** Class for supporting resource locking. Lock can be exclusive or shared,
 *  so multiple-readers single-writer protocol can be implemented. 
 *  Locks can be nested, i.e. a thread can set a lock several times and 
 *  resources will be unlocked only after execution of correspondent number 
 *  of unlock requests.
 */
public class Lock {
    /** Lock resource in shared mode. Several threads can set shared lock, but 
     *  no one can set exclusive lock until all shared locks will be released. 
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return <code>true</code> if lock is successfully set, 
     *  <code>false</code>  if resource can't be locked before expiration 
     *  of timeout.
     */
    public synchronized boolean setShared(long timeout) { 
        Thread self = Thread.currentThread();
	if (self == owner) { 
	    Assert.that(nested != 0 && nWriters == 1 && nReaders <= 1);
	    if (nReaders == 1) { 
	        Assert.that(sharedLockChain.owner == self);
		sharedLockChain.count += 1;
	    } else { 
	        nReaders = 1;
		LockObject lck = createLock();
		lck.owner = self;
		lck.count = 1;
		lck.next = sharedLockChain;
		sharedLockChain = lck;
	    }
	    return true;
	}
        long startTime = System.currentTimeMillis();      
	while (nWriters != 0) { 
	    long currentTime = System.currentTimeMillis();
            if (currentTime < startTime) { 
                currentTime = startTime;
            }
	    if (currentTime - startTime >= timeout) { 
	        return false;
	    }
	    waiting = true;
	    try { 
                if (timeout == Long.MAX_VALUE) { 
                    wait();
                } else { 
                    wait(timeout - currentTime + startTime );
                }
	    } catch(InterruptedException ex) { 
	        throw new InterruptedError();
	    }
	}
	Assert.that(owner == null && nested == 0);
	LockObject lck;
	for (lck = sharedLockChain; lck != null; lck = lck.next) { 
	    if (lck.owner == self) { 
	        lck.count += 1;
		return true;
	    }
	}
	lck = createLock();
	lck.owner = self;
	lck.count = 1;
	lck.next = sharedLockChain;
	sharedLockChain = lck;
	nReaders += 1;
	return true;
    }

    /** Lock resource in shared mode without limitation of waiting time.
     */
    public synchronized void setShared() { 
        setShared(Long.MAX_VALUE);
    }

    /** Lock resource in exclusive mode. Only one thread can lock resource in 
     *  exclusive mode and all other shared and exclusive lock requests will be 
     *  blocked until this lock is released.
     * 
     * @param timeout the maximum time to wait in milliseconds.
     * @return <code>true</code> if lock is successfully set, 
     *  <code>false</code> if resource can't be locked within specified time.
     */
    public synchronized boolean setExclusive(long timeout) { 
        Thread self = Thread.currentThread();
	if (self == owner) { 
	    Assert.that(nested != 0 && nWriters == 1);
	    nested += 1;
	    return true;
	}
        long startTime = System.currentTimeMillis();      
	while (nWriters != 0 
	       || (nReaders != 0 
		   && (nReaders != 1 || sharedLockChain.owner != self)))
        { 
	    long currentTime = System.currentTimeMillis();
            if (currentTime < startTime) { 
                currentTime = startTime;
            }
	    if (currentTime - startTime >= timeout) { 
	        return false;
	    }
	    waiting = true;
	    try { 
                if (timeout == Long.MAX_VALUE) { 
                    wait();
                } else { 
                    wait(timeout - currentTime + startTime );
                }
	    } catch(InterruptedException ex) { 
	        throw new InterruptedError();
	    }
	}
	Assert.that(owner == null && nested == 0);
	owner = self;
	nested = 1;
	nWriters = 1;
	return true;
    }

    /** Lock resource in exclusive mode without limitation of waiting time.
     */
    public synchronized void setExclusive() { 
        setExclusive(Long.MAX_VALUE);
    }

    /** Release shared lock. Throws <code>NotOwnerError</code> if shared 
     *  lock was not previously set by this thread. 
     */
    public synchronized void unsetShared() { 
        Thread self = Thread.currentThread();
	LockObject prev = null;
	for (LockObject lck = sharedLockChain; lck != null; lck = lck.next){
   	    if (lck.owner == self) { 
	        if (--lck.count == 0) { 
		    if (prev == null) { 
			sharedLockChain = lck.next;
		    } else { 
			prev.next = lck.next;
		    }
		    freeLock(lck);
		    nReaders -= 1;
		    if (waiting) { 
			waiting = false;
			notifyAll();
		    }
		}
		return;
	    }
	    prev = lck;
	}
	throw new NotOwnerError();
    }


    /** Release exclusive lock. If thread locked resource several time, than
     *  lock will be taken off only after correspondent number of 
     *  <code>unsetExclusive</code> requests. 
     *  Throws <code>NotOwnerError</code> if exclusive lock was not previously 
     *  set by this thread. 
     */
    public synchronized void unsetExclusive() { 
        Thread self = Thread.currentThread();
        if (owner != self) { 
	    throw new NotOwnerError();
	}
	if (--nested == 0) { 
	    owner = null;
	    nWriters = 0;
	    if (waiting) { 
	        waiting = false;
		notifyAll();
	    }
	}
    }

    //
    // Implementation
    //
    protected int        nReaders;
    protected int        nWriters;
    protected Thread     owner;
    protected int        nested;
    protected boolean    waiting;
    protected LockObject sharedLockChain;

    protected static LockObject freeLockChain;

    protected synchronized final static LockObject createLock() { 
	LockObject lck = freeLockChain;
        if (lck != null) { 
	    freeLockChain = lck.next;
	    return lck;
	} 
	return new LockObject();
    }
    
    protected synchronized final static void freeLock(LockObject lck) {
        lck.next = freeLockChain;
	freeLockChain = lck;
    }
}
 
class LockObject {
    LockObject next;
    Thread     owner;
    int        count;
}


