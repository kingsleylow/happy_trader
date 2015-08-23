/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package btce;

import java.util.HashMap;
import org.json.JSONObject;



/**
 *
 * @author Victor_Zoubok
 */
public class Btce {

		/**
		 * @param args the command line arguments
		 */
		public static void main(String[] args) {
				// TODO code application logic here
				sender2 s = new sender2();
				try {
						HashMap<String, String> args_r = new HashMap<String, String>();
						JSONObject result  =s.executeQuery("getInfo", args_r);
				}
				catch(Exception e)
				{
						e.printStackTrace();
				}
		}
		
}
