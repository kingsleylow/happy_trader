/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth;

import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.servlets.HappyTraderProxyGateServlet;
import com.mtrunner.client.userlist.authentities.UserRole;
import com.mtrunner.client.userlist.authentities.UserToAuthenticate;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;
import java.util.List;
import javax.sql.DataSource;

import org.apache.commons.dbutils.DbUtils;
import org.apache.log4j.Logger;

/**
 *
 * @author Administrator
 */
public class JoomlaDBUtil {

	
	
	private org.apache.commons.dbcp.BasicDataSource db;
	private static volatile JoomlaDBUtil instance_m = null;
	private static int sessionTimeOut = 10;
	private String tprfx;
	
	static Logger log_m = Logger.getLogger(JoomlaDBUtil.class.getName());
	
	public static JoomlaDBUtil getInstance()
	{
		if (instance_m == null) {
			synchronized(JoomlaDBUtil.class) {
				instance_m = new JoomlaDBUtil();
			}
		}
		return instance_m;
	}

	public static int getSessionTimeOut() {
		return sessionTimeOut;
	}

	public static void setSessionTimeOut(int aSessionTimeOut) {
		sessionTimeOut = aSessionTimeOut;
	}
	
	public org.apache.commons.dbcp.BasicDataSource getDb() {
		return db;
	}

	public void setDb(org.apache.commons.dbcp.BasicDataSource db) {
		this.db = db;
	}
	
	public String getTprfx() {
		return tprfx;
	}

	public void setTprfx(String tprfx) {
		if (tprfx == null)
			this.tprfx = "";
		else
			this.tprfx = tprfx;
	}
	
	public UserToAuthenticate resolveJoomlaUser(String joomlauser) throws Exception
	{
		Connection conn = null;
		
		
		UserToAuthenticate user = new UserToAuthenticate();
		
		try {
			conn = db.getConnection();
			String sql1 = "SELECT u.username, u.password, u.name, u.email, u.id, UNIX_TIMESTAMP()-s.time FROM "+tprfx+"_users u, " +
							tprfx + "_session s " + 
							"where u.username=? and s.userid=u.id and u.block=0 and s.guest=0";
			
		
			PreparedStatement ps = conn.prepareStatement(sql1);
			ps.setString(1, joomlauser);
			
			long id = -1;
			ResultSet rs = ps.executeQuery();
			if (rs.next()) {
				user.setEmail(rs.getString(4));
				user.setUsername(rs.getString(1));
				user.setFirstName(rs.getString(3));
				user.setEnabled(true);
				user.setPassword(rs.getString(2));
				
				long session_length = rs.getLong(6);
			
				if (session_length > sessionTimeOut)
					throw new Exception("Joomla session expired");
				
				id = rs.getLong(5);
			}
			
			if (id < 0)
				throw new Exception("No valid session found");
			
			// extract roles
			List<UserRole> roles = new ArrayList<>();
			String sql2 = "SELECT ug.title FROM "+tprfx+"_users u, "+tprfx+"_usergroups ug, "+
				tprfx+"_user_usergroup_map ugm " +
				"where u.id = ugm.user_id and " +
				"ugm.group_id = ug.id and u.id=?";
			
			PreparedStatement ps2 = conn.prepareStatement(sql2);
			
			ps2.setLong(1, id);
			
			ResultSet rs2 = ps2.executeQuery();
			while (rs2.next()) {
				UserRole role = new UserRole();
				role.setName(rs2.getString(1));
				
				roles.add(role);
			}
			
			user.setAuthorities(roles);
			
		}
		finally
		{
			if (conn != null)
				DbUtils.closeQuietly(conn);
		}
		
		return user;
	}

	
	
}
