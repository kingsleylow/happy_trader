/*
 * HtDataProfile.java
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtDataProfileProp;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.LongParam;
import java.io.IOException;
import java.util.Iterator;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtDataProfile extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtDataProfile */
	public HtDataProfile() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new UID
			this.generateUniquePageId();

			setStringSessionValue(getUniquePageId(), req, "profile_list", "");
			setStringSessionValue(getUniquePageId(), req, "profile_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(0));
			setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", "");


			// read initially
			clearTempPropertyStorage(req);
			readProfileProperty(req, null, false);


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			this.setUniquePageId(getStringParameter(req, "page_uid", false));
			String operation = getStringParameter(req, "operation", false);

			setStringSessionValue(getUniquePageId(), req, "profile_list", "");
			setStringSessionValue(getUniquePageId(), req, "profile_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(0));
			setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", "");

			if (operation.equalsIgnoreCase("refresh_page")) {
				clearTempPropertyStorage(req);
				readProfileProperty(req, null, true);
			} 
			else if (operation.equalsIgnoreCase("apply_changes")) {
				applyChanges(req);
				clearTempPropertyStorage(req);
				readProfileProperty(req, null, true);
				
			} 
			else if (operation.equalsIgnoreCase("synch_profile")) {
				synchProfile(req);
				readProfileProperty(req, null, true);
			}
			else if (operation.equalsIgnoreCase("add_new_profile")) {
				String new_profile = addNewProfile(req);
				clearTempPropertyStorage(req);
				readProfileProperty(req, new_profile, false);

			} 
			else if (operation.equalsIgnoreCase("delete_profile")) {
				deleteProfile(req);
				clearTempPropertyStorage(req);
				readProfileProperty(req, null, false);

			} 
			else if (operation.equalsIgnoreCase("update_temp")) {
				updateTemporarly(req);
				readProfileProperty(req, null, true);
			} 
			else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}
	
	/**
	 * 
	 * Helpers 
	 */
	
	
	private String addNewProfile(HttpServletRequest req) throws Exception {

		// new provider name
		String new_profile = getStringParameter(req, "new_profile", false);

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();
		if (profiles.contains(new_profile)) {
			throw new HtException(getContext(), "addNewProfile", "Data Profile already exists: \"" + new_profile + "\"");
		}


		LongParam result = new LongParam();
		StringBuilder textResult = new StringBuilder();
		
		
		HtDataProfileProp prop = new HtDataProfileProp();
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setDataProfileProperty(new_profile, prop,result,textResult );

		setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(result.getLong()));
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", textResult.toString());

				
		return new_profile;
	}

	private void synchProfile(HttpServletRequest req) throws Exception {
		String cur_profile = getStringParameter(req, "cur_profile", false);
		
		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();
		if (!profiles.contains(cur_profile)) {
			throw new HtException(getContext(), "synchProfile", "Data Profile does not exist: \"" + cur_profile + "\"");
		}
		
	
		HtDataProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileProperty(cur_profile);
			
		// try to create 
		LongParam result = new LongParam();
		StringBuilder textResult = new StringBuilder();
	
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_createDataProfileTables(cur_profile,  prop.getPartitionId(), result, textResult);
		
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(result.getLong()));
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", textResult.toString());
		
	}
	
	private void deleteProfile(HttpServletRequest req) throws Exception {
		String cur_profile = getStringParameter(req, "cur_profile", false);

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();
		if (!profiles.contains(cur_profile)) {
			throw new HtException(getContext(), "deleteProfile", "Data Profile does not exist: \"" + cur_profile + "\"");
		}
		
		LongParam result = new LongParam();
		StringBuilder textResult = new StringBuilder();

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeDataProfileProperty(cur_profile, result, textResult);
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(result.getLong()));
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", textResult.toString());
	}

	private void applyChanges(HttpServletRequest req) throws Exception {
		String cur_profile = getStringParameter(req, "cur_profile", false);
		HtDataProfileProp prop = (HtDataProfileProp) getObjectSessionValue(getUniquePageId(), req, "tmp_prop");
		if (prop == null) {
			prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileProperty(cur_profile);
		}

		LongParam result = new LongParam();
		StringBuilder textResult = new StringBuilder();
		
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setDataProfileProperty(cur_profile, prop, result, textResult);
		
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result", String.valueOf(result.getLong()));
		setStringSessionValue(getUniquePageId(), req, "profile_operation_result_string", textResult.toString());

	}

	private void updateTemporarly(HttpServletRequest req) throws Exception {
		String new_time_scale = getStringParameter(req, "new_time_scale", true);
		String new_subscribed = getStringParameter(req, "new_subscribed", true);
		String new_subscribed_level1 = getStringParameter(req, "new_subscribed_level1", true);
		String new_subscribed_drawobj = getStringParameter(req, "new_subscribed_drawobj", true);

		String new_multfactor = getStringParameter(req, "new_mult_factor", true);

		String cur_profile =  getStringParameter(req, "cur_profile", false);
		String cur_provider = getStringParameter(req, "cur_provider", false);

		// try to resolve from session
		HtDataProfileProp prop = (HtDataProfileProp) getObjectSessionValue(getUniquePageId(), req, "tmp_prop");

		if (prop == null) {
			prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileProperty(cur_profile);
		}


		boolean old_is_subscribed = prop.isProviderSubscribed_RTHist(cur_provider);
		boolean old_is_subscribed_level1 = prop.isProviderSubscribed_Level1(cur_provider);
		boolean old_is_subscribed_drawobj = prop.isProviderSubscribed_DrawObj(cur_provider);

		int old_mult_factor = prop.getProviderMultFactor(cur_provider);
		int old_time_scale = prop.getProviderTimeScale(cur_provider);

		// set up default mult factor if not initialized
		if (old_mult_factor < 0) {
			old_mult_factor = 1;
		}

		prop.updateProvider(cur_provider,
						new_multfactor.length() > 0 ? Integer.valueOf(new_multfactor) : old_mult_factor,
						new_time_scale.length() > 0 ? Integer.valueOf(new_time_scale) : old_time_scale,
						new_subscribed.length() > 0 ? Boolean.valueOf(new_subscribed) : old_is_subscribed,
						new_subscribed_level1.length() > 0 ? Boolean.valueOf(new_subscribed_level1): old_is_subscribed_level1,
						new_subscribed_drawobj.length() > 0 ? Boolean.valueOf(new_subscribed_drawobj): old_is_subscribed_drawobj
		);

		setObjectSessionValue(getUniquePageId(), req, "tmp_prop", prop);

	}

	private void readProfileProperty(HttpServletRequest req, String cur_profile_passed, boolean read_param) throws Exception {
		// list of providers
		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();

		// list is empty
		if (profiles.size() <= 0) {
			return;
		}

		// current provider:
		String cur_profile = null;


		if (cur_profile_passed == null) {
			if (read_param) {
				cur_profile = getStringParameter(req, "cur_profile", true);
			}
		} else {
			cur_profile = cur_profile_passed;
		}

		// fisr parameter if no current
		if (cur_profile == null || cur_profile.length() <= 0) {
			cur_profile = (String) profiles.toArray()[0];
		}





		setStringSessionValue(getUniquePageId(), req, "profile_list", HtWebUtils.createOptionList(cur_profile, profiles, false));

		// read profile parameters
		StringBuilder out = new StringBuilder();
		HtDataProfileProp prop = (HtDataProfileProp) getObjectSessionValue(getUniquePageId(), req, "tmp_prop");

		if (prop == null) {
			prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getDataProfileProperty(cur_profile);
		}

		// looping through all providers
		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();
		int cnt = 0;
		
		out.append("<tr NOWRAP=true>");
		out.append("<td type='ro'>RT Provider</td>");
		out.append("<td type='ro'>Multiplication Factor</td>");
		out.append("<td type='ro'>Time Scale</td>");
		out.append("<td type='ro'>RT/HISTORY Subscribed</td>");
		out.append("<td type='ro'>LEVEL 1 Subscribed</td>");
		out.append("<td type='ro'>Drawable Objects Subscribed</td>");
		out.append("</tr>");;


		for (Iterator<String> itp = providers.iterator(); itp.hasNext();) {
			out.append("<tr NOWRAP=true id='");
			out.append(++cnt);
			out.append("'>");

			String value = itp.next();
			
			int timescale = prop.getProviderTimeScale(value);
			int multfactor = prop.getProviderMultFactor(value);


			out.append("<td>");
			out.append(value);
			out.append("</td>");

			out.append("<td>");
			Integer[] mfactors = HtDataProfileProp.returnAllMultFactors(timescale);
			if (mfactors == null) {
				throw new HtException(getContext(), "readProfileProperty", "Invalid multiply factor for time scale: " + timescale);
			}

			if (mfactors.length != 0) {
				out.append("<select id='d_").append(cnt).append("' style='font-size:smaller;width=100%' onchange='change_multfactor(\"").append(value).append("\", " + "d_").append(cnt).append(");'>");
				for (int k = 0; k < mfactors.length; k++) {
					if (mfactors[k] == multfactor) {
						out.append("<option value=").append(mfactors[k]).append(" selected>").append(mfactors[k]).append("</option>");
					} else {
						out.append("<option value=").append(mfactors[k]).append(">").append(mfactors[k]).append("</option>");
					}
				}
				out.append("</select>");
			} else {
				out.append("Not Available");
			}


			out.append("</td>");



			out.append("<td>");
			Integer[] allTimeScales = HtDataProfileProp.returnAllTimeScales();
			out.append("<select id='o_").append(cnt).append("' style='font-size:smaller;width=120px' onchange='change_timescale(\"").append(value).append("\", " + "o_").append(cnt).append(");'>");

			for (int k = 0; k < allTimeScales.length; k++) {

				if (allTimeScales[k] == timescale) {
					out.append("<option selected value=").append(allTimeScales[k]).append(">").append(HtDataProfileProp.timeScaleToString(allTimeScales[k])).append("</option>");
				} else {
					out.append("<option value=").append(allTimeScales[k]).append(">").append(HtDataProfileProp.timeScaleToString(allTimeScales[k])).append("</option>");
				}

			}
			out.append("</select>");
			out.append("</td>");

			out.append("<td>");
			boolean is_subsc = prop.isProviderSubscribed_RTHist(value);
			out.append("<input id='cb_").append(cnt).append("' type=checkbox ").append(is_subsc ? "checked=true" : "").append(" onclick='change_subscribed_rthist(\"").append(value).append("\", " + "cb_").append(cnt).append(");' />");
			out.append("</td>");

			out.append("<td>");
			boolean is_level1_subsc = prop.isProviderSubscribed_Level1(value);
			out.append("<input id='cblevel1_").append(cnt).append("' type=checkbox ").append(is_level1_subsc ? "checked=true" : "").append(" onclick='change_subscribed_level1(\"").append(value).append("\", " + "cblevel1_").append(cnt).append(");' />");
			out.append("</td>");

			out.append("<td>");
			boolean is_drawobj_subsc = prop.isProviderSubscribed_DrawObj(value);
			out.append("<input id='cbdrawobj_").append(cnt).append("' type=checkbox ").append(is_drawobj_subsc ? "checked=true" : "").append(" onclick='change_subscribed_drawobj(\"").append(value).append("\", " + "cbdrawobj_").append(cnt).append(");' />");
			out.append("</td>");

			out.append("</tr>");
		}

		
		setStringSessionValue(getUniquePageId(), req, "profile_parameters", out.toString());

		setStringSessionValue(getUniquePageId(), req, "partition_id", String.valueOf(prop.getPartitionId()));



	}

	private void clearTempPropertyStorage(HttpServletRequest req) throws Exception {
		setObjectSessionValue(getUniquePageId(), req, "tmp_prop", null);
	}
}
