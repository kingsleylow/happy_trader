/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.configprops.helper;

/**
 *
 * @author Victor_Zoubok
 */
public class PluginPageDescriptor {
  public PluginPageDescriptor()
  {
  }
  
  public PluginPageDescriptor(int id, String url, String description)
  {
	this.description = description;
	this.id = id;
	this.url = url;
  }
  
  public String description = "";
  public String url = "";
  public int id = -1;
}
