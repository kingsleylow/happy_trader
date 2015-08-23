//-< Activity.java >-------------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update:  5-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Interface for concurrently executed activity 
//-------------------------------------------------------------------*--------*

package jsync; 

/** Interface for activities executed concurrently by class 
 *  <code>Concurrent</code>. 
 *
 * @see jsync.Concurrent
 */
public interface Activity { 
    /** Method invoked by <code>Concurrent</code> class.
     * 
     * @return result of method excution which is collected by 
     *  <code>Concurrent</code> class.
     */
    Object run();
}
