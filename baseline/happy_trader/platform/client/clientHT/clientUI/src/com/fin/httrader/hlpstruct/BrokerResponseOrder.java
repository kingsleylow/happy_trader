/*
 * BrokerResponseOrder.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

import com.fin.httrader.configprops.HtSaveAnnotation;

/**
 *
 * @author Victor_Zoubok
 */
public class BrokerResponseOrder extends BrokerResponseBase{
    
    public BrokerResponseOrder()
    {
			type_m = ResponseTypes.RT_BrokerResponseOrder;
    }
    
    @HtSaveAnnotation
    public StringBuilder brokerOrderID_m = new StringBuilder();

    // broker order ID
		@HtSaveAnnotation
    public StringBuilder brokerPositionID_m = new StringBuilder();
        
    // if broker opened or installed position - this is price
		@HtSaveAnnotation
    public double  opPrice_m = -1;

		// this is STOP price
		@HtSaveAnnotation
		public double opStopPrice_m = -1;
    
     
    // if broker set up TP - this is broker TP price
		@HtSaveAnnotation
    public double  opTPprice_m = -1;
    
    // if broker set up SL  - this is broker SL price
		@HtSaveAnnotation
    public double  opSLprice_m = -1;
    
    // broker volume if broker made operation
    // here volume has the sence of the direct amount
    // it must be re-calculated to lots if ANY
		@HtSaveAnnotation
    public int  opVolume_m = -1;


		// sometimes it is remaining volume
		@HtSaveAnnotation
		public int opRemainingVolume_m = -1;
    
    
    // provider
		@HtSaveAnnotation
    public StringBuilder provider_m = new StringBuilder();
    
    // symbol that is unique for the defined broker - in numenator
		//@HtSaveAnnotation
    //public StringBuilder symbolNum_m = new StringBuilder();
    
    // the symbol in denuminator
		//@HtSaveAnnotation
    //public StringBuilder symbolDenom_m = new StringBuilder();
		
		@HtSaveAnnotation
		public StringBuilder symbol_m = new StringBuilder();
    
    // if broker change expiration time  - this is reflected here
		@HtSaveAnnotation
    public double  opExpirationTime_m = -1;

		@HtSaveAnnotation
		public int opOrderValidity_m = OrderValidity.OV_DUMMY;

		@HtSaveAnnotation
		public int opOrderType_m = OrderType.OP_DUMMY;
    
    // order response time
		@HtSaveAnnotation
    public double  opTime_m = -1;
           
    // comission
		@HtSaveAnnotation
    public double  opComission_m = -1;
    
    
}
