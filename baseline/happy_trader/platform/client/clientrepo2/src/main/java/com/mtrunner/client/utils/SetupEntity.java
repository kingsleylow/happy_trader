/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.utils;

import java.util.ArrayList;
import java.util.List;


public class SetupEntity {
	
	public static final int heartbeatjavahtsec_DEF = 7;
	
	public static final int mt4loglevel_DEF = 4;
	
	public static final int mt4datareadintrvalsec_DEF = 3;
	
	public static final boolean debugconsole_DEF = true;
	
	public static final int htserverport_DEF = 5573;
	
	public static final int htserverinternalloglevel_DEF = 5;
	
	public static final int htserverinternaleventlevel_DEF = 6;
	
	public static final int zombiemt4killersec_DEF = 3;
	
	public static final int MAX_DELAY_POLL_QUEUE_ALIVE_SEC_DEF = 25;
	
	public static final boolean PROCESS_BALANCE_UPDATE_DEF = true;
	
	public static final boolean PROCESS_ACCOUNT_UPDATE_DEF = false;
	
	
	//
	private String otherData = "{}";
	
	private int heartbeatjavahtsec = heartbeatjavahtsec_DEF;
	
	private int mt4loglevel = mt4loglevel_DEF;
	
	private int mt4datareadintrvalsec = mt4datareadintrvalsec_DEF;
	
	private boolean debugconsole = debugconsole_DEF;
	
	private int htserverport = htserverport_DEF;
	
	private int htserverinternalloglevel = htserverinternalloglevel_DEF;
	
	private int htserverinternaleventlevel = htserverinternaleventlevel_DEF;
	
	private int zombiemt4killersec = zombiemt4killersec_DEF;
	
	private int MAX_DELAY_POLL_QUEUE_ALIVE_SEC = MAX_DELAY_POLL_QUEUE_ALIVE_SEC_DEF;
	
	private boolean PROCESS_BALANCE_UPDATE = PROCESS_BALANCE_UPDATE_DEF;
	
	private boolean PROCESS_ACCOUNT_UPDATE = PROCESS_ACCOUNT_UPDATE_DEF;

	public int getHeartbeatjavahtsec() {
		return heartbeatjavahtsec;
	}

	public void setHeartbeatjavahtsec(int heartbeatjavahtsec) {
		this.heartbeatjavahtsec = heartbeatjavahtsec;
	}

	public int getMt4loglevel() {
		return mt4loglevel;
	}

	public void setMt4loglevel(int mt4loglevel) {
		this.mt4loglevel = mt4loglevel;
	}

	public int getMt4datareadintrvalsec() {
		return mt4datareadintrvalsec;
	}

	public void setMt4datareadintrvalsec(int mt4datareadintrvalsec) {
		this.mt4datareadintrvalsec = mt4datareadintrvalsec;
	}

	public boolean isDebugconsole() {
		return debugconsole;
	}

	public void setDebugconsole(boolean debugconsole) {
		this.debugconsole = debugconsole;
	}

	public int getHtserverport() {
		return htserverport;
	}

	public void setHtserverport(int htserverport) {
		this.htserverport = htserverport;
	}

	public int getHtserverinternalloglevel() {
		return htserverinternalloglevel;
	}

	public void setHtserverinternalloglevel(int htserverinternalloglevel) {
		this.htserverinternalloglevel = htserverinternalloglevel;
	}

	public int getHtserverinternaleventlevel() {
		return htserverinternaleventlevel;
	}

	public void setHtserverinternaleventlevel(int htserverinternaleventlevel) {
		this.htserverinternaleventlevel = htserverinternaleventlevel;
	}

	public int getMAX_DELAY_POLL_QUEUE_ALIVE_SEC() {
		return MAX_DELAY_POLL_QUEUE_ALIVE_SEC;
	}

	public void setMAX_DELAY_POLL_QUEUE_ALIVE_SEC(int MAX_DELAY_POLL_QUEUE_ALIVE_SEC) {
		this.MAX_DELAY_POLL_QUEUE_ALIVE_SEC = MAX_DELAY_POLL_QUEUE_ALIVE_SEC;
	}

	public String getOtherData() {
		return otherData;
	}

	public void setOtherData(String otherData) {
		this.otherData = otherData;
	}

	public int getZombiemt4killersec() {
		return zombiemt4killersec;
	}

	public void setZombiemt4killersec(int zombiemt4killersec) {
		this.zombiemt4killersec = zombiemt4killersec;
	}

	public boolean isPROCESS_BALANCE_UPDATE() {
		return PROCESS_BALANCE_UPDATE;
	}

	public void setPROCESS_BALANCE_UPDATE(boolean PROCESS_BALANCE_UPDATE) {
		this.PROCESS_BALANCE_UPDATE = PROCESS_BALANCE_UPDATE;
	}

	public boolean isPROCESS_ACCOUNT_UPDATE() {
		return PROCESS_ACCOUNT_UPDATE;
	}

	public void setPROCESS_ACCOUNT_UPDATE(boolean PROCESS_ACCOUNT_UPDATE) {
		this.PROCESS_ACCOUNT_UPDATE = PROCESS_ACCOUNT_UPDATE;
	}

	
	
	
}
