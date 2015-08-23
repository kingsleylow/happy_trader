/*
 * RtSecurityManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;



import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.Uid;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 */
public class RtSecurityManager implements RtManagerBase {

	public static class UserStructure
{
    public StringBuilder userName_m = new StringBuilder();

    public boolean isEnabled_m = false;

		public StringBuilder hashedPassword_m = new StringBuilder();

		

};


	public static final String HT_SEC_COOKIE_NAME = "HT_SECURITY_SESSION_COOKIE";
	// ---------------------------------------
	static private RtSecurityManager instance_m = null;
	private final Object mtx_m = new Object();

	static public RtSecurityManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtSecurityManager();
		}

		return instance_m;
	}

	static public RtSecurityManager instance() {
		return instance_m;
	}

	// -----------------------------------
	/** Creates a new instance of RtSecurityManager */
	public RtSecurityManager() {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	public UserStructure resolveUserViaSessionId(String sessionStringId) throws Exception {
		UserStructure us = new UserStructure();
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("select usr.username, usr.disabled , usr.password from USERS usr, USER_SESSIONS uss where usr.id = uss.user_id and uss.session_id=?");


		int cnt = 0;
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, sessionStringId);

			ResultSet rs = stmt.executeQuery();

			
			while (rs.next()) {


				us.userName_m.append(rs.getString(1));
				us.isEnabled_m = (rs.getInt(2) == 0);
				us.hashedPassword_m.append( rs.getString(3));


				cnt++;
			}

			if (cnt > 1) {
				throw new HtException(getContext(), "resolveUserViaSessionId", "More than one user detected for session: \"" + sessionStringId + "\"");
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "resolveUserViaSessionId", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}

		if (cnt==0)
			return null;

		return us;
	}

	public List<UserStructure> queryAvailableUsers() throws Exception {

		synchronized (mtx_m) {
			List<UserStructure> result = new ArrayList<UserStructure>();

			PreparedStatement stmt = null;
			StringBuilder sql = new StringBuilder();
			sql.append("select username, disabled, password from USERS");

			Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
			try {
				stmt = conn.prepareStatement(sql.toString());
				ResultSet rs = stmt.executeQuery();

				while (rs.next()) {
					UserStructure us = new UserStructure();
					us.userName_m.append(rs.getString(1));
					us.isEnabled_m = (rs.getInt(2) == 0);
					us.hashedPassword_m.append(rs.getString(3));

					result.add(us);
				}

			} catch (SQLException sqe) {
				throw new HtException(getContext(), "queryAvailableUsers", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
			} catch (Throwable e) {
				throw new Exception(e);
			} finally {

				if (stmt != null) {
					stmt.close();
				}

				if (conn != null) {
					conn.close();
				}
			}
			;

			return result;
		}


	}

	public String resolveHashedUserPassword(String user) throws Exception
	{
		LongParam isDisabled = new LongParam();
		LongParam userId = new LongParam();
		StringBuilder hashedPassword = new StringBuilder();

		getUserInfo(user, hashedPassword, isDisabled, userId);

		if (isDisabled.getLong() == -1 || isDisabled.getLong() == 1) // user not found or disabled
		{
			return null;
		}

		return hashedPassword.toString();

	}
	
	public boolean isUserCredentialsValid_HashedUser(String user_hashed, String password_hashed) throws Exception
	{
		synchronized (mtx_m) {
			
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();
			StringBuilder hashedPassword = new StringBuilder();
			
			getUserInfo_HashedUser(user_hashed, hashedPassword, isDisabled, userId);
			
			if (isDisabled.getLong() == -1 || isDisabled.getLong() == 1) // user not found or disabled
			{
				return false;
			}
			
			// check password with already hashed one
			if (!hashedPassword.toString().equalsIgnoreCase(password_hashed)) // password invalid
			{
				return false;
			}
			
			return true;
			
		}
	}

	// just checks credentials without password
	public boolean isUserCredentialsValid(String user, String password_hashed) throws Exception
	{
		
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();
			StringBuilder hashedPassword = new StringBuilder();

			getUserInfo(user, hashedPassword, isDisabled, userId);

			if (isDisabled.getLong() == -1 || isDisabled.getLong() == 1) // user not found or disabled
			{
				return false;
			}
			
			// check password with already hashed one
			if (!hashedPassword.toString().equalsIgnoreCase(password_hashed)) // password invalid
			{
				return false;
			}
			
			return true;
		}
		
	}
					

	// it starts user session when credentials are valid
	// if it is ok it returns sessionID
	// if current session for that user exists it just update last logintime
	// and returns current session ID
	// if credentials are invalid it returns null
	public String startUserSessionWithHashedPassword(String user, String password_hashed, String userIp) throws Exception
	{
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();
			StringBuilder hashedPassword = new StringBuilder();

			getUserInfo(user, hashedPassword, isDisabled, userId);

			if (isDisabled.getLong() == -1 || isDisabled.getLong() == 1) // user not found or disabled
			{
				return null;
			}


			// check password with already hashed one
			if (!hashedPassword.toString().equalsIgnoreCase(password_hashed)) // password invalid
			{
				return null;
			}


			// add new
			Uid newSessionUid = new Uid();
			newSessionUid.generate();
			String newSessionId = newSessionUid.toString();
			addNewSessionInfo(newSessionId, userId.getLong(), userIp);

			return newSessionId;

		}
	}

	public String startUserSession(String user, String password, String userIp) throws Exception {
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();
			StringBuilder hashedPassword = new StringBuilder();

			getUserInfo(user, hashedPassword, isDisabled, userId);

			if (isDisabled.getLong() == -1 || isDisabled.getLong() == 1) // user not found or disabled
			{
				return null;
			}


			// check password
			if (!hashedPassword.toString().equalsIgnoreCase(HtMathUtils.md5HashString(password))) // password invalid
			{
				return null;
			}


			// add new
			Uid newSessionUid = new Uid();
			newSessionUid.generate();
			String newSessionId = newSessionUid.toString();
			addNewSessionInfo(newSessionId, userId.getLong(), userIp);

			return newSessionId;

		}
	}

	// if session is ok it returns true
	// it must check last log in and invalidate session if timeout expired
	// otherwise it updates last logintime
	public boolean verifyUserSession(String sessionStringID, long timeoutMsec) throws Exception {
		synchronized (mtx_m) {
			Uid uid = new Uid();
			uid.fromString(sessionStringID);

			if (uid.isValid()) {
				LongParam lastLogin = new LongParam();
				LongParam sessionID = new LongParam();

				getSessionInfo_viaSessionId(sessionStringID, lastLogin, sessionID);
				if (sessionID.getLong() != -1) {
					// found
					if ((HtDateTimeUtils.getCurGmtTime() - lastLogin.getLong()) < timeoutMsec) {
						// valid session - update last login time

						setLastLoginTimeSession(sessionID.getLong());
						return true;
					}

					// invalidate
					disableSession(sessionID.getLong());

				}
			}

			// invalidate
			return false;
		}
	}

	// it stops user session invalidating this sessionID
	public void stopUserSession(String sessionStringID) throws Exception {
		synchronized (mtx_m) {
			Uid uid = new Uid();
			uid.fromString(sessionStringID);

			if (uid.isValid()) {
				LongParam lastLogin = new LongParam();
				LongParam sessionID = new LongParam();

				getSessionInfo_viaSessionId(sessionStringID, lastLogin, sessionID);
				if (sessionID.getLong() != -1) {
					// found, disable
					disableSession(sessionID.getLong());
				}
			}
		}
	}

	public void registerNewUser(String username, String password) throws Exception {
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();

			getUserInfo(username, null, isDisabled, null);
			if (isDisabled.getLong() != -1) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" already exists");
			}


			PreparedStatement stmt = null;
			StringBuilder sql = new StringBuilder();
			sql.append("insert into USERS(username, password, disabled) values (?,?,0)");

			Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
			try {
				stmt = conn.prepareStatement(sql.toString());
				stmt.setString(1, username);
				stmt.setString(2, HtMathUtils.md5HashString(password));

				stmt.execute();
				conn.commit();

			} catch (SQLException sqe) {
				throw new HtException(getContext(), "registerNewUser", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
			} catch (Throwable e) {
				throw new Exception(e);
			} finally {

				if (stmt != null) {
					stmt.close();
				}

				if (conn != null) {
					conn.close();
				}
			}
			;
		}
	}

	public void disableUser(String username) throws Exception {
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();

			getUserInfo(username, null, isDisabled, userId);
			if (isDisabled.getLong() == 1) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" already disabled");
			}

			if (isDisabled.getLong() == -1) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" not found");
			}



			PreparedStatement stmt = null;
			PreparedStatement stmt2 = null;
		
			String sql= "";
		
			Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
			try {
				sql= "update USERS set disabled=1 where id=?";
				stmt = conn.prepareStatement(sql);
				stmt.setLong(1, userId.getLong());
				stmt.execute();

				sql = "update USER_SESSIONS set disabled=1 where user_id=?";
				stmt2 = conn.prepareStatement(sql);
				stmt2.setLong(1, userId.getLong());
				stmt2.execute();

				conn.commit();

			} catch (SQLException sqe) {
				throw new HtException(getContext(), "disableUser", "SQL exception in \"" + sql + "\" happened: " + sqe.getMessage());
			} catch (Throwable e) {
				throw new Exception(e);
			} finally {

				if (stmt != null) {
					stmt.close();
				}

				if (stmt2 != null) {
					stmt2.close();
				}

				if (conn != null) {
					conn.close();
				}
			}
			
		}
	}

	public void enableUser(String username) throws Exception {
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();

			getUserInfo(username, null, isDisabled, userId);
			if (isDisabled.getLong() == 0) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" already enabled");
			}

			if (isDisabled.getLong() == -1) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" not found");
			}


			PreparedStatement stmt = null;
			StringBuilder sql = new StringBuilder();
			sql.append("update USERS set disabled=0 where id=?");

			Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
			try {
				stmt = conn.prepareStatement(sql.toString());
				stmt.setLong(1, userId.getLong());


				stmt.execute();
				conn.commit();

			} catch (SQLException sqe) {
				throw new HtException(getContext(), "enableUser", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
			} catch (Throwable e) {
				throw new Exception(e);
			} finally {

				if (stmt != null) {
					stmt.close();
				}

				if (conn != null) {
					conn.close();
				}
			}
			;
		}
	}

	public void updateUserPassword(String username, String password) throws Exception {
		synchronized (mtx_m) {
			LongParam isDisabled = new LongParam();
			LongParam userId = new LongParam();

			getUserInfo(username, null, isDisabled, userId);
			if (isDisabled.getLong() == -1) {
				throw new HtException(getContext(), "registerNewUser", "User: \"" + username + "\" does not exist");
			}


			PreparedStatement stmt = null;
			StringBuilder sql = new StringBuilder();
			sql.append("update USERS set password=? where id=?");

			Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
			try {
				stmt = conn.prepareStatement(sql.toString());
				stmt.setString(1, HtMathUtils.md5HashString(password));
				stmt.setLong(2, userId.getLong());

				stmt.execute();
				conn.commit();

			} catch (SQLException sqe) {
				throw new HtException(getContext(), "updateUserPassword", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
			} catch (Throwable e) {
				throw new Exception(e);
			} finally {

				if (stmt != null) {
					stmt.close();
				}

				if (conn != null) {
					conn.close();
				}
			}
			;
		}
	}

	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// --------------------------------------------
	// helpers
	// get user info from DB
	private void getUserInfo(String userName, StringBuilder hashedPassword, LongParam isDisabled, LongParam userId) throws Exception {
		// not found
		if (userId != null) {
			userId.setLong(-1);
		}

		if (isDisabled != null) {
			isDisabled.setLong(-1);
		}

		if (hashedPassword != null) {
			hashedPassword.setLength(0);
		}

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("select password, disabled, id from USERS where username=?");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, userName);

			ResultSet rs = stmt.executeQuery();

			if (rs.next()) {
				if (isDisabled != null) {
					isDisabled.setLong(rs.getLong(2));
				}

				if (userId != null) {
					userId.setLong(rs.getLong(3));
				}

				if (hashedPassword != null) {
					hashedPassword.append(rs.getString(1));
				}
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getUserInfo", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;
	}

	private void getUserInfo_HashedUser(String hashedUserName, StringBuilder hashedPassword, LongParam isDisabled, LongParam userId) throws Exception {
		// not found
		if (userId != null) {
			userId.setLong(-1);
		}

		if (isDisabled != null) {
			isDisabled.setLong(-1);
		}

		if (hashedPassword != null) {
			hashedPassword.setLength(0);
		}

		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("select password, disabled, id from USERS where md5(username)=?");

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, hashedUserName);

			ResultSet rs = stmt.executeQuery();

			if (rs.next()) {
				if (isDisabled != null) {
					isDisabled.setLong(rs.getLong(2));
				}

				if (userId != null) {
					userId.setLong(rs.getLong(3));
				}

				if (hashedPassword != null) {
					hashedPassword.append(rs.getString(1));
				}
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getUserInfo", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;
	}

	private void getSessionInfo_viaSessionId(String sessionStringId, LongParam lastLogin, LongParam sessionID) throws Exception {
		if (lastLogin != null) {
			lastLogin.setLong(-1);
		}

		if (sessionID != null) {
			sessionID.setLong(-1);
		}



		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("select uss.last_login, uss.id from USER_SESSIONS uss where uss.session_id=? and uss.disabled=0");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setString(1, sessionStringId);

			ResultSet rs = stmt.executeQuery();

			int cnt = 0;
			while (rs.next()) {


				if (lastLogin != null) {
					lastLogin.setLong(rs.getLong(1));
				}


				if (sessionID != null) {
					sessionID.setLong(rs.getLong(2));
				}

				cnt++;
			}

			if (cnt > 1) {
				throw new HtException(getContext(), "getSessionInfo_viaSessionId", "More than one user sessions detected for session ID: \"" + sessionStringId + "\"");
			}

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "getSessionInfo_viaSessionId", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;

	}

	
	private void addNewSessionInfo(String sessionID, long userId, String userIp) throws Exception {
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("insert into USER_SESSIONS(user_id, session_id, last_login, disabled, user_ip) values (?,?,?,0,?)");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setLong(1, userId);
			stmt.setString(2, sessionID);
			stmt.setLong(3, HtDateTimeUtils.getCurGmtTime());

			if (userIp.length() <=32)
				stmt.setString(4, userIp);
			else
				stmt.setString(4, userIp.substring(0,31));

			stmt.execute();
			conn.commit();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "addNewSessionInfo", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;
	}

	private void disableSession(long sessionID) throws Exception {
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("update USER_SESSIONS set disabled=1 where id=?");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setLong(1, sessionID);

			stmt.execute();
			conn.commit();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "disableSession", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;
	}

	private void setLastLoginTimeSession(long sessionID) throws Exception {
		PreparedStatement stmt = null;
		StringBuilder sql = new StringBuilder();
		sql.append("update USER_SESSIONS set last_login=? where id=? and disabled=0");


		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		try {
			stmt = conn.prepareStatement(sql.toString());
			stmt.setLong(1, HtDateTimeUtils.getCurGmtTime());
			stmt.setLong(2, sessionID);

			stmt.execute();
			conn.commit();

		} catch (SQLException sqe) {
			throw new HtException(getContext(), "setLastLoginTimeSession", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
		;
	}
}
