/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.logging.Level;

/**
 *
 * @author Victor_Zoubok
 * This is a helper class which parses a derived and looks all
 * public static final int...
 * and then return then
 */
public class HtEnumeratorDescriptor {
	private final TreeMap<Integer, String> nameMap_m = new TreeMap<Integer, String>();
	private final TreeMap<String, Integer> idMap_m = new TreeMap<String, Integer>();

		public HtEnumeratorDescriptor(String baseClass)
	{
		try {
			// get class object
			Class clazz = Class.forName(baseClass);
			
			Field[] fields = clazz.getFields();

			for(int i = 0; i < fields.length; i++) {
				int modif = fields[i].getModifiers();
				if (Modifier.isStatic(modif) && Modifier.isFinal(modif) && Modifier.isPublic(modif)) {
						
					String typename_i = fields[i].getType().getCanonicalName();
					if (typename_i.equalsIgnoreCase("int")) {
					
						String name_i = fields[i].getName();
						int key_i = fields[i].getInt(null);

						nameMap_m.put(key_i, name_i);
						idMap_m.put(name_i, key_i);
					}
					
				}
			}


			
		}
		catch(Throwable e)
		{
			HtLog.log(Level.SEVERE, "HtBaseEnumeration", "parseEnumeration", "Exception on processing enumeration for class: '" + baseClass + "' - " + e.getMessage());
		}
	}

	public Map<Integer, String> getNameMap()
	{
		return nameMap_m;
	}

	public Map<String, Integer> getIdMap()
	{
		return idMap_m;
	}

	public Set<Integer> getIdSet()
	{
		return nameMap_m.keySet();
	}

	public Set<Integer> getIdSet(int excluded_id)
	{
		TreeSet<Integer> result = new TreeSet<Integer>();
		result.addAll( nameMap_m.keySet() );
		result.remove(excluded_id);
		
		return result;
	}

	public Set<String> getNameSet()
	{
		return idMap_m.keySet();
	}

	public String getNameById_StrictValue(int id)
	{
		String val =getNameMap().get(id);
		return (val != null ? val: "N/A");
	}

	public String getNameById_StrictValue(int id, String default_val)
	{
		String val =getNameMap().get(id);
		return (val != null ? val: default_val);
	}

	public int getIdByName_StrictValue(String value)
	{
		Integer pt_i = getIdMap().get(value);
		return (pt_i != null ? pt_i: -1);
	}

	public int getIdByName_StrictValue(String value, int id_default)
	{
		Integer pt_i = getIdMap().get(value);
		return (pt_i != null ? pt_i: id_default);
	}

	public String getNameById_BitWize(int id)
	{
		StringBuilder out = new StringBuilder();

		Map<Integer, String> namemap = getNameMap();
		for(Iterator<Integer> it = namemap.keySet().iterator(); it.hasNext(); ) {
			int key = it.next();
			String val = namemap.get(key);

			if ((id & key) > 0) {
				out.append(val);
				out.append(" | ");
			}

		}

		return out.toString();
	}

}
