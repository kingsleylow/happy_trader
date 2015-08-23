//-< Event.java >----------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Event flag with manual reset
//-------------------------------------------------------------------*--------*
package jsync;

/** Event is synchronization object which can be either in signaled or 
 *  non-signaled state. Event is set to signaled state by <code>signal()</code>
 *  method and remains in this state until <code>reset()</code> method will be 
 *  invoked. Setting event to signaled state will awoke all waiting threads.
 */
public class Event {

	/** Wait until event is set to signaled state.
	 */
	public synchronized void waitEvent() {
		int nSignalsBefore = nSignals;
		while (!signaled && nSignals == nSignalsBefore) {
			try {
				wait();
			} catch (InterruptedException ex) {
				throw new InterruptedError();
			}
		}
	}

	/** Wait during specified amount of time until event is set to signaled
	 *  state.
	 * @param timeout the maximum time to wait in milliseconds.
	 * @return <code>true</code> if event is signaled, <code>false</code>
	 *  if <code>wait()</code> was terminated due to timeout expiration.
	 */
	public synchronized boolean waitEvent(long timeout) {
		if (!signaled) {
			if (timeout == 0) {
				return false;
			}
			int nSignalsBefore = nSignals;
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
					wait(timeout);
				} catch (InterruptedException ex) {
					throw new InterruptedError();
				}
			} while (nSignals == nSignalsBefore);
		}
		return true;
	}

	/** Set event to signaled state. Awoke all waiting threads.
	 */
	public synchronized void signal() {
		signaled = true;
		nSignals += 1;
		notifyAll();
	}

	/** Reset event to non-signaled state.
	 */
	public synchronized void reset() {
		signaled = false;

		//TO DO
		// nSignals = 0;
	}

	/** Create event with non-signaled initial state.
	 */
	public Event() {
		signaled = false;
		nSignals = 0;
	}

	/** Create event with specified initial state.
	 *
	 * @param initState initial state of event
	 */
	public Event(boolean initState) {
		signaled = initState;
		nSignals = 0;
	}
	protected boolean signaled;
	protected int nSignals;
} 
