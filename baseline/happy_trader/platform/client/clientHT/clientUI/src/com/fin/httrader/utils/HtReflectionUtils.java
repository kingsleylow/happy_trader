/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import java.io.File;
import java.io.IOException;

import java.util.zip.ZipFile;

/**
 *
 * @author DanilinS
 */
public class HtReflectionUtils {

	public static <T> T simpleObjectCreate(String classname) throws Exception
	{
		Class c = Class.forName(classname);
		return (T)c.newInstance();
	}
	
	public static <T> T jarObjectCreate(String full_jar_path, String classname) throws Exception
	{
		ZipClassLoader loader = new ZipClassLoader(full_jar_path);
		Class c= loader.findClass(classname);
		
		return (T)c.newInstance();
	}



	public static boolean isClassImplementInterface(String full_class_name, Class<?> clazz) {
		
		Class class_i = null;

		try {
			class_i = Class.forName(full_class_name);
		} catch (LinkageError le) {
		} catch (ClassNotFoundException nfe) {
		}

		if (class_i != null) {
			try {
				class_i.asSubclass(clazz);
				return true;
			}
			catch(ClassCastException notimp) {
				return false;
			}

		}

		return false;

	}

	public static boolean isJarClassImplementInterface(ZipFile zipfile, String full_class_name, Class<?> clazz)
	{

		Class class_i = null;
		ZipClassLoader loader = null;

		/*
		
		try {
			
			// default class loader
			class_i  = Class.forName(full_class_name);
		
		} 
		catch (ClassNotFoundException nfe) 
		{
		} 
		catch (NoClassDefFoundError ndfe) 
		{
		} 
		
		if (class_i == null) {
			*/
				try {
			
						loader = new ZipClassLoader(zipfile);
						class_i = loader.findClass(full_class_name);
				} 
				catch (IOException le) 
				{
				} 
				catch (ClassNotFoundException nfe) 
				{
				} 
				catch (NoClassDefFoundError ndfe) 
				{
				} 
				/*
		}
		*/

		if (class_i != null) {
			try {
				class_i.asSubclass(clazz);
				return true;
			}
			catch(ClassCastException notimp) {
				return false;
			}

		}

		return false;

	}



}
