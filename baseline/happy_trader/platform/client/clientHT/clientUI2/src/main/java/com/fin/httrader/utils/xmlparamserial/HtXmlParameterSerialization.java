/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.xmlparamserial;

import com.fin.httrader.configprops.HtSaveAnnotation;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.lang.annotation.Annotation;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class HtXmlParameterSerialization {

	public static String getContext() {
		return HtXmlParameterSerialization.class.getSimpleName();
	}

	// serialize object to xml parameter with
	// registered types
	private static boolean serializePrimitiveType(
					String name,
					Object object,
					XmlParameter xmlparameter) throws Exception {
		Class thisClass = object.getClass();
		String className = thisClass.getCanonicalName();

		if (className.equals("java.lang.String[]")) {
			String[] arraystr = ( String[] ) object;

			xmlparameter.setStringList(name, Arrays.asList(arraystr));
			return true;
	  }
		else if (className.equals("java.lang.StringBuilder")) {
			xmlparameter.setString(name, ((StringBuilder) object).toString());
			return true;
		} else if (className.equals("java.lang.String")) {
			xmlparameter.setString(name, (String) object);
			return true;
		} else if (className.equals("java.lang.Integer")) {
			xmlparameter.setInt(name, (long) ((Integer) object).intValue());
			return true;
		} else if (className.equals("java.lang.Long")) {
			xmlparameter.setInt(name, ((Long) object).longValue());
			return true;
		} else if (className.equals("java.lang.Double")) {
			xmlparameter.setDouble(name, ((Double) object).doubleValue());
			return true;
		} else if (className.equals("java.lang.Boolean")) {
			xmlparameter.setInt(name, ((Boolean) object).booleanValue() ? 1 : 0);
			return true;
		} else if (className.equals("com.fin.httrader.utils.Uid")) {
			xmlparameter.setUid(name, (Uid) object);
			return true;
		} else if (className.equals("java.util.LinkedHashMap")) {

			LinkedHashMap hashmap = (LinkedHashMap) object;

			List<XmlParameter> xmlparamlist = new ArrayList<XmlParameter>();

			for (Iterator it = hashmap.keySet().iterator(); it.hasNext();) {
				Object key_it = it.next();
				if (key_it == null)
					throw new HtException(getContext(), "serializePrimitiveType", "In attempt to serialize: " + className + " NULL key value was found");
				Object value_it = hashmap.get(key_it);

				XmlParameter param_key = new XmlParameter();
				XmlParameter param_value = new XmlParameter();

				// recursive call
				XmlParameter entry_it = new XmlParameter();
				serializeObjectToXmlParameter(value_it, param_value);
				serializeObjectToXmlParameter(key_it, param_key);

				entry_it.setXmlParameter("key", param_key);
				entry_it.setXmlParameter("value", param_value);

				entry_it.setString("java_key_type", key_it.getClass().getCanonicalName());
				entry_it.setString("java_value_type", value_it.getClass().getCanonicalName());

				xmlparamlist.add(entry_it);
			}

			xmlparameter.setXmlParameterList(name, xmlparamlist);

			return true;
		}
		else if (className.equals("java.util.concurrent.atomic.AtomicBoolean")) {
			xmlparameter.setInt(name, ((AtomicBoolean) object).get()? 1 : 0);
			return true;
		} 
		else if (className.equals("java.util.concurrent.atomic.AtomicInteger")) {
			xmlparameter.setInt(name, ((AtomicInteger) object).get());
			return true;
		} 


		return false;

	}

	private static Object deserializePrimitiveType(XmlParameter xmlparameter, String objectype) throws Exception {

		int type_i = xmlparameter.getType("value");

		if (type_i == XmlParameter.VT_Int) {
			long parval = xmlparameter.getInt("value");
			if (objectype == null) {
				return Long.valueOf(parval);
			} else if (objectype.equals("java.lang.Boolean")) {
				return Boolean.valueOf(parval == 1);
			} else if (objectype.equals("java.lang.Integer")) {
				return Integer.valueOf((int) parval);
			} else if (objectype.equals("java.lang.Long")) {
				return Long.valueOf(parval);
			} else if (objectype.equals("java.lang.Double")) {
				return Double.valueOf(parval);
			}	else if (objectype.equals("java.util.concurrent.atomic.AtomicBoolean")) {
				return Boolean.valueOf(parval == 1);
			}	else if (objectype.equals("java.util.concurrent.atomic.AtomicInteger")) {
				return Integer.valueOf((int)parval);
			}

		} else if (type_i == XmlParameter.VT_String) {
			String str = xmlparameter.getString("value");
			if (objectype == null) {
				return str;
			} else if (objectype.equals("java.lang.StringBuilder")) {
				return new StringBuilder(str);
			} else if (objectype.equals("java.lang.String")) {
				return str;
			}

		} else if (type_i == XmlParameter.VT_Double) {
			double parval = xmlparameter.getDouble("value");

			if (objectype == null) {
				return Double.valueOf(parval);
			} else if (objectype.equals("java.lang.Double")) {
				return Double.valueOf(parval);
			}


		} else if (type_i == XmlParameter.VT_Uid) {
			Uid uid = xmlparameter.getUid("value");
			if (objectype == null) {
				return new Uid(uid);
			} else if (objectype.equals("com.fin.httrader.utils.Uid")) {
				return new Uid(uid);
			} else if (objectype.equals("java.lang.StringBuilder")) {
				return new StringBuilder(uid.toString());
			} else if (objectype.equals("java.lang.String")) {
				return uid.toString();
			}
		}
		else if (type_i==XmlParameter.VT_StringList) {
			List<String> strlist = xmlparameter.getStringList("value");

			if (objectype==null) {
				String[] strarr = new String[ strlist.size() ];
				for(int i = 0; i < strarr.length; i++)
					strarr[i] = strlist.get(i);
				return strarr;
			}
			else if (objectype.equals("java.lang.String[]")) {
				String[] strarr = new String[ strlist.size() ];
				for(int i = 0; i < strarr.length; i++)
					strarr[i] = strlist.get(i);
				
				return strarr;
			}
			
		}
		else if (type_i == XmlParameter.VT_XmlParameterList) {
			return deserializeXmlParameterList(xmlparameter, objectype, "value");
		}

		return null;

	}

	// helper that returns LinkedHashMap as the only container of the list
	private static LinkedHashMap deserializeXmlParameterList(XmlParameter xmlparameter, String objectype, String name_i) throws Exception {


		//
		if (objectype.equals("java.util.LinkedHashMap")) {

			LinkedHashMap value_list_obj = new LinkedHashMap();
			List<XmlParameter> ldata = xmlparameter.getXmlParameterList(name_i);

			for (int i = 0; i < ldata.size(); i++) {
				XmlParameter ldata_i = ldata.get(i);

				XmlParameter key_i = ldata_i.getXmlParameter("key");
				XmlParameter value_i = ldata_i.getXmlParameter("value");

				String value_type = null;
				String key_type = null;

				if (ldata_i.getKeys().contains("java_value_type")) {
					value_type = ldata_i.getString("java_value_type");
				}

				if (ldata_i.getKeys().contains("java_key_type")) {
					key_type = ldata_i.getString("java_key_type");
				}

				// now need to parse this
				Object key_obj = deserializePrimitiveType(key_i, key_type);
				Object val_obj = deserializePrimitiveType(value_i, value_type);

				if (val_obj == null) {
					throw new HtException(getContext(), "deserializeXmlParameterList", "Cannot deserialize complex parameter of type: " + objectype);
				}

				if (key_obj != null) {
					value_list_obj.put(key_obj, val_obj);
				}

			}

			return value_list_obj;
		}


		return null;

	}

	// ------------------------------------------
	
	public static void serializeObjectToXmlParameter(Object object, XmlParameter xmlparameter) throws Exception {
		xmlparameter.clear();

		Class thisClass = object.getClass();
		// check if Object is of primitive type
		boolean isPrimitive = serializePrimitiveType("value", object, xmlparameter);

		if (isPrimitive) {
			return;
		}

		Map<String,Field> allFields = getInheritedDeclaredFields(thisClass);
		
		//Field[] fields = thisClass.getDeclaredFields();
		for(Iterator<String> it = allFields.keySet().iterator(); it.hasNext(); ) {
		//for (int i = 0; i < fields.length; i++) {
			Field field_i = allFields.get( it.next() );
			
			String name_i = field_i.getName();
			String typename_i = field_i.getType().getCanonicalName();

			
			Annotation ai = field_i.getAnnotation(HtSaveAnnotation.class);
      if (ai==null)
        continue;
			
			//if(!(ai instanceof HtSaveAnnotation))
      //  continue;
								
			// must skip static final
			/*
			int modif = field_i.getModifiers();

			if (Modifier.isStatic(modif)) {
				continue;
			}

			if (Modifier.isFinal(modif)) {
				continue;
			}

			if (Modifier.isPrivate(modif)) {
				continue;
			}

			if (Modifier.isProtected(modif)) {
				continue;
			}
			 * 
			 */



			field_i.setAccessible(true);

			Object obj_i = field_i.get(object);
			if (obj_i == null) {
				throw new HtException(getContext(), "serializeObjectToXmlParameter", "NULL parameter value: " + typename_i + " - " + name_i);
			}

			
			serializePrimitiveType(name_i, obj_i, xmlparameter);

		}
	}

	
	private static Map<String,Field> getInheritedDeclaredFields(Class<?> type) 
	{   
		Map<String, Field> result = new HashMap<String,Field>();      
		Class<?> i = type;     
		while (i != null && i != Object.class) { 
			Field[] r=i.getDeclaredFields();
			if (r!=null) {
				
				for(int k = 0; k < r.length; k++) {
					Field f = r[k];
					result.put( f.getName(), f );
				}
				
			}
			i = i.getSuperclass();     
		}   
		return result; 
	} 
	
	// only simple scalar types are available
	public static void deserializeObjectFromXmlParameter(Object object, XmlParameter xmlparameter) throws Exception {

		Class thisClass = object.getClass();


		Map<String,Field> allFields = getInheritedDeclaredFields(thisClass);
		
		for (Iterator<String> it = xmlparameter.getKeys().iterator(); it.hasNext();) {
			String name_i = it.next();

			
			Field field_i = allFields.get( name_i );
			
			
			if (field_i == null)
				continue;
			
			Annotation ai = field_i.getAnnotation(HtSaveAnnotation.class);
      if (ai==null)
        continue;
			
			//if(!(ai instanceof HtSaveAnnotation))
      //  continue;
			

			// must skip static final
			/*
			int modif = field_i.getModifiers();

			if (Modifier.isStatic(modif)) {
				continue;
			}

			if (Modifier.isFinal(modif)) {
				continue;
			}

			if (Modifier.isPrivate(modif)) {
				continue;
			}

			if (Modifier.isProtected(modif)) {
				continue;
			}
			 * 
			 */

			field_i.setAccessible(true);

			String typename_i = field_i.getType().getCanonicalName();

			int type_i = xmlparameter.getType(name_i);
			if (type_i == XmlParameter.VT_Int) {
				long parval = xmlparameter.getInt(name_i);
				if (typename_i.equals("int")) {
					field_i.setInt(object, (int) parval);
				} else if (typename_i.equals("long")) {
					field_i.setLong(object, parval);
				} else if (typename_i.equals("double")) {
					field_i.setDouble(object, (double) parval);
				} else if (typename_i.equals("boolean")) {
					field_i.setBoolean(object, parval == 1 ? true : false);
				} else {
					throw new HtException(getContext(), "deserializeObjectFromXmlParameter", "Object and XmlParameter types does not match for the field: " + typename_i + " - " + name_i);
				}
			} else if (type_i == XmlParameter.VT_Uid) {
				Uid uid = xmlparameter.getUid(name_i);

				if (typename_i.equals("com.fin.httrader.utils.Uid")) {
					Uid field_uid = (Uid) field_i.get(object);
					field_uid.fromUid(uid);
				} else if (typename_i.equals("java.lang.StringBuilder")) {
					StringBuilder field_str = (StringBuilder) field_i.get(object);
					field_str.setLength(0);
					field_str.append(uid.toString());

				} else if (typename_i.equals("java.lang.String")) {
					field_i.set(object, uid.toString());
				} else {
					throw new HtException(getContext(), "deserializeObjectFromXmlParameter", "Object and XmlParameter types does not match for the field: " + typename_i + " - " + name_i);
				}

			} else if (type_i == XmlParameter.VT_String) {
				String str = xmlparameter.getString(name_i);

				if (typename_i.equals("java.lang.StringBuilder")) {
					StringBuilder field_str = (StringBuilder) field_i.get(object);
					field_str.setLength(0);
					field_str.append(str);
				} else if (typename_i.equals("java.lang.String")) {
					field_i.set(object, str);
				} else {
					throw new HtException(getContext(), "deserializeObjectFromXmlParameter", "Object and XmlParameter types does not match for the field: " + typename_i + " - " + name_i);
				}

			} else if (type_i == XmlParameter.VT_Double) {
				double parval = xmlparameter.getDouble(name_i);
				if (typename_i.equals("double")) {
					field_i.setDouble(object, parval);
				} else {
					throw new HtException(getContext(), "deserializeObjectFromXmlParameter", "Object and XmlParameter types does not match for the field: " + typename_i + " - " + name_i);
				}
			} else if (type_i == XmlParameter.VT_XmlParameterList) {
				// this is composite type
				LinkedHashMap result = deserializeXmlParameterList(xmlparameter, typename_i, name_i);

				LinkedHashMap field_map = (LinkedHashMap) field_i.get(object);
				field_map.clear();

				if (result == null) {
					throw new HtException(getContext(), "deserializeObjectFromXmlParameter", "Cannot deserialize complex parameter of type: " + type_i + " and name: " + name_i);
				}

				field_map.putAll(result);

			}

		}
	}

	// -----------------------------------------
}
