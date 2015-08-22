/*
 * RtConfigurationManager.java
 *
  *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.configprops.HtBaseProp;
import com.fin.httrader.configprops.HtStartupConstants;


import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;

import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.logging.Level;
import org.apache.commons.io.FilenameUtils;

/**
 *
 * @author Victor_Zoubok
 * This manager intends to hold configuration of all the types
 */
public class RtConfigurationManager implements RtManagerBase {

	private String baseConfigDir_m = null;
	
	private final Object fileMtx_m = new Object();
	
	static private RtConfigurationManager instance_m = null;

	// this is the main single pointer to static constant configuration
	public static HtStartupConstants StartUpConst = null;

	

	// ---------------------------------------------------
	
	static public RtConfigurationManager create(String baseDir) throws Exception {
		if (instance_m == null) {
			instance_m = new RtConfigurationManager(baseDir);
		}

		return instance_m;
	}

	static public RtConfigurationManager instance() {
		return instance_m;
	}

	protected String getContext() {
		return this.getClass().getSimpleName();
	}

	private RtConfigurationManager(String baseDir) throws Exception {

		baseConfigDir_m = new String(baseDir);
		if (baseConfigDir_m == null || baseConfigDir_m.length() <= 0) {
			throw new HtException(getContext(), getContext(), "Invalid base directory");
		}

		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated, base directory: " + baseConfigDir_m);
		
		getStartupConstants();

	}


	


	/**
	 */


	
	// this returns the instance if entry
	public HtBaseProp getEntry(String basename, String name) throws Exception {

		XmlParameter xmlparameter = readFile(getCompleteFileName(basename, name));

		HtBaseProp prop = HtBaseProp.instantiate(xmlparameter);
		HtXmlParameterSerialization.deserializeObjectFromXmlParameter(prop, xmlparameter);
		prop.setPropertyName(name);

		return prop;
	}

	// update/creates entry
	public void setEntry(String basename, String name, HtBaseProp prop) throws Exception {
		XmlParameter xmlparameter = new XmlParameter();

		HtXmlParameterSerialization.serializeObjectToXmlParameter(prop, xmlparameter);
		xmlparameter.setCommandName(prop.getType());

		writeFile(getCompleteFileName(basename, name), xmlparameter);
	}

	public void removeEntry(String basename, String name) throws Exception {
		removeFile(getCompleteFileName(basename, name));
	}

	public Set<String> getAllEntries(String basename) throws Exception {
		return getEntries(basename);
	}

	public boolean isEntryExists(String basename, String name) throws Exception {
		return isFileExists(getCompleteFileName(basename, name));
	}

	// no base name versions
	public HtBaseProp getEntry(String name) throws Exception {
		return getEntry(null, name);
	}

	public void setEntry(String name, HtBaseProp prop) throws Exception {
		setEntry(null, name, prop);
	}

	public void removeEntry(String name) throws Exception {
		removeEntry(null, name);
	}

	public Set<String> getAllEntries() throws Exception {
		return getAllEntries(null);
	}

	public boolean isEntryExists(String name) throws Exception {
		return isEntryExists(null, name);
	}

	public void shutdown() {
		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	/**
	helpers
	 */
	/**
	 * this returns startup constants
	 *
	*/

	
	private HtStartupConstants getStartupConstants() throws Exception
	{
		if (StartUpConst==null) {


			try {
				StartUpConst = (HtStartupConstants)getEntry(HtBaseProp.ID_GONFIGURE_STARTUP_CONST);
			}
			catch(FileNotFoundException fnfe)
			{
				// intercept file not found and try to create new file
				HtLog.log(Level.INFO, getContext(), "remote_getStartupConstants", "Startup parameters file not found, trying to create the new one...");

				HtStartupConstants nprops = new HtStartupConstants();
				setEntry(HtBaseProp.ID_GONFIGURE_STARTUP_CONST,  nprops);

				HtLog.log(Level.INFO, getContext(), "remote_getStartupConstants", "Created the startup parameters file");

				// read  this file
				StartUpConst = (HtStartupConstants)getEntry(HtBaseProp.ID_GONFIGURE_STARTUP_CONST);

			}


			HtLog.log(Level.INFO, getContext(), "remote_getStartupConstants", "Read the startup parameters file");


		}

	  return StartUpConst;
	}

	// read file, throws an exception if doesn't exist
	private String convertToString(XmlParameter xmlparameter) throws Exception {
		XmlHandler hndl = new XmlHandler();

		StringBuilder out = new StringBuilder();
		hndl.serializeParameter(xmlparameter, out);

		return out.toString();
	}

	private XmlParameter convertFromString(String content) throws Exception {
		XmlParameter xmlparameter = new XmlParameter();
		XmlHandler hndl = new XmlHandler();

		hndl.deserializeParameter(content, xmlparameter);

		return xmlparameter;
	}

	private XmlParameter readFile(String fileName) throws Exception {
		synchronized (fileMtx_m) {
			
			File file = new File(fileName);

			// some kind of crack :)
			// because don't want to fix profiles manually

			//if (!file.exists()) {
			// another attempt
			//	file = new File(this.appendBasePath_NoEncoding(fileName));
			//}

			//if (!file.exists()) {
			//	throw new HtException(getContext(), "readFile", "File doesn't exist: " + file.getAbsolutePath());
			//}

			StringBuilder content = new StringBuilder();
			FileInputStream reader = new FileInputStream(file);
			try {

				byte[] buff = new byte[2048];

				int rd = -1;
				while (true) {
					rd = reader.read(buff);

					if (rd < 0) {
						break;
					}

					ByteBuffer sbuf = ByteBuffer.wrap(buff, 0, rd);
					CharBuffer cbuf = Charset.forName("ISO-8859-1").newDecoder().decode(sbuf);

					content.append(cbuf);
				}
				
			} 
			catch(FileNotFoundException fnfe)
			{
				// propagate that 
				throw new FileNotFoundException(fnfe.getMessage());
			}
			catch (Throwable e)
			{
				throw new HtException(getContext(), "readFile", "Read exception: " + e.getMessage());
			} finally {
				if (reader != null) {
					reader.close();
				}
			}

			return convertFromString(content.toString());
		}
	}

	// write file updating the content
	private void writeFile(String fileName, XmlParameter xmlparameter) throws Exception {
		synchronized (fileMtx_m) {
			File file = new File(fileName);
			(new File(file.getParent())).mkdirs();

			file.createNewFile();
			FileOutputStream writer = new FileOutputStream(file);

			try {
				CharBuffer cb = CharBuffer.wrap(convertToString(xmlparameter));
				ByteBuffer outbytes = Charset.forName("ISO-8859-1").newEncoder().encode(cb);

				writer.write(outbytes.array());

			} catch (Throwable e) {
				throw new HtException(getContext(), "writeFile", "Write exception: " + e.getMessage());
			} finally {
				if (writer != null) {
					writer.close();
				}
			}
		}

	}

	// removes file if exist
	private void removeFile(String fileName) throws Exception {
		synchronized (fileMtx_m) {
			File file = new File(fileName);
			if (file.exists()) {
				boolean res = file.delete();
				if (!res) {
					throw new HtException(getContext(), "removeFile", "File cannot be deleted: " + file.getAbsolutePath());
				}
			}
		}
	}

	private boolean isFileExists(String fileName) throws Exception {
		synchronized (fileMtx_m) {
			File file = new File(fileName);
			return file.exists();
		}
	}

	// read all the files contained under base name
	private Set<String> getEntries(String baseName) {
		synchronized (fileMtx_m) {
			HashSet<String> entries = new HashSet<String>();
			StringBuilder dir = new StringBuilder(baseConfigDir_m).append( File.separatorChar).append( baseName );

			File file = new File(dir.toString());

			File[] flist = file.listFiles();
			if (flist != null && flist.length > 0) {
				for (int i = 0; i < flist.length; i++) {
					String name = flist[i].getName();
					
					if (name.toLowerCase().indexOf(".xml") != -1) {
						entries.add(URLUTF8Encoder.unescape(FilenameUtils.removeExtension(name)));
					}
				}
			}

			return entries;
		}
	}

	private String getCompleteFileName(String basename, String name) {
		
	 if (basename != null) {
				
			StringBuilder result = new StringBuilder(baseConfigDir_m).append(File.separatorChar).
				append(basename).append(File.separatorChar).
				append(URLUTF8Encoder.encode(name)).append(".xml");
			
			return result.toString();
			
		} else {
			
			StringBuilder result = new StringBuilder(baseConfigDir_m).append(File.separatorChar).
				append(URLUTF8Encoder.encode(name)).append(".xml");
			
			return result.toString();
		}
	}

	/*
	private String appendBasePath(String fileName) {
		// not as filename here we have a full path
		File ftmp = new File(fileName);
		String pre_path = ftmp.getParent();
		if (pre_path==null)
			pre_path = "";

		String last_name = FilenameUtils.removeExtension(ftmp.getName());
	
		StringBuilder result = new StringBuilder(baseConfigDir_m).append(File.separatorChar).append(URLUTF8Encoder.encode(last_name)).append(".xml");
		
		return baseConfigDir_m +  File.separatorChar + pre_path + File.separatorChar + URLUTF8Encoder.encode(last_name);
	}

	
	private String appendBasePath_NoEncoding(String fileName) {
		return baseConfigDir_m + File.separatorChar + fileName;
	}
	*/
	
}
