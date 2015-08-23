/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.interfaces;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class BackgroundJobFunctor
{
		
	public abstract void execute(BackgroundJobStatus status) throws Exception;
		
};
