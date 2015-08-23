/*
 * DDEItem.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.win32;

/**
 *
 * @author Victor_Zoubok
 */
public class DDEItem {

	/** Creates a new instance of DDEItem */
	public DDEItem(String topic, String item) {
		topic_m.append(topic);
		item_m.append(item);
	}
	public StringBuilder topic_m = new StringBuilder();
	public StringBuilder item_m = new StringBuilder();

	public String getTopic() {
		return topic_m.toString();
	}

	public String getItem() {
		return item_m.toString();
	}
}