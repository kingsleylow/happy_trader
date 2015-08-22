/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtStartupConstants;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtSecurityProxy;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.HtUtils;
import java.io.BufferedReader;
import java.io.Console;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 * This is code responsible for processing commands from keyboard
 */
public class RtConsoleManager implements RtManagerBase {

	
	// ------------------------------------------
	static private RtConsoleManager instance_m = null;

	static public RtConsoleManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtConsoleManager();
		}

		return instance_m;
	}
	

	static public RtConsoleManager instance() {
		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	private RtConsoleManager() throws Exception {

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	@Override
	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// -----------------------------------------
	public void consoleLoop() {
		consolePrintLine("Enter command (type \"help\" for the list of available commands):");
		StringBuilder command = new StringBuilder();
		List<String> arguments = new ArrayList<String>();


		while (true) {
			consolePrintLine(HtDateTimeUtils.time2SimpleString_Local(HtDateTimeUtils.getCurGmtTime())  + " > ");

			// keyboard input
			String token = getKbdString();
			tokenizeCommand(command, arguments, token);


			if (command.length() > 0) {
				consolePrintLine("Executing the command: " + command + "\n");
				try {

					if (command.toString().equalsIgnoreCase("help")) {
						consolePrintLine("***Happy Trader Server [ build:" + HtStartupConstants.VERSION + " ] ***");
						consolePrintLine("q: quit");
						//consolePrintLine("recreate_tables: this recreates common tables");
						consolePrintLine("[jdbc:mysql://localhost:3306/htrader?user=xxx&password=xxx&characterEncoding=UTF-8 INFO INFO recreate_tables] - the last parameter to recretate all tables");
						consolePrintLine("create_user <user name>: register new user");
						consolePrintLine("change_user_password <user name>: change a registered user password");
						consolePrintLine("enable_user <user name>: enable a registered user");
						consolePrintLine("disable_user <user name>: disable a registered user");
						consolePrintLine("list_user: list of users");
						consolePrintLine("help: list of commands");
						consolePrintLine("****************************************************");
					} else if (command.toString().equalsIgnoreCase("q")) {
						break;
					} else if (command.toString().equalsIgnoreCase("change_user_password")) {
						String user_name = arguments.get(0);

						consolePrintLine("Enter passord for the existing user: " + user_name);

						String password = getKbdStringHidden();


						if (password.length() == 0) {
							throw new HtException(getContext(), "consoleLoop", "Invalid password for new user");
						} else {
							consolePrintLine("The password was accepted");
						}


						HtCommandProcessor.instance().get_HtSecurityProxy().remote_updateUserPassword(user_name, password);

					} else if (command.toString().equalsIgnoreCase("create_user")) {
						String user_name = arguments.get(0);
						consolePrintLine("Enter passord for the existing user: " + user_name);

						String password = getKbdStringHidden();


						if (password.length() == 0) {
							throw new HtException(getContext(), "consoleLoop", "Invalid password for new user");
						}


						HtCommandProcessor.instance().get_HtSecurityProxy().remote_registerNewUser(user_name, password);

					} else if (command.toString().equalsIgnoreCase("enable_user")) {
						String user_name = arguments.get(0);
						HtCommandProcessor.instance().get_HtSecurityProxy().remote_enableUser(user_name);

					} else if (command.toString().equalsIgnoreCase("disable_user")) {
						String user_name = arguments.get(0);
						HtCommandProcessor.instance().get_HtSecurityProxy().remote_disableUser(user_name);
					} else if (command.toString().equalsIgnoreCase("list_user")) {
						List<RtSecurityManager.UserStructure> users = HtCommandProcessor.instance().get_HtSecurityProxy().remote_queryAvailableUsers();
						for (int i = 0; i < users.size(); i++) {
							if (users.get(i).isEnabled_m) {
								System.out.print("enabled - \"");
							} else {
								System.out.print("disabled - \"");
							}

							System.out.print(users.get(i).userName_m);
							consolePrintLine("\"");
						}
					}
					//else if (command.toString().equalsIgnoreCase("recreate_tables")) {
					//	HtCommandProcessor.instance().get_HtDatabaseProxy().remote_recreateGlobalTables();
					//}
					else if(command.toString().equalsIgnoreCase("show_threads")) {
						List<Thread> descrlist = new ArrayList<Thread>();
						HtSystem.getAllRunningThreads(descrlist);
						for(int i = 0; i < descrlist.size(); i++) {
							System.out.println(descrlist.get(i).toString());
						}
					}

					else {
						throw new HtException(getContext(), "consoleLoop", "Invalid command: " + token);
					}

				} catch (Throwable e) {
					consolePrintLine("Exception: " + e.getMessage());
				}

				consolePrintLine("\nCommand processed\n");

			} // end of token.length() > 0

		} // end of loop
	}

	private String getKbdStringHidden() {

		StringBuilder result = new StringBuilder();

		String s = null;
		try {

			BufferedReader br = new BufferedReader(new InputStreamReader(System.in, "Cp866"));
			s = br.readLine();
			//String s = getConsole().readLine();
			result.append(s);


		} catch (Exception ex) {
			consolePrintLine(ex.toString());

			return "";
		}


		return s.trim();
	}

	private String getKbdString() {

		StringBuilder result = new StringBuilder();

		String s;
		try {

			BufferedReader br = new BufferedReader(new InputStreamReader(System.in, "Cp866"));
			s = br.readLine();
			//String s = getConsole().readLine();
			result.append(s);


		} catch (Exception ex) {
			consolePrintLine(ex.toString());

			return "";
		}


		return s.trim();
	}

	private void tokenizeCommand(StringBuilder command, List<String> arguments, String token) {
		arguments.clear();
		command.setLength(0);

		String[] splitted = token.split(" ");
		if (splitted != null) {
			for (int i = 0; i < splitted.length; i++) {
				if (i == 0) {
					command.append(splitted[0] != null ? splitted[0] : "");
					continue;
				}
				arguments.add(splitted[i]);
			}
		}
	}

	private void consolePrintLine(String line) {

		//getConsole().writer().println(line);
		System.out.println(line);

	}

	/*
	private Console getConsole()
	{
	Console cons = System.console();
	return cons;
	}
	 */
}
