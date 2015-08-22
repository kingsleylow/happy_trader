/*
 * XmlParameter.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;
import com.fin.httrader.utils.Uid;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;


/**
 *
 * @author victor_zoubok
 * This class incapsulate information packest that are used for exchange information with server
 */
public class XmlParameter {

	// types
	public static final int VT_Dummy = 0x0000;
	public static final int VT_Int = 0x0001;
	public static final int VT_Double = 0x0002;
	public static final int VT_Date = 0x0004;
	public static final int VT_String = 0x0008;
	public static final int VT_Uid = 0x0010;
	public static final int VT_IntList = 0x1001;
	public static final int VT_DoubleList = 0x1002;
	public static final int VT_DateList = 0x1004;
	public static final int VT_StringList = 0x1008;
	public static final int VT_UidList = 0x1010;
	public static final int VT_XmlParameter = 0x2000;
	public static final int VT_XmlParameterList = 0x2010;
	public static final int VT_StringMap = 0x1040;
	// XML parser
	//private static DOMParser xmlparser_m = new DOMParser();
	//private static Object mtxXmlParser_m = new Object();
	// name of the parameter
	private StringBuilder name_m = new StringBuilder();
	// parameters
	private HashMap params_m = new HashMap();

	private HashMap<String, Integer> types_m = new HashMap<String, Integer>();

	// accessors
	public void clear() {
		name_m.setLength(0);
		params_m.clear();
		types_m.clear();
	}

	// make a copy from scr object, values vut not references are copied
	public void create(XmlParameter src) {
		clear();
		name_m.append(src.name_m);

		if (src.types_m.isEmpty()) {
			return;
		}

		// this must be copied by value
		for (Iterator<String> it = src.types_m.keySet().iterator(); it.hasNext();) {
			String key = it.next();
			Integer value = src.types_m.get(key);

			int type_code = value.intValue();
			switch (type_code) {
				case XmlParameter.VT_Date: {
					setDate(key, src.getDate(key));
					break;
				}
				case XmlParameter.VT_Int: {
					setInt(key, src.getInt(key));
					break;
				}
				case XmlParameter.VT_String: {
					setString(key, src.getString(key));
					break;
				}
				case XmlParameter.VT_Double: {

					setDouble(key, src.getDouble(key));
					break;
				}
				case XmlParameter.VT_Uid: {
					setUid(key, src.getUid(key));
					break;
				}
				case XmlParameter.VT_DateList: {

					setDateList(key, src.getDateList(key));
					break;
				}
				case XmlParameter.VT_IntList: {
					setIntList(key, src.getIntList(key));
					break;
				}
				case XmlParameter.VT_StringList: {
					setStringList(key, src.getStringList(key));
					break;
				}
				case XmlParameter.VT_DoubleList: {
					setDoubleList(key, src.getDoubleList(key));
					break;
				}
				case XmlParameter.VT_UidList: {
					setUidList(key, src.getUidList(key));
					break;
				}
				case XmlParameter.VT_XmlParameter: {
					setXmlParameter(key, src.getXmlParameter(key));
					break;
				}
				case XmlParameter.VT_XmlParameterList: {
					setXmlParameterList(key, src.getXmlParameterList(key));
					break;
				}
				case XmlParameter.VT_StringMap: {
					setStringMap(key, src.getStringMap(key));
					break;
				}

				default:
					
			}
			

		} // end of loop


	}

	public Set<String> getKeys() {

		return params_m.keySet();
	}

	public void setCommandName(String name) {
		name_m.setLength(0);
		name_m.append(name);
	}

	public String getCommandName() {
		return name_m.toString();
	}

	public int getType(String parname) {
		return (Integer) types_m.get(parname);
	}
	//

	public void setString(String parname, String value) {
		if (types_m.containsKey(parname) && types_m.get(parname)==VT_String) {
			// already inited
			StringBuilder data = (StringBuilder)params_m.get(parname);
			data.setLength(0);
			data.append(value);
		}
		else {
			types_m.put(parname, VT_String);
			params_m.put(parname, new StringBuilder(value));
		}

		
	}

	public String getString(String parname) {
		return (String) params_m.get(parname).toString();
	}

	public void setInt(String parname, long value) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_Int) {
			// already inited
			LongParam data = (LongParam)params_m.get(parname);
			data.setLong(value);
		}
		else {
			types_m.put(parname, VT_Int);
			params_m.put(parname, new LongParam(value));
		}
	
	}

	public long getInt(String parname) {
		return ((LongParam) params_m.get(parname)).getLong();
	}

	public void setDouble(String parname, double value) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_Double) {
			// already inited
			DoubleParam data = (DoubleParam)params_m.get(parname);
			data.setDouble(value);

		}
		else {
			types_m.put(parname, VT_Double);
			params_m.put(parname, new DoubleParam(value));
		}
	
	}

	public double getDouble(String parname) {
		return ((DoubleParam) params_m.get(parname)).getDouble();
	}

	public void setDate(String parname, long value) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_Date) {
			// already inited
			LongParam data = (LongParam)params_m.get(parname);
			data.setLong(value);
		}
		else {
			types_m.put(parname, VT_Date);
			params_m.put(parname, new LongParam(value));
		}

		
	}

	public long getDate(String parname) {
		return ((LongParam) params_m.get(parname)).getLong();
	}

	public void setUid(String parname, Uid value) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_Uid) {
			// already inited
			Uid data = (Uid)params_m.get(parname);
			data.fromUid(value);
		}
		else {
			types_m.put(parname, VT_Uid);
			params_m.put(parname, new Uid(value));
		}

		
	}

	public Uid getUid(String parname) {
		return (Uid) params_m.get(parname);
	}

	/////
	public void setStringList(String parname, List<String> values) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_StringList) {
			// already inited
			ArrayList<String> data = (ArrayList<String>)params_m.get(parname);
			data.clear();
			data.addAll(values);

		}
		else {
			types_m.put(parname, VT_StringList);
			params_m.put(parname, new ArrayList<String>(values));
		}

		
	}

	public List<String> getStringList(String parname) {
		return (List<String>) params_m.get(parname);
	}

	public void setIntList(String parname, List<Long> values) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_IntList) {
			// already inited
			ArrayList<Long> data = (ArrayList<Long>)params_m.get(parname);
			data.clear();
			data.addAll(values);

		}
		else {
			types_m.put(parname, VT_IntList);
			params_m.put(parname, new ArrayList<Long>(values));
		}

	}

	public List<Long> getIntList(String parname) {
		return (List<Long>) params_m.get(parname);
	}

	public void setDoubleList(String parname, List<Double> values) {
		if (types_m.containsKey(parname) && types_m.get(parname)==VT_DoubleList) {
			// already inited
			ArrayList<Double> data = (ArrayList<Double>)params_m.get(parname);
			data.clear();
			data.addAll(values);

		}
		else {
			types_m.put(parname, VT_DoubleList);
			params_m.put(parname, new ArrayList<Double>(values));
		}

	}

	public List<Double> getDoubleList(String parname) {
		return (List<Double>) params_m.get(parname);
	}

	public void setDateList(String parname, List<Long> values) {
		if (types_m.containsKey(parname) && types_m.get(parname)==VT_DateList) {
			// already inited
			ArrayList<Long> data = (ArrayList<Long>)params_m.get(parname);
			data.clear();
			data.addAll(values);

		}
		else {
			types_m.put(parname, VT_DateList);
			params_m.put(parname, new ArrayList<Long>(values));
		}
	}

	public List<Long> getDateList(String parname) {
		return (List<Long>) params_m.get(parname);
	}

	public void setUidList(String parname, List<Uid> values) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_UidList) {
			// already inited
			ArrayList<Uid> data = (ArrayList<Uid>)params_m.get(parname);
			data.clear();
			data.addAll(values);

		}
		else {
			types_m.put(parname, VT_UidList);
			params_m.put(parname, new ArrayList<Uid>(values));
		}


	}

	public List<Uid> getUidList(String parname) {
		return (List<Uid>) params_m.get(parname);
	}

	public XmlParameter getXmlParameter(String parname) {
		return (XmlParameter) params_m.get(parname);
	}

	public void setXmlParameter(String parname, XmlParameter value) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_XmlParameter) {
			// already inited
			XmlParameter data = (XmlParameter)params_m.get(parname);
			data.create(value);


		}
		else {
			types_m.put(parname, VT_XmlParameter);
			XmlParameter value_cp = new XmlParameter();
			value_cp.create(value);

			params_m.put(parname, value_cp);
		}

	
	}

	public List<XmlParameter> getXmlParameterList(String parname) {
		List<XmlParameter> value = (List<XmlParameter>) params_m.get(parname);
		return value;

	}

	public void setXmlParameterList(String parname, List<XmlParameter> values) {

		if (types_m.containsKey(parname) && types_m.get(parname)==VT_XmlParameterList) {
			// already inited
			List<XmlParameter> data = (List<XmlParameter>)params_m.get(parname);

			data.clear();
			data.addAll( values);
			

		}
		else {
			types_m.put(parname, VT_XmlParameterList);
			ArrayList<XmlParameter> value_cp = new ArrayList<XmlParameter>( values );
			
			params_m.put(parname, value_cp);
		}

	}

	public Map<String, String> getStringMap(String parname)
	{
		Map<String, String> value = (Map<String, String>) params_m.get(parname);
		return value;
	}

	public void setStringMap(String parname, Map<String, String> values )
	{
		if (types_m.containsKey(parname) && types_m.get(parname)==VT_StringMap) {
			// already inited
			Map<String, String> data = (Map<String, String> )params_m.get(parname);

			data.clear();
			data.putAll( values);
			

		}
		else {
			types_m.put(parname, VT_StringMap);
			Map<String, String> value_cp = new TreeMap<String, String>( values );
			params_m.put(parname, value_cp);
		}
	}


} // end of the class
