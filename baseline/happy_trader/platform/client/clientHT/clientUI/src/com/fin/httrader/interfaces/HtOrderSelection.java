/*
 * HtOrderSelection.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.hlpstruct.Order;

/**
 *
 * @author Victor_Zoubok
 */
public interface HtOrderSelection {

	public class OrderRow {

		public String orderSource;
		public Order order;
	}

	public void onSelectionFinish() throws Exception;

	public void newRowArrived(OrderRow orderRow) throws Exception;
} 
