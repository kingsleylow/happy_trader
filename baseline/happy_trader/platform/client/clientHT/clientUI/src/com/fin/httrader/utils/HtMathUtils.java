/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.io.InputStream;
import java.security.NoSuchAlgorithmException;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author DanilinS
 * some maths/value routines
 */
public class HtMathUtils {
	// converts integer value representation to C++ DWORD representation

	static public byte[] convertLongToByte(long iVal) {
		byte[] buffer = new byte[HtUtils.SOCKET_LENGTH_HEADER_BYTES];

		buffer[0] = (byte) (iVal >>> 56);
		buffer[1] = (byte) ((iVal >>> 48) & 0x0ff);
		buffer[2] = (byte) ((iVal >>> 40) & 0x0ff);
		buffer[3] = (byte) ((iVal >>> 32) & 0x0ff);
		buffer[4] = (byte) ((iVal >>> 24) & 0x0ff);
		buffer[5] = (byte) ((iVal >>> 16) & 0x0ff);
		buffer[6] = (byte) ((iVal >>> 8) & 0x0ff);
		buffer[7] = (byte) iVal;

		return buffer;
	}

	static public long convertByteToLong(byte[] buffer) {
		return (((long) buffer[0]) << 56) |
						(((long) buffer[1] & 0x0ffL) << 48) |
						(((long) buffer[2] & 0x0ffL) << 40) |
						(((long) buffer[3] & 0x0ffL) << 32) |
						(((long) buffer[4] & 0x0ffL) << 24) |
						(((long) buffer[5] & 0x0ffL) << 16) |
						(((long) buffer[6] & 0x0ffL) << 8) |
						((long) buffer[7] & 0x0ff);

	}

	// converts 4 byte integer value representation
	static public byte[] convertIntToByte4(int iVal) {
		byte[] buffer = new byte[4];

		buffer[3] = (byte) ((iVal >>> 24) & 0x0ff);
		buffer[2] = (byte) ((iVal >>> 16) & 0x0ff);
		buffer[1] = (byte) ((iVal >>> 8) & 0x0ff);
		buffer[0] = (byte) iVal;

		return buffer;
	}


	static public int convertByteToInt4(byte[] buffer, int bidx) {
		return (((int) buffer[3+bidx] & 0x0ff) << 24) |
						(((int) buffer[2+bidx] & 0x0ff) << 16) |
						(((int) buffer[1+bidx] & 0x0ff) << 8) |
						((int) buffer[0+bidx] & 0x0ff);
	}

	static public int convertByteToInt4(byte[] buffer) {
		return (((int) buffer[3] & 0x0ff) << 24) |
						(((int) buffer[2] & 0x0ff) << 16) |
						(((int) buffer[1] & 0x0ff) << 8) |
						((int) buffer[0] & 0x0ff);
	}

	// read double value from byte stream
	public static double readDoubleValueFromStream(InputStream stream) throws Exception {
		long bits = 0;
		for (int i = 0; i < 64; i += 8) {
			int ib = stream.read();
			bits |= ((long) ib << i);
		}

		return Double.longBitsToDouble(bits);

	}

	public static float readFloatValueFromStream(InputStream stream) throws Exception {
		int bits = 0;
		for (int i = 0; i < 32; i += 8) {
			bits |= ((int) stream.read() << i);
		}
		return Float.intBitsToFloat(bits);

	}

	public static double readDoubleValueFromBytes(byte[] buffer, int bidx) throws Exception {
		long bits = 0;
		for (int i = 0; i < 8; i++) {
			int ib = (int) buffer[i+bidx];
			if (ib < 0) {
				ib = ib + 256;
			}
			bits |= (long) ib << (i * 8);
		}

		return Double.longBitsToDouble(bits);
	}

	// IEEE conversion
	public static double readDoubleValueFromBytes(byte[] doubleBytes) throws Exception {
		long bits = 0;
		for (int i = 0; i < 8; i++) {
			int ib = (int) doubleBytes[i];
			if (ib < 0) {
				ib = ib + 256;
			}
			bits |= (long) ib << (i * 8);
		}

		return Double.longBitsToDouble(bits);

	}

	public static float readFloatValueFromBytes(byte[] doubleBytes) throws Exception {
		int bits = 0;
		for (int i = 0; i < 8; i++) {
			int ib = (int) doubleBytes[i];
			if (ib < 0) {
				ib = ib + 256;
			}

			bits |= (long) ib << (i * 8);
		}

		return Float.intBitsToFloat(bits);

	}

	//
	public static byte[] readBytesValueFromDouble(double value) throws Exception {

		long bits = Double.doubleToLongBits(value);
		byte[] result = new byte[8];

		for (int i = 0; i < 8; i++) {
			result[i] = (byte) (bits >> (i * 8) & 0x0ff);
		}

		return result;

	}

	public static byte[] readBytesValueFromFloat(float value) throws Exception {

		int bits = Float.floatToIntBits(value);
		byte[] result = new byte[4];

		for (int i = 0; i < 4; i++) {
			result[i] = (byte) (bits >> (i * 8) & 0x0ff);
		}

		return result;

	}




	public static byte[] readBytesValueFromInt4(int value) throws Exception {

		byte[] result = new byte[4];

		for (int i = 0; i < 4; i++) {
			result[i] = (byte) (value >> (i * 8) & 0x0ff);
		}

		return result;

	}


	/**
	Some math
	 */
	// n= input.size();
	// k <= n
	public static Vector<Vector< Integer> > getCombination(int n, int m) {
		Vector<Vector<Integer>> result = new Vector<Vector<Integer>>();

		Vector<Integer> result_helper = new Vector<Integer>();
		result_helper.setSize(m);

		getCombination_Helper(result, result_helper, n, m, 1, 1);


		return result;
	}

	private static void getCombination_Helper(
					Vector<Vector<Integer>> result,
					Vector<Integer> result_helper,
					int n,
					int m,
					int first,
					int ind) {

		if (ind > m) {
			Vector<Integer> result_helper_cpy = new Vector<Integer>(result_helper);
			result.add(result_helper_cpy);
			return;
		}

		for (int i = first; i <= n - (m - ind); i++) {
			result_helper.set(ind - 1, i);
			getCombination_Helper(result, result_helper, n, m, i + 1, ind + 1);
		}

	}

	public static double calculateAverage(Collection<? extends Number> valueslist) throws Exception
	{
		double result = -1;
		double prev_result = -1;
	
		int idx = 1;
		for(Iterator<? extends Number> it = valueslist.iterator(); it.hasNext(); ) {
			double val = it.next().doubleValue();

			if (idx==1) {
				result = val;
				prev_result = result;
			}
			else {
				result = (prev_result * idx + val ) / (idx + 1);

				prev_result = result;
				idx++;
			}
		}

		return result;
	}

	public static byte[] md5Hash(String source) {
		byte[] srcBytes = source.getBytes();
		java.security.MessageDigest algorithm= null;

		try {
			algorithm = java.security.MessageDigest.getInstance("MD5");
		}
		catch(NoSuchAlgorithmException e)
		{
			return null;
		}

		algorithm.reset();
		algorithm.update(srcBytes);

		return algorithm.digest();

	}

	public static String md5HashString(String source) {
		byte[] data = md5Hash(source);
		if (data != null)
			return BinConverter.bytesToBinHex(data);
		else
			return null;
	}

}
