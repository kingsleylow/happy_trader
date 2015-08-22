/*
 * ResponseTypes.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

/**
 *
 * @author Victor_Zoubok
 */
public class ResponseTypes {
    
 public static final int RT_BrokerResponseDummy	=0;
 public static final int RT_BrokerResponseOrder	=1;
 public static final int RT_BrokerResponseEquity=2;
 public static final int RT_BrokerResponseTick	=3;
 public static final int RT_BrokerResponseInfo	=4;
 public static final int RT_BrokerResponseHistory=5;
 public static final int RT_BrokerErrorInfo	 =6;
 public static final int RT_BrokerResponseMetaInfo=7;
 public static final int RT_BrokerResponseDelayedOrder = 8;
 public static final int RT_BrokerResponseClosePos = 9;
 public static final int RT_BrokerResponsePositionOperation = 10;
}
