/*
 * HtExtCommandBase.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.externalapi;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtCommandBase {

	/** Creates a new instance of HtExtCommandBase */
	public HtExtCommandBase() {
	}
	// --------------------------------
	private HtExtBaseLogger logger_m = null;

	// -----------------------------
	public HtExtCommandBase(HtExtBaseLogger logger) {
		logger_m = logger;
	}

	// ------------------------------
	public HtExtBaseLogger getLogger() {
		return logger_m;
	}

	public static void commandRunWrapperNoError(HtExtBaseLogger logger, HtExtCommandResult result) throws HtExtException {


		if (result.getCode() != 0) {
			logger.addErrorEntry("Exception occured in command: " + result.getCaller() + " - " + result.getReason());
		} else {
			logger.addStringEntry(result.getStringResult());
		}
	}
	
	public static void commandRunWrapper(HtExtBaseLogger logger, HtExtCommandResult result) throws HtExtException {


		if (result.getCode() != 0) {
			throw new HtExtException("Exception occured in command: " + result.getCaller() + " - " + result.getReason());
		} else {
			logger.addStringEntry(result.getStringResult());
		}
	}
	
	public static void commandStringWrapper(HtExtBaseLogger logger, String result) throws HtExtException
	{
		StringBuilder out = new StringBuilder();

		out.append("( ");
		out.append(result);
		out.append(" ) - OK");

		logger.addStringEntry(out.toString());
	}

	
}
