/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct.externalrunstruct;

import com.fin.httrader.hlpstruct.*;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.TreeMap;

/**
 *
 * @author DanilinS
 * This is for external API
 */
public class RunClassEntry {
		public RunClassEntry(String className)
		{
			className_m.append(className);
		}

		public String getClassName()
		{
			return className_m.toString();
		}

		public Map<String, RunParameter> getParameters()
		{
			return parameters_m;
		}
		
		

		// class name
		private StringBuilder className_m = new StringBuilder();

		// parameters
		private LinkedHashMap<String, RunParameter> parameters_m = new LinkedHashMap<String, RunParameter>();

		
}
