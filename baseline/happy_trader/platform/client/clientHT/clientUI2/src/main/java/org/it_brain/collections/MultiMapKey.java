package org.it_brain.collections;

import java.io.*;
import java.util.*;

//import org.it_brain.util.*;

/**
 * This class is used as key wrapper for maps that allow duplicate key entries.
 */
class MultiMapKey implements Comparable, Serializable, Cloneable {

	Object ivKey;
	int ivIndex;

	MultiMapKey(Object key, int index) {
		ivKey = key;
		ivIndex = index;
	}

	public int compareTo(Object obj) {
		MultiMapKey cmpObj = (MultiMapKey)obj;
		int retVal = 0;
		if (ivKey == null) {
			retVal = cmpObj.ivKey == null ? 0 : -1;
		} else {
			retVal = ((Comparable)ivKey).compareTo(cmpObj.ivKey);
		}
		if (retVal != 0) return retVal;
		return ivIndex - cmpObj.ivIndex;
	}

	public int hashCode() {
		return ivKey.hashCode();
	}

	protected Object clone() throws CloneNotSupportedException {
		return super.clone();
	}

	public boolean equals(Object obj) {
		MultiMapKey cmpObj = (MultiMapKey)obj;
		if (! MultiTreeMap.equalsObj(ivKey, cmpObj.ivKey)) return false;
		return ivIndex == cmpObj.ivIndex;
	}

	public String toString() {
		return ivKey.toString() + ":" + ivIndex;
	}

}
