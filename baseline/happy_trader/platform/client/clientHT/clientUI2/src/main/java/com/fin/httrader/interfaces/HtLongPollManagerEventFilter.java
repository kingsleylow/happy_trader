/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import com.fin.httrader.utils.hlpstruct.XmlEvent;

/**
 *
 * @author Victor_Zoubok
 * used in long RtLongPollManager to pass filter to filter out events
 */
public interface HtLongPollManagerEventFilter {
		
		public boolean match(XmlEvent xml_event);
		
}
