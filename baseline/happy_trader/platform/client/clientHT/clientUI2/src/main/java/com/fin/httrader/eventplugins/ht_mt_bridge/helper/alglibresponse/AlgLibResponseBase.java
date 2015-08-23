/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class AlgLibResponseBase {

		private String mt_instance_name_m = null;
		private final Uid mt_instance_uid_m = new Uid();
		private String result_message_m = null;
		private String mt_result_message_m = null;
		private int result_code_m = -1;
		private int mt_result_code_m = -1;

		public AlgLibResponseBase()
		{
		}
		
		private String getContext()
		{
				return AlgLibResponseBase.class.getSimpleName();
		}
		
		// -------------------
		public String getMt_instance_name() {
				return mt_instance_name_m;
		}

		public void setMt_instance_name(String mt_instance_name) {
				this.mt_instance_name_m = mt_instance_name;
		}

		public Uid getMt_instance_uid() {
				return mt_instance_uid_m;
		}

		public void setMt_instance_uid(Uid mt_instance_uid) {
				this.mt_instance_uid_m.fromUid(mt_instance_uid);
		}
		public void setMt_instance_uid(String mt_instance_uid) {
				this.mt_instance_uid_m.fromString(mt_instance_uid);
		}

		public String getResult_message() {
				return result_message_m;
		}

		public void setResult_message(String result_message) {
				this.result_message_m = result_message;
		}

		public String getMt_result_message() {
				return mt_result_message_m;
		}

		public void setMt_result_message(String mt_result_message) {
				this.mt_result_message_m = mt_result_message;
		}

		public int getResult_code() {
				return result_code_m;
		}

		public void setResult_code(int result_code) {
				this.result_code_m = result_code;
		}

		public int getMt_result_code() {
				return mt_result_code_m;
		}

		public void setMt_result_code(int mt_result_code) {
				this.mt_result_code_m = mt_result_code;
		}
		
		// ---------------------------------
		
		public void fromJson(JSONObject result_json) throws Exception
		{
				if (result_json != null) {
					
						setMt_result_message( (String)result_json.get("MT_MESSAGE") );
						setMt_result_code ( ((Number)result_json.get("MT_CODE")).intValue() );
						setResult_message ( (String)result_json.get("RESULT_MESSAGE") );
						setResult_code( ((Number)result_json.get("RESULT_CODE")).intValue() );
						setMt_instance_name((String)result_json.get("MT_INSTANCE_NAME"));
						setMt_instance_uid((String)result_json.get("MT_INSTANCE_UID"));
				}
				else 
						throw new HtException(getContext(), "fromJson", "Invalid JSON data");

		}
		
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				throw new HtException(getContext(), "toJson", "Not implemented");
		}
}
