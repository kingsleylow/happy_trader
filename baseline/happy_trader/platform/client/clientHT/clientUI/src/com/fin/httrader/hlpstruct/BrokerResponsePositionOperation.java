/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponsePositionOperation extends BrokerResponseBase {

  public BrokerResponsePositionOperation() {
	type_m = ResponseTypes.RT_BrokerResponsePositionOperation;
  }
  
  @HtSaveAnnotation
  public StringBuilder brokerPositionID_m = new StringBuilder();

  @HtSaveAnnotation
  public StringBuilder brokerOrderID_m = new StringBuilder();

  @HtSaveAnnotation
  public StringBuilder provider_m = new StringBuilder();

  @HtSaveAnnotation
  public StringBuilder symbol_m = new StringBuilder();

  @HtSaveAnnotation
  public double volume_m = 0;
  
  @HtSaveAnnotation
  public double price_m = 0;

  @HtSaveAnnotation
  public int direction_m = TradeDirection.TD_NONE;
}
