/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest;

/**
 *
 * @author Victor_Zoubok
 */
public class Req_G_GET_ALL_METADATA extends AlgLibRequestBase {
		public static final String COMMAND = "G_GET_ALL_METADATA";
		
		public Req_G_GET_ALL_METADATA()
		{
			
		}
		
		@Override
		public String getMtCommand()
		{
				return COMMAND;
		}
		
}
