/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;


import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.URLUTF8Encoder;
import com.fin.httrader.utils.hlpstruct.HtPair;
import java.io.File;
import java.io.IOException;

import java.io.StringWriter;
import java.net.URL;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


/**
 *
 * @author DanilinS
 * this is intended to load jar file from the server
 * no auth is required
 */
public class HtWebStart2_LoadJar extends HtServletsBase {



	public String getContext() {
		return HtWebStart2_LoadJar.class.getSimpleName();
	}

	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {


		try {

			/*
			This is the base URL
			/loadjar/<app name>/<jar name>
			 */

			URL url = new URL(req.getRequestURL().toString());
			String userIp = url.getHost();
			int userPort = url.getPort();
			
			String scheme = req.getScheme();
			String jar_uri = req.getRequestURI();

			Set<String> allowedFiles= HtCommandProcessor.instance().get_HtConfigurationProxy().remote_AllowedJars();


			StringBuilder app_name = new StringBuilder();
			StringBuilder lib_name = new StringBuilder();
			StringBuilder user_name = new StringBuilder();
			StringBuilder hashed_password = new StringBuilder();

			parseURL(app_name, lib_name, user_name, hashed_password, jar_uri);



			if (lib_name.length() < 0) {
				throw new HtException(getContext(), "doGet", "Invalid jar path");
			}

			if (app_name.length() <= 0) {
				throw new HtException(getContext(), "doGet", "Invalid application name");
			}

			if (user_name.length() <= 0 || hashed_password.length() <= 0) {
				throw new HtException(getContext(), "doGet", "Invalid user/password");
			}

		

			// now we are ready to provide jar
			String path = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getWebRootDir() +
							File.separatorChar + ".." + File.separatorChar + ".." + File.separatorChar + app_name.toString() + File.separatorChar + "dist" + File.separatorChar + lib_name.toString();

			File f = new File(path.toString());
			if (!f.exists()) {
				throw new HtException(getContext(), "doGet", "File (jar/jnlp) not found: \"" + lib_name + "\"");
			}

			if (!allowedFiles.contains(f.getCanonicalFile().getAbsolutePath()))
				throw new HtException(getContext(), "doGet", "File (jar/jnlp) is not registered as available for download");
			


			String resourceName = f.getName().toLowerCase();

			if (resourceName.endsWith(".jar")) {
				readJarFile(f, res);
			}
			else if (resourceName.endsWith(".jnlp"))	{


				// optional parameters

				
				Map<String, String> parameters = new TreeMap<String, String>();

				// as well as pass some standard params
				parameters.put("user_host", userIp);
				parameters.put("user_port", String.valueOf(userPort));
				parameters.put("user_name", user_name.toString());
				parameters.put("hashed_password", hashed_password.toString());

							
				
				// need to substiture
				readJNLP(f, app_name.toString(), user_name.toString(), hashed_password.toString(), scheme, userIp, userPort, parameters, res);
			}
			else
				throw new HtException(getContext(), "doGet", "Unknown resource type");




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			//HtLog.log(Level.WARNING, getContext(), "doGet", "Exception on downloading jar/jnlp: " + e.getMessage());
		}
	}

	/*
	 * Helpers
	 */
	private void parseURL(
					StringBuilder app_name,
					StringBuilder lib_name,
					StringBuilder user_name,
					StringBuilder hashed_password,
					String jar_uri
	) throws Exception {
		app_name.setLength(0);
		lib_name.setLength(0);
		user_name.setLength(0);
		hashed_password.setLength(0);
		
		String[] pieces = jar_uri.split("[/]");

		if (pieces != null) {
			int idx = 0;
			for (int i = 0; i < pieces.length; i++) {

				if (pieces[i] != null && pieces[i].length() > 0) {
					if (idx == 0) {
						idx++;
						continue;
					}

					if (idx == 1) {
						app_name.append(pieces[i]);
					}
					else if (idx==2) {
						user_name.append(pieces[i]);
					}
					else if (idx==3) {
						hashed_password.append(pieces[i]);
					}
					else {
						lib_name.append(pieces[i]);

						if (pieces[i].indexOf(".") < 0) {
							lib_name.append(File.separatorChar);
						}
					}

					idx++;
				}

			}
		}
	}

	private void readJarFile(File f, HttpServletResponse res) throws Exception {
		res.setDateHeader("Expires", 1);
		res.setHeader("Content-disposition", "attachment; filename=" + URLUTF8Encoder.encode(f.getName()));
		res.setHeader("Content-Type", "application/x-java-archive");
		res.setHeader("Transfer-Encoding", "chunked");



		long last_modofied = f.lastModified();

		//Tue, 15 Nov 1994 12:45:26 GMT
		String dformat = HtWebUtils.HTTP_DF.format(new Date(last_modofied));
		res.setHeader("Last-Modified", dformat );
		
		res.getOutputStream().print(Long.toHexString(f.length()));
		res.getOutputStream().print("\r\n");
		HtFileUtils.simpleReadFile(f, res.getOutputStream());
		res.getOutputStream().print("\r\n");
	}

	private void readJNLP(
					File f,
					String application_name,
					String user_name,
					String hashed_password,
					String scheme,
					String userIp,
					int userPort,
					Map<String, String> parameters,
					HttpServletResponse res
	) throws Exception
	{
		String newCodeBase = scheme + "://" + userIp + ":" +userPort+ "/loadjar/" + application_name + "/" + user_name + "/" +hashed_password + "/";
		String newContent = modifyJnlpContent(f, newCodeBase, parameters);

		res.setDateHeader("Expires", 1);
		res.setContentType("application/x-java-jnlp-file");
		//res.setHeader("Cache-Control", "must-revalidate");
		res.setHeader("Accept-Ranges", "bytes");

		res.getOutputStream().print(newContent);
	}

	private String modifyJnlpContent(File f, String newCodebase, Map<String, String> parameters) throws Exception {



		DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = docFactory.newDocumentBuilder();


		try {
			Document doc = docBuilder.parse(f);
			Node jnlp_node = doc.getFirstChild();

			NamedNodeMap attrm = jnlp_node.getAttributes();
			Node cb_attr = attrm.getNamedItem("codebase");

			cb_attr.setNodeValue(newCodebase);
			attrm.setNamedItem(cb_attr);

			// add optional parameters
			if (parameters != null) {

				// find application-desc
				NodeList application_desc_node_l = doc.getElementsByTagName("resources");

				if (application_desc_node_l.getLength() > 0 ) {
					Node application_desc_node_l_1 = application_desc_node_l.item(0);

					// <property name="parameter_name" value="value"/>

					for(Iterator<String> it = parameters.keySet().iterator(); it.hasNext(); ) {

						String param_name = it.next();
						String param_value = parameters.get(param_name);
						
						
						Element property_el = doc.createElement("property");
						property_el.setAttribute("name", param_name);
						property_el.setAttribute("value", param_value);

						application_desc_node_l_1.appendChild(property_el);

					}
				}
			}

			// save

			Transformer transformer = TransformerFactory.newInstance().newTransformer();
			transformer.setOutputProperty(OutputKeys.INDENT, "yes");

			//initialize StreamResult with File object to save to file
			StreamResult result = new StreamResult(new StringWriter());
			DOMSource source = new DOMSource(doc);
			transformer.transform(source, result);

			return result.getWriter().toString();


		} catch (Throwable e) {
			throw new HtException(getContext(), "modifyJnlpContent", "Exception on parsing JNLP: " + e.getMessage());
		}



	}
}
