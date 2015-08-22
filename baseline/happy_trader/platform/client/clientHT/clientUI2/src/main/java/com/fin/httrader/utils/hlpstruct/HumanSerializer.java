/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils.hlpstruct;

import java.util.*;

/**
 *
 * @author Victor_Zoubok
 */
public class HumanSerializer {
	public static void serializeXmlParameter(XmlParameter  xmlparameter, StringBuilder content)
	{
		content.setLength(0);
		Set<String> keys = xmlparameter.getKeys();
		for(Iterator<String> it = keys.iterator(); it.hasNext(); ) {
			String key_i = it.next();

			if (xmlparameter.getType(key_i) == XmlParameter.VT_String) {
				String param_i = xmlparameter.getString(key_i);
				content.append(key_i).append(":").append(param_i).append("\r\n");
			}

		}
	}

	public static void deserializeXmlParameter(String content, XmlParameter xmlparameter)
	{
		xmlparameter.clear();
		String content_s = content.replaceAll("\r\n", "\n");

		String[] lines = content_s.split("\n");
		if (lines != null) {
			for(int i = 0; i < lines.length; i++) {
				String tokens[] = lines[i].split(":");
				
				if (tokens != null && tokens.length == 2) {
					if (tokens[0] != null && tokens[0].length() > 0) {
						xmlparameter.setString(tokens[0], tokens[1] != null ? tokens[1]: "" );
					}
					
					
				}

			}
		}
	}
	
}
