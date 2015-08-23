/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;


import java.io.Serializable;
import java.nio.LongBuffer;

/**
 *
 * @author DanilinS
 */
public class Uid implements Serializable, Comparable<Uid> {

	public static final String INVALID_UID = "00000000000000000000000000000000";
	public static final int LENGTH = 16;
	private final static char[] digits = {
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'};

	private static int digit(char charachter) {
		switch (charachter) {
			case '0':
				return 0;
			case '1':
				return 1;
			case '2':
				return 2;
			case '3':
				return 3;
			case '4':
				return 4;
			case '5':
				return 5;
			case '6':
				return 6;
			case '7':
				return 7;
			case '8':
				return 8;
			case '9':
				return 9;
			case 'a':
				return 10;
			case 'b':
				return 11;
			case 'c':
				return 12;
			case 'd':
				return 13;
			case 'e':
				return 14;
			case 'f':
				return 15;
			case 'A':
				return 10;
			case 'B':
				return 11;
			case 'C':
				return 12;
			case 'D':
				return 13;
			case 'E':
				return 14;
			case 'F':
				return 15;
			default:
				return -1;
		}

	}
	private LongBuffer data_m = LongBuffer.wrap(new long[]{0, 0});

	// ----------------------------------------------------
	/** Creates a new instance of Uid */
	public Uid() {
	}

	public Uid(String val) {
		fromString(val);
	}

	public Uid(Uid val) {
		fromUid(val);
	}

	@Override
	public String toString() {
		if (!isValid()) {
			return INVALID_UID;
		}

		StringBuilder s = new StringBuilder(INVALID_UID);

		long n1 = getN1(); // s1 - left part  of string
		long n2 = getN2(); // s2 - write part of string

		int idx_n1 = LENGTH - 1;
		int idx_n2 = LENGTH * 2 - 1;

		
		do {

			s.setCharAt(idx_n1--, digits[(int) (n1 & 0x0f)]);
			n1 >>>= 4;


			s.setCharAt(idx_n2--, digits[(int) (n2 & 0x0f)]);
			n2 >>>= 4;


		} while (idx_n1 >= 0);


		return s.toString();

	}

	public final void fromUid(Uid val) {
		if (val == null) {
			invalidate();
			return;
		}
		setValue(val.getN1(), val.getN2());
	}

	public final void fromString(String val) {

		if (val.equals(INVALID_UID) || val.length() < LENGTH * 2) {
			setValue(0, 0);
			return;
		}

		int idx_n1 = LENGTH - 1;
		int idx_n2 = LENGTH * 2 - 1;


		long n1 = 0;
		long n2 = 0;
		long value = 0;

		int shift = 0;

	
		do {
			// the left part (n1)
			char c1 = val.charAt(idx_n1--);
			long c1_byte = (long) Uid.digit(c1);
			value = c1_byte << shift;

			n1 |= value;

			// the right part (n2)
			char c2 = val.charAt(idx_n2--);
			long c2_byte = (long) Uid.digit(c2);
			value = c2_byte << shift;

			n2 |= value;


			shift += 4;


		} while (idx_n1 >= 0);

		setValue(n1, n2);

	}

	public void generate()
	{

		// attempt twise - firsly with external lib, secondly - with native JAVA 
		//try {
			generateUniqueUid();
		//}
		//catch(UnsatisfiedLinkError eul)
		//{
		//	PlatformIndependentGuidGen.getInstance().genNewGuid(data_m);
		//}

	}

	@Override
	public boolean equals(Object obj) {
		Uid rhs = (Uid) obj;
		return ((this.getN1() == rhs.getN1()) && (this.getN2() == rhs.getN2()));
	}

	@Override
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

	@Override
	public int compareTo(Uid o) {
		return this.toString().compareTo(o.toString());
	}

	public static Uid generateInvalidUid()
	{
		Uid uid = new Uid();
		return uid;
	}

	public static String generateNewStringUid() {
		Uid uid = new Uid();
		uid.generate();

		return uid.toString();
	}

	
	private native void generateUniqueUid();
	
	
}
