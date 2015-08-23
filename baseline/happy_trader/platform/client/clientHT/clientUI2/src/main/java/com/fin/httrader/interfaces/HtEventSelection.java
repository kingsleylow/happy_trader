/*
 * HtEventSelection.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

/**
 *
 * @author Administrator
 */
public interface HtEventSelection {
  
  
  
   // whether we may need to apply finalizing steps
   public void onSelectionFinish() throws Exception;
   
   public class CommonLog {
     public long id;
     public long tlast_created;
     public int etype;
     public int level;
     public int thread;
     public String session;
     public String context;
     public String content_small;
     public String content_large;
     public String attribute1;
     public String attribute2;
     public String attribute3;
     public String attribute4;
     public String attribute5;
     public String attribute6;
     public String attribute7;
     public String attribute8;
     public String attribute9;
     public String attribute10;
   };
   
   public class ExpImpLog {
     public long id;
     public long groupid;
     public long tlast_created_expimp;
     public int opertype;
     public int operstatus;
     public String opermessage;
     public int symbol_id_expimp;
     public String tdata;
   }
   
   // 
   public void newRowArrived(
     CommonLog commonlog,
     ExpImpLog expimplog
   ) throws Exception;
   
   
     
}
