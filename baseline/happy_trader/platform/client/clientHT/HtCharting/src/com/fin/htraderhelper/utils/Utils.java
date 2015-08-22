/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper.utils;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

/**
 *
 * @author DanilinS
 */
public class Utils {

	

	public static String formatDateTime(long ttime) {
		return (new SimpleDateFormat("dd-MM-yyyy'T'HH:mm:ss.0")).format(new Date(ttime));
	}

	public static String createTextFromAllTextObjects(List<HtDrawableObject.Text> text_obj_list, long ttime) {
		StringBuilder out = new StringBuilder();

		for (int i = 0; i < text_obj_list.size(); i++) {
			HtDrawableObject.Text text_i = text_obj_list.get(i);

			out.append("PRIORITY: [ ");
			out.append(AlertPriorities.getAlertpriorityByName(text_i.priority_m));
			out.append(" ] - [ ");
			out.append(Utils.formatDateTime(ttime));
			out.append(" ] - SHORT TEXT: ( ");
			out.append(text_i.shortText_m);
			out.append(" )\n");
			out.append("LARGE TEXT:\n");
			out.append(text_i.text_m);
			out.append("\n\n");
		}

		return out.toString();
	}
}
