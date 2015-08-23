//-< InterruptedError.java >-----------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 20-Jun-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Error raise by JSYNC when waiting thread is interrupted.
//-------------------------------------------------------------------*--------*

package jsync;

/** Exception raised when thread is blocked in <code>wait()</code>
 *  or <code>join()</code> method was interrupted by another thread.
 *  JSYNC catch <code>InterruptedException</code> and throws 
 *  <code>InterruptedError</code> to make it posible for applications
 *  using JSYNC classes not to worry about catching this exception or 
 *  declaring it in <code>throws</code> list.
 */
public class InterruptedError extends Error {
    InterruptedError() { 
			super("thread is interrupted");
    }
}
