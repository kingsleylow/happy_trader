/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.delivery;

import com.fin.httrader.utils.HtDateTimeUtils;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtEventWrapperHeartbeat extends HtMtEventWrapperBase{
		
		public HtMtEventWrapperHeartbeat() {
				type_m = HtMtEventWrapperBase.TYPE_HEARTBEAT;
		}

		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
		
		@Override
		public void toJson(JSONObject upper_level_obj) throws Exception {
				super.toJson(upper_level_obj);
					
				upper_level_obj.put("TIME", HtDateTimeUtils.getCurGmtTime());
		}
		
		@Override
		public boolean routEventFor(String accountId, String companyName) throws Exception
		{
				// heartbeat t be propagated everywhere
				return true;
		}
}
