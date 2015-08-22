/*
 * RtExternalApiRunManager.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.managers;

import com.fin.httrader.externalapi.HtExtException;
import com.fin.httrader.externalapi.HtExtRunner;
import com.fin.httrader.externalapi.HtExtScriptLogger;
import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.hlpstruct.externalrunstruct.*;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.*;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.logging.Level;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

/**
 *
 * @author Victor_Zoubok
 */
public class RtExternalApiRunManager implements RtManagerBase {

	private static final String XML_DESCRIPTOR_FILE = "run.xml";

	/*
	 * // if we decide a thread run - just use this // note that job is not
	 * cancelable private class RunningThread extends Thread {
	 *
	 * private RtExternalApiRunManager manager_m = null; private Event started_m =
	 * new Event(); private String packageLoadPath_m = null; private String
	 * packageName_m = null; private Uid uid_m = new Uid();
	 *
	 * // -------------- public RunningThread(RtExternalApiRunManager base,
	 * String packageLoadPath, String packageName, Uid runUid) throws Exception {
	 * manager_m = base; packageLoadPath_m = packageLoadPath; packageName_m =
	 * packageName; uid_m.fromUid(runUid);
	 *
	 *
	 *
	 * // start the the thread start();
	 *
	 * if (!started_m.waitEvent(1000)) { throw new HtException("RunningThread",
	 * "RunningThread", "Running thread cannot be started"); }
	 *
	 * }
	 *
	 * public String getPackageFullPath() { return packageLoadPath_m; }
	 *
	 * public boolean isFinished() { return !isAlive(); }
	 *
	 * public void run() { started_m.signal();
	 *
	 * //addStringEntry(res, "Started at: " +
	 * HtUtils.time2SimpleString_Gmt(HtUtils.getCurGmtTime()) + " of id: " +
	 * runId);
	 *
	 * //Class clazz = null;
	 *
	 *
	 * try { List<RunClassEntry> classesToLoad =
	 * manager_m.resolveDescriptorFromJar(packageLoadPath_m);
	 *
	 * for (int i = 0; i < classesToLoad.size(); i++) { RunClassEntry class_i =
	 * classesToLoad.get(i); //addStringEntry(res, "Will execute the class: <div
	 * style=\"background-color:lawngreen;\">" + class_i.className_m + "</div>");
	 *
	 * try {
	 *
	 *
	 * //Arguments to be passed into method HtExtLogger newLogger = null;
	 *
	 *
	 * manager_m.run_calling_thread(packageLoadPath_m, class_i, newLogger);
	 *
	 * } catch (HtExtException e0) { //addErrorEntry(res, "Execution error: " +
	 * class_i + " : " + HtException.getFullErrorString(e0));
	 * XmlEvent.createSimpleLog(uid_m.toString(), class_i.getClassName(),
	 * CommonLog.LL_ERROR, HtException.getFullErrorString(e0));
	 * HtLog.log(Level.WARNING, getContext(), "run", "Execution error: " +
	 * class_i.getClassName() + " : " + HtException.getFullErrorString(e0));
	 *
	 * } catch (Throwable e1) {
	 *
	 * //addErrorEntry(res, "Common error on executing the class: " +
	 * class_i.className_m.toString() + " : " +
	 * HtException.getFullErrorString(e1));
	 * XmlEvent.createSimpleLog(uid_m.toString(), class_i.getClassName(),
	 * CommonLog.LL_ERROR, "Common error in running context thread of ext API of
	 * id: " + uid_m.toString() + " - " + e1.getMessage());
	 *
	 * HtLog.log(Level.WARNING, getContext(), "run", "Common error in running
	 * context thread of ext API of id: " + uid_m.toString() + " - " +
	 * e1.getMessage()); } }
	 *
	 * } catch (Throwable e2) { //addErrorEntry(res, "Error on loading the
	 * package: " + uid_m.toString() + " : " +
	 * HtException.getFullErrorString(e2));
	 * XmlEvent.createSimpleLog(uid_m.toString(), packageName_m,
	 * CommonLog.LL_ERROR, "Exception in loading the package: " + packageName_m +
	 * " when running context thread of ext API of id: " + uid_m.toString() + " -
	 * " + e2.getMessage());
	 *
	 * HtLog.log(Level.WARNING, getContext(), "run", "Exception in loading the
	 * package: " + packageName_m + " when running context thread of ext API of
	 * id: " + uid_m.toString() + " - " + e2.getMessage()); }
	 *
	 *
	 * //clazz = null;
	 *
	 * // deregister thread manager_m.deregisterThread(this);
	 *
	 *
	 * // remove temp file //manager_m.removeTempFile(packageLoadPath_m);
	 *
	 *
	 * }
	 * };
	 */
	// --------------------------
	//private final Set<RunningThread> runningThreads_m = new HashSet<RunningThread>();
	static private RtExternalApiRunManager instance_m = null;

	static public RtExternalApiRunManager create() throws Exception {
		if (instance_m == null) {
			instance_m = new RtExternalApiRunManager();
		}

		return instance_m;
	}

	static public RtExternalApiRunManager instance() {

		return instance_m;
	}

	protected String getContext() {
		return getClass().getSimpleName();
	}

	/**
	 * Creates a new instance of RtExternalApiRunManager
	 */
	private RtExternalApiRunManager() {
		HtLog.log(Level.INFO, getContext(), getContext(), "Instantiated");
	}

	/*
	 * public void startThreadJob(String packageLoadPath, String packageName, Uid
	 * runUid) throws Exception { RunningThread p = new RunningThread(this,
	 * packageLoadPath, packageName, runUid); synchronized (runningThreads_m) {
	 * runningThreads_m.add(p); } }
	 *
	 * public void deregisterThread(RunningThread p) { synchronized
	 * (runningThreads_m) { runningThreads_m.remove(p); } }
	 *
	 */
	// this just executes whol jar
	public void executeWholeJarInCallingThread(HtExtScriptLogger logger, Uid runUid) throws Exception {
		if (!runUid.isValid()) {
			runUid.generate();
		}

		String runUidS = runUid.toString();

		ExternalScriptStructure estruct = RtExternalApiRunManager.instance().queryScriptRun(runUid);
		estruct.setStatus(ExternalScriptStructure.STATUS_RUN);
		HtCommandProcessor.instance().get_HtExternalApiProxy().remote_updateEntry(estruct);
		
		StringBuilder customOutput = new StringBuilder();

		try {

			//Arguments to be passed into method

			logger.addStringEntry("Started at: " + HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()) + " of id: " + runUidS);

			RunPackageDescriptor rdescriptor = RtExternalApiRunManager.instance().resolveDescriptorFromJar(runUid);

			for (int i = 0; i < rdescriptor.classEntries_m.size(); i++) {
				RunClassEntry class_i = rdescriptor.classEntries_m.get(i);
				logger.addStringEntry("Will execute the class: <div style=\"background-color:lawngreen;\">" + class_i.getClassName() + "</div>");

				try {

					run_calling_thread(runUid, class_i, logger, rdescriptor.runKeys_m,customOutput);

				} catch (HtExtException e0) {
					logger.addErrorEntry("Execution error: " + class_i + " : " + HtException.getFullErrorString(e0));
					XmlEvent.createSimpleLog(runUidS, class_i.getClassName(), CommonLog.LL_ERROR, HtException.getFullErrorString(e0));
					HtLog.log(Level.WARNING, getContext(), "run", "Execution error: " + class_i.getClassName() + " : " + HtException.getFullErrorString(e0));

				} catch (Throwable e1) {

					logger.addErrorEntry("Common error on executing the class: " + class_i.getClassName() + " : " + HtException.getFullErrorString(e1));
					XmlEvent.createSimpleLog(runUidS, class_i.getClassName(), CommonLog.LL_ERROR, "Common error in running context thread of ext API of id: " + runUidS + " - " + e1.getMessage());

					HtLog.log(Level.WARNING, getContext(), "run", "Common error in running context thread of ext API of id: " + runUidS + " - " + e1.getMessage());
				}
			}

		} catch (Throwable e2) {
			logger.addErrorEntry("Error on loading the package: " + estruct.getName() + " : " + HtException.getFullErrorString(e2));
			XmlEvent.createSimpleLog(runUidS, estruct.getName(), CommonLog.LL_ERROR, "Exception in loading the package: " + estruct.getName() + " when running context thread of ext API of id: " + runUidS + " - " + e2.getMessage());

			HtLog.log(Level.WARNING, getContext(), "run", "Exception in loading the package: " + estruct.getName() + " when running context thread of ext API of id: " + runUidS + " - " + e2.getMessage());
		}

		//

		if (logger != null) {
			logger.finishLogWriting();
		}


		estruct.setStatus(ExternalScriptStructure.STATUS_FINISHED);
		estruct.setCustomresultString(customOutput.toString());
		estruct.setEndTime(HtDateTimeUtils.getCurGmtTime());
		HtCommandProcessor.instance().get_HtExternalApiProxy().remote_updateEntry(estruct);

	}

	public void updateXmlDescriptorFromJar(String packageLoadPath, String newXml) throws Exception {
		JarFile jarfile = null;
		try {


			File ftmp_xml = File.createTempFile("new_xml_jar", new Uid().toString());
			if (!HtFileUtils.writeContentToFile(ftmp_xml, newXml)) {
				throw new HtException(getContext(), "updateXmlDescriptorFromJar", "Cannot write XML content to tmp file: " + ftmp_xml.getAbsolutePath());
			}


			File fi = new File(packageLoadPath);
			if (!fi.exists()) {
				throw new HtException(getContext(), "updateXmlDescriptorFromJar", "Jar file does not exist: " + packageLoadPath);
			}


			HtFileUtils.addFileToExistingJar(fi, ftmp_xml, XML_DESCRIPTOR_FILE);

			ftmp_xml.delete();

		} catch (Throwable e) {
			throw new HtException(getContext(), "updateXmlDescriptorFromJar", "Exception: " + e.getMessage());
		} finally {
			try {
				if (jarfile != null) {
					jarfile.close();
				}
			} catch (Throwable ign1) {
			}
		}

	}

	/*
	public String getXmlDescriptorFromJar(String packageLoadPath) throws Exception {

		JarFile jarfile = null;
		try {
			jarfile = new JarFile(packageLoadPath, false);

			JarEntry descriptor_xml = jarfile.getJarEntry(XML_DESCRIPTOR_FILE);
			if (descriptor_xml == null) {
				throw new HtException(getContext(), "getXmlDescriptorFromJar", "The package must contain \"run.xml\" descriptor: " + packageLoadPath);
			}

			InputStream inp = jarfile.getInputStream(descriptor_xml);
			return HtUtils.convertInputStreamToString(inp, null);

		} catch (Throwable e) {
			throw new HtException(getContext(), "getXmlDescriptorFromJar", "Exception: " + e.getMessage());
		} finally {
			try {
				if (jarfile != null) {
					jarfile.close();
				}
			} catch (Throwable ign1) {
			}
		}


	}
	* 
	*/ 

	public RunPackageDescriptor resolveDescriptorFromJar(Uid runUid) throws Exception {
		File rf = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory()
						+ File.separatorChar + runUid.toString() + ".jar");

		if (!rf.exists()) {
			throw new HtException(getContext(), "resolveClassesFromJar", "The jar file for the external package identified by UID :\"" + runUid + "\" doesn't exist");
		}

		return resolveDescriptorFromJar(rf.getAbsolutePath());
	}

	/*
	 * public void removeTempFile(String packageLoadPath) { try { File f = new
	 * File(packageLoadPath); if (f != null) { if (f.exists()) { boolean result =
	 * f.delete();
	 *
	 * if (!result) { HtLog.log(Level.INFO, getContext(), "removeTempFile",
	 * "Cannot delete temp file: " + f.getAbsolutePath()); } }
	 *
	 * f = null; } } catch (Throwable e) { HtLog.log(Level.INFO, getContext(),
	 * "removeTempFile", "Can't delete temp file: " + e.getMessage()); }
	 *
	 *
	 * }
	 *
	 */
	public void copyJarFileToStream(OutputStream out, Uid runUid) throws Exception {


		HtFileUtils.simpleReadFile(resolveJarFilePath(runUid), out);
	}

	public void copyLogFileToStream(OutputStream out, Uid runUid) throws Exception {

		HtFileUtils.simpleReadFile(resolveHtmlLogFilePath(runUid, true), out);
	}

	public void removeEntry(Uid uid) throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		String sql = "delete from SCRIPTS_RUNS where run_id = ?";
		PreparedStatement stmt = null;

		try {
			stmt = conn.prepareStatement(sql);
			stmt.setString(1, uid.toString());
			stmt.execute();


			conn.commit();
		} catch (SQLException sqe) {
			throw new HtException(getContext(), "removeEntry", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}

			// remove files from file system
			removeFiles(uid);
		}
	}

	// have a lot of functions working with DB
	public void insertNewEntry(ExternalScriptStructure estruct) throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		String sql = "insert into SCRIPTS_RUNS(run_id, run_status, start_time, finish_time, name) values (?,?,?,?,?)";
		PreparedStatement stmt = null;

		try {
			stmt = conn.prepareStatement(sql);
			stmt.setString(1, estruct.getUid().toString());
			stmt.setInt(2, estruct.getStatus());
			stmt.setDouble(3, estruct.getBeginTime());
			stmt.setDouble(4, estruct.getEndTime());
			stmt.setString(5, estruct.getName());

			stmt.execute();
			conn.commit();


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "insertNewEntry", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
	}

	public void updateEntry(ExternalScriptStructure estruct) throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		String sql = "update SCRIPTS_RUNS set run_status=?, start_time=?, finish_time=?, name=?, custom_result_string=? where run_id=?";
		PreparedStatement stmt = null;

		try {
			stmt = conn.prepareStatement(sql);


			stmt.setInt(1, estruct.getStatus());
			stmt.setDouble(2, estruct.getBeginTime());
			stmt.setDouble(3, estruct.getEndTime());
			stmt.setString(4, estruct.getName());
			stmt.setString(5, estruct.getCustomresultString());

			stmt.setString(6, estruct.getUid().toString());
			

			stmt.execute();
			conn.commit();


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "updateEntry", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}
	}

	public ExternalScriptStructure queryScriptRun(Uid runUid) throws Exception {
		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		String sql = "select run_id, run_status, start_time, finish_time, name, custom_result_string from SCRIPTS_RUNS where run_id=?";
		PreparedStatement stmt = null;

		try {
			stmt = conn.prepareStatement(sql);
			stmt.setString(1, runUid.toString());

			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				ExternalScriptStructure estruct = new ExternalScriptStructure(
						new Uid(rs.getString(1)), rs.getInt(2), rs.getDouble(3), rs.getDouble(4), rs.getString(5), rs.getString(6)
				);
				return estruct;

			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "queryScriptRun", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}

		return null;
	}

	public List<ExternalScriptStructure> queryScriptRuns() throws Exception {
		List<ExternalScriptStructure> result = new ArrayList<ExternalScriptStructure>();

		Connection conn = RtDatabaseManager.instance().resolveSQLConnection();
		String sql = "select run_id, run_status, start_time, finish_time, name, custom_result_string from SCRIPTS_RUNS order by start_time desc";
		PreparedStatement stmt = null;

		try {
			stmt = conn.prepareStatement(sql);
			ResultSet rs = stmt.executeQuery();
			while (rs.next()) {
				ExternalScriptStructure estruct = new ExternalScriptStructure(
						new Uid(rs.getString(1)), rs.getInt(2), rs.getDouble(3), rs.getDouble(4), rs.getString(5), rs.getString(6)
				);
				
				result.add(estruct);

			}


		} catch (SQLException sqe) {
			throw new HtException(getContext(), "queryScriptRuns", "SQL exception in \"" + sql.toString() + "\" happened: " + sqe.getMessage());
		} catch (Throwable e) {
			throw new Exception(e);
		} finally {

			if (stmt != null) {
				stmt.close();
			}

			if (conn != null) {
				conn.close();
			}
		}

		return result;
	}

	public File resolveJarFilePath(Uid uid) throws Exception {
		File rf = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory()
						+ File.separatorChar + uid.toString() + ".jar");

		if (!rf.exists()) {
			throw new HtException(getContext(), "resolveJarFilePath", "The jar file for the external package identified by UID :\"" + uid.toString() + "\" doesn't exist");
		}

		return rf;

	}

	public File resolveTemplateJarFilePath(String tmplFileName) throws Exception {
		File rf = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalTemplatesScriptsDirectory()
						+ File.separatorChar + tmplFileName);

		if (!rf.exists()) {
			throw new HtException(getContext(), "resolveTemplateJarFilePath", "Template jar does not exist: " + tmplFileName);
		}

		return rf;

	}

	public File resolveHtmlLogFilePath(Uid uid, boolean mustexist) throws Exception {
		File rf = new File(HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory()
						+ File.separatorChar + uid.toString() + ".html");

		if (mustexist && (!rf.exists())) {
			throw new HtException(getContext(), "resolveHtmlLogFilePath", "The log HTML file for the external package identified by UID :\"" + uid.toString() + "\" doesn't exist");
		}

		return rf;

	}

	public Set<File> getListOfTemplateJars() throws Exception {
		Set<File> result = new TreeSet<File>();

		HtFileUtils.readAllfilesRecursively("jar", HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalTemplatesScriptsDirectory(), result);

		return result;
	}

	public void removeTemplateJar(String fileName) throws Exception {

		resolveTemplateJarFilePath(fileName).delete();

	}

	public Uid copyTemplateToWorkingDirectory(Uid newUid, String templateName) throws Exception {
		Uid resultUid = null;

		if (newUid == null) {
			resultUid = new Uid();
		} else {
			resultUid = newUid;
		}


		if (!resultUid.isValid()) {
			resultUid.generate();
		}

		HtFileUtils.copyFile(
						resolveTemplateJarFilePath(templateName).getAbsolutePath(),
						HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalScriptsDirectory() + File.separatorChar + resultUid.toString() + ".jar");

		return resultUid;
	}

	// ----------------------------------------------
	@Override
	public void shutdown() {

		HtLog.log(Level.INFO, getContext(), "shutdown", "Finished");
	}

	// ---------------------------------------------
	// helpers
	// ---------------------------------------------
	private void removeFiles(Uid uid) {
		try {
			File html_file = resolveHtmlLogFilePath(uid, false);

			html_file.delete();

			File jar_file = this.resolveJarFilePath(uid);

			jar_file.delete();


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "removeFiles", "Exception: " + e.getMessage());
		}
	}

	public RunPackageDescriptor resolveDescriptorFromJar(String packageLoadPath) throws Exception {

		
		RunPackageDescriptor r = new RunPackageDescriptor();

		JarFile jarfile = null;

		// found file - need to parse it
		InputStream inp = null;
		try {

			jarfile = new JarFile(packageLoadPath, false);


			JarEntry descriptor_xml = jarfile.getJarEntry(XML_DESCRIPTOR_FILE);
			if (descriptor_xml == null) {
				throw new HtException(getContext(), "resolveClassesFromJar", "The package must contain \"run.xml\" descriptor: " + packageLoadPath);
			}

			DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
			DocumentBuilder db = dbf.newDocumentBuilder();

			


			// parse it
			inp = jarfile.getInputStream(descriptor_xml);
			r.xmlDescriptor_m.append(HtUtils.convertInputStreamToString(inp, null));
			inp.close();
			
			// obtain the stream the second time
			inp = jarfile.getInputStream(descriptor_xml);
			
			Document dom = db.parse(inp);

			Element docEle = dom.getDocumentElement();
			
			// get desriptor;
			
			// module version
			r.moduleVersion_m.append( getFirstElementByTagName(docEle, "moduleversion").getTextContent().trim() );
			
			// list of startip parameters
			NodeList allvalues_list = docEle.getElementsByTagName("allvalues");
			if (allvalues_list.getLength() > 0) {
			  Element allvalues_element = (Element) allvalues_list.item(0);
			  
			  NodeList param_list = allvalues_element.getElementsByTagName("param");
			  for (int i = 0; i < param_list.getLength(); i++) {
				Element param_i = (Element) param_list.item(i);
				
				
				String key_i = getFirstElementByTagName(param_i, "key").getTextContent().trim();
				String value_i = getFirstElementByTagName(param_i, "value").getTextContent().trim();
				
				r.runKeys_m.put(key_i, value_i);
			  }
			}
			
			

			// classes only
			NodeList class_list = docEle.getElementsByTagName("class");


			for (int i = 0; i < class_list.getLength(); i++) {

				//get the employee element
				Element class_i = (Element) class_list.item(i);


				// single
				NodeList name_list = class_i.getElementsByTagName("name");
				if (name_list.getLength() > 0) {
					// expected to be a single
					Element name_i = (Element) name_list.item(0);

					// check is_run attribute
					// by default=true
					RunClassEntry class_entry_i = null;
					String is_run_attr = name_i.getAttribute("is_run");
					if (is_run_attr != null) {
						if (is_run_attr.equalsIgnoreCase("true") || (is_run_attr.length() <= 0)) {
							class_entry_i = new RunClassEntry(name_i.getTextContent().trim());
							r.classEntries_m.add(class_entry_i);
						}
					}

					if (class_entry_i == null) {
						continue;
					}


					NodeList parameter_list = class_i.getElementsByTagName("parameters");
					if (parameter_list.getLength() > 0) {
						Element parameter_i = (Element) parameter_list.item(0);

						// actual list of parameters
						NodeList parameter_actual_list = parameter_i.getElementsByTagName("param");
						for (int t = 0; t < parameter_actual_list.getLength(); t++) {
							Element param_t = (Element) parameter_actual_list.item(t);

							// key & value
							NodeList key_list_t = param_t.getElementsByTagName("key");
							if (key_list_t.getLength() > 0) {
								Element key_t = (Element) key_list_t.item(0);

								NodeList value_list_t = param_t.getElementsByTagName("value");
								if (value_list_t.getLength() > 0) {
									Element value_t = (Element) value_list_t.item(0);
									

									String key_t_s = key_t.getTextContent().trim();
									if (key_t_s.length() <=0 )
									  throw new HtException(getContext(), "resolveClassesFromJar", "Invalid key");
									RunParameter rp = null;

									// resolve type
									// if we have <list> entries inside...
									NodeList value_string_list_t = value_t.getElementsByTagName("list");
									NodeList value_string_map_t = value_t.getElementsByTagName("map");


									if (value_string_list_t.getLength() > 0) {

										ArrayList<String> param_data = new ArrayList<String>();

										//NodeList value_string_list_t = value_t.getElementsByTagName("list");
										Element value_string_list_t_e = (Element) value_string_list_t.item(0);

										NodeList entries_list_t = value_string_list_t_e.getElementsByTagName("entry");

										for (int ic = 0; ic < entries_list_t.getLength(); ic++) {
											Element entry_element = (Element) entries_list_t.item(ic);
											param_data.add(entry_element.getTextContent().trim());
										}

										rp = new RunParameterStringList(param_data);

										// <value><list><entry></entry><entry></entry></list></value>
									} else if (value_string_map_t.getLength() > 0) {
										LinkedHashMap<String, String> param_data = new LinkedHashMap<String, String>();

										//NodeList value_string_map_t = value_t.getElementsByTagName("map");

										Element value_string_map_t_e = (Element) value_string_map_t.item(0);

										NodeList entries_list_t = value_string_map_t_e.getElementsByTagName("entry");
										for (int ic = 0; ic < entries_list_t.getLength(); ic++) {
											Element entry_element = (Element) entries_list_t.item(ic);

											NodeList entries_key_t = entry_element.getElementsByTagName("key");
											if (entries_key_t.getLength() > 0) {
												Element key_element = (Element) entries_key_t.item(0);

												NodeList values_key_t = value_string_map_t_e.getElementsByTagName("value");
												if (values_key_t.getLength() > 0) {
													Element value_element = (Element) values_key_t.item(ic);

													String k_ic = key_element.getTextContent().trim();
													String v_ic = value_element.getTextContent().trim();
													if (k_ic.length() <= 0) {
														throw new HtException(getContext(), "resolveClassesFromJar", "String Map must contain valid keys");
													}

													param_data.put(k_ic, v_ic);

												}

											}

										}

										rp = new RunParameterStringMap(param_data);
										// <value><map><entry><key></key><value></value></entry><entry><key></key><value></value></entry></map></value>
									} else {
										// assume string
										String value_t_s = value_t.getTextContent().trim();
										rp = new RunParameterString(value_t_s);
									}


									class_entry_i.getParameters().put(key_t_s, rp);


								}

							}

						}

					}

				}


			}


		}
		catch (SAXParseException  e) {
			throw new HtException(getContext(), "resolveClassesFromJar", "Error on parsing XML descriptor: [" + e.getMessage() + 
		   "], column: " + e.getColumnNumber() + ", line: " + e.getLineNumber() + ", public ID: " + e.getPublicId()
		  );
		} 
		catch (Throwable e) {
			throw new HtException(getContext(), "resolveClassesFromJar", "Error on reading package properies: " + e.getMessage());
		} finally {

			try {
				if (jarfile != null) {
					jarfile.close();
				}
			} catch (Throwable ign1) {
			}


			try {
				if (inp != null) {
					inp.close();
				}
			} catch (Throwable ign2) {
			}



		}
		
		
		return r;
	}

	private void run_calling_thread(Uid runUid, RunClassEntry classEntry, HtExtScriptLogger logger, Map<String, String> runKeys, StringBuilder customOutput) throws Exception {

		run_calling_thread(resolveJarFilePath(runUid).getAbsolutePath(), classEntry, logger,  runKeys, customOutput);

	}

	// run in the calling thread
	private void run_calling_thread(
			String packageLoadPath, 
			RunClassEntry classEntry, 
			HtExtScriptLogger logger, 
			Map<String, String> runKeys, 
			StringBuilder customOutput
	) throws Exception {

		ZipClassLoader classLoader = new ZipClassLoader(packageLoadPath.toString());

		// need to read the content of Jar file
		Class clazz = classLoader.loadClass(classEntry.getClassName());

		//Class[] partypes = new Class[]{HtExtLogger.class};

		//parameter types for constructor
		Class[] constrpartypes = new Class[]{};

		//Create constructor object . parameter types
		Constructor constr = clazz.getDeclaredConstructor(constrpartypes);

		//create instance
		Object realInstance = constr.newInstance();

		HtExtRunner realInstanceS = (HtExtRunner) realInstance;

		realInstanceS.run(logger, classEntry.getParameters(), runKeys, customOutput);

		// free whatever to remove jar
		classLoader.close();

		classLoader = null;

		realInstanceS = null;

		System.gc();
		System.gc();

	}
	
	private Element getFirstElementByTagName(Element parent, String tagName) throws Exception
	{
	  NodeList all_list = parent.getElementsByTagName(tagName);
	  if (all_list.getLength() > 0)
		return (Element)all_list.item(0);
	  
	  throw new HtException(getContext(), "getFirstElementByTagName", "Cannot resolve first descendant element from a list by tag name: " + tagName);
	}
}
