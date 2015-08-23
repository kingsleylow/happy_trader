/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.hlpstruct;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

/**
 *
 * @author Victor_Zoubok
 * this is to describe CPP server running thread
 */
public class ThreadDescriptor {
		public Set<String> loadedAlgBrkPairs_m = new TreeSet<String>();
		public int tid_m = -1;
		public int inputQueuePendingSize_m = 0;
		public int tstatus_m = -1;
}
