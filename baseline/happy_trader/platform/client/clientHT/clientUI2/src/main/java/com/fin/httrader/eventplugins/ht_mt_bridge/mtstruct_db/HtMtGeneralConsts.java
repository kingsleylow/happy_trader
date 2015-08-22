/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtGeneralConsts {
		// as we store all values as long
		// we will multiply all these figures by 1000
		public static final int PRICE_GENERAL_MULTIPLIER = 10000;
		
		public static final int OP_BUY = 0;
		public static final int OP_SELL = 1;
		
		public static final int OP_BUYLIMIT = 2;
		public static final int OP_BUYSTOP = 4;
		
		public static final int OP_SELLLIMIT = 3;
		public static final int OP_SELLSTOP = 5;
		
		public static class FreeMargineMode 
		{
				//0 - floating profit/loss is not used for calculation;
				static final int FPL_NOT_USED = 0;
				//1 - both floating profit and loss on open positions on the current account are used for free margin calculation;
				static final int FPL_USED =1;
				//2 - only profit value is used for calculation, the current loss on open positions is not considered;
				static final int FP_USED =2;
				//3 - only loss value is used for calculation, the current loss on open positions is not considered. 
				static final int FL_USED = 3;
				
		};
		
		public static class StopOutMode 
		{
				//0 - %;
				static final int SM_PCT = 0;
				//1 - abs
				static final int SM_ABS = 1;
		
				
		};
		
}
