/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout;

import static com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibout.MtDataPacketOut.COMMAND;

/**
 *
 * @author Victor_Zoubok
 */
public class HeartBeatOut  extends OutPacketBase{
		
		public static final String COMMAND = "HEARTBEAT";
		
		public HeartBeatOut()
		{
		}
		
		private String getContext()
		{
				return HeartBeatOut.class.getSimpleName();
		}
		
		public String getCommand()
		{
				return COMMAND;
		}
		
}
