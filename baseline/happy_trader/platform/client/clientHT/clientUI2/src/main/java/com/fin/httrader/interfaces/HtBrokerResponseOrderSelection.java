/*
 * HtBrokerResponseOrderSelection.java
 *
 
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.hlpstruct.BrokerResponseOrder;

/**
 *
 * @author Victor_Zoubok
 */
public interface HtBrokerResponseOrderSelection {

	public class BrokerResponseRow {

		public String brokerSource;
		public BrokerResponseOrder order;
	};

	public void onSelectionFinish() throws Exception;

	public void newRowArrived(BrokerResponseRow brokerRow) throws Exception;
}
