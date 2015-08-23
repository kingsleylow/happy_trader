/*
 * HtLastDataRow.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.utils.hlpstruct;



/**
 *
 * @author Administrator
 */
public class HtLastDataRow
{
  public HtLastDataRow(int tid, HtRtData rtdata, int aflag )
  {
    this.tid = tid;
    this.rtdata = rtdata;
    this.aflag = aflag;
		
  }
  
  public long tid = -1;
  public HtRtData rtdata = null;
  public int aflag = -1;
	
	

}
