/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibrequest;

/**
 *
 * @author Administrator
 */
public class Req_G_QUERY_AUTOLOAD_USERS extends AlgLibRequestBase {
		public static final String COMMAND = "G_QUERY_AUTOLOAD_USERS";
		
		public Req_G_QUERY_AUTOLOAD_USERS()
		{
			
		}
		
		@Override
		public String getMtCommand()
		{
				return COMMAND;
		}
}
