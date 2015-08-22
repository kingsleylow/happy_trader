/*
 * HtDbWritePacket.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.utils.hlpstruct;

/**
 *
 * @author Administrator
 * class responsible for writing may king of DB data to database
 */
public class HtDbWritePacket {
  
  public static final int RT_PACKET = 0;

  //public static final int COMMIT_PACKET = 1;
  
  public static final int EVENT_PACKET = 2;
  
  public static final int DYMMY_PACKET = 3;
  
  // construct dummy packet
  public HtDbWritePacket() {
    type_m = DYMMY_PACKET;
  }
  
  /*
  public void constructCommitPacket() {
    type_m = COMMIT_PACKET;
  }
   */
  
  public HtDbWritePacket(XmlEvent event, String profile, int multfactor, int tsval) {
    type_m = RT_PACKET;
    
    
    //xmlEvent_m = new XmlEvent();
    //xmlEvent_m.create(event);

		xmlEvent_m = event;
    
    profileName_m.setLength(0);
    profileName_m.append(profile);
    
    multFactor_m = multfactor;
    
    tsVal_m = tsval;
    
  }
 
  public HtDbWritePacket(XmlEvent event) {
    type_m = EVENT_PACKET;
    
    //xmlEvent_m = new XmlEvent();
    //xmlEvent_m.create(event);

		xmlEvent_m= event;
    
   
  }
  
  // packet type
  public int type_m; 
    
    
  public StringBuilder profileName_m = new StringBuilder();
  
   
  public int multFactor_m = -1;
  
  public int tsVal_m = -1;
  
  // data for event
  public XmlEvent xmlEvent_m = null;
  
 
  
  
}
