/*
 * HtDataSelection.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

/**
 *
 * Interface allowing derived classes to be used in the functions returning rows from DB
 */
public interface HtDataSelection {
    
    public void onSelectionFinish(long cnt) throws Exception;
    
    public void newRowArrived(
       long ttime,
       String tsymbol,
       String tprovider,
       int ttype,
       int aflag,
       double tbid,
       double task, 
       double topen,
       double thigh,
       double tlow,
       double tclose,
       double topen2,
       double thigh2,
       double tlow2,
       double tclose2,
       double tvolume,
       long tid,
			 int color,
			 int operation
      ) throws Exception;
}
