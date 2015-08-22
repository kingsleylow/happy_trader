/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.model;

import com.fin.httrader.interfaces.HtLongPollManagerEventFilter;
import com.fin.httrader.managers.RtLongPollManager;
import com.fin.httrader.utils.hlpstruct.XmlEvent;

/**
 *
 * @author Victor_Zoubok
 */
public class HtLongPollProxy extends HtProxyBase {

		
		public String getContext() {
				return this.getClass().getSimpleName();
		}
		
		@Override
		public String getProxyName() {
				return "long_poll_proxy";
		}
		
		public boolean remote_isQueueValid(int cookie)
		{
				return RtLongPollManager.instance().isQueueValid(cookie);
		}
		
		public int remote_getQueuePendingSize(int cookie) throws Exception
		{
				return RtLongPollManager.instance().getQueuePendingSize(cookie);
		}
			
		public XmlEvent remote_popQueueElement(int cookie, int timeout_msec) throws Exception
		{
				return RtLongPollManager.instance().popQueueElement(cookie, timeout_msec);
		}
			
		public void remote_pushQueueElement(XmlEvent xml_event)
		{
				RtLongPollManager.instance().pushQueueElement(xml_event);
		}
			
		public void remote_initEventQueue
		(
			int cookie, 
			int event_type_to_subscribe, 
			int max_delay_afeter_queue_access_sec,
			HtLongPollManagerEventFilter filter
		) throws Exception
		{
				RtLongPollManager.instance().initEventQueue( cookie,event_type_to_subscribe,max_delay_afeter_queue_access_sec, filter);
		}
			
		public void remote_removeEventQueue(int cookie) throws Exception 
		{
				RtLongPollManager.instance().removeEventQueue(cookie);
		}
		
		
}
