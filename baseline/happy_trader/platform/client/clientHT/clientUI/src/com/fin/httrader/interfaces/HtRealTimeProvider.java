/*
 * HtRealTimeProvider.java
 *
 * Created on December 3, 2006, 5:07 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.utils.*;
import java.util.List;
import java.util.Map;
/**
 * 
 * 
 * @author Administrator
 * This interface must be implemented in derived classes as RT data provider.
 * The caller is responsilbe for spawing separate thread and inside this thread call the callback class
 * RtProviderCallback must be implemented on the client side and passed to the plugin for registration.
 * HtRealTimeProvider must be implemented on the plugin side.
 * Upon load plugin spawns separate thread and pushes RT data to the callee.
 * function
 */
public interface HtRealTimeProvider {
  
	 // thease are statuses

	

		 // ----------------------------
     
   // called by the client  to register its callback
   public abstract void initialize(Map<String, String> initdata) throws Exception;
   
    // called by the client to unsubscribe from RT retreival
   public abstract void deinitialize() throws Exception;
   
   public abstract String getProviderId();
   
   public abstract void setProviderId(String providerId);
   
   public abstract void setTimeShift(long timeshift);
    
   public abstract long getTimeShift();
	 
	 // optionally return the list of symbols
	 public abstract void returnAvailableSymbolList(List<String> symbols);

	 // returns provider status
	 public abstract BackgroundJobStatus returnProviderStatus();

	   
   
   // 
  
      
  
}
