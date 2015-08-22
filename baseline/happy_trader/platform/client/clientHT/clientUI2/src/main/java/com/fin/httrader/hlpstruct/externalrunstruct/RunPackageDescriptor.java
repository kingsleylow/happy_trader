/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

import com.fin.httrader.utils.hlpstruct.HtPair;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class RunPackageDescriptor {
  
  
  public RunPackageDescriptor()
  {
  }
  
  public StringBuilder moduleVersion_m = new StringBuilder();
  public List<RunClassEntry> classEntries_m = new ArrayList<RunClassEntry>();
  public StringBuilder xmlDescriptor_m = new StringBuilder();
  // this is 
  public Map<String, String> runKeys_m = new LinkedHashMap<String, String>(); 
  
  
}
