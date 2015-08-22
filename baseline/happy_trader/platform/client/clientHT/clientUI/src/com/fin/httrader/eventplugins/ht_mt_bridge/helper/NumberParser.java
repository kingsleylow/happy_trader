/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper;

import com.fin.httrader.eventplugins.ht_mt_bridge.helper.mt.Mtproxy_Data;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtUtils;
import java.math.BigDecimal;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;

/**
 *
 * @author Victor_Zoubok
 */
public class NumberParser {
		private DecimalFormatSymbols symbols_m = new DecimalFormatSymbols();
		DecimalFormat decimalFormat_m = new DecimalFormat("#.#", symbols_m);
		
		private String getContext()
		{
				return NumberParser.class.getSimpleName();
		}
		
		public NumberParser()
		{
				//symbols_m.setGroupingSeparator('');
				symbols_m.setDecimalSeparator('.');
				decimalFormat_m.setParseBigDecimal(true);
				
		}
		
		BigDecimal parse(String value) throws Exception
		{
				if (HtUtils.nvl(value))
						throw new HtException(getContext(), "parse", "Invalid value");
				
				return (BigDecimal) decimalFormat_m.parse(value);
		}
		
		
}
