/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.AlgLibResponseBase;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 * base class encapsulating requests to alg lib layer
 */
public abstract class AlgLibRequestBase {
		
		private String mtInstnaceName_m = null;
	

		private String getContext()
		{
				return AlgLibRequestBase.class.getSimpleName();
		}
		public String getMtInstnaceName() {
				return mtInstnaceName_m;
		}

		public void setMtInstnaceName(String mtInstnaceName) {
				if (!HtUtils.nvl(mtInstnaceName))
						this.mtInstnaceName_m = mtInstnaceName;
				else
						this.mtInstnaceName_m = "";
		}

		abstract public String getMtCommand();

	
		public void fromJson(JSONObject result_json) throws Exception
		{
				if (result_json != null) {
					
						setMtInstnaceName( (String)result_json.get("MT_INSTANCE") );
						
				}
				else 
						throw new HtException(getContext(), "fromJson", "Invalid JSON data");

		}
		
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				String mt_inst_name = getMtInstnaceName();
				json_root_obj.put("MT_INSTANCE", mt_inst_name != null ? mt_inst_name : "");
				json_root_obj.put("MTCOMMAND_NAME", getMtCommand());
		}
}
