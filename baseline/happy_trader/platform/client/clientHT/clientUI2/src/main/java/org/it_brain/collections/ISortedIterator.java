package org.it_brain.collections;

import java.util.*;

/**
 * An iterator that supports a lot more of positionings and moving methods.
 * <P>
 * Classes implementing this interface must never through the
 * NoSuchElementException instead null has to be returned and the flags
 * for beforeFirst and afterLast have to be set correctly.
 */
public interface ISortedIterator extends Iterator {

	/**
	 * Positions on the n-th object in the tree.
	 * If n is negative positioning starts at the end.
	 * (-1 gives the last element).
	 * <P>
	 * This method has linear time cost for execution in most implementations!
	 * @return The found object.
	 */
	Object absolute(int n);

	boolean hasPrevious();
	Object previous();
	void beforeFirst();
	boolean isBeforeFirst();
	Object first();
	boolean isFirst();
	void afterLast();
	boolean isAfterLast();
	Object last();
	boolean isLast();
	Object relative(int n);

	/**
	 * Gives the object received by the last accessing call of this Iterator.
	 */
	Object current();

	/**
	 * @param exact If true only a exact match is taken as hit.
	 *              Otherwise the next key found greater than the given one is used.
	 * @return The object found.
	 *         If no one is found, the Iterator is set to afterLast.
	 */
	Object seek(Object key, boolean exact);

	/**
	 * Does a seek with exact set to fale.
	 */
	Object seek(Object key);

}
