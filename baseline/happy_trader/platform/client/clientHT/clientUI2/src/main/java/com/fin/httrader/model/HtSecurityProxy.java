/*
 * HtSecurityProxy.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;


import com.fin.httrader.managers.RtSecurityManager;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.List;


/**
 *
 * @author Victor_Zoubok
 */
public class HtSecurityProxy extends HtProxyBase {

	private String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtSecurityProxy */
	public HtSecurityProxy() {
	}

	public String remote_resolveHashedUserPassword(String user) throws Exception
	{
		return RtSecurityManager.instance().resolveHashedUserPassword(user);
	}

	public boolean remote_checkUserCredentialsWithHashedPassword(String user, String password_hashed) throws Exception {
		return RtSecurityManager.instance().isUserCredentialsValid(user, password_hashed);
	}
	
	public boolean remote_checkUserCredentialsWithHashedPasswordAndHashedUser(String user_hashed, String password_hashed) throws Exception {
		return RtSecurityManager.instance().isUserCredentialsValid_HashedUser(user_hashed, password_hashed);
	}
	
	public String remote_startUserSessionWithHashedPassword(String user, String password_hashed, String userIp) throws Exception {
		return RtSecurityManager.instance().startUserSessionWithHashedPassword(user, password_hashed, userIp);
	}

	public String remote_startUserSession(String user, String password, String userIp) throws Exception {
		return RtSecurityManager.instance().startUserSession(user, password, userIp);
	}
	
	public boolean remote_verifyUserSession(String sessionID, long timeoutMsec) throws Exception {
		return RtSecurityManager.instance().verifyUserSession(sessionID, timeoutMsec);
	}

	public void remote_stopUserSession(String sessionID) throws Exception {
		RtSecurityManager.instance().stopUserSession(sessionID);
	}

	public void remote_registerNewUser(String username, String password) throws Exception {
		RtSecurityManager.instance().registerNewUser(username, password);
	}

	public void remote_disableUser(String username) throws Exception {
		RtSecurityManager.instance().disableUser(username);
	}

	public void remote_enableUser(String username) throws Exception {
		RtSecurityManager.instance().enableUser(username);
	}

	public void remote_updateUserPassword(String username, String password) throws Exception {
		RtSecurityManager.instance().updateUserPassword(username, password);
	}

	public List<RtSecurityManager.UserStructure> remote_queryAvailableUsers() throws Exception {
		return RtSecurityManager.instance().queryAvailableUsers();
	}

	public RtSecurityManager.UserStructure remote_resolveUserViaSessionId(String sessionID) throws Exception {
		return RtSecurityManager.instance().resolveUserViaSessionId(sessionID);
	}



	public String getProxyName() {
		return "rt_security_proxy";
	}
}
