/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import java.util.Collection;

/**
 *
 * @author Victor_Zoubok
 */
public interface SingleReaderQueueInterface {
		public int getQueueSize();
		public String getName();
		public void get(Collection list, int msecs);
		public void get(Collection list, int maxsize, int msecs);
		public Object get( int msecs);
		public void get(Collection list);
		public void getNoWait(Collection list);
		public void releaseMonitoring();
		public void put(Object o) throws SingleReaderQueueExceedLimit;
		public void shutdown();
		
}
