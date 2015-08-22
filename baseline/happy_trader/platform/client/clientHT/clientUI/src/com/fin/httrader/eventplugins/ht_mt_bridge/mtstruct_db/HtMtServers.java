/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtServers {
		private long serversMetanfoUid = -1; // PK
		
		private long accountMetanfoUid = -1; // ref to account metainfo
		private String serverName;

		/**
		 * @return the serversMetanfoUid
		 */
		public long getServersMetanfoUid() {
				return serversMetanfoUid;
		}

		/**
		 * @param serversMetanfoUid the serversMetanfoUid to set
		 */
		public void setServersMetanfoUid(long serversMetanfoUid) {
				this.serversMetanfoUid = serversMetanfoUid;
		}

		/**
		 * @return the accountMetanfoUid
		 */
		public long getAccountMetanfoUid() {
				return accountMetanfoUid;
		}

		/**
		 * @param accountMetanfoUid the accountMetanfoUid to set
		 */
		public void setAccountMetanfoUid(long accountMetanfoUid) {
				this.accountMetanfoUid = accountMetanfoUid;
		}

		/**
		 * @return the serverName
		 */
		public String getServerName() {
				return serverName;
		}

		/**
		 * @param serverName the serverName to set
		 */
		public void setServerName(String serverName) {
				this.serverName = serverName;
		}
}
