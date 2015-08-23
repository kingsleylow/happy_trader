/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtAlgBrkPairProp;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.interfaces.ReturnValueEnterface;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtAlgLibPair_v2 extends HtServletsBase {

	@Override
	public String getContext() {
		return getClass().getSimpleName();
	}

	/**
	 * Creates a new instance of HtAlgLibPair
	 */
	public HtAlgLibPair_v2() {
	}

	@Override
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		// this is only intended to return session variables
		try {

			// generate new UID
			this.generateUniquePageId();


			// this is initial load
			readPairProperty(req, null, false);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;

		}
		
		return true;


	}

	@Override
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);


		try {

			setUniquePageId(getStringParameter(req, "page_uid", false));

			setStringSessionValue(getUniquePageId(), req, "pair_list", "");
			setStringSessionValue(getUniquePageId(), req, "alg_pair_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters2", "");
			setStringSessionValue(getUniquePageId(), req, "alg_path", "");
			setStringSessionValue(getUniquePageId(), req, "brk_path", "");
			setStringSessionValue(getUniquePageId(), req, "brk_path2", "");


			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("refresh_page")) {
				readPairProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("apply_changes")) {
				applyChanges(req);
				readPairProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("add_new_pair")) {
				String new_pair = addNewPair(req);
				readPairProperty(req, new_pair, false);

			} else if (operation.equalsIgnoreCase("delete_pair")) {
				deletePair(req);
				readPairProperty(req, null, false);

			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}

	; //

	private String addNewPair(HttpServletRequest req) throws Exception {

		// new provider name
		String new_pair = getStringParameter(req, "new_pair", false);

		Set<String> pairs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList();
		if (pairs.contains(new_pair)) {
			throw new HtException(getContext(), "addNewPair", "Algorithm Broker Pair already exists: \"" + new_pair + "\"");
		}

		HtAlgBrkPairProp prop = new HtAlgBrkPairProp();
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setAlgBrkPairProperty(new_pair, prop);

		return new_pair;
	}

	private void deletePair(HttpServletRequest req) throws Exception {
		String cur_pair = getStringParameter(req, "cur_pair", false);

		Set<String> pairs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList();
		if (!pairs.contains(cur_pair)) {
			throw new HtException(getContext(), "deletePair", "Algorithm Broker Pair does not exist: \"" + cur_pair + "\"");
		}

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeAlgBrkPairProperty(cur_pair);

	}

	private void savePropertyEntries(List<HtPair<String, String>> properties, Map<String, String> target) throws Exception {
		
			
		target.clear();
		
		if (properties == null)
				return;
		
		for (int i = 0; i < properties.size(); i++) {
			HtPair<String, String> entry = properties.get(i);
			if (entry.first == null || entry.first.length() <= 0) {
				throw new HtException(getContext(), "checkPropertiesEntry", "Invalid property entry");
			}

			target.put(entry.first, entry.second);
		}
	}

	private void applyChanges(HttpServletRequest req) throws Exception {
		// current provider - raise an exception if not found
		String cur_pair = getStringParameter(req, "cur_pair", false);

		// other parameters
		HtAlgBrkPairProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getAlgBrkPairProperty(cur_pair);

		// properies to update
		List<HtPair<String, String>> alg_properties = ExportImportXGridUtils.getAsPropertiesList(getStringParameter(req, "alg_param", true));
		savePropertyEntries(alg_properties, prop.getAlgoritmParams());


		// properies to update
		List<HtPair<String, String>> brk_properties = ExportImportXGridUtils.getAsPropertiesList(getStringParameter(req, "brk_param", true));
		savePropertyEntries(brk_properties, prop.getBrokerParams());


		//

		List<HtPair<String, String>> brk_properties2 = ExportImportXGridUtils.getAsPropertiesList(getStringParameter(req, "brk_param2", true));
		savePropertyEntries(brk_properties2, prop.getBrokerParams2());


		// can't be empty
		prop.setAlgorithmPath(getStringParameter(req, "alg_path", false));
		if (prop.getAlgorithmPath().length() <= 0) {
			throw new HtException(getContext(), "applyChanges", "Algorithm path cannot be empty");
		}

		// may be empty
		prop.setBrokerPath(getStringParameter(req, "brk_path", true));
		prop.setBrokerPath2(getStringParameter(req, "brk_path2", true));


		// save
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setAlgBrkPairProperty(cur_pair, prop);

	}

	private void readPairProperty(HttpServletRequest req, String cur_pair_passed, boolean read_param) throws Exception {
		setStringSessionValue(getUniquePageId(), req, "is_broker_here", "0");

		Set<String> pairs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredAlgBrkPairList();
		// list is empty
		if (pairs.size() <= 0) {
			return;
		}

		// current provider:
		String cur_pair = null;


		if (cur_pair_passed == null) {
			if (read_param) {
				cur_pair = getStringParameter(req, "cur_pair", true);
			}
		} else {
			cur_pair = cur_pair_passed;
		}

		// fisr parameter if no current
		if (cur_pair == null || cur_pair.length() <= 0) {
			cur_pair = (String) pairs.toArray()[0];
		}

		// provider property
		HtAlgBrkPairProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getAlgBrkPairProperty(cur_pair);

		setStringSessionValue(getUniquePageId(), req, "pair_list", HtWebUtils.createOptionList(cur_pair, pairs, false));

		//

		//Set<File> available_executables = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllDlls();


		//Set<File> available_alg_libs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllAlgorithmDlls();
		//Set<File> available_brk_libs = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_findAllBrokerDlls();


		/*
		Set<String> available_paths = new TreeSet<String>();
		for (Iterator<File> it = available_executables.iterator(); it.hasNext();) {
			File file_i = it.next();
			available_paths.add(file_i.getAbsolutePath());
		}
		
		*/ 


		/*
		//setStringSessionValue(getUniquePageId(), req, "available_dlls", HtWebUtils.createOptionList(null, available_paths, false ));
		setStringSessionValue(getUniquePageId(), req, "available_dlls_alg", HtWebUtils.createOptionList(null, available_alg_libs,
						new ReturnValueEnterface() {

							@Override
							public Object getValue(Object key) {
								File f=  (File)key;
								return f.getAbsolutePath();
							}

							@Override
							public Object getAttributeValue(String attributeName, Object key) {
								throw new UnsupportedOperationException("Not supported yet.");
							}
						}));
		
		
		setStringSessionValue(getUniquePageId(), req, "available_dlls_brk", HtWebUtils.createOptionList(null, available_brk_libs,
						new ReturnValueEnterface() {

							@Override
							public Object getValue(Object key) {
								File f=  (File)key;
								return f.getAbsolutePath();
							}

							@Override
							public Object getAttributeValue(String attributeName, Object key) {
								throw new UnsupportedOperationException("Not supported yet.");
							}
						}));
						
		*/ 

		setStringSessionValue(getUniquePageId(), req, "alg_path", prop.getAlgorithmPath());

		//
		setStringSessionValue(getUniquePageId(), req, "brk_path", prop.getBrokerPath());
		setStringSessionValue(getUniquePageId(), req, "brk_path2", prop.getBrokerPath2());
		//


		setStringSessionValue(getUniquePageId(), req, "alg_pair_parameters", HtWebUtils.createHtmlParametersTable(prop.getAlgoritmParams()));
		setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters", HtWebUtils.createHtmlParametersTable(prop.getBrokerParams()));
		setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters2", HtWebUtils.createHtmlParametersTable(prop.getBrokerParams2()));



		setStringSessionValue(getUniquePageId(), req, "is_broker_here_2", prop.getBrokerPath2().length() > 0 ? "1" : "0");
		setStringSessionValue(getUniquePageId(), req, "is_broker_here", prop.getBrokerPath().length() > 0 ? "1" : "0");

	}
}
