/*
 * XmlHandler.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;



import com.fin.httrader.utils.Uid;
import com.sun.org.apache.xerces.internal.parsers.DOMParser;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 *
 * @author Administrator
 * helper class to parse XML
 */
public class XmlHandler {

	// XML parser
	private DOMParser xmlparser_m = new DOMParser();

	/** Creates a new instance of XmlHandler */
	public XmlHandler() {
	}

	// functions for parsing this
	//
	public void serializeParameter(XmlParameter xmlparameter, StringBuilder out) {
		out.setLength(0);
		//out.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>");
		out.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		serializeParameterHelper(xmlparameter, out);
	}
	
	// this jsi just for logging
	private void serializeParameterHelper_simpleVer(XmlParameter xmlparameter, StringBuilder out)
	{
		out.append("caption: [");
		out.append(xmlparameter.getCommandName());
		out.append("]\n");

		Set<String> keys = xmlparameter.getKeys();
		if (keys != null) {
			Iterator<String> it = keys.iterator();
			while (it.hasNext()) {
				String keyname = it.next();
				if (xmlparameter.getType(keyname) != XmlParameter.VT_XmlParameter &&
								xmlparameter.getType(keyname) != XmlParameter.VT_XmlParameterList) {

					out.append("[ scalar parameter name=\"");
					out.append(keyname);
					out.append("\"");
					
					
					switch (xmlparameter.getType(keyname)) {
						case XmlParameter.VT_Date:
							out.append(" value=\""); out.append(xmlparameter.getDate(keyname)); out.append("\"");
							break;
						case XmlParameter.VT_Int:
							out.append(" value=\"");out.append(xmlparameter.getInt(keyname)); out.append("\"");
							break;
						case XmlParameter.VT_String:
							out.append(" value=\"");out.append(xmlparameter.getString(keyname)); out.append("\"");
							break;
						case XmlParameter.VT_Double:
							out.append(" value=\"");out.append(xmlparameter.getDouble(keyname)); out.append("\"");
							break;
						case XmlParameter.VT_Uid:
							out.append(" value=\""); out.append(xmlparameter.getUid(keyname).toString());out.append("\"");
							break;
						case XmlParameter.VT_DateList: {
							List<Long> vals = xmlparameter.getDateList(keyname);
							out.append(" list value=( ");
							for (int j = 0; j < vals.size(); j++) {
								out.append("\""); out.append(vals.get(j)); out.append("\" ");
							}
							out.append(" )");
							break;
						}
						case XmlParameter.VT_IntList: {
							List<Long> vals = xmlparameter.getIntList(keyname);
							out.append(" list value=( ");
							for (int j = 0; j < vals.size(); j++) {
								out.append("\""); out.append(vals.get(j)); out.append("\" ");
							}
							out.append(" )");
							break;
						}
						case XmlParameter.VT_StringList: {
							List<String> vals = xmlparameter.getStringList(keyname);
							out.append(" list value=( ");
							for (int j = 0; j < vals.size(); j++) {
								out.append("\""); out.append(vals.get(j)); out.append("\" ");
							}
							out.append(" )");
							break;
						}
						case XmlParameter.VT_DoubleList: {
							List<Double> vals = xmlparameter.getDoubleList(keyname);
							out.append(" list value=( ");
							for (int j = 0; j < vals.size(); j++) {
								out.append("\""); out.append(vals.get(j)); out.append("\" ");
							}
							out.append(" )");
							break;
						}
						case XmlParameter.VT_UidList: {
							List<Uid> vals = xmlparameter.getUidList(keyname);
							out.append(" list value=( ");
							for (int j = 0; j < vals.size(); j++) {
								out.append("\""); out.append(vals.get(j).toString()); out.append("\" ");
							}
							out.append(" )");
							break;
						}
						case XmlParameter.VT_StringMap: {
								Map<String, String> vals = xmlparameter.getStringMap(keyname);
								out.append(" map value=( ");
								for (String key_p : vals.keySet()) {
										String val_p = vals.get(key_p);

										out.append("[ id=\"");
										out.append(key_p);
										out.append("\" value=\"");
										out.append(val_p);
										out.append("\" ] ");
								}
								
								out.append(" )");
						}

						default:
							;
					}

				
				}

				out.append(" ] \n");
			}

			// again iterate to find XML parameters
			it = keys.iterator();

			while (it.hasNext()) {
				String keyname = it.next();
				if (xmlparameter.getType(keyname) == XmlParameter.VT_XmlParameter) {


					out.append("[ nested parameter name=\"");
					out.append(keyname);
					out.append("\"\n");
					
					
					// get content
					StringBuilder out2 = new StringBuilder();
					XmlParameter param2 = xmlparameter.getXmlParameter(keyname);

					serializeParameterHelper_simpleVer(param2, out2);
					out.append(out2);

					out.append("\n] \n");
				} else if (xmlparameter.getType(keyname) == XmlParameter.VT_XmlParameterList) {


					out.append("[ nested parameter list name=\"");
					out.append(keyname);
					out.append("\"\n");
					
					

					// get content
					StringBuilder out2 = new StringBuilder();
					List<XmlParameter> paramlist2 = xmlparameter.getXmlParameterList(keyname);

					for (int i = 0; i < paramlist2.size(); i++) {
						out2.setLength(0);
						serializeParameterHelper_simpleVer(paramlist2.get(i), out2);
						out.append(out2);
					}
					out.append("\n] \n");
				}

			}
			
		

		}

		
	}
	
	private void serializeParameterHelper(XmlParameter xmlparameter, StringBuilder out) {
		out.append("<ent name=\"");
		out.append(xmlparameter.getCommandName());
		out.append("\">");

		Set<String> keys = xmlparameter.getKeys();
		if (keys != null) {
			Iterator<String> it = keys.iterator();
			while (it.hasNext()) {
				String keyname = it.next();
				if (xmlparameter.getType(keyname) != XmlParameter.VT_XmlParameter &&
								xmlparameter.getType(keyname) != XmlParameter.VT_XmlParameterList) {

					out.append("<par name=\"");
					out.append(keyname);
					out.append("\" type=\"");
					out.append(xmlparameter.getType(keyname));
					out.append("\">");
					
					switch (xmlparameter.getType(keyname)) {
						case XmlParameter.VT_Date:
							out.append("<val>"); out.append(xmlparameter.getDate(keyname)); out.append("</val>");
							break;
						case XmlParameter.VT_Int:
							out.append("<val>");out.append(xmlparameter.getInt(keyname)); out.append("</val>");
							break;
						case XmlParameter.VT_String:
							out.append("<val>"); out.append(xmlparameter.getString(keyname)); out.append("</val>");
							break;
						case XmlParameter.VT_Double:
							out.append("<val>"); out.append(xmlparameter.getDouble(keyname)); out.append("</val>");
							break;
						case XmlParameter.VT_Uid:
							out.append("<val>"); out.append(xmlparameter.getUid(keyname).toString()); out.append("</val>");
							break;
						case XmlParameter.VT_DateList: {
							List<Long> vals = xmlparameter.getDateList(keyname);
							for (int j = 0; j < vals.size(); j++) {
								out.append("<val>"); out.append(vals.get(j)); out.append("</val>");
							}
							break;
						}
						case XmlParameter.VT_IntList: {
							List<Long> vals = xmlparameter.getIntList(keyname);
							for (int j = 0; j < vals.size(); j++) {
								out.append("<val>"); out.append(vals.get(j)); out.append("</val>");
							}
							break;
						}
						case XmlParameter.VT_StringList: {
							List<String> vals = xmlparameter.getStringList(keyname);
							for (int j = 0; j < vals.size(); j++) {
								out.append("<val>"); out.append(vals.get(j)); out.append("</val>");
							}
							break;
						}
						case XmlParameter.VT_DoubleList: {
							List<Double> vals = xmlparameter.getDoubleList(keyname);
							for (int j = 0; j < vals.size(); j++) {
								out.append("<val>"); out.append(vals.get(j)); out.append("</val>");
							}
							break;
						}
						case XmlParameter.VT_UidList: {
							List<Uid> vals = xmlparameter.getUidList(keyname);
							for (int j = 0; j < vals.size(); j++) {
								out.append("<val>"); out.append(vals.get(j).toString()); out.append("</val>");
							}
							break;
						}
						case XmlParameter.VT_StringMap: {
								Map<String, String> vals = xmlparameter.getStringMap(keyname);
								for (String key_p : vals.keySet()) {
										String val_p = vals.get(key_p);

										out.append("<val><k>");
										out.append(key_p);
										out.append("</k><v>");
										out.append(val_p);
										out.append("</v></val>");
								}
						}

						default:
							;
					}

					out.append("</par>");
				}


			}

			// again iterate to find XML parameters
			it = keys.iterator();

			while (it.hasNext()) {
				String keyname = it.next();
				if (xmlparameter.getType(keyname) == XmlParameter.VT_XmlParameter) {


					out.append("<xmlpar name=\""); out.append(keyname); out.append("\">");

					// get content
					StringBuilder out2 = new StringBuilder();
					XmlParameter param2 = xmlparameter.getXmlParameter(keyname);

					serializeParameterHelper(param2, out2);
					out.append(out2);

					out.append("</xmlpar>");
				} else if (xmlparameter.getType(keyname) == XmlParameter.VT_XmlParameterList) {


					out.append("<xmlparlist name=\""); out.append(keyname); out.append("\">");

					// get content
					StringBuilder out2 = new StringBuilder();
					List<XmlParameter> paramlist2 = xmlparameter.getXmlParameterList(keyname);

					for (int i = 0; i < paramlist2.size(); i++) {
						out2.setLength(0);
						serializeParameterHelper(paramlist2.get(i), out2);
						out.append(out2);
					}
					out.append("</xmlparlist>");
				}

			}

		}

		out.append("</ent>");

	}

	private void deserializeParameterHelper(NodeList par_par, XmlParameter xmlparameter) throws
					SAXException,
					IOException,
					Exception {
		for (int i = 0; i < par_par.getLength(); i++) {
			Node par_par_node = par_par.item(i);

			// debug
			//String text_content = par_par_node.getTextContent();

			String nodename = par_par_node.getNodeName();
			if (nodename.equalsIgnoreCase("xmlpar")) {
				////
				NamedNodeMap par_par_node_attr = par_par_node.getAttributes();
				Node name_node = par_par_node_attr.getNamedItem("name");
				String name = name_node.getNodeValue();

				// get ent node
				NodeList ent_list = par_par_node.getChildNodes();
				Node ent_node2 = ent_list.item(0);

				// get pars
				NodeList par_par2 = ent_node2.getChildNodes();

				XmlParameter xmlparameter2 = new XmlParameter();

				// return entity attribute
				NamedNodeMap ent_node_attr = ent_node2.getAttributes();
				Node ent_node_name = ent_node_attr.getNamedItem("name");
				xmlparameter2.setCommandName(ent_node_name.getNodeValue());

				deserializeParameterHelper(par_par2, xmlparameter2);

				// insert
				xmlparameter.setXmlParameter(name, xmlparameter2);

				continue;
			} else if (nodename.equalsIgnoreCase("xmlparlist")) {
				////
				NamedNodeMap par_par_node_attr = par_par_node.getAttributes();
				Node name_node = par_par_node_attr.getNamedItem("name");
				String name = name_node.getNodeValue();

				// get ent node
				NodeList ent_list = par_par_node.getChildNodes();
				List<XmlParameter> paramlist2 = new ArrayList<XmlParameter>();
				for (int k = 0; k < ent_list.getLength(); k++) {
					Node ent_node2 = ent_list.item(k);

					XmlParameter xmlparameter2 = new XmlParameter();

					// return entity attribute
					NamedNodeMap ent_node_attr = ent_node2.getAttributes();
					Node ent_node_name = ent_node_attr.getNamedItem("name");
					xmlparameter2.setCommandName(ent_node_name.getNodeValue());

					// get pars
					NodeList par_par2 = ent_node2.getChildNodes();

					deserializeParameterHelper(par_par2, xmlparameter2);

					paramlist2.add(xmlparameter2);
				}

				// insert
				xmlparameter.setXmlParameterList(name, paramlist2);

				continue;
			}


			// get name and type of the parameters
			NamedNodeMap par_par_node_attr = par_par_node.getAttributes();

			Node name_node = par_par_node_attr.getNamedItem("name");
			Node type_node = par_par_node_attr.getNamedItem("type");

			int param_type = Integer.parseInt(type_node.getNodeValue());
			String param_name = name_node.getNodeValue();

			// internal node list
			NodeList vallist = par_par_node.getChildNodes();
			int val_cnt = vallist.getLength();

			Node val_i = null;
			if (val_cnt == 1) {
				val_i = vallist.item(0);
			}


			switch (param_type) {
				case XmlParameter.VT_Date: {

					xmlparameter.setDate(param_name, (long) Double.parseDouble(val_i.getTextContent()));
					break;
				}
				case XmlParameter.VT_Int: {
					xmlparameter.setInt(param_name, Long.parseLong(val_i.getTextContent()));
					break;
				}
				case XmlParameter.VT_String: {

					xmlparameter.setString(param_name, val_i.getTextContent());
					break;
				}
				case XmlParameter.VT_Double: {

					xmlparameter.setDouble(param_name, Double.parseDouble(val_i.getTextContent()));
					break;
				}
				case XmlParameter.VT_Uid: {

					xmlparameter.setUid(param_name, new Uid(val_i.getTextContent()));
					break;
				}
				case XmlParameter.VT_DateList: {
					List<Long> vals = new ArrayList<Long>();
					for (int j = 0; j < vallist.getLength(); j++) {
						vals.add((long) Double.parseDouble(vallist.item(j).getTextContent()));
					}
					xmlparameter.setDateList(param_name, vals);
					break;
				}
				case XmlParameter.VT_IntList: {
					List<Long> vals = new ArrayList<Long>();
					for (int j = 0; j < vallist.getLength(); j++) {
						vals.add(Long.parseLong(vallist.item(j).getTextContent()));
					}
					xmlparameter.setIntList(param_name, vals);
					break;
				}
				case XmlParameter.VT_StringList: {
					List<String> vals = new ArrayList<String>();
					for (int j = 0; j < vallist.getLength(); j++) {
						vals.add(vallist.item(j).getTextContent());
					}
					xmlparameter.setStringList(param_name, vals);
					break;
				}
				case XmlParameter.VT_DoubleList: {
					List<Double> vals = new ArrayList<Double>();
					for (int j = 0; j < vallist.getLength(); j++) {
						vals.add(Double.parseDouble(vallist.item(j).getTextContent()));
					}
					xmlparameter.setDoubleList(param_name, vals);
					break;
				}
				case XmlParameter.VT_UidList: {
					List<Uid> vals = new ArrayList<Uid>();
					for (int j = 0; j < vallist.getLength(); j++) {
						vals.add(new Uid(vallist.item(j).getTextContent()));
					}
					xmlparameter.setUidList(param_name, vals);
					break;
				}
				case XmlParameter.VT_StringMap: {
						Map<String, String> vals = new TreeMap<String, String>();
						for (int j = 0; j < vallist.getLength(); j++) {
								Node map_node_i = vallist.item(j);
								NodeList key_and_value_node = map_node_i.getChildNodes();

								String key_q = null, val_q = null;
								if (key_and_value_node.getLength() == 2) {
										
										for(int q = 0; q < 2; q++) {
												Node n_q = key_and_value_node.item(q);
												String node_name_q = n_q.getNodeName().toLowerCase();
												if (node_name_q.equals("k")) {
														// key
														key_q = n_q.getTextContent();
												}
												else if (node_name_q.equals("v"))	{
														// value
														val_q = n_q.getTextContent();
												}
												
										}


								}

								if (key_q != null && val_q != null) {
										vals.put(key_q, val_q);
								}

						}
						xmlparameter.setStringMap(param_name, vals);
						break;
				}

				default:
					;
			}
		}

	}

	public void deserializeParameter(String xmlcontent, XmlParameter xmlparameter) throws
					SAXException,
					IOException,
					Exception {

		// reset the parser
		xmlparser_m.reset();
		xmlparameter.clear();

		if (xmlcontent == null || xmlcontent.length() <= 0) {
			return;
		}

		StringReader reader = new StringReader(xmlcontent);
		InputSource inpsrc = new InputSource(reader);


		// parse the content

		xmlparser_m.parse(inpsrc);
		Document doc = xmlparser_m.getDocument();

		// entity element
		Element ent_element = doc.getDocumentElement();
		Node ent_node = (Node) ent_element;



		NamedNodeMap ent_node_attr = ent_node.getAttributes();
		Node ent_node_name = ent_node_attr.getNamedItem("name");
		xmlparameter.setCommandName(ent_node_name.getNodeValue());

		// all the par and xml par tags
		NodeList par_par = ent_node.getChildNodes();

		// process parameter and xml parameters lists
		deserializeParameterHelper(par_par, xmlparameter);
	}

	// static versions
	public static String serializeParameterStatic(XmlParameter xmlparameter) throws Exception
	{
		 StringBuilder out = new StringBuilder();

		 XmlHandler hndl = new XmlHandler();
		 hndl.serializeParameter(xmlparameter, out);
		 return out.toString();
	}
	
	public static String serializeParameterStatic_simpleVer(XmlParameter xmlparameter) throws Exception
	{
		 StringBuilder out = new StringBuilder();

		 XmlHandler hndl = new XmlHandler();
		 hndl.serializeParameterHelper_simpleVer(xmlparameter, out);
		 return out.toString();
	}

	public static XmlParameter deserializeParameterStatic(String xmlcontent) throws Exception
	{
		 XmlParameter out = new XmlParameter();

		 if (xmlcontent != null && xmlcontent.length() > 0) {

				XmlHandler hndl = new XmlHandler();
				hndl.deserializeParameter(xmlcontent, out);
		 }
		 
		 return out;
	}

}
