/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

/**
 *
 * @author DanilinS
 */
public class HtSettingsStruct {
	public String user = null;
		public String hashed_password = null;
		public int port = -1;
		public String host = null;
		public int log_history_depth_days_m = 1;
		public boolean isServletDebug_m = false;
		public int alertSignalLengthSec_m = 10;
		
		public boolean isValid() {
			if (host == null || port <= 0 || hashed_password == null || user == null) {
				return false;
			}

			return true;
		}

		public String toString()
		{
			return "user="+user+" hashed_password="+hashed_password+" port="+port + " host=" +host;
		}
}
