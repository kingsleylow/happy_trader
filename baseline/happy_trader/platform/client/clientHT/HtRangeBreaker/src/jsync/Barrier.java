//-< Barrier.java >--------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Barrier for concurrent threads. 
//-------------------------------------------------------------------*--------*

package jsync;


/** Class for synchronizing concurrent execution of several threads.
 *  By means of this class threads can wait until all of them reach 
 *  some particular state (barrier) and only after it continue execution.
 */
public class Barrier { 
    /** Reset <code>Barrier</code> object.
     * 
     * @param n specifies number of threads which should reach barrier.
     */
    public synchronized void reset(int n) { 
        count = n;
    }
 
    /** This method is called by thread reached barrier. Current thread will
     *  be suspended until all threads reach the barrier.
     */
    public synchronized void reach() { 
        Assert.that(count > 0);
	if (--count != 0) { 
	    do { 
	        try { 
	            wait();
		} catch(InterruptedException ex) { 
		    throw new InterruptedError(); 
		}
	    } while (count != 0); // preclude spurious wakeups
	} else { 
	    notifyAll();
	}
    }

    /** Default constructor of <code>Barrier</code> object.
     */
    public Barrier() { 
        count = 0;
    }

    /** Constructor of <code>Barrier</code> object, which also set number
     *  of threads to be meet at barrier.
     * 
     * @param n specifies number of threads which should reach barrier.
     */
    public Barrier(int n) { 
        count = n;
    }

    protected int count;
}




