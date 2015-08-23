/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

import java.util.List;

/**
 *
 * @author Victor_Zoubok
 */
public class RunParameter {
		
		public static final int DUMMY_TYPE = -1;
		public static final int STRING_TYPE = 1;
		public static final int STRING_LIST_TYPE = 2;
		public static final int STRING_MAP_TYPE = 3;
		
		public int getType()
		{
			return type_m;
		}
		
		
	  public <T> T getData() {
		  return (T)this;
	  }
		
		protected int type_m = DUMMY_TYPE;
}
