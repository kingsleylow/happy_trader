/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper.alglibresponse;

/**
 *
 * @author Victor_Zoubok
 */
public class Resp_START_ORDER_LIST extends AlgLibResponseBase {
		
		public Resp_START_ORDER_LIST() {
		}
		
		private String getContext()
		{
				return this.getClass().getSimpleName();
		}
}
