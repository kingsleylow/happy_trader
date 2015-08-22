/*
 * HtUtils.java
 *
 * Created on December 3, 2006, 5:09 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import com.fin.httrader.utils.ThreadLocalFormats.HtThreadLocalDecimalFormat;
import java.io.InputStream;
import java.text.NumberFormat;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.logging.Level;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.transport.HtCommonSocket;
import com.fin.httrader.utils.transport.HtTCPIPSocket;
import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.ParseException;
import java.util.Collection;

/**
 *
 * @author Administrator
 */
public class HtUtils {

		public static int SOCKET_LENGTH_HEADER_BYTES = 8;
		private final static String[] to_search_g = {"&", "<", ">", "\"", "'", ""};
		private final static String[] to_subst_g = {"&amp;", "&lt;", "&gt;", "&quot;", "&apos;", ""};
		private final static HtThreadLocalDecimalFormat nf_m = new HtThreadLocalDecimalFormat('.', 20);

		// ------------------------------------------
		public static String getContext() {
				return HtUtils.class.getSimpleName();
		}

		private HtUtils() {
		}

		public static String bytesToStringUTFNIO(byte[] bytes) {

				CharBuffer cBuffer = ByteBuffer.wrap(bytes).asCharBuffer();

				return cBuffer.toString();

		}

		public static byte[] stringToBytesASCII(String str) {

				byte[] b = new byte[str.length()];

				for (int i = 0; i < b.length; i++) {

						b[i] = (byte) str.charAt(i);

				}

				return b;

		}

		;

	public static byte[] stringToBytesUTFCustom(String str) {

				byte[] b = new byte[str.length() << 1];

				for (int i = 0; i < str.length(); i++) {

						char strChar = str.charAt(i);

						int bpos = i << 1;

						b[bpos] = (byte) ((strChar & 0xFF00) >> 8);

						b[bpos + 1] = (byte) (strChar & 0x00FF);

				};

				return b;

		}

		public static String formatPriceValue(double val, int signdig, boolean positive) {

				if (positive && val < 0) {
						return "";
				}

				if (signdig > 0) {
						nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(signdig);
						nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(signdig);
				} else {
						nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(4);
						nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(4);

				}

				return nf_m.getNumberFormatThreadCopy().format(val);
		}

		public static String formatIntValue(int val, boolean positive) {

				if (positive && val < 0) {
						return "";
				}

				nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(0);
				nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(0);

				return nf_m.getNumberFormatThreadCopy().format(val);
		}

		public static String formatIntValue(double val, boolean positive) {

				if (positive && val < 0) {
						return "";
				}

				nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(0);
				nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(0);

				return nf_m.getNumberFormatThreadCopy().format(val);
		}

		public static String formatNonNegativePriceValue(double val, int signdig) {

				if (val <= 0) {
						return "";
				}

				if (signdig > 0) {
						nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(signdig);
						nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(signdig);
				} else {
						nf_m.getNumberFormatThreadCopy().setMinimumFractionDigits(4);
						nf_m.getNumberFormatThreadCopy().setMaximumFractionDigits(4);

				}

				return nf_m.getNumberFormatThreadCopy().format(val);
		}

		public static String getResponseError(XmlParameter resp) {
				try {
						if (resp.getInt("status_code") != 0) {
								StringBuilder r = new StringBuilder("Exception: ");
								r.append(HtUtils.unhexlifyString(resp.getString("error_msg"), "ISO-8859-1")).append(" on argument: ");
								r.append(HtUtils.unhexlifyString(resp.getString("error_arg"), "ISO-8859-1")).append(" in context: ");
								r.append(HtUtils.unhexlifyString(resp.getString("error_ctx"), "ISO-8859-1"));

								return r.toString();
						}
				} catch (Throwable e) {
						HtLog.log(Level.WARNING, getContext(), "getResponseError", "Exception: " + e.getMessage());
				}

				return "";

		}

		static public boolean isArrayContainsValue(Object value, Object[] array) {
				if (value == null) {
						return false;
				}

				if (array == null) {
						return false;
				}

				for (int i = 0; i < array.length; i++) {
						if (value.equals(array[i])) {
								return true;
						}
				}

				return false;
		}

		;

	// general helper to wrap Map<String, String> into XmlCommand
	public static void mapToParameter(String name, Map<String, String> params, XmlParameter xmlparameter) {
				xmlparameter.clear();
				xmlparameter.setCommandName(name);

				for (Iterator<String> it = params.keySet().iterator(); it.hasNext();) {
						String key = it.next();
						xmlparameter.setString(key, params.get(key));

				}

		}

		public static void parameterToMap(Map<String, String> params, XmlParameter xmlparameter) {

				params.clear();

				for (Iterator<String> it = xmlparameter.getKeys().iterator(); it.hasNext();) {
						String key = it.next();
						params.put(key, xmlparameter.getString(key));

				}

		}

		// sleep a number of seconds
		public static void Sleep(double sleep_sec) {
				if (sleep_sec <= 0) {
						return;
				}

				try {
						Thread.sleep((int) sleep_sec * 1000);
				} catch (InterruptedException e) {
						HtLog.log(Level.WARNING, getContext(), "Sleep", e.toString());
				}

		}

		public static Map<String, String> getMapFromXmlParameter(XmlParameter in) throws Exception {
				HashMap<String, String> result = new HashMap<String, String>();

				for (Iterator<String> it = in.getKeys().iterator(); it.hasNext();) {
						String key = it.next();
						String value = in.getString(key);

						result.put(key, value);
				}

				return result;
		}

		public static String prepareValidXml(String src) {
				StringBuilder result = new StringBuilder();
				StringBuilder srcs = new StringBuilder(src);

				for (int i = 0; to_search_g[i].length() > 0; i++) {

						int fidx = 0;
						result.setLength(0);
						while (true) {
								int idx = srcs.toString().indexOf(to_search_g[i], fidx);
								if (idx >= 0) {
										// before idx
										result.append(srcs.toString().substring(fidx, idx));
										result.append(to_subst_g[i]);

										// skip symbol
										fidx = idx + 1;
								} else {
										result.append(srcs.toString().substring(fidx));
										break;
								}
						}

						// store result;
						srcs.setLength(0);
						srcs.append(result.toString());

				}

				return result.toString();
		}

		public static String convertFromValidXml(String src) {
				StringBuilder result = new StringBuilder();
				StringBuilder srcs = new StringBuilder(src);

				for (int i = 0; to_search_g[i].length() > 0; i++) {

						int fidx = 0;
						result.setLength(0);
						while (true) {
								int idx = srcs.toString().indexOf(to_subst_g[i], fidx);
								if (idx >= 0) {
										// before idx
										result.append(srcs.toString().substring(fidx, idx));
										result.append(to_search_g[i]);

										// skip symbol
										fidx = idx + to_subst_g[i].length();
								} else {
										result.append(srcs.toString().substring(fidx));
										break;
								}
						}

						// store result;
						srcs.setLength(0);
						srcs.append(result.toString());

				}

				return result.toString();
		}

		// -----------------------------------------
		public static String convertInputStreamToString(InputStream is, String encoding) throws Exception {

				int BUFFER_SIZE = 2048;

				StringBuilder out = new StringBuilder();
				byte[] buffer = new byte[BUFFER_SIZE];
				//int off = 0;
				while (true) {

						int read = is.read(buffer, 0, BUFFER_SIZE);
						if (read <= 0) {
								break;
						}

						byte[] sbuffer = new byte[read];
						System.arraycopy(buffer, 0, sbuffer, 0, read);

						if (encoding != null) {
								out.append(new String(sbuffer, encoding));
						} else {
								out.append(new String(sbuffer));
						}

				}

				return out.toString();
		}

		public static String getParameterWithException(Map<String, String> parameter_list, String parameter) throws Exception {
				if (parameter_list.containsKey(parameter)) {
						return parameter_list.get(parameter);
				}

				throw new HtException(getContext(), "getParameterWithException", "Parameter is not valid: '" + parameter + "'");
		}

		public static String getParameterWithoutException(Map<String, String> parameter_list, String parameter) throws Exception {
				if (parameter_list.containsKey(parameter)) {
						return parameter_list.get(parameter);
				}

				return "";
		}

		public static int parseInt(String value) {
				if (nvl(value)) {
						return -1;
				}

				try {
						return Integer.valueOf(value);
				} catch (NumberFormatException e) {
						return -1;
				}
		}

		public static long parseLong(String value) {
				if (nvl(value)) {
						return -1;
				}

				try {
						return Long.valueOf(value);
				} catch (NumberFormatException e) {
						return -1;
				}
		}

		public static boolean parseBoolean(String value) {
				if (nvl(value)) {
						return false;
				}

				return value.equalsIgnoreCase("true");
		}

		public static int parseInt(String value, NumberFormat formatter) {
				if (nvl(value)) {
						return -1;
				}

				try {
						return formatter.parse(value).intValue();
				} catch (ParseException e) {
						return -1;
				}
		}

		public static double parseDouble(String value) {
				if (nvl(value)) {
						return -1;
				}

				try {
						return Double.valueOf(value);
				} catch (NumberFormatException e) {
						return -1;
				}
		}

		public static double parseDouble(String value, NumberFormat formatter) {
				if (nvl(value)) {
						return -1;
				}

				try {
						return formatter.parse(value).doubleValue();
				} catch (ParseException e) {
						return -1;
				}
		}

		public static <T> void addArrayToCollection(Collection<T> collection, T[] dataarray) {
				collection.clear();
				if (dataarray != null) {
						for (int i = 0; i < dataarray.length; i++) {
								collection.add(dataarray[i]);
						}
				}
		}

		public static <T> T[] addCollectionToArray(Collection<T> collection) {
				T[] result = null;
				if (collection != null || collection.size() > 0) {

						// hack :)
						result = (T[]) new Object[collection.size()];

						int i = 0;
						for (Iterator<T> it = collection.iterator(); it.hasNext();) {
								result[i++] = it.next();
						}
				}

				return result;
		}

		public static Object resizeArray(Object oldArray, int newSize) {
				int oldSize = java.lang.reflect.Array.getLength(oldArray);
				Class elementType = oldArray.getClass().getComponentType();
				Object newArray = java.lang.reflect.Array.newInstance(elementType, newSize);
				int preserveLength = Math.min(oldSize, newSize);
				if (preserveLength > 0) {
						System.arraycopy(oldArray, 0, newArray, 0, preserveLength);
				}
				return newArray;
		}

		// funstion to search in simle arrays
		public static Object findInArray(Object[] array, Object value) {
				for (int i = 0; i < array.length; i++) {
						if (array[i].equals(value)) {
								return value;
						}
				}

				return null;

		}

		public static boolean isLocalHost(String host) throws Exception {
				InetAddress addr = InetAddress.getByName(host);
				if (addr.isAnyLocalAddress() || addr.isLoopbackAddress()) {
						return true;
				} else {
						return false;
				}

		}

		public static String wrapToCDataTags(String srcstring) {
				StringBuilder result = new StringBuilder("<![CDATA[");
				result.append(srcstring.replaceAll("]]>", "]]]]><![CDATA[>"));
				result.append("]]>");

				return result.toString();
		}

		public static String arrayToString(Object[] darray) {
				StringBuilder result = new StringBuilder();

				if (darray != null) {
						for (int i = 0; i < darray.length; i++) {
								result.append(darray[i].toString());
								result.append(" ");
						}
				}

				return result.toString();
		}

		public static String hexlifyString(String data, String charsetName) throws UnsupportedEncodingException {
				return BinConverter.bytesToBinHex(data.getBytes(charsetName));
		}

		public static String unhexlifyString(String hexdata, String charsetName) throws UnsupportedEncodingException {
				byte[] data = new byte[hexdata.length() / 2];
				BinConverter.binHexToBytes(hexdata.toLowerCase(), data, 0, 0, data.length);

				return new String(data, charsetName);

		}

		public static String truncateString(String source, int maxlength) {
				if (maxlength < 0) {
						return source;
				}

				if (source == null) {
						return source;
				}

				if (source.length() <= maxlength) {
						return source;
				}

				return source.substring(0, maxlength);

		}

		public static byte[] readFromStreamUntilBytesAreReached(int tmsec, InputStream stream, byte[] termbytes) throws Exception {
				int cur_chunk = 1024;
				byte[] result = new byte[1024];
				int total_counter = 0;
				long timeOut = tmsec > 0 ? System.currentTimeMillis() : -1;
				byte tmp_buf[] = new byte[termbytes.length];

				int read;
				while (true) {

						read = stream.read();
						if (read <= 0) {
								throw new HtException(getContext(), "readFromStreamUntilByteIsReached", "Cannot read from stream");
						}

						if (tmsec > 0 && (System.currentTimeMillis() - timeOut > tmsec)) {
								throw new HtException(getContext(), "readFromStreamUntilByteIsReached", "Timeout exceded when reading from stream");
						}

						// push through queue 
						for (int j = 0; j < (termbytes.length - 1); j++) {
								tmp_buf[j] = tmp_buf[j + 1];
						}
						tmp_buf[termbytes.length - 1] = (byte) read;

						if (total_counter++ > result.length) {
								cur_chunk += 1024;
								result = resizeByteArray(result, cur_chunk);

						}

						result[total_counter - 1] = (byte) read;

						//
						if (total_counter >= termbytes.length) {
								// check the match
								boolean found = true;
								for (int j = 0; j < termbytes.length; j++) {
										if (termbytes[j] != tmp_buf[j]) {
												found = false;
												break;
										}
								}

								if (found) {
										break;
								}
						}

				}

				return resizeByteArray(result, total_counter - termbytes.length);
		}

		// reads from socket using timeout in msec until byte is reached
		public static byte[] readFromStreamUntilByteIsReached(int tmsec, InputStream stream, byte termbyte) throws Exception {
				int cur_chunk = 1024;
				byte[] result = new byte[1024];

				int total_counter = 0;

				long timeOut = tmsec > 0 ? System.currentTimeMillis() : -1;

				int read;
				while (true) {
						read = stream.read();
						if (read <= 0) {
								throw new HtException(getContext(), "readFromStreamUntilByteIsReached", "Cannot read from stream");
						}

						if (tmsec > 0 && (System.currentTimeMillis() - timeOut > tmsec)) {
								throw new HtException(getContext(), "readFromStreamUntilByteIsReached", "Timeout exceded when reading from stream");
						}

						if (read == termbyte) {
								break;
						}

						if (total_counter++ > result.length) {
								cur_chunk += 1024;
								result = resizeByteArray(result, cur_chunk);

						}

						result[total_counter - 1] = (byte) read;

				}

				return resizeByteArray(result, total_counter);
		}

		public static byte[] resizeByteArray(byte[] src, int newsize) {
				if (src == null) {
						return null;
				}
				if (src.length == 0) {
						return null;
				}

				byte[] result = new byte[newsize];

				if (newsize >= src.length) {
						System.arraycopy(src, 0, result, 0, src.length);
				} else {
						System.arraycopy(src, 0, result, 0, newsize);
				}

				return result;
		}

		public static <T> String convertCollectionIntoString(Collection<T> c) {
				StringBuilder s = new StringBuilder();

				for (Iterator<T> it = c.iterator(); it.hasNext();) {
						T e = it.next();
						s.append(e.toString()).append("\n");
				}

				return s.toString();
		}

		public static boolean nvl(String value) {
				if (value == null) {
						return true;
				} else if (value.length() == 0) {
						return true;
				}

				return false;
		}

		public static void turnCollectionToUpperCase(Collection<String> source, Collection<String> dest) {
				dest.clear();
				if (source != null) {
						for (String o : source) {
								dest.add(o.toUpperCase());
						}
				}
		}

		// simple searcg ignoring case, returns original found falue
		public static String searchCollectionIgnoreCase(Collection<String> source, String key) {
				if (source == null) {
						return null;
				}
				if (key == null) {
						return null;
				}

				for (String val : source) {
						if (val.equalsIgnoreCase(key)) {
								return val;
						}
				}

				return null;
		}

		public static String whereFrom(Object o) {
				if (o == null) {
						return null;
				}
				Class<?> c = o.getClass();
				ClassLoader loader = c.getClassLoader();
				if (loader == null) {
						// Try the bootstrap classloader - obtained from the System Class Loader.
						loader = ClassLoader.getSystemClassLoader();
						while (loader != null && loader.getParent() != null) {
								loader = loader.getParent();
						}
				}
				if (loader != null) {
						String name = c.getCanonicalName();
						URL resource = loader.getResource(name.replace(".", "/") + ".class");
						if (resource != null) {
								return resource.toString();
						}
				}
				return "Unknown";
		}
		
		
	public static void silentlyCloseInputStream(InputStream is)
	{
		if (is != null) {

			

			try {
				is.close();
			}
			catch(IOException e)
			{
			}
		}
	}
	
	public static void silentlyCloseOutputStream(OutputStream os)
	{
		if (os != null) {

			

			try {
				os.close();
			}
			catch(IOException e)
			{
			}
		}
	}
	
	public static void silentlyCloseReader(Reader ri)
	{
		if (ri != null) {
			try {
				ri.close();
			}
			catch(IOException e)
			{
			}
		}
	}
	
}
