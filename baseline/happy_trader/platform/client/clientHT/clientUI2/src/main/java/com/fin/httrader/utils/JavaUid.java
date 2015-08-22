/*
 * Uid.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.io.Serializable;
import java.math.BigInteger;
import java.nio.LongBuffer;

/**
 *
 * Uid class serving as unique identifier
 */
public class JavaUid implements Serializable, Comparable<Uid> {

	public static final String INVALID_UID = "00000000000000000000000000000000";

	/** Creates a new instance of Uid */
	public JavaUid() {
	}

	public JavaUid(String val) {
		fromString(val);
	}

	public JavaUid(JavaUid val) {
		fromUid(val);
	}

	public String toString() {
		StringBuffer s1 = new StringBuffer(Long.toHexString(getN1()).toUpperCase());
		StringBuffer s2 = new StringBuffer(Long.toHexString(getN2()).toUpperCase());

		while (s1.length() < 16) {
			s1.insert(0, '0');
		}

		while (s2.length() < 16) {
			s2.insert(0, '0');
		}


		return isValid() ? s1.append(s2).toString() : INVALID_UID;
	}

	public void fromUid(JavaUid val) {
		if (val == null) {
			invalidate();
			return;
		}
		setValue(val.getN1(), val.getN2());
	}

	public void fromString(String val) {
		try {

			String s1 = val.substring(0, 16);
			String s2 = val.substring(16, 32);

			BigInteger bi1 = new BigInteger(s1, 16);
			BigInteger bi2 = new BigInteger(s2, 16);

			long n1 = bi1.longValue();
			long n2 = bi2.longValue();
			setValue(n1, n2);
		} catch (Exception e) {
			setValue(0, 0);
		}

	}

	public void generate() {
		PlatformIndependentGuidGen.getInstance().genNewGuid(data_m);
	}

	public boolean equals(Object obj) {
		JavaUid rhs = (JavaUid) obj;
		return ((getN1() == rhs.getN1()) && (getN2() == rhs.getN2()));
	}

	public int hashCode() {
		return (int) this.getN2();
	}

	public void invalidate() {
		setValue(0, 0);
	}

	public boolean isValid() {
		return (getN1() != 0 && getN2() != 0);
	}

	/**
	 * Helpers
	 */

	private void setValue(long n1, long n2) {
		data_m.put(0, n1);
		data_m.put(1, n2);
	}
	
	private long getN1() {
		return data_m.get(0);
	}

	private long getN2() {
		return data_m.get(1);
	}

	public int compareTo(Uid o) {
		return this.toString().compareTo(o.toString());
	}

	public static String generateNewStringUid() {
		Uid uid = new Uid();
		uid.generate();

		return uid.toString();
	}
	// -----------------------
	LongBuffer data_m = LongBuffer.wrap(new long[]{0, 0});
}
