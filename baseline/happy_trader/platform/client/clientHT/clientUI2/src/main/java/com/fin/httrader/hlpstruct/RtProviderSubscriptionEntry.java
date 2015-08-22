/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

/**
 *
 * @author DanilinS
 * This structure describes subscription for data profiles for RT Providers
 */
public class RtProviderSubscriptionEntry {
	public RtProviderSubscriptionEntry()
	{
	}

	public boolean subscribedRtHist_m = false;

	public boolean subscribeLevel1_m = false;

	public boolean subscribeDrawableObj_m = false;

	public int multFact_m = -1;

	public int timeScale_m = -1;

}
