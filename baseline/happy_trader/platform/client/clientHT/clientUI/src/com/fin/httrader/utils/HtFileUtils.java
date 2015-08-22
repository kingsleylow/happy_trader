/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils;

import au.com.bytecode.opencsv.CSVReader;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.model.HtCommandProcessor;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.jar.JarInputStream;
import java.util.jar.JarOutputStream;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Pattern;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

/**
 *
 * @author DanilinS
 * Some file helper functions
 */
public class HtFileUtils {

	public static String getContext() {
		return HtFileUtils.class.getSimpleName();
	}

	/*
	 * Get the extension of a file.
	 */
	public static String getExtension(File f) {
		String ext = null;
		String s = f.getName();
		int i = s.lastIndexOf('.');

		if (i > 0 && i < s.length() - 1) {
			ext = s.substring(i + 1).toLowerCase();
		}
		return ext;
	}

	/*
	 * This reads files from a directoty, extract class name and optionally checks if it can be cast to clazz
	 */
	
	// this functions checks if we use main jar or not
	public static Set<String> readPackagedFilesIncudingMainJar(String packagename, boolean nonested, Class<?> clazz) throws Exception
	{
			String mainJarPath = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getJavaMainJarPath();
			if (HtUtils.nvl(mainJarPath)) {
					
					// normal way
					return HtFileUtils.readPackagedFiles(packagename, true, clazz);
			}
			else {
					return HtFileUtils.readAllClassesFromJarBasePackage(packagename, mainJarPath, true, clazz );
			}
	}
	
	public static Set<String> readPackagedFiles(String packagename, boolean nonested, Class<?> clazz) {
		HashSet<String> keys = new HashSet<String>();

		File rtdir = Classpath.find_file(Classpath.to_filename(packagename));

		if (rtdir != null) {
			List<String> classnames = Classpath.to_jvm_name(packagename, rtdir.listFiles());
			for (int i = 0; i < classnames.size(); i++) {
				String class_name_i = null;

				if (nonested) {
					if (classnames.get(i).indexOf('$') == -1) {
						class_name_i = classnames.get(i);
					}
				} else {
					class_name_i = classnames.get(i);
				}

				//
				if (class_name_i != null) {

					if (clazz != null) {
						if (HtReflectionUtils.isClassImplementInterface(class_name_i, clazz)) {
							keys.add(class_name_i);
						}
					} else {
						keys.add(class_name_i);
					}
				}

			}
		}
		
		

		return keys;
	}

	
		public static Set<String> readAllClassesFromJarBasePackage(String basePackageName, String full_jar_path, boolean nonested, Class<?> clazz) throws Exception {
		HashSet<String> keys = new HashSet<String>();


		JarFile jarfile = null;

		try {
			jarfile = new JarFile(full_jar_path, false);
			Enumeration<JarEntry> entries = jarfile.entries();

			while (entries.hasMoreElements()) {
				JarEntry entry_i = entries.nextElement();
				String entry_name = entry_i.getName();

				if (entry_name.indexOf(".class") >= 0) {
					String base_name = entry_name.substring(0, entry_name.length() - ".class".length()).replaceAll("/", "\\.");
					String class_name_i = null;

					if (nonested) {
						if (base_name.indexOf('$') == -1) {
							class_name_i = base_name;
						}
					} else {
						// add everything
						class_name_i = base_name;
					}

					//
					if (class_name_i != null) {

						// check with base package name
						if (class_name_i.indexOf(basePackageName) != -1) {
								if (clazz != null) {
									if (HtReflectionUtils.isJarClassImplementInterface(jarfile, class_name_i, clazz)) {
										keys.add(class_name_i);
									}
								} else {
									keys.add(class_name_i);
								}
						}
					}


				}

			}
		} catch (IOException e) {
			throw new HtException(getContext(), "readAllClassesFromJar", "Exception on read jar file: " + e.getMessage());
		} finally {
			if (jarfile != null)
				jarfile.close();
		}



		return keys;
	}

		
	// reads all classes from JAR file
	public static Set<String> readAllClassesFromJar(String full_jar_path, boolean nonested, Class<?> clazz) throws Exception {
		HashSet<String> keys = new HashSet<String>();


		JarFile jarfile = null;

		try {
			jarfile = new JarFile(full_jar_path, false);
			Enumeration<JarEntry> entries = jarfile.entries();

			while (entries.hasMoreElements()) {
				JarEntry entry_i = entries.nextElement();
				String entry_name = entry_i.getName();

				if (entry_name.indexOf(".class") >= 0) {
					String base_name = entry_name.substring(0, entry_name.length() - ".class".length()).replaceAll("/", "\\.");
					String class_name_i = null;

					if (nonested) {
						if (base_name.indexOf('$') == -1) {
							class_name_i = base_name;
						}
					} else {
						// add everything
						class_name_i = base_name;
					}

					//
					if (class_name_i != null) {

						if (clazz != null) {
							if (HtReflectionUtils.isJarClassImplementInterface(jarfile, class_name_i, clazz)) {
								keys.add(class_name_i);
							}
						} else {
							keys.add(class_name_i);
						}
					}


				}

			}
		} catch (IOException e) {
			throw new HtException(getContext(), "readAllClassesFromJar", "Exception on read jar file: " + e.getMessage());
		} finally {
			if (jarfile != null)
				jarfile.close();
		}



		return keys;
	}

	// returns class name <-> jar File containng that class
	public static Map<String, File> readAllClassesFromJarDirectory(String full_dir_path, boolean nonested, Class<?> clazz) throws Exception {
		HashMap<String, File> result = new HashMap<String, File>();


		Set<File> jarFiles = new HashSet<File>();
		HtFileUtils.readAllfilesRecursively("jar", full_dir_path, jarFiles);

		for (Iterator<File> it = jarFiles.iterator(); it.hasNext();) {
			File file_i = it.next();

			Set<String> from_jar_i = HtFileUtils.readAllClassesFromJar(file_i.getAbsolutePath(), nonested, clazz);
			for (Iterator<String> it2 = from_jar_i.iterator(); it2.hasNext();) {
				result.put(it2.next(), file_i);
			}

		}

		return result;
	}

	public static void readAllfilesRecursively(String filterExt, String rootpath, Set<File> result) throws IOException 
	{


		File baseRoot = new File(rootpath);
		if (baseRoot.isDirectory()) {

			File[] herefiles = baseRoot.listFiles();
			if (filterExt != null && filterExt.length() > 0) {

				final String filterExt_s = filterExt.toLowerCase();
				FilenameFilter fnfilter = new FilenameFilter() {

					@Override
					public boolean accept(File dir, String name) {
						StringBuilder path_i = new StringBuilder(dir.getAbsolutePath());
						path_i.append(File.separator);
						path_i.append(name);

						File f = new File(path_i.toString());
						if (f.isDirectory()) {
							return true;
						}

						if (filterExt_s != null && filterExt_s.length() > 0 && name.toLowerCase().endsWith(filterExt_s)) {
							return true;
						} else {
							return false;
						}

					}
				};

				herefiles = baseRoot.listFiles(fnfilter);
			} else {
				herefiles = baseRoot.listFiles();
			}

			if (herefiles != null && herefiles.length > 0) {
				for (int i = 0; i < herefiles.length; i++) {

					if (herefiles[i].isFile()) {
						result.add(herefiles[i].getCanonicalFile());

					} else {
						readAllfilesRecursively(filterExt, herefiles[i].getAbsolutePath(), result);
					}

				}
			}
		}



	}
	
	public static String enforceSeparatorInPath(String basePath)
	{
		if (basePath==null)
			return null;
		
		if (File.separatorChar == '/')
			basePath = basePath.replace('\\', File.separatorChar);
		else if (File.separatorChar == '\\')
			basePath = basePath.replace('/', File.separatorChar);
		
		return basePath;
	}

	public static String getRelativePath(
					String targetPath,
					String basePath,
					String pathSeparator) {

		//  We need the -1 argument to split to make sure we get a trailing
		//  "" token if the base ends in the path separator and is therefore
		//  a directory. We require directory paths to end in the path
		//  separator -- otherwise they are indistinguishable from files.
		String[] base = basePath.split(Pattern.quote(pathSeparator), -1);
		String[] target = targetPath.split(Pattern.quote(pathSeparator), 0);

		//  First get all the common elements. Store them as a string,
		//  and also count how many of them there are.
		String common = "";
		int commonIndex = 0;
		for (int i = 0; i
						< target.length && i < base.length; i++) {
			if (target[i].equals(base[i])) {
				common += target[i] + pathSeparator;
				commonIndex++;

			} else {
				break;
			}

		}

		if (commonIndex == 0) {
			//  Whoops -- not even a single common path element. This most
			//  likely indicates differing drive letters, like C: and D:.
			//  These paths cannot be relativized. Return the target path.
			return targetPath;
			//  This should never happen when all absolute paths
			//  begin with / as in *nix.
		}

		String relative = "";
		if (base.length == commonIndex) {
			//  Comment this out if you prefer that a relative path not start with ./
			//relative = "." + pathSeparator;
		} else {
			int numDirsUp = base.length - commonIndex - 1;
			//  The number of directories we have to backtrack is the length of
			//  the base path MINUS the number of common path elements, minus
			//  one because the last element in the path isn't a directory.
			for (int i = 1; i <= (numDirsUp); i++) {
				relative += ".." + pathSeparator;
			}

		}
		relative += targetPath.substring(common.length());

		return relative;
	}

// write arbitradry content
	public static boolean writeContentToFile(String fileName, String content) {
		boolean result = false;
		try {
			FileOutputStream f = new FileOutputStream(fileName);
			f.write(content.getBytes());
			f.flush();
			f.close();

			result = true;

		} catch (Throwable e) {
			// na
			HtLog.log(Level.WARNING, getContext(), "writeContentToFile", "Exception: " + e.getMessage());
		}

		return result;
	}

	public static boolean writeContentToFile(String fileName, byte[] content) {
		boolean result = false;
		try (FileOutputStream f = new FileOutputStream(fileName)) {
			
			f.write(content);
			f.flush();
			f.close();

			result = true;

		} 
		catch (IOException e) {
			// na
			HtLog.log(Level.WARNING, getContext(), "writeContentToFile", "Exception: " + e.getMessage());
		}

		return result;
	}

// write arbitradry content
	public static boolean writeContentToFile(File sfile, String content) {
		boolean result = false;
		try (FileOutputStream f = new FileOutputStream(sfile)) {
			
			f.write(content.getBytes());
			f.flush();
			f.close();

			result = true;

		} catch (IOException e) {
			// na
			HtLog.log(Level.WARNING, getContext(), "writeContentToFile", "Exception: " + e.getMessage());
		}

		return result;
	}

	public static void simpleReadFile(File f, OutputStream out) throws Exception {
		FileInputStream fi = null;
		BufferedInputStream bis = null;

		try {
			fi = new FileInputStream(f);
			bis = new BufferedInputStream(fi);

			copyInputStream(bis, out);
		} catch (Throwable e) {
			throw new HtException(getContext(), "simpleReadFile", "Exception on read file: " + f.getAbsolutePath() + " - \"" + e.getMessage() + "\"");
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

	public static void copyFile(String srFile, String dtFile) throws Exception {
		InputStream in = null;
		OutputStream out = null;
		try {
			File f1 = new File(srFile);
			File f2 = new File(dtFile);
			in = new FileInputStream(f1);

			//For Append the file.
//      OutputStream out = new FileOutputStream(f2,true);

			//For Overwrite the file.
			out = new FileOutputStream(f2);

			byte[] buf = new byte[1024];
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}

		} catch (Throwable ex) {
			throw new HtException(getContext(), "copyFile", ex.getMessage());
		} finally {
			try {
				if (in != null) {
					in.close();
				}
			} catch (Throwable i1) {
			}


			try {
				if (out != null) {
					out.close();
				}
			} catch (Throwable i1) {
			}

		}
	}

	// - zip dir
	public static void zipDir(String dir2zip, ZipOutputStream zos) throws Exception {
		try {
			//create a new File object based on the directory we have to zip
			File zipDir = new File(dir2zip);
			//get a listing of the directory content
			String[] dirList = zipDir.list();
			byte[] readBuffer = new byte[2156];
			int bytesIn = 0;
			//loop through dirList, and zip the files
			for (int i = 0; i < dirList.length; i++) {
				File f = new File(zipDir, dirList[i]);
				if (f.isDirectory()) {
					//if the File object is a directory, call this
					//function again to add its content recursively
					String filePath = f.getPath();
					zipDir(filePath, zos);
					//loop again
					continue;
				}
				//if we reached here, the File object f was not   a directory
				//create a FileInputStream on top of f
				FileInputStream fis = new FileInputStream(f);
				//create a new zip entry
				ZipEntry anEntry = new ZipEntry(f.getName());
				//place the zip entry in the ZipOutputStream object
				zos.putNextEntry(anEntry);
				//now write the content of the file to the ZipOutputStream
				while ((bytesIn = fis.read(readBuffer)) != -1) {
					zos.write(readBuffer, 0, bytesIn);
				}
				//close the Stream
				fis.close();
			}
		} catch (Exception e) {
			//handle exception
			throw new HtException(getContext(), "zipDir", "Exception occured: " + e.getMessage());

		} finally {
			zos.close();
		}
	}

	public static String removeExtention(String fileName) {
		int idx = fileName.lastIndexOf('.');
		if (idx >= 0) {
			return fileName.substring(0, idx);
		} else {
			return fileName;
		}

	}

	public static File createTempDirectory() throws IOException {
		final File temp;

		temp = File.createTempFile("temp", Long.toString(System.nanoTime()));

		if (!(temp.delete())) {
			throw new IOException("Could not delete temp file: " + temp.getAbsolutePath());
		}

		if (!(temp.mkdir())) {
			throw new IOException("Could not create temp directory: " + temp.getAbsolutePath());
		}

		return (temp);
	}

	public static Set<String> readSymbolListFromFile(String filename) throws Exception {
		HashSet<String> result = new HashSet<String>();
		CSVReader reader = null;

		try {
			reader = new CSVReader(new FileReader(filename), ',');
			String[] nextLine;

			while ((nextLine = reader.readNext()) != null) {
				if (nextLine[0].length() == 0) {
					continue;
				}

				String trimmed = nextLine[0].trim();
				if (trimmed != null && trimmed.length() > 0) {
					result.add(trimmed);
				}
			}
		} catch (Throwable e) {
			throw new HtException(getContext(), "readSymbolListFromFile", "Exception on processing file: " + filename + " - " + e.getMessage());
		} finally {
			if (reader != null) {
				reader.close();
			}
		}



		return result;
	}
	
	// multicolumn vesrion
	// column idx begins with 0
	public static Set<String> readSymbolListFromFile(String filename, int columnIdx) throws Exception {
		HashSet<String> result = new HashSet<String>();
		CSVReader reader = null;

		try {
			reader = new CSVReader(new FileReader(filename), ',');
			String[] nextLine;

			while ((nextLine = reader.readNext()) != null) {
				if (nextLine.length <= columnIdx)
				  continue;

				String trimmed = nextLine[columnIdx].trim();
				if (trimmed != null && trimmed.length() > 0) {
					result.add(trimmed);
				}
			}
		} catch (Throwable e) {
			throw new HtException(getContext(), "readSymbolListFromFile", "Exception on processing file: " + filename + " - " + e.getMessage());
		} finally {
			if (reader != null) {
				reader.close();
			}
		}



		return result;
	}

	public static void addFileToExistingJar(File zipFile, File f, String entry_name) throws IOException {
		// get a temp file
		File tempFile = File.createTempFile(zipFile.getName(), null);
		// delete it, otherwise you cannot rename your existing zip to it.
		tempFile.delete();

		boolean renameOk = zipFile.renameTo(tempFile);
		if (!renameOk) {
			throw new RuntimeException("could not rename the file " + zipFile.getAbsolutePath() + " to " + tempFile.getAbsolutePath());
		}
		byte[] buf = new byte[1024];

		JarInputStream zin = new JarInputStream(new FileInputStream(tempFile));
		JarOutputStream out = new JarOutputStream(new FileOutputStream(zipFile));

		String entry_name_check = (entry_name != null ? entry_name : f.getName());

		JarEntry entry = (JarEntry) zin.getNextEntry();
		while (entry != null) {
			String name = entry.getName();
			boolean notInFiles = true;

			if (entry_name_check.equals(name)) {
				notInFiles = false;
				break;
			}

			if (notInFiles) {
				// Add ZIP entry to output stream.
				out.putNextEntry(new ZipEntry(name));
				// Transfer bytes from the ZIP file to the output file
				int len;
				while ((len = zin.read(buf)) > 0) {
					out.write(buf, 0, len);
				}
			}
			entry = (JarEntry) zin.getNextEntry();
		}
		// Close the streams
		zin.close();
		// Compress the files

		InputStream in = new FileInputStream(f);
		// Add ZIP entry to output stream.
		out.putNextEntry(new JarEntry(entry_name_check));
		// Transfer bytes from the file to the ZIP file
		int len;
		while ((len = in.read(buf)) > 0) {
			out.write(buf, 0, len);
		}
		// Complete the entry
		out.closeEntry();
		in.close();

		// Complete the ZIP file
		out.close();
		tempFile.delete();
	}

	public static byte[] getBytesFromFile(File file) throws IOException {
		InputStream is = new FileInputStream(file);

		// Get the size of the file
		long length = file.length();

		// You cannot create an array using a long type.
		// It needs to be an int type.
		// Before converting to an int type, check
		// to ensure that file is not larger than Integer.MAX_VALUE.
		if (length > Integer.MAX_VALUE) {
			// File is too large
		}

		// Create the byte array to hold the data
		byte[] bytes = new byte[(int) length];

		// Read in the bytes
		int offset = 0;
		int numRead = 0;
		while (offset < bytes.length && (numRead = is.read(bytes, offset, bytes.length - offset)) >= 0) {
			offset += numRead;
		}

		// Ensure all the bytes have been read in
		if (offset < bytes.length) {
			throw new IOException("Could not completely read file " + file.getName());
		}

		// Close the input stream and return bytes
		is.close();
		return bytes;
	}
	

}
