/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest.Req_START_POLLING_USERS;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import java.util.ArrayList;
import java.util.List;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

/**
 *
 * @author Administrator
 */
public class Resp_G_QUERY_AUTOLOAD_USERS extends AlgLibResponseBase {

	private String getContext() {
		return Resp_G_QUERY_AUTOLOAD_USERS.class.getSimpleName();
	}

	public static class MtUser {

		MtUser() {
			lifeTime_m = -1;
		}

		private String username_m;
		private String password_m;
		private String server_m;
		private int lifeTime_m = -1;

		/**
		 * @return the username_m
		 */
		public String getUsername() {
			return username_m;
		}

		/**
		 * @param username the username_m to set
		 */
		public void setUsername(String username) {
			this.username_m = username;
		}

		/**
		 * @return the password_m
		 */
		public String getPassword() {
			return password_m;
		}

		/**
		 * @param password the password_m to set
		 */
		public void setPassword(String password) {
			this.password_m = password;
		}

		/**
		 * @return the server_m
		 */
		public String getServer() {
			return server_m;
		}

		/**
		 * @param server the server_m to set
		 */
		public void setServer(String server) {
			this.server_m = server;
		}

		/**
		 * @return the lifeTime_m
		 */
		public int getLifeTime() {
			return lifeTime_m;
		}

		/**
		 * @param lifeTime the lifeTime_m to set
		 */
		public void setLifeTime(int lifeTime) {
			this.lifeTime_m = lifeTime;
		}

		public static void convertToJson(Resp_G_QUERY_AUTOLOAD_USERS.MtUser e, JSONObject je) throws Exception {
			je.put("NAME", e.getUsername());
			je.put("PASSWORD", e.getPassword());
			je.put("SERVER", e.getServer());
			int lt = e.getLifeTime();
			if (lt < 0) {
				lt = -1;
			}

			je.put("LIFETIME", lt);
		}

		public static void convertFromJson(Resp_G_QUERY_AUTOLOAD_USERS.MtUser e, JSONObject je) throws Exception {

			e.setUsername(HtMtJsonUtils.<String>getJsonField(je, "NAME"));
			e.setPassword(HtMtJsonUtils.<String>getJsonField(je, "PASSWORD"));
			e.setServer(HtMtJsonUtils.<String>getJsonField(je, "SERVER"));
			e.setLifeTime((HtMtJsonUtils.<Number>getJsonField(je, "LIFETIME")).intValue());

		}
	}; // end MtUser

	public static class MtInstance {

		private List<MtUser> autoLoadUserList_m = new ArrayList<MtUser>();

		// company ID
		private String companyId_m;

		// mt instance name
		private String name_m;

		// mt instnace UID
		private Uid isntanceUid_m = new Uid();

		/**
		 * @return the autoLoadUserList_m
		 */
		public List<MtUser> getAutoLoadUserList() {
			return autoLoadUserList_m;
		}

		/**
		 * @return the companyId_m
		 */
		public String getCompanyId() {
			return companyId_m;
		}

		/**
		 * @param companyId the companyId_m to set
		 */
		public void setCompanyId(String companyId) {
			this.companyId_m = companyId;
		}

		/**
		 * @return the name_m
		 */
		public String getName() {
			return name_m;
		}

		/**
		 * @param name the name_m to set
		 */
		public void setName(String name) {
			this.name_m = name;
		}

		/**
		 * @return the isntanceUid_m
		 */
		public Uid getIsntanceUid() {
			return isntanceUid_m;
		}

		/**
		 * @param isntanceUid the isntanceUid_m to set
		 */
		public void setIsntanceUid(Uid isntanceUid) {
			this.isntanceUid_m = isntanceUid;
		}
		
		public static void convertToJson(Resp_G_QUERY_AUTOLOAD_USERS.MtInstance e, JSONObject je) throws Exception {
			je.put("COMPANY_ID", e.getCompanyId());
			je.put("INSTANCE_UID", e.getIsntanceUid().toString());
			je.put("NAME", e.getName());
			
			JSONArray user_list_obj = new JSONArray();
			
			for(int i = 0; i < e.getAutoLoadUserList().size(); i++) {
				Resp_G_QUERY_AUTOLOAD_USERS.MtUser user = e.getAutoLoadUserList().get(i);
				
				JSONObject user_json_obj = new JSONObject();
				Resp_G_QUERY_AUTOLOAD_USERS.MtUser.convertToJson(user, user_json_obj);
				
				user_list_obj.add(user_json_obj);
			}
			je.put("AUTOLOAD_USER_LIST", user_list_obj);
			
		}

		public static void convertFromJson(Resp_G_QUERY_AUTOLOAD_USERS.MtInstance e, JSONObject je) throws Exception {

			e.setCompanyId(HtMtJsonUtils.<String>getJsonField(je, "COMPANY_ID"));
			e.setIsntanceUid(new Uid(HtMtJsonUtils.<String>getJsonField(je, "INSTANCE_UID")));
			e.setName(HtMtJsonUtils.<String>getJsonField(je, "NAME"));
			
			JSONArray user_list_obj = HtMtJsonUtils.<JSONArray>getJsonField(je, "AUTOLOAD_USER_LIST");
			for(int i = 0; i < user_list_obj.size(); i++) {
				JSONObject user_json_obj = (JSONObject)user_list_obj.get(i);
				
				Resp_G_QUERY_AUTOLOAD_USERS.MtUser user = new Resp_G_QUERY_AUTOLOAD_USERS.MtUser();
				Resp_G_QUERY_AUTOLOAD_USERS.MtUser.convertFromJson(user, user_json_obj);
				
				e.getAutoLoadUserList().add(user);
				
			}
			

		}
		
		
	}; // MtInstance

	private final List<MtInstance> instances_m = new ArrayList<MtInstance>();

	public List<MtInstance> getInstances() {
		return instances_m;
	}

	public Resp_G_QUERY_AUTOLOAD_USERS() {
	}

	@Override
	public void fromJson(JSONObject result_json) throws Exception {

		super.fromJson(result_json);
		
		JSONObject mt_data = HtMtJsonUtils.<JSONObject>getJsonField(result_json, "DATA");
		JSONArray inst_list_obj = HtMtJsonUtils.<JSONArray>getJsonField(mt_data, "MT_LIST");
		
		for(int i = 0; i < inst_list_obj.size(); i++) {
			JSONObject mt_inst = (JSONObject)inst_list_obj.get(i);
			
			MtInstance inst_i = new MtInstance();
			Resp_G_QUERY_AUTOLOAD_USERS.MtInstance.convertFromJson(inst_i, mt_inst);
			
			getInstances().add(inst_i);
		}
		
	}

}
