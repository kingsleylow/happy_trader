/*
 * HtSystem.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;

/**
 *
 * @author victor_zoubok
 * Helper class responsible for some system-dependent functionality
 */
public class HtSystem {

	/** Creates a new instance of HtSystem */
	public HtSystem() {
	}

	public static String getEnvironmentValue(String env) {
		try {

			String result = System.getenv(env);
			if (result == null) {
				return "";
			}

			return result;

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, "HtSystem", "getEnvironmentValue", e.getMessage());

		}

		return "";

	}

	public static void getAllEnvironmentEntries(Map<String, String> outentries) throws Exception {
		try {
			outentries.putAll(System.getenv());
		} catch (Throwable e) {
			throw new HtException("HtSystem", "getEnvironmentValue", e);
		}

	}

	public static String getSystemTempPath() {
		return System.getProperty("java.io.tmpdir");
	}

	public static String resolveJavaHome() {
		String sysJavaHome = System.getProperty("java.home");

		int idx = sysJavaHome.indexOf("jre");
		if (idx >= 0) {
			idx--;
			return (sysJavaHome.substring(0, idx));
		} else {
			return sysJavaHome;
		}
	}

	public static String resolveCurrentJavaJVMDLL() {
		String home = HtSystem.resolveJavaHome() + File.separatorChar
						+ "jre" + File.separatorChar
						+ "bin" + File.separatorChar
						+ "client" + File.separatorChar
						+ "jvm.dll";

		return home;

	}

	public static void getAllRunningThreads(List<Thread> descrlist) {
		descrlist.clear();

		ThreadGroup root = Thread.currentThread().getThreadGroup().getParent();
		while (root.getParent() != null) {
			root = root.getParent();
		}

		// Visit each thread group
		visit(descrlist, root, 0);


	}

	// This method recursively visits all thread groups under `group'.
	public static void visit(List<Thread> descrlist, ThreadGroup group, int level) {
		// Get threads in `group'
		int numThreads = group.activeCount();
		Thread[] threads = new Thread[numThreads * 2];
		numThreads = group.enumerate(threads, false);

		// Enumerate each thread in `group'
		for (int i = 0; i < numThreads; i++) {
			// Get thread
		
			descrlist.add(threads[i]);
		}

		// Get thread subgroups of `group'
		int numGroups = group.activeGroupCount();
		ThreadGroup[] groups = new ThreadGroup[numGroups * 2];
		numGroups = group.enumerate(groups, false);

		// Recursively visit each subgroup
		for (int i = 0; i < numGroups; i++) {
			visit(descrlist, groups[i], level + 1);
		}
	}
}
