/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

import java.nio.charset.Charset;

/**
 *
 * @author Victor_Zoubok
 * Helper class to covert strings to bytes and visa versa
 */
public class HtStringByteConverter {
	public static final Charset UTF8_CHARSET = Charset.forName("UTF-8");
	public static final Charset WIN_1251_CHARSET = Charset.forName("windows-1251");

	public static String decodeUTF8(byte[] bytes)
	{
		return new String(bytes, UTF8_CHARSET);
	}

	public static byte[] encodeUTF8(String string)
	{
		return string.getBytes(UTF8_CHARSET);
	}

	public static String decodeWin1251(byte[] bytes)
	{
		return new String(bytes, WIN_1251_CHARSET);
	}

	public static byte[] encodeWin1251(String string)
	{
		return string.getBytes(WIN_1251_CHARSET);
	}


}
