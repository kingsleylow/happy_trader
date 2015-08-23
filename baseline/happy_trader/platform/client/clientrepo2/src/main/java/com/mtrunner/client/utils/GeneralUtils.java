/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.utils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;
import java.util.Map;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.jsp.PageContext;
import org.apache.commons.io.FileUtils;

/**
 *
 * @author Administrator
 */
public class GeneralUtils {

	private final static String[] to_search_g = {"&", "<", ">", "\"", "'", "\n", null};
	private final static String[] to_subst_g = {"&amp;", "&lt;", "&gt;", "&quot;", "&apos;", "<br>", null};

	public static String dumpMap(Map<String, Object> map) {
		StringBuilder r = new StringBuilder();
		for (Iterator<String> it = map.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			Object obj = map.get(key);
			String obj_s = null;

			if (obj instanceof String) {
				obj_s = (String) obj;
			} else {
				obj_s = obj.toString();
			}
			r.append(key).append("= [ ").append(obj_s).append(" ]\n");

		}

		return r.toString();
	}

	public static void deleteDirectoryQuietly(File dir) {
		try {
			FileUtils.deleteDirectory(dir);
		} catch (IOException e) {
			// n/a
		}
	}

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

	public static String getFullErrorString(Throwable e) {
		StringBuilder out = new StringBuilder();
		String es = e.getMessage();

		out.append(es != null ? es : "N/A");

		out.append("\nError type: ");
		out.append(e.getClass().getSimpleName());
		out.append("\n");

		out.append("\nStacktrace: \n");
		out.append(getErrorStack(e));
		return out.toString();
	}

	public static String getErrorStack(Throwable e) {
		StringBuilder out = new StringBuilder();
		StackTraceElement[] stack = e.getStackTrace();
		if (stack != null) {
			out.append("\n");
			for (int i = 0; i < stack.length; i++) {

				String fname = stack[i].getFileName();
				out.append(fname == null ? "<NO FILE NAME>" : fname);
				out.append("#");
				out.append(stack[i].getLineNumber());
				out.append(" - \"");
				out.append(stack[i].getClassName());
				out.append("@");
				out.append(stack[i].getMethodName());
				out.append("\"");

				if (stack[i].isNativeMethod()) {
					out.append(" native\n");
				} else {
					out.append(" non-native\n");
				}

			}
		} else {
			out.append("Stack trace is not available\n");
		}

		return out.toString();
	}

	public static String createFullBasePath(String base, PageContext pageContext) {
		StringBuilder r = new StringBuilder();

		r.append(pageContext.getRequest().getScheme()).append("://").append(pageContext.getRequest().getServerName()).append(":");
		r.append(pageContext.getRequest().getServerPort()).append(pageContext.getServletContext().getContextPath());

		if (base != null && base.length() > 0) {
			r.append("/").append(base).append("/");
		} else {
			r.append("/");
		}

		return r.toString();
	}

	public static String prepareValidHtml(String src) {
		StringBuilder result = new StringBuilder();
		StringBuilder srcs = new StringBuilder(src);

		for (int i = 0; to_search_g[i] != null; i++) {

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
	
	public static boolean nvl(String data)
	{
		if (data == null)
			return true;
		
		if (data.length() ==0)
			return true;
		
		return false;
	}
	
	public static String extractUserFromURI(HttpServletRequest request, String basePathId)
	{
		String requestURI = request.getRequestURI();
		
		
		String[] splitted = requestURI.split("/");
		for(int i = 0; i < splitted.length; i++) {
			if (splitted[i] != null && splitted[i].equalsIgnoreCase(basePathId)) {
				int j = i + 2;
				
				if (j < splitted.length) {
					return splitted[j];
				}
				
			}
		}
		
		return null;
	}
}
