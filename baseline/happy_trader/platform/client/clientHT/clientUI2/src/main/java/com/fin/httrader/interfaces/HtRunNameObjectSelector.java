/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.utils.hlpstruct.HtRunNameEntry;



/**
 *
 * @author Victor_Zoubok
 */
public interface HtRunNameObjectSelector {
	
	public void onRunNameEntryArrived(HtRunNameEntry rentry) throws Exception;

	public void onFinish(long cntr) throws Exception;
}
