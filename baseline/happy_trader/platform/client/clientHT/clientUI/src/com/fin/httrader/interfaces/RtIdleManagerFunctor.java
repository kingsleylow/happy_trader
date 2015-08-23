/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.interfaces;

/**
 *
 * @author DanilinS
 * Idle manager functor
 */
public abstract class RtIdleManagerFunctor {

	private final StringBuilder name_m = new StringBuilder();

	private int executeCounter_m = 1;

	private int executeNumber_m = 1;

	public RtIdleManagerFunctor(String name, int executeCounter)
	{
		name_m.append( name );
		if (executeCounter > 0) {
			executeCounter_m = executeCounter;
			executeNumber_m = executeCounter;
		}
	}

	public String getFunctorName()
	{
		return name_m.toString();
	}

	public boolean checkIfExecuting()
	{
		if (--executeCounter_m <= 0) {
			executeCounter_m  = executeNumber_m;
			return true;
		}
		else {
			return false;
		}
	}

	

	// to be implemented
	public abstract void execute() throws Exception;
}
