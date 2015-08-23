package org.it_brain.collections;

import java.io.Serializable;
import java.util.*;

/**
 * Based on the TreeMap, with one big difference: Duplicate key values are supported.
 * <P>Differences in detail:
 * <UL>
 *   <LI> The put() method do not replace an evantually existing value.
 *   <LI> The get() method gives the first entry with the given key.
 *   <LI> The get(Object key, int index) method allowes to access the n-th
 *        entry with the given key.
 *   <LI> The remove() method removes the first entry with a given key.
 *   <LI> An iterator() method is implemented that returns an Enhanced iterator.
 *        <BR> The objects returned by the iterator are MapEntry instances.
 * <LI>
 * <P>
 * Implementation detail:
 * Duplicate keys are given an index internally starting at zero.
 * That way two keys can never be the same for the internal methods.
 */

public class MultiTreeMap implements IMultiSortedMap, Cloneable, Serializable {

	private static final long serialVersionUID = 7107866968963352653L;

	static final boolean RED = false;
	static final boolean BLACK = true;

	transient volatile Set ivKeySet;
	transient volatile Collection ivValues;

	private Comparator ivComparator;

	private transient MultiTreeEntry ivRoot;

	private transient int ivSize;

	/**
	 * The number of structural modifications to the tree.
	 * Neccassery for checking modifications to a MultiTreeMap instance during
	 * accessing it through an iterator.
	 */
	private transient int ivModCount;

	/**
	 * Constructs a new, empty map, sorted according to the keys' natural
	 * order.
	 */
	public MultiTreeMap() {
	}

	/**
	 * Constructs a new, empty map, sorted according to the given comparator.
	 */
	public MultiTreeMap(Comparator comparator) {
		ivComparator = comparator;
	}

	/**
	 * Constructs a new map containing the same mappings as the given map,
	 * sorted according to the keys' natural order.
	 */
	public MultiTreeMap(Map map) {
		putAll(map);
	}

	/**
	 * Constructs a new map containing the same mappings as the given
	 * <tt>SortedMap</tt>, sorted according to the same ordering.
	 */
	public MultiTreeMap(SortedMap map) {
		ivComparator = map.comparator();
		try {
			buildFromSorted(map.size(), map.entrySet().iterator(), null, null);
		} catch (java.io.IOException ex) {
			// Cannot happen
			throw new RuntimeException("Impossible exception occured!", ex);
		} catch (ClassNotFoundException ex) {
			// Cannot happen
			throw new RuntimeException("Impossible exception occured!", ex);
		}
	}

// --- Cloning --------------------------------------------
	/**
	 * Returns a shallow copy of this map instance.
	 * (The keys and values themselves are not cloned.)
	 */
	public Object clone() {
		MultiTreeMap clone = null;
		try {
			clone = (MultiTreeMap)super.clone();
		} catch (CloneNotSupportedException e) {
			throw new InternalError();
		}

		clone.ivRoot = null;
		clone.ivSize = 0;
		clone.ivModCount = 0;
		clone.ivEntrySet = null;

		// Initialize clone with our mappings
		try {
			clone.buildFromSorted(ivSize, entrySet().iterator(), null, null);
		} catch (java.io.IOException ex) {
			// Cannot happen
			throw new RuntimeException("Impossible exception occured!", ex);
		} catch (ClassNotFoundException ex) {
			// Cannot happen
			throw new RuntimeException("Impossible exception occured!", ex);
		}
		return clone;
	}

// --- getter setter methods ---
	public boolean isEmpty() {
		return ivSize == 0;
	}

	public int size() {
		return ivSize;
	}

	public Comparator comparator() {
		return ivComparator;
	}


// --- handling of entries (interface methods) ------------
	public boolean containsKey(Object key) {
		return containsKey(key, 0);
	}

	/**
	 *
	 * @return True (This method makes only sense for calling on a SubMap.
	 */
	public boolean isKeyInRange(Object key) {
		return true;
	}

	public boolean containsKey(Object key, int index) {
		MultiTreeEntry entry = getCeilEntry(new MultiMapKey(key, 0));
		entry = entry.relative(index);
		return ((entry != null) && (equalsObj(entry.ivKey.ivKey, key)));
	}

	public boolean containsValue(Object value) {
		return (ivRoot == null ? false :
						(value == null ? valueSearchNull(ivRoot)
						 : valueSearchNonNull(ivRoot, value)));
	}

	public Object get(Object key) {
		return get(key, 0);
	}

	public Object get(Object key, int index) {
		MultiTreeEntry entry = getEntry(new MultiMapKey(key, index));
		return (entry == null ? null : entry.ivValue);
	}

	public Object firstKey() {
		return key(firstEntry());
	}

	public Object lastKey() {
		return key(lastEntry());
	}

	/**
	 * Associates the specified value with the specified key in this map.
	 * Duplicate keys are stored in the order of entrance within this TreeMap.
	 * @return null.
	 */
	public Object put(Object key, Object value) {
		MultiMapKey multiKey = new MultiMapKey(key, Integer.MAX_VALUE - 1);
		MultiTreeEntry entry = getCeilEntry(multiKey);
		if ((entry != null) && entry.ivKey.equals(multiKey)) {
			// An entry with the given key and the highest possible index value has been found.
			// @todo Search first free index position for the given key
			// No more entries are possible for this key.
			throw new	IllegalArgumentException("to many duplicate keys \"" + key.toString() + "\"!");
		}
		entry = (entry == null) ? entry = lastEntry() : entry.predecessor();
		int newIndex = 0;
		if (entry == null) {
			entry	= ivRoot;
			multiKey = new MultiMapKey(key, 0);
		} else {
			if (equalsObj(entry.ivKey.ivKey, key)) {
				newIndex = entry.ivKey.ivIndex + 1;
				multiKey = new MultiMapKey(key, newIndex);
			} else {
				multiKey = new MultiMapKey(key, 0);
			}
		}
		doPut(entry, multiKey, value);
		return null;
	}

	/**
	 * Puts a value under the given key with the specified index.
	 * If an index different to zero is given, the value has to exist.
	 * Negative index values are not allowed.
	 * @return The old value stored under the given key with the specified index.
	 */
	public Object put(Object key, Object value, int index) {
		if (index < 0) throw new IllegalArgumentException("Index must be positive!");
		MultiTreeEntry entry = null;
		if ((entry = getEntry(key, index)) == null) {
			throw	new NoSuchElementException("No entry found for \"" + key + ":" + index + "\"!");
		}
		Object oldValue = entry.ivValue;
		entry.ivValue = value;
		return oldValue;
	}

	/**
	 * Replaces all entries with the ones given in the parameter map.
	 */
	public void putAll(Map map) {
		int mapSize = map.size();
		if (ivSize == 0 && mapSize != 0 && map instanceof SortedMap) {
			Comparator c = ((SortedMap)map).comparator();
			if (c == ivComparator || (c != null && c.equals(ivComparator))) {
				++ivModCount;
				try {
					buildFromSorted(mapSize, map.entrySet().iterator(),
													null, null);
				} catch (java.io.IOException cannotHappen) {
				} catch (ClassNotFoundException cannotHappen) {
				}
				return;
			}
		}
		Iterator it = map.entrySet().iterator();
		while (it.hasNext()) {
			MultiTreeEntry e = (MultiTreeEntry)it.next();
			put(e.getKey(), e.getValue());
		}
	}

	public Object remove(Object key) {
		return remove(key, 0);
	}

	public Object remove(Object key, int index) {
		MultiTreeEntry entry = getEntry(key, index);
		if (entry == null) {
			return null;
		}

		Object oldValue = entry.ivValue;
		deleteEntry(entry);
		return oldValue;
	}

	/**
	 * Removes all entries from this map.
	 */
	public void clear() {
		ivModCount++;
		ivSize = 0;
		ivRoot = null;
	}

// --- handling of entries (utility methods) --------------
	private boolean valueSearchNull(MultiTreeEntry entry) {
		if (entry.ivValue == null) {
			return true;
		}
		return (entry.ivLeft != null && valueSearchNull(entry.ivLeft)) ||
				(entry.ivRight != null && valueSearchNull(entry.ivRight));
	}

	private boolean valueSearchNonNull(MultiTreeEntry entry, Object value) {
		if (value.equals(entry.ivValue)) {
			return true;
		}
		return (entry.ivLeft != null && valueSearchNonNull(entry.ivLeft, value)) ||
				(entry.ivRight != null && valueSearchNonNull(entry.ivRight, value));
	}

	/**
	 * searches down from the given searchEntry for the place to insert the given
	 * key.
	 * <P>
	 * Does rebalancing after insertion.
	 */
	protected void doPut(MultiTreeEntry searchEntry, MultiMapKey key, Object value) {
		incrementSize();
		if (ivRoot == null) {
			ivRoot = new MultiTreeEntry(key, value, null);
			return;
		}

		while (true) {
			int cmp = compare(key, searchEntry.ivKey);
			if (cmp == 0) {
				// cannot happen
				throw new IllegalStateException("Algorithm implemented incorrectly!");
			} else if (cmp < 0) {
				if (searchEntry.ivLeft != null) {
					searchEntry = searchEntry.ivLeft;
				} else {
					searchEntry.ivLeft = new MultiTreeEntry(key, value, searchEntry);
					fixAfterInsertion(searchEntry.ivLeft);
					return;
				}
			} else { // cmp > 0
				if (searchEntry.ivRight != null) {
					searchEntry = searchEntry.ivRight;
				} else {
					searchEntry.ivRight = new MultiTreeEntry(key, value, searchEntry);
					fixAfterInsertion(searchEntry.ivRight);
					return;
				}
			}
		}
	}

	protected MultiTreeEntry getEntry(Object key, int index) {
		MultiMapKey multiKey = new MultiMapKey(key, 0);
		MultiTreeEntry entry = getFloorEntry(multiKey);
		entry = entry.relative(index);
		if ((entry != null) && (equalsObj(entry.ivKey.ivKey, multiKey.ivKey))) {
			return entry;
		}
		return null;
	}

	private MultiTreeEntry getEntry(MultiMapKey key) {
		MultiTreeEntry entry = ivRoot;
		while (entry != null) {
			int cmp = compare(key, entry.ivKey);
			if (cmp == 0) {
				return entry;
			} else if (cmp < 0) {
				entry = entry.ivLeft;
			} else {
				entry = entry.ivRight;
			}
		}
		return null;
	}

	/**
	 * Gets the entry for the specified key.
	 * <P> If no such entry exists, the entry for the least key greater
	 * than the specified one is given.
	 */
	private MultiTreeEntry getCeilEntry(MultiMapKey key) {
		MultiTreeEntry entry = ivRoot;
		if (entry == null) {
			return null;
		}

		while (true) {
			int cmp = compare(key, entry.ivKey);
			if (cmp == 0) {
				return entry;
			} else if (cmp < 0) {
				if (entry.ivLeft != null) {
					entry = entry.ivLeft;
				} else {
					return entry;
				}
			} else {
				if (entry.ivRight != null) {
					entry = entry.ivRight;
				} else {
					MultiTreeEntry parent = entry.ivParent;
					MultiTreeEntry ch = entry;
					while (parent != null && ch == parent.ivRight) {
						ch = parent;
						parent = parent.ivParent;
					}
					return parent;
				}
			}
		}
	}

	/**
	 * Gets the entry for the specified key.
	 * <P> If no such entry exists, the entry for the greatest key lesser
	 * than the specified one is given.
	 */
	private MultiTreeEntry getFloorEntry(MultiMapKey key) {
		MultiTreeEntry entry = ivRoot;
		if (entry == null) {
			return null;
		}

		while (true) {
			int cmp = compare(key, entry.ivKey);
			if (cmp == 0) {
				return entry;
			} else if (cmp < 0) {
				if (entry.ivLeft != null) {
					entry = entry.ivLeft;
				} else {
					MultiTreeEntry parent = entry.ivParent;
					MultiTreeEntry ch = entry;
					while (parent != null && ch == parent.ivLeft) {
						ch = parent;
						parent = parent.ivParent;
					}
					return parent;
				}
			} else {
				if (entry.ivRight != null) {
					entry = entry.ivRight;
				} else {
					return entry;
				}
			}
		}
	}

	private int compare(MultiMapKey k1, MultiMapKey k2) {
		if (ivComparator == null) return ((Comparable)k1).compareTo(k2);
		int retValue = ivComparator.compare(k1.ivKey, k2.ivKey);
		if (retValue != 0) return retValue;
		return k1.ivIndex - k2.ivIndex;
	}

	/**
	 * Returns the first Entry in the TreeMap (according to the TreeMap's
	 * key-sort function).  Returns null if the TreeMap is empty.
	 */
	private MultiTreeEntry firstEntry() {
		MultiTreeEntry entry = ivRoot;
		if (entry != null) {
			while (entry.ivLeft != null) {
				entry = entry.ivLeft;
			}
		}
		return entry;
	}

	/**
	 * Returns the last Entry in the TreeMap (according to the TreeMap's
	 * key-sort function).  Returns null if the TreeMap is empty.
	 */
	private MultiTreeEntry lastEntry() {
		MultiTreeEntry entry = ivRoot;
		if (entry != null) {
			while (entry.ivRight != null) {
				entry = entry.ivRight;
			}
		}
		return entry;
	}

// --- Balancing operations. ------------------------------
	/**
	 *
	 * Implementations of rebalancings during insertion and deletion are
	 * slightly different than the CLR version.  Rather than using dummy
	 * nilnodes, we use a set of accessors that deal properly with null.  They
	 * are used to avoid messiness surrounding nullness checks in the main
	 * algorithms.
	 */
	private static boolean colorOf(MultiTreeEntry entry) {
		return (entry == null ? BLACK : entry.ivColor);
	}

	private static MultiTreeEntry parentOf(MultiTreeEntry entry) {
		return (entry == null ? null : entry.ivParent);
	}

	private static void setColor(MultiTreeEntry entry, boolean c) {
		if (entry != null) {
			entry.ivColor = c;
		}
	}

	private static MultiTreeEntry leftOf(MultiTreeEntry entry) {
		return (entry == null) ? null : entry.ivLeft;
	}

	private static MultiTreeEntry rightOf(MultiTreeEntry entry) {
		return (entry == null) ? null : entry.ivRight;
	}

	/** From CLR **/
	private void rotateLeft(MultiTreeEntry entry) {
		MultiTreeEntry r = entry.ivRight;
		entry.ivRight = r.ivLeft;
		if (r.ivLeft != null) {
			r.ivLeft.ivParent = entry;
		}
		r.ivParent = entry.ivParent;
		if (entry.ivParent == null) {
			ivRoot = r;
		} else if (entry.ivParent.ivLeft == entry) {
			entry.ivParent.ivLeft = r;
		} else {
			entry.ivParent.ivRight = r;
		}
		r.ivLeft = entry;
		entry.ivParent = r;
	}

	/** From CLR **/
	private void rotateRight(MultiTreeEntry entry) {
		MultiTreeEntry l = entry.ivLeft;
		entry.ivLeft = l.ivRight;
		if (l.ivRight != null) {
			l.ivRight.ivParent = entry;
		}
		l.ivParent = entry.ivParent;
		if (entry.ivParent == null) {
			ivRoot = l;
		} else if (entry.ivParent.ivRight == entry) {
			entry.ivParent.ivRight = l;
		} else {
			entry.ivParent.ivLeft = l;
		}
		l.ivRight = entry;
		entry.ivParent = l;
	}

	/** From CLR **/
	private void fixAfterInsertion(MultiTreeEntry x) {
		x.ivColor = RED;

		while (x != null && x != ivRoot && x.ivParent.ivColor == RED) {
			if (parentOf(x) == leftOf(parentOf(parentOf(x)))) {
				MultiTreeEntry y = rightOf(parentOf(parentOf(x)));
				if (colorOf(y) == RED) {
					setColor(parentOf(x), BLACK);
					setColor(y, BLACK);
					setColor(parentOf(parentOf(x)), RED);
					x = parentOf(parentOf(x));
				} else {
					if (x == rightOf(parentOf(x))) {
						x = parentOf(x);
						rotateLeft(x);
					}
					setColor(parentOf(x), BLACK);
					setColor(parentOf(parentOf(x)), RED);
					if (parentOf(parentOf(x)) != null) {
						rotateRight(parentOf(parentOf(x)));
					}
				}
			} else {
				MultiTreeEntry y = leftOf(parentOf(parentOf(x)));
				if (colorOf(y) == RED) {
					setColor(parentOf(x), BLACK);
					setColor(y, BLACK);
					setColor(parentOf(parentOf(x)), RED);
					x = parentOf(parentOf(x));
				} else {
					if (x == leftOf(parentOf(x))) {
						x = parentOf(x);
						rotateRight(x);
					}
					setColor(parentOf(x), BLACK);
					setColor(parentOf(parentOf(x)), RED);
					if (parentOf(parentOf(x)) != null) {
						rotateLeft(parentOf(parentOf(x)));
					}
				}
			}
		}
		ivRoot.ivColor = BLACK;
	}

	/**
	 * Delete node entry, and then rebalance the tree.
	 */
	private void deleteEntry(MultiTreeEntry entry) {
		decrementSize();

		if (entry.ivLeft != null && entry.ivRight != null) {
			MultiTreeEntry s = entry.successor();
			entry.ivKey = s.ivKey;
			entry.ivValue = s.ivValue;
			entry = s;
		} // entry has 2 children

		// Start fixup at replacement node, if it exists.
		MultiTreeEntry replacement = (entry.ivLeft != null ? entry.ivLeft : entry.ivRight);

		if (replacement != null) {
			// Link replacement to parent
			replacement.ivParent = entry.ivParent;
			if (entry.ivParent == null) {
				ivRoot = replacement;
			} else if (entry == entry.ivParent.ivLeft) {
				entry.ivParent.ivLeft = replacement;
			} else {
				entry.ivParent.ivRight = replacement;

				// Null out links so they are OK to use by fixAfterDeletion.
			}
			entry.ivLeft = entry.ivRight = entry.ivParent = null;

			// Fix replacement
			if (entry.ivColor == BLACK) {
				fixAfterDeletion(replacement);
			}
		} else if (entry.ivParent == null) { // return if we are the only node.
			ivRoot = null;
		} else { //  No children. Use self as phantom replacement and unlink.
			if (entry.ivColor == BLACK) {
				fixAfterDeletion(entry);

			}
			if (entry.ivParent != null) {
				if (entry == entry.ivParent.ivLeft) {
					entry.ivParent.ivLeft = null;
				} else if (entry == entry.ivParent.ivRight) {
					entry.ivParent.ivRight = null;
				}
				entry.ivParent = null;
			}
		}
	}

	private void fixAfterDeletion(MultiTreeEntry x) {
		while (x != ivRoot && colorOf(x) == BLACK) {
			if (x == leftOf(parentOf(x))) {
				MultiTreeEntry sib = rightOf(parentOf(x));

				if (colorOf(sib) == RED) {
					setColor(sib, BLACK);
					setColor(parentOf(x), RED);
					rotateLeft(parentOf(x));
					sib = rightOf(parentOf(x));
				}

				if (colorOf(leftOf(sib)) == BLACK &&
						colorOf(rightOf(sib)) == BLACK) {
					setColor(sib, RED);
					x = parentOf(x);
				} else {
					if (colorOf(rightOf(sib)) == BLACK) {
						setColor(leftOf(sib), BLACK);
						setColor(sib, RED);
						rotateRight(sib);
						sib = rightOf(parentOf(x));
					}
					setColor(sib, colorOf(parentOf(x)));
					setColor(parentOf(x), BLACK);
					setColor(rightOf(sib), BLACK);
					rotateLeft(parentOf(x));
					x = ivRoot;
				}
			} else {
				MultiTreeEntry sib = leftOf(parentOf(x));

				if (colorOf(sib) == RED) {
					setColor(sib, BLACK);
					setColor(parentOf(x), RED);
					rotateRight(parentOf(x));
					sib = leftOf(parentOf(x));
				}

				if (colorOf(rightOf(sib)) == BLACK &&
						colorOf(leftOf(sib)) == BLACK) {
					setColor(sib, RED);
					x = parentOf(x);
				} else {
					if (colorOf(leftOf(sib)) == BLACK) {
						setColor(rightOf(sib), BLACK);
						setColor(sib, RED);
						rotateLeft(sib);
						sib = leftOf(parentOf(x));
					}
					setColor(sib, colorOf(parentOf(x)));
					setColor(parentOf(x), BLACK);
					setColor(leftOf(sib), BLACK);
					rotateRight(parentOf(x));
					x = ivRoot;
				}
			}
		}

		setColor(x, BLACK);
	}

// --- Serialization --------------------------------------
	private void writeObject(java.io.ObjectOutputStream s) throws java.io.IOException {
		s.defaultWriteObject();
		s.writeInt(ivSize);
		for (Iterator i = entrySet().iterator(); i.hasNext(); ) {
			MultiTreeEntry e = (MultiTreeEntry)i.next();
			s.writeObject(e.ivKey);
			s.writeObject(e.ivValue);
		}
	}

	private void readObject(final java.io.ObjectInputStream s) throws java.io.IOException, ClassNotFoundException {
		s.defaultReadObject();
		int size = s.readInt();
		buildFromSorted(size, null, s, null);
	}

	private void readTreeSet(int size, java.io.ObjectInputStream s, Object defaultVal) throws java.io.IOException, ClassNotFoundException {
		buildFromSorted(size, null, s, defaultVal);
	}

	private void addAllForTreeSet(SortedSet set, Object defaultVal) {
		try {
			buildFromSorted(set.size(), set.iterator(), null, defaultVal);
		} catch (java.io.IOException cannotHappen) {
		} catch (ClassNotFoundException cannotHappen) {
		}
	}

	/**
	 * The entries are built from the given data.
	 * <P> The data is given with the Itearator or within the ObjectStream.
	 */
	private void buildFromSorted(int size, Iterator it,
															 java.io.ObjectInputStream str,
															 Object defaultVal) throws java.io.IOException, ClassNotFoundException {
		ivSize = size;
		ivRoot = buildFromSorted(0, 0, size - 1, computeRedLevel(size),
														 it, str, defaultVal);
	}

	private static MultiTreeEntry buildFromSorted(int level, int lo, int hi,
																			 int redLevel,
																			 Iterator it,
																			 java.io.ObjectInputStream str,
																			 Object defaultVal) throws java.io.IOException, ClassNotFoundException {
		if (hi < lo) {
			return null;
		}

		int mid = (lo + hi) / 2;

		MultiTreeEntry left = null;
		if (lo < mid) {
			left = buildFromSorted(level + 1, lo, mid - 1, redLevel,
														 it, str, defaultVal);
		}
		Object key;
		Object value;
		if (it != null) { // use iterator
			if (defaultVal == null) {
				Map.Entry entry = (Map.Entry)it.next();
				key = entry.getKey();
				value = entry.getValue();
			} else {
				key = it.next();
				value = defaultVal;
			}
		} else { // use stream
			key = str.readObject();
			value = (defaultVal != null ? defaultVal : str.readObject());
		}

		MultiMapKey multiKey = null;
		if (key instanceof MultiMapKey) {
			multiKey = (MultiMapKey)key;
		} else {
			multiKey = new MultiMapKey(key, 0);
		}

		MultiTreeEntry middle = new MultiTreeEntry(multiKey, value, null);

		// color nodes in non-full bottommost level red
		if (level == redLevel) {
			middle.ivColor = RED;

		}
		if (left != null) {
			middle.ivLeft = left;
			left.ivParent = middle;
		}

		if (mid < hi) {
			MultiTreeEntry right = buildFromSorted(level + 1, mid + 1, hi, redLevel,
																		it, str, defaultVal);
			middle.ivRight = right;
			right.ivParent = middle;
		}

		return middle;
	}

	private static int computeRedLevel(int sz) {
		int level = 0;
		for (int m = sz - 1; m >= 0; m = m / 2 - 1) {
			level++;
		}
		return level;
	}

// --- private helper methods -----------------------------
	private void incrementSize() { ivModCount++; ivSize++; }
	private void decrementSize() { ivModCount++; ivSize--; }

// --- private static helper methods ----------------------
	/**
	 * Returns the key corresonding to the specified Entry.  Throw
	 * NoSuchElementException if the Entry is <tt>null</tt>.
	 */
	private static MultiMapKey key(MultiTreeEntry e) {
		if (e == null) {
			throw new NoSuchElementException();
		}
		return e.ivKey;
	}

	static boolean equalsObj(Object obj1, Object obj2) {
		return (obj1 == null ? obj2 == null : obj1.equals(obj2));
	}

// --- Views ----------------------------------------------
	/**
	 * This field is initialized to contain an instance of the entry set
	 * view the first time this view is requested.  The view is stateless,
	 * so there's no reason to create more than one.
	 */
	private transient volatile Set ivEntrySet = null;

	/**
	 * Returns a Set view of the keys contained in this map.  The set's
	 * iterator will return the keys in ascending order.  The map is backed by
	 * this <tt>TreeMap</tt> instance, so changes to this map are reflected in
	 * the Set, and vice-versa.  The Set supports element removal, which
	 * removes the corresponding mapping from the map, via the
	 * <tt>Iterator.remove</tt>, <tt>Set.remove</tt>, <tt>removeAll</tt>,
	 * <tt>retainAll</tt>, and <tt>clear</tt> operations.  It does not support
	 * the <tt>add</tt> or <tt>addAll</tt> operations.
	 *
	 * @return a set view of the keys contained in this TreeMap.
	 */
	public Set keySet() {
		if (ivKeySet == null) {
			ivKeySet = new AbstractSet() {
				public Iterator iterator() {
					return new KeyIterator();
				}

				public int size() {
					return MultiTreeMap.this.size();
				}

				public boolean contains(Object o) {
					return containsKey(o);
				}

				public boolean remove(Object o) {
					int oldSize = ivSize;
					MultiTreeMap.this.remove(o);
					return ivSize != oldSize;
				}

				public void clear() {
					MultiTreeMap.this.clear();
				}
			};
		}
		return ivKeySet;
	}

	/**
	 * Returns a collection view of the values contained in this map.  The
	 * collection's iterator will return the values in the order that their
	 * corresponding keys appear in the tree.  The collection is backed by
	 * this <tt>TreeMap</tt> instance, so changes to this map are reflected in
	 * the collection, and vice-versa.  The collection supports element
	 * removal, which removes the corresponding mapping from the map through
	 * the <tt>Iterator.remove</tt>, <tt>Collection.remove</tt>,
	 * <tt>removeAll</tt>, <tt>retainAll</tt>, and <tt>clear</tt> operations.
	 * It does not support the <tt>add</tt> or <tt>addAll</tt> operations.
	 *
	 * @return a collection view of the values contained in this map.
	 */
	public Collection values() {
		if (ivValues == null) {
			ivValues = new AbstractCollection() {
				public Iterator iterator() {
					return new ValueIterator();
				}

				public int size() {
					return MultiTreeMap.this.size();
				}

				public boolean contains(Object o) {
					for (MultiTreeEntry entry = firstEntry(); entry != null; entry = entry.successor()) {
						if (equalsObj(entry.getValue(), o)) {
							return true;
						}
					}
					return false;
				}

				public boolean remove(Object o) {
					for (MultiTreeEntry entry = firstEntry(); entry != null; entry = entry.successor()) {
						if (equalsObj(entry.getValue(), o)) {
							deleteEntry(entry);
							return true;
						}
					}
					return false;
				}

				public void clear() {
					MultiTreeMap.this.clear();
				}
			};
		}
		return ivValues;
	}

	public Set entrySet() {
		if (ivEntrySet == null) {
			ivEntrySet = new AbstractSet() {
				public Iterator iterator() {
					return new EntryIterator();
				}

				public boolean contains(Object o) {
					if (!(o instanceof Map.Entry)) {
						return false;
					}
					Map.Entry mapEntry = (Map.Entry)o;
					Object value = mapEntry.getValue();
					Object key = mapEntry.getKey();
					MultiMapKey multiKey = null;
					if (key instanceof MultiMapKey) {
						multiKey = (MultiMapKey)key;
					} else {
						multiKey = new MultiMapKey(key, 0);
					}
					MultiTreeEntry entry = getEntry(multiKey, 0);
					boolean bRetVal = false;
					while ((entry != null) && (equalsObj(entry.ivKey, key)) && !(bRetVal = equalsObj(entry.ivValue, value))) {
						entry = entry.successor();
					}
					return bRetVal;
				}

				public boolean remove(Object o) {
					if (!(o instanceof Map.Entry)) {
						return false;
					}
					Map.Entry mapEntry = (Map.Entry)o;
					Object value = mapEntry.getValue();
					Object key = mapEntry.getKey();
					if (! (key instanceof MultiMapKey)) {
						key = new MultiMapKey(key, 0);
					}
					MultiTreeEntry entry = getEntry((MultiMapKey)key);
					if (entry != null && equalsObj(entry.getValue(), value)) {
						deleteEntry(entry);
						return true;
					}
					return false;
				}

				public int size() {
					return MultiTreeMap.this.size();
				}

				public void clear() {
					MultiTreeMap.this.clear();
				}
			};
		}
		return ivEntrySet;
	}

// --- SubMap ---------------------------------------------
	public SortedMap subMap(Object fromKey, Object toKey) {
		return subMap(fromKey, toKey, false);
	}

	public SortedMap subMap(Object fromKey, Object toKey, boolean toKeyIsInclusive) {
		return new SubMap(false, fromKey, false, toKey, toKeyIsInclusive);
	}

	public SortedMap headMap(Object toKey) {
		return headMap(toKey, false);
	}

	public SortedMap headMap(Object toKey, boolean toKeyIsInclusive) {
		return new SubMap(true, null, false, toKey, toKeyIsInclusive);
	}

	public SortedMap tailMap(Object fromKey) {
		return new SubMap(false, fromKey, true, null, false);
	}

	private class SubMap extends AbstractMap implements IMultiSortedMap, Serializable {
		private static final long serialVersionUID = 1448212873711223510L;

		/**
		 * fromKey is significant only if fromStart is false.  Similarly,
		 * toKey is significant only if toStart is false.
		 */
		private boolean ivFromStart, ivToEnd, ivToKeyIsInclusive;
		private Object ivFromKey, ivToKey;

		SubMap(boolean fromStart, Object fromKey, boolean toEnd, Object toKey, boolean toKeyIsInclusive) {
			if ((! fromStart) && (! toEnd) && compare(new MultiMapKey(fromKey, 0), new MultiMapKey(toKey, 0)) > 0) {
				throw new IllegalArgumentException("fromKey > toKey");
			}
			ivFromStart = fromStart;
			ivFromKey = fromKey;
			ivToEnd = toEnd;
			ivToKey = toKey;
			ivToKeyIsInclusive = toKeyIsInclusive;
		}

		public boolean isEmpty() {
			return ivEntrySet.isEmpty();
		}

		public boolean containsKey(Object key) {
			return containsKey(key, 0);
		}

		public boolean containsKey(Object key, int index) {
			return inRange(key) && MultiTreeMap.this.containsKey(key, index);
		}

		public boolean isKeyInRange(Object key) {
			return inRange(key);
		}

		public Object get(Object key) {
			return get(key, 0);
		}

		public Object get(Object key, int index) {
			if (!inRange(key)) {
				return null;
			}
			return MultiTreeMap.this.get(key, index);
		}

		public Object put(Object key, Object value) {
			if (!inRange(key)) {
				throw new IllegalArgumentException("key out of range");
			}
			return MultiTreeMap.this.put(key, value);
		}

		public Object put(Object key, Object value, int index) {
			if (!inRange(key)) {
				throw new IllegalArgumentException("key out of range");
			}
			return MultiTreeMap.this.put(key, value);
		}

		public Object remove(Object key) {
			return remove(key, 0);
		}

		public Object remove(Object key, int index) {
			if (!inRange(key)) {
				throw new IllegalArgumentException("key out of range");
			}
			return MultiTreeMap.this.remove(key, index);
		}

		public Comparator comparator() {
			return MultiTreeMap.this.ivComparator;
		}

		public Object firstKey() {
			MultiMapKey first = key(ivFromStart ? firstEntry() : getCeilEntry(new MultiMapKey(ivFromKey, 0)));
			if (!ivToEnd && compare(first, new MultiMapKey(ivToKey, 0)) >= 0) {
				throw (new NoSuchElementException());
			}
			return first;
		}

		public Object lastKey() {
			MultiMapKey last = key(ivToEnd ? lastEntry() : getFloorEntry(new MultiMapKey(ivToKey, 0)).predecessor());
			if (!ivFromStart && compare(last, new MultiMapKey(ivFromKey, 0)) < 0) {
				throw (new NoSuchElementException());
			}
			return last;
		}

		private transient Set ivEntrySet = new EntrySetView();

		public Set entrySet() {
			return ivEntrySet;
		}

		private class EntrySetView extends AbstractSet {
			private transient int ivSize = -1, sizeModCount;

			public int size() {
				if (ivSize == -1 || sizeModCount != MultiTreeMap.this.ivModCount) {
					ivSize = 0;
					sizeModCount = MultiTreeMap.this.ivModCount;
					Iterator i = iterator();
					while (i.hasNext()) {
						ivSize++;
						i.next();
					}
				}
				return ivSize;
			}

			public boolean isEmpty() {
				return!iterator().hasNext();
			}

			public boolean contains(Object o, int index) {
				if (!(o instanceof Map.Entry)) {
					return false;
				}
				Map.Entry entry = (Map.Entry)o;
				Object key = entry.getKey();
				if (!inRange(key)) {
					return false;
				}
				MultiTreeEntry node = getEntry(key, index);
				return node != null && equalsObj(node.getValue(), entry.getValue());
			}

			public boolean remove(Object o, int index) {
				if (!(o instanceof Map.Entry)) {
					return false;
				}
				Map.Entry entry = (Map.Entry)o;
				Object key = entry.getKey();
				if (!inRange(key)) {
					return false;
				}
				MultiTreeEntry node = getEntry(key, index);
				if (node != null && equalsObj(node.getValue(), entry.getValue())) {
					deleteEntry(node);
					return true;
				}
				return false;
			}

			public Iterator iterator() {
				return new EntryIterator(
						(ivFromStart ? null : new MultiMapKey(ivFromKey, 0)),
						(ivToEnd ? null : new MultiMapKey(ivToKey, (ivToKeyIsInclusive ? Integer.MAX_VALUE : 0))));
			}
		}

		public SortedMap subMap(Object fromKey, Object toKey) {
			return subMap(fromKey, toKey, false);
		}

		public SortedMap subMap(Object fromKey, Object toKey, boolean toKeyIsInclusive) {
			if (!inRange(fromKey, true)) {
				throw new IllegalArgumentException("fromKey out of range");
			}
			if (!inRange(toKey, true)) {
				throw new IllegalArgumentException("toKey out of range");
			}
			return MultiTreeMap.this.subMap(fromKey, toKey, toKeyIsInclusive);
		}

		public SortedMap headMap(Object toKey) {
			return headMap(toKey, false);
		}

		public SortedMap headMap(Object toKey, boolean toKeyIsInclusive) {
			if (!inRange(toKey, true)) {
				throw new IllegalArgumentException("toKey out of range");
			}
			return MultiTreeMap.this.headMap(toKey, toKeyIsInclusive);
		}

		public SortedMap tailMap(Object fromKey) {
			if (!inRange(fromKey, true)) {
				throw new IllegalArgumentException("fromKey out of range");
			}
			return MultiTreeMap.this.tailMap(fromKey);
		}

		private boolean inRange(Object key) {
			return inRange(key, ivToKeyIsInclusive);
		}

		private boolean inRange(Object key, boolean toKeyIsInclusive) {
			boolean bOk = (ivFromStart || compare(new MultiMapKey(key, 0), new MultiMapKey(ivFromKey, 0)) >= 0);
			bOk &= (toKeyIsInclusive) ? (ivToEnd || compare(new MultiMapKey(key, 0), new MultiMapKey(ivToKey, 0)) < 0)
																: (ivToEnd || compare(new MultiMapKey(key, 0), new MultiMapKey(ivToKey, Integer.MAX_VALUE)) <= 0);
			return bOk;
		}

	}

// --- Iterators ------------------------------------------
	/**
	 * TreeMap Iterator.
	 */
	private class EntryIterator implements ISortedIterator {
		private int ivExpectedModCount = MultiTreeMap.this.ivModCount;
		private MultiTreeEntry ivLastReturned = null;
		private boolean ivDeletedLastReturned;
		private boolean ivBeforeFirst;
		private boolean ivAfterLast;
		private MultiMapKey ivFromKey;
		private MultiMapKey ivToKey;

		EntryIterator() {
			this(null, null);
		}

		// Used by SubMapEntryIterator
		EntryIterator(MultiMapKey fromKey, MultiMapKey toKey) {
			ivBeforeFirst = true;
			ivAfterLast = false;
			ivFromKey = fromKey;
			ivToKey = toKey;
		}

		/**
		 * Template method to be overridden by subclasse, if they want to
		 * return some internal object of the entry.
		 */
		protected Object getObjectImpl(MultiTreeEntry entry) {
			return entry;
		}

		private Object getObject(MultiTreeEntry entry) {
			return (entry == null) ? null : getObjectImpl(entry);
		}

		public Object current() {
			return getObject(ivLastReturned);
		}

		public boolean hasNext() {
			return nextEntry(1, false) != null;
		}

		public boolean hasPrevious() {
			return previousEntry(-1, false) != null;
		}

		public Object next() {
			return getObject(nextEntry(1, true));
		}

		public Object previous() {
			return getObject(previousEntry(-1, true));
		}

		public void beforeFirst() {
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			ivBeforeFirst = true;
			ivAfterLast = false;
			ivLastReturned = null;
		}

		public boolean isBeforeFirst() {
			return ivBeforeFirst;
		}

		public Object first() {
			return getObject(firstEntry());
		}

		public boolean isFirst() {
			if (ivBeforeFirst || ivAfterLast) return false;
			return ivLastReturned == firstEntry();
		}

		public MultiTreeEntry firstEntry() {
			beforeFirst();
			return nextEntry(1, true);
		}

		public boolean isAfterLast() {
			return ivAfterLast;
		}

		public void afterLast() {
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			ivBeforeFirst = false;
			ivAfterLast = true;
			ivLastReturned = null;
		}

		public Object last() {
			return getObject(lastEntry());
		}

		public boolean isLast() {
			if (ivBeforeFirst || ivAfterLast) return false;
			return ivLastReturned == lastEntry();
		}

		public MultiTreeEntry lastEntry() {
			afterLast();
			return previousEntry(-1, true);
		}

		public void remove() {
			if (ivDeletedLastReturned || ivLastReturned == null) {
				throw new IllegalStateException();
			}
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			deleteEntry(ivLastReturned);
			ivExpectedModCount++;
			ivDeletedLastReturned = true;
		}

		public Object absolute(int n) {
			if (n == 0) return ivDeletedLastReturned ? null : ivLastReturned;
			MultiTreeEntry entry = null;
			if (n >= 0) {
				beforeFirst();
				entry = nextEntry(n + 1, true);
			} else {
				afterLast();
				entry = previousEntry(n, true);
			}
			return getObject(entry);
		}

		public Object relative(int n) {
			MultiTreeEntry entry = null;
			if (n == 0) return ivDeletedLastReturned ? null : ivLastReturned;
			if (n > 0) {
				entry = nextEntry(n, true);
			} else {
				entry = previousEntry(n, true);
			}
			return getObject(entry);
		}

		public Object seek(Object key) {
			return seek(key, false);
		}

		public Object seek(Object key, boolean exact) {
			return getObject(seekEntry(key, exact));
		}

		public MultiTreeEntry seekEntry(Object key, boolean exact) {
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			ivBeforeFirst = false;
			Object value = null;
			MultiMapKey multiKey = null;
			if (key instanceof MultiMapKey) {
				multiKey = (MultiMapKey)key;
			} else {
				multiKey = new MultiMapKey(key, 0);
			}
			// @todo check whether key is within range
			MultiTreeEntry entry = getCeilEntry(multiKey);
			if ((entry != null) && exact) {
				if (key instanceof MultiMapKey) {
					if (! key.equals(entry.ivKey)) entry = null;
				} else {
					if (! equalsObj(entry.ivKey.ivKey, key)) entry = null;
				}
			}

			ivLastReturned = entry;
			ivAfterLast = (entry == null);
			return entry;
		}

		/**
		 * @param bRealGet Tells wether this method is a lookup or a real get
		 */
		private MultiTreeEntry nextEntry(int n, boolean bRealGet) {
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			MultiTreeEntry entry = null;
			if (ivBeforeFirst) {
				entry = (ivFromKey == null) ? MultiTreeMap.this.firstEntry() : getCeilEntry(ivFromKey);
				n--;
			} else {
				entry = ivLastReturned;
			}
			if ((entry != null) &&(ivToKey != null)) {
				if (entry.ivKey.compareTo(ivToKey) >= 0) entry = null;
			}
			entry = (entry == null) ? null : entry.relative(n);
			if ((entry != null) && (ivToKey != null)) {
				if (entry.ivKey.compareTo(ivFromKey) > 0) entry = null;
			}
			if (bRealGet) {
        ivBeforeFirst = false;
				if (entry == null) ivAfterLast = true;
				ivLastReturned = entry;
				ivDeletedLastReturned = false;
			}
			return entry;
		}

		/**
		 * @param bRealGet Tells wether this method is a lookup or a real get
		 */
		private MultiTreeEntry previousEntry(int n, boolean bRealGet) {
			if (ivModCount != ivExpectedModCount) {
				throw new ConcurrentModificationException();
			}
			MultiTreeEntry entry = null;
			if (ivAfterLast) {
				n++;
				if (ivToKey == null) {
					entry = MultiTreeMap.this.lastEntry();
				} else {
					entry = getCeilEntry(ivToKey);
					if (entry.ivKey.compareTo(ivToKey) >= 0) entry = entry.predecessor();
				}
			} else {
				entry = ivLastReturned;
			}
			entry = (entry == null) ? null : entry.relative(n);
			if ((entry != null) && (ivFromKey != null)) {
				if (entry.ivKey.compareTo(ivFromKey) < 0) entry = null;
			}
			if (bRealGet) {
        ivAfterLast = false;
				if (entry == null) ivBeforeFirst = true;
				ivLastReturned = entry;
				ivDeletedLastReturned = false;
			}
			return entry;
		}

	}

	private class KeyIterator extends EntryIterator {
		protected Object getObjectImpl(MultiTreeEntry entry) {
			return entry.ivKey.ivKey;
		}
	}

	private class ValueIterator extends EntryIterator {
		protected Object getObjectImpl(MultiTreeEntry entry) {
			return entry.ivValue;
		}
	}

}
