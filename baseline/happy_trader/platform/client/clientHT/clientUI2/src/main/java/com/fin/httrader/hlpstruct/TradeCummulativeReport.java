/*
 * TradeCummulativeReport.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

// this is intended to hold cummulative report for a period 
package com.fin.httrader.hlpstruct;

/**
 *
 * @author Victor_Zoubok
 */
public class TradeCummulativeReport {
    
    public long periodBegin_m = -1;
    
    public long periodEnd_m = 1;
    
    // the number of trades per period
    public int periodTradesNumber_m = -1;
    
    // the number of trades closed by SL
    public int periodTradesSL_ExitReason_m = -1;
    
    // the number of trades closed by TP
    public int periodTradesTP_ExitReason_m = -1;
    
    // the number of trades closed by other (expiration or normal close)
    public int periodTradesOther_ExitReason_m = -1;
    
    public double periodProfit_m = -1;
    
    public double cummulativeProfit_m = -1;
    
    public int periodProfitTradesNum_m = -1;
    
    public int periodLossTradesNum_m = -1;
    
    // equity after-equity before/equity before
    public double periodEquityChange_m = -1;
    
 
    //
    
    public StringBuilder strPeriodBegin_m = new StringBuilder();
    
    public StringBuilder strPeriodEnd_m = new StringBuilder();
    
};
