//-< NotOwnerError.java >--------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Error raised when thread tries to unlock resource, 
// which is not locked by this thread
//-------------------------------------------------------------------*--------*

package jsync;

/** This error is thrown when thread releasing lock
 *  is not one who set this lock. This error can be thrown by 
 *  <code>Mutex</code> and <code>Lock</code> classes.
 * 
 * @see jsync.Lock
 * @see jsync.Mutex
 */
public class NotOwnerError extends Error { 
    public NotOwnerError() { 
        super("Thread is not owner of resource");
    }
}
