/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSmartComSynthetic implements HtEventPlugin {
  public static String getContext() {
	return HtSmartComLoader.class.getSimpleName();
  }

  @Override
  public Integer[] returnSubscribtionEventTypes() {
	throw new UnsupportedOperationException("Not supported yet.");
  }

  @Override
  public void initialize(Map<String, String> initdata) throws Exception {
	throw new UnsupportedOperationException("Not supported yet.");
  }

  @Override
  public void deinitialize() throws Exception {
	throw new UnsupportedOperationException("Not supported yet.");
  }

  @Override
  public String getEventPluginId() {
	throw new UnsupportedOperationException("Not supported yet.");
  }

  @Override
  public void setEventPluginId(String pluginId) {
	throw new UnsupportedOperationException("Not supported yet.");
  }

  @Override
  public void execute(XmlEvent alertData) throws Exception {
	throw new UnsupportedOperationException("Not supported yet.");
  }
}
