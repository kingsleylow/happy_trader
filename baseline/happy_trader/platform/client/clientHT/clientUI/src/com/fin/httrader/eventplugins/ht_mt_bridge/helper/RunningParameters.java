/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper;

import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

/**
 *
 * @author Victor_Zoubok
 */
public class RunningParameters {
		@HtSaveAnnotation
		private final AtomicBoolean proocessAccountUpdate_m = new AtomicBoolean(false);
		
		@HtSaveAnnotation
		private final AtomicBoolean proocessBalanceUpdate_m = new AtomicBoolean(false);
		
		@HtSaveAnnotation
		private final AtomicInteger maxDelayPollQueueAliveSec_m = new AtomicInteger(20);
		
		//@HtSaveAnnotation
		//private final AtomicInteger heartBeatIntervalToJavaHtSec_m = new AtomicInteger(6); // this is heart beat interval from C++ from alglib_metatrader
		

		/*
		public int getHeartBeatIntervalToJavaHtSec() {
				return heartBeatIntervalToJavaHtSec_m.get();
		}

		public void setHeartBeatIntervalToJavaHtSec(int heartBeatIntervalToJavaHtSec) {
				this.heartBeatIntervalToJavaHtSec_m.set(heartBeatIntervalToJavaHtSec);
		}
		*/
		
		public boolean getProcessAccountUpdate() {
				return proocessAccountUpdate_m.get();
		}

		public void setProcessAccountUpdate(boolean proocessAccountUpdate) {
				this.proocessAccountUpdate_m.set(proocessAccountUpdate);
		}

		public boolean getProcessBalanceUpdate() {
				return proocessBalanceUpdate_m.get();
		}

		public void setProcessBalanceUpdate(boolean proocessBalanceUpdate) {
				this.proocessBalanceUpdate_m.set(proocessBalanceUpdate);
		}

		public int getMaxDelayPollQueueAliveSec() {
				return maxDelayPollQueueAliveSec_m.get();
		}

		public void setMaxDelayPollQueueAliveSec(int maxDelayPollQueueAliveSec) {
				this.maxDelayPollQueueAliveSec_m.set(maxDelayPollQueueAliveSec);
		}
		
		public void initFromParamMap(Map<String, String> initdata) throws Exception
		{
				setProcessAccountUpdate(	HtUtils.parseBoolean( initdata.get("PROCESS_ACCOUNT_UPDATE") ) );
				setProcessBalanceUpdate(	HtUtils.parseBoolean( initdata.get("PROCESS_BALANCE_UPDATE") ) );
				
				int md = HtUtils.parseInt( initdata.get("MAX_DELAY_POLL_QUEUE_ALIVE_SEC") );
				if (md > 0)
						setMaxDelayPollQueueAliveSec(	md );
				
				/*
				int hd = HtUtils.parseInt( initdata.get("HEARTBEAT_INTERVAL_TO_JAVAHT_SEC") );
				if (hd > 0)
						setHeartBeatIntervalToJavaHtSec(	hd );
				*/
		}
		
		@Override
		public String toString()
		{
				try {
						XmlParameter param = new XmlParameter();
						HtXmlParameterSerialization.serializeObjectToXmlParameter(this, param);
						return XmlHandler.serializeParameterStatic_simpleVer(param);
				}
				catch(Throwable e)
				{
						
				}
				return "N/A";
				
		}
		
		
		
}
