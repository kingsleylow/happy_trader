/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

import com.fin.httrader.utils.hlpstruct.HtDrawableObject;

/**
 *
 * @author DanilinS
 */
public interface HtDrawableObjectSelector
{
	public void onDrawableObjectArrived(HtDrawableObject dobj) throws Exception;

	public void onFinish(long cntr) throws Exception;

}
