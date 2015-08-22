/*
 * HtSysPowerEventListener.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.utils.win32;

import com.fin.httrader.utils.win32.DDEItem;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public interface HtSysPowerEventListener {
    public void DDEDataArrived(DDEItem item, String value);
    
    public void onDDEXLTableDataArrived(DDEItem item, Vector< Vector<String> > data);
}
