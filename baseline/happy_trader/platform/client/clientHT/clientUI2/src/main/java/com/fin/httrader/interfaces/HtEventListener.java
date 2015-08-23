/*
 * HtEventListener.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.XmlEvent;

/**
 *
 * @author Victor_Zoubok
 * this to be implemented by all listeners  that are able to receive events
 */
public interface HtEventListener {
     public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit;

		 public String getListenerName();
}
