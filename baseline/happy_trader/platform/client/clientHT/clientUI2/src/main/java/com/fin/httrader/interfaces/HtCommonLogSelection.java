/*

 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.utils.hlpstruct.CommonLog;

/**
 *
 * @author victor_zoubok
 */
public interface HtCommonLogSelection {

	public void newRowArrived(CommonLog commonLogRow) throws Exception;

	public void onFinish(long cntr) throws Exception;
}
