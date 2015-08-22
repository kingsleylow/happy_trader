/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import org.json.simple.JSONObject;


/**
 *
 * @author Victor_Zoubok
 */
public abstract class HtMtEventWrapperBase {
		public static final int TYPE_ACCOUNT_MINFO_UPDATE = 1;
		public static final int TYPE_BALANCE_UPDATE = 2;
		public static final int TYPE_POSITION_UPDATE = 3;
		public static final int TYPE_HEARTBEAT = 4;
		public static final int TYPE_ACCOUNT_UPDATE = 5;
		public static final int TYPE_POSITION_HISTORY = 6;
		public static final int TYPE_ACCOUNT_HISTORY = 7;
		
		
		public HtMtEventWrapperBase()
		{
		}
		
	
		public int getEventType()
		{
				return type_m;
		}
		
		protected int type_m = -1;
		
		
		public void toJson(JSONObject upper_level_obj) throws Exception
		{
				// set type
				upper_level_obj.put("DELIVERY_TYPE_FLAG", type_m);
		}
		
		// tells the system if we are ok
		// rout to relevan recepient when propagating
		public abstract boolean routEventFor(String accountId, String companyName) throws Exception;
}
