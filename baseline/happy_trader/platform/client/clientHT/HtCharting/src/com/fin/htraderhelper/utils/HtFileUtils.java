/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper.utils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 *
 * @author DanilinS
 */
public class HtFileUtils {

	public static void simpleReadFile(File f, OutputStream out) throws Exception {
		FileInputStream fi = null;
		BufferedInputStream bis = null;

		try {
			fi = new FileInputStream(f);
			bis = new BufferedInputStream(fi);

			copyInputStream(bis, out);
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {
			try {
				if (bis != null) {
					bis.close();
				}

			} catch (Throwable ign2) {
			}

			try {
				if (fi != null) {
					fi.close();
				}

			} catch (Throwable ign) {
			}


		}


	}

	public static void simpleReadFile(File f, StringBuilder out, String encoding) throws Exception {
		FileInputStream fi = new FileInputStream(f);
		out.setLength(0);
		out.append(HtUtils.convertInputStreamToString(fi, encoding));
	}

	public static void copyInputStream(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[1024];
		int len;

		while ((len = in.read(buffer)) >= 0) {
			out.write(buffer, 0, len);
		}
	}

	public static boolean deleteDirectory(File path) {
		if (path.exists()) {
			File[] files = path.listFiles();
			for (int i = 0; i < files.length; i++) {
				if (files[i].isDirectory()) {
					deleteDirectory(files[i]);
				} else {
					files[i].delete();
				}
			}
		}
		return (path.delete());
	}
}
