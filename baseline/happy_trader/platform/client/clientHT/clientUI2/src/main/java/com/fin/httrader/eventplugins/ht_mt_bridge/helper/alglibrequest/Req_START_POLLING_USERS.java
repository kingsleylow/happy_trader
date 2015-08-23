/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_G_GET_ALL_METADATA.COMMAND;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse.Resp_G_GET_ALL_METADATA;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class Req_START_POLLING_USERS extends AlgLibRequestBase {
		public static final String COMMAND = "START_POLLING_USERS";
		
		public static class MtUser
		{
				private String UserName_m;
				private String password_m;
				private String server_m;
				private int lifeTime_m = -1;

				public String getUserName() {
						return UserName_m;
				}

				public void setUserName(String UserName) {
						this.UserName_m = UserName;
				}
				
				public String getServer() {
						return this.server_m;
				}

				public void setServer(String server) {
						this.server_m = server;
				}

				public String getPassword() {
						return password_m;
				}

				public void setPassword(String password) {
						this.password_m = password;
				}

				public int getLifeTime() {
						return lifeTime_m;
				}

				public void setLifeTime(int lifeTime) {
						this.lifeTime_m = lifeTime;
				}
				
				public boolean isValid()
				{
						if (HtUtils.nvl(UserName_m) ||HtUtils.nvl(password_m) || HtUtils.nvl(server_m))
								return false;
						
						return true;
				}
				
				public static void convertToJson(MtUser e,JSONObject je) throws Exception
				{
						je.put("NAME", e.getUserName());
						je.put("PASSWORD", e.getPassword());
						je.put("SERVER", e.getServer());
						int lt = e.getLifeTime();
						if (lt < 0)
								lt = -1;
						
						je.put("LIFETIME", lt);
				}
				
				public static void convertFromJson(MtUser e, JSONObject je) throws Exception
				{
						
						e.setUserName(HtMtJsonUtils.<String>getJsonField(je, "NAME"));
						e.setPassword(HtMtJsonUtils.<String>getJsonField(je, "PASSWORD"));
						e.setServer(HtMtJsonUtils.<String>getJsonField(je, "SERVER"));
						e.setLifeTime((HtMtJsonUtils.<Number>getJsonField(je, "LIFETIME")).intValue());
						
				}
		}
		
		private final List<MtUser> users_m = new ArrayList<MtUser>();
		private String companyName_m = null;
		
		public List<MtUser> getUsers()
		{
				return users_m;
		}
		
		public void setCompanyName(String copanyName)
		{
				companyName_m = copanyName;
		}
		
		public String getCompanyName()
		{
				return companyName_m;
		}
		
		public Req_START_POLLING_USERS()
		{
			
		}
		
		@Override
		public String getMtCommand()
		{
				return COMMAND;
		}
		
		
		@Override
		public void toJson(JSONObject json_root_obj) throws Exception
		{
				super.toJson(json_root_obj);
				
				
				// 
					
				JSONObject data_obj = new JSONObject();
				JSONArray user_list_obj = new JSONArray();
				
				List<MtUser> users = getUsers();
				for(int i = 0; i < users.size(); i++) {
					MtUser user_i = users.get(i);
					
					JSONObject user_obj = new JSONObject();
					MtUser.convertToJson(user_i, user_obj);
					
					
					user_list_obj.add( user_obj );
					
				}
				
				data_obj.put("USER_LIST", user_list_obj);
				json_root_obj.put("DATA", data_obj);
				json_root_obj.put("COMPANY_NAME", this.getCompanyName());
		}
}
