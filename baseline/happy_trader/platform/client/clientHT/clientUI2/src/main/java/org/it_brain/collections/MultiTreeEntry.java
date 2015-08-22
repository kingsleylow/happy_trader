package org.it_brain.collections;

import java.io.Serializable;
import java.util.*;

class MultiTreeEntry implements Map.Entry, Serializable {

	private static final long serialVersionUID = -2365242478907114661L;

	MultiMapKey ivKey;
	Object ivValue;
	MultiTreeEntry ivLeft = null;
	MultiTreeEntry ivRight = null;
	MultiTreeEntry ivParent;
	boolean ivColor;

	/**
	 * Make a new cell with given key, value, and parent, and with
	 * <tt>null</tt> child links, and BLACK color.
	 */
	MultiTreeEntry(MultiMapKey key, Object value, MultiTreeEntry parent) {
		ivKey = key;
		ivValue = value;
		ivParent = parent;
		boolean ivColor = MultiTreeMap.BLACK;
	}

	public Object getKey() {
		return ivKey;
	}

	public Object getValue() {
		return ivValue;
	}

	/**
	 * @return the old value.
	 */
	public Object setValue(Object value) {
		Object oldValue = ivValue;
		ivValue = value;
		return oldValue;
	}

	public boolean equals(Object o) {
		if (!(o instanceof Map.Entry)) {
			return false;
		}
		MultiTreeEntry e = (MultiTreeEntry)o;
		return MultiTreeMap.equalsObj(ivKey, e.getKey()) && MultiTreeMap.equalsObj(ivValue, e.getValue());
	}

	public int hashCode() {
		int keyHash = (ivKey == null ? 0 : ivKey.hashCode());
		int valueHash = (ivValue == null ? 0 : ivValue.hashCode());
		return keyHash ^ valueHash;
	}

// --- traversing through entries -------------------------
	/**
	 * Returns the successor of the specified Entry, or null if no such.
	 */
	MultiTreeEntry successor() {
		return relative(1);
	}

	/**
	 * Returns the predeccessor of the specified Entry, or null if no such.
	 */
	MultiTreeEntry predecessor() {
		return relative(-1);
	}

	/**
	 * Moves to another entry
	 * @param t Entry where the moving starts
	 * @param step number of steps to move (negative value moves to the left.)
	 */
	MultiTreeEntry relative(int step) {
		boolean bForward = (step >= 0);
		if (! bForward) step = -step;
		MultiTreeEntry entry, parent, child, checkEntry;
		entry = this;
		boolean down;
		while ((step != 0) && (entry != null)) {
			checkEntry = bForward ? entry.ivRight : entry.ivLeft;
			if (checkEntry != null) {
				while ((bForward ? checkEntry.ivLeft : checkEntry.ivRight) != null) {
					checkEntry = bForward ? checkEntry.ivLeft : checkEntry.ivRight;
				}
				entry = checkEntry;
				step--;
			} else {
				parent = entry.ivParent;
				child = entry;
				while ((parent != null) && (child == (bForward ? parent.ivRight : parent.ivLeft))) {
					child = parent;
					parent = parent.ivParent;
				}
				entry = parent;
				step--;
			}
		}
		return entry;
	}

	public String toString() {
		return ivKey.toString() + "=" + ivValue.toString();
	}

}

