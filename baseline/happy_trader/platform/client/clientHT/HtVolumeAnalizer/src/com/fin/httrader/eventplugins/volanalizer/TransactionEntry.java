/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.volanalizer;

/**
 *
 * @author DanilinS
 */
public class TransactionEntry {

	
		public String symbol_m = "";
		public int qty_m = -1;
		public double price_m = -1;
		public double time_m = -1;
		public int operation_m = -1;

		public TransactionEntry(String symbol,  int qty, double price, double ttime, int operation) throws Exception
		{
			symbol_m = symbol;
		
			qty_m = qty;
			price_m = price;
			time_m = ttime;
			operation_m = operation;

			
		}
	};
