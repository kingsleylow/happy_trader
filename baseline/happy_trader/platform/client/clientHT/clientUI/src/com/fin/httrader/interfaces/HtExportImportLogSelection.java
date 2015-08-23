/*
 * HtExportImportLogSelection.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

/**
 *
 * @author victor_zoubok
 */
public interface HtExportImportLogSelection {
    
    public void onSelectionFinish() throws Exception;
    
    public void newRowArrived (
     long id,
     long groupid,
     long tlast_created,
     int opertype,
     int operstatus,
     String opermessage,
     String symbol,
     String tdata
   ) throws Exception;
    
}
