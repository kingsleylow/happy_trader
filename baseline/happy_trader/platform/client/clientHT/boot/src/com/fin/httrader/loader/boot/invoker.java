/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.loader.boot;

import java.io.File;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 * @author Victor_Zoubok
 */
public class invoker {
		
		public static final String MAIN_CLASS_TO_LAUNCH = "com.fin.httrader.HtMain";

		public static Set<File> listFileTree(File dir) {
				Set<File> fileTree = new HashSet<File>();
				for (File entry : dir.listFiles()) {
						if (entry.isFile()) {
								fileTree.add(entry);
						} else {
								fileTree.addAll(listFileTree(entry));
						}
				}
				return fileTree;
		}

		/**
		 * @param args the command line arguments
		 */
		public static void main(String[] args) {
				try {
						// TODO code application logic here
						String HT_SERVER_DIR = System.getenv("HT_SERVER_DIR");
						String HT_DEBUG_LEVEL = System.getenv("HT_DEBUG_LEVEL");
						String HT_CLIENT_DEBUG_LEVEL = System.getenv("HT_CLIENT_DEBUG_LEVEL");
						String WINSTONE_DEBUG_LEVEL = System.getenv("WINSTONE_DEBUG_LEVEL");
						String JAVA_HOME = System.getenv("JAVA_HOME");
						String MYSQL_CONNECTION_URL = System.getenv("MYSQL_CONNECTION_URL");
						String JAVA_MEMORY_PARAM = System.getenv("JAVA_MEMORY_PARAM");
						
						String CLASSPATH = System.getenv("CLASSPATH");
						
						
						
						

						List<URL> urls = new ArrayList();

						Set<File> libFiles= listFileTree(new File(""));
						
						for (File f : libFiles) {
								urls.add(f.toURL());
						}

						URL[] url_arr = new URL[urls.size()];
						for (int i = 0; i < url_arr.length; i++) {
								url_arr[i] = urls.get(i);
								System.out.println("lib: " + url_arr[i].toString());
						}

						
						ClassLoader classloader
							= new CustomClassLoader(
								url_arr,
								ClassLoader.getSystemClassLoader().getParent());
						
						
						Class mainClass = Class.forName(MAIN_CLASS_TO_LAUNCH, true, classloader);
					

						String[] params = {
								MYSQL_CONNECTION_URL,
								HT_CLIENT_DEBUG_LEVEL,
								WINSTONE_DEBUG_LEVEL
						};

						Class[] mainParamsClass = new Class[]{params.getClass()};

						Thread.currentThread().setContextClassLoader(classloader);
						Method main = mainClass.getMethod("main", mainParamsClass);

						
						main.invoke(null, new Object[]{params});

				} catch (Exception e) {
						e.printStackTrace();
				}

		}

}
