/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

/**
 *
 * @author DanilinS
 * special class for comboboxes, etc...
 */
public class HtKeyedPair {
	public HtKeyedPair(int key_, String value_)
	{
		key = key_;
		value = value_;
	};

	
	@Override
	public String toString()
	{
		return value;
	}

	public int key = -1;
	public String value = null;
}
