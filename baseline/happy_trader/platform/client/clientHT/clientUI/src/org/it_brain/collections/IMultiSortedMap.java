package org.it_brain.collections;

import java.util.*;

/**
 * Interface for MultiSortedMaps
 * <P> Duplicate key values are supported.
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

interface IMultiSortedMap extends SortedMap {

	boolean containsKey(Object key, int index);

	/**
	 * Tells whether the given key is within the range of a MultiSortMap
	 */
	boolean isKeyInRange(Object key);

	Object remove(Object key, int index);
	Object get(Object key, int index);
	Object put(Object key, Object value, int index);

}
