/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

/**
 *
 * @author DanilinS
 */
public class HtDbUtils {

	public static void silentlyCloseConnection(Connection conn)
	{
		if (conn !=  null) {
			try {
				conn.close();
			}
			catch(SQLException e)
			{
			}
		}
	}

	public static void silentlyCloseStatement(Statement stmt)
	{
		if (stmt != null) {

			

			try {
				stmt.close();
			}
			catch(SQLException e)
			{
			}
		}
	}
}
