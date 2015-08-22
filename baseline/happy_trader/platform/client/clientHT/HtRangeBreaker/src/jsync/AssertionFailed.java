//-< AssertionFailed.java >------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update:  5-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Error raise when assertion is failed
//-------------------------------------------------------------------*--------*

package jsync;

/** Exception raised by <code>Assert</code> class when assertion is failed.
 *
 * @see     jsync.Assert
 */
public class AssertionFailed extends Error {
    AssertionFailed() { 
        super("Assertion failed");
    }

    AssertionFailed(String description) { 
        super("Assertion " + description + " failed");
    }
}
