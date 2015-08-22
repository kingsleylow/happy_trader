/*
 * HtExportImportWizard.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import java.io.IOException;

import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExportImportWizard extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtExportImportWizard */
	public HtExportImportWizard() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// generate new Uid and set initial values
			generateUniquePageId();

			// init wizard params
			initWizardParameters(req);

			initThisPageGlobals(req);

			// step_start
			setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "start_done");

			setStringSessionValue(getUniquePageId(), req, "current_dialog_oper", "start_done");
			setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_1");

			// get provider list
			Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredHistoryProvidersList();

			//String selected_provider = getStringSessionValue(getUniquePageId(), req, "wiz_history_provider");
			setStringSessionValue(getUniquePageId(), req, "provider_list", HtWebUtils.createOptionList(null, providers, false));




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	// this is initialization function for dialog
	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);



		try {

			setUniquePageId(getStringParameter(req, "page_uid", false));


			String operation = getStringParameter(req, "operation", false);
			initThisPageGlobals(req);


			// force to set parameters
			String set_param = getStringParameter(req, "set_param", true);

			// this must exist
			if (operation.equals("step_start")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "start_done");

				setStringSessionValue(getUniquePageId(), req, "current_dialog_oper", "start_done");
				setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_1");

				// get provider list
				Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredHistoryProvidersList();


				String selected_provider = getStringSessionValue(getUniquePageId(), req, "wiz_history_provider");
				setStringSessionValue(getUniquePageId(), req, "provider_list", HtWebUtils.createOptionList(selected_provider, providers, false));

			} else if (operation.equals("do_step_1")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_1_done");

				setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_2");
				setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "step_start");


				if (set_param.equals("true")) {
					// expect history provider
					setStringSessionValue(getUniquePageId(), req, "wiz_history_provider", getStringParameter(req, "history_provider", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_history_provider_idx", getStringParameter(req, "history_provider_idx", true));
				}
			} else if (operation.equals("do_step_2")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_2_done");

				setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_25");
				setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "do_step_1");

				// list of RT providers
				Set<String> rt_providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();

				String selected_rt_provider = getStringSessionValue(getUniquePageId(), req, "wiz_rt_provider");
				setStringSessionValue(getUniquePageId(), req, "rt_provider_list", HtWebUtils.createOptionList(selected_rt_provider, rt_providers, false));

				// data profile list
				Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();

				
				String selected_profile = getStringSessionValue(getUniquePageId(), req, "wiz_profile");
				setStringSessionValue(getUniquePageId(), req, "profile_list", HtWebUtils.createOptionList(selected_profile, profiles, false));


				if (set_param.equals("true")) {
					// expect whether import or export
					setStringSessionValue(getUniquePageId(), req, "wiz_operation_type", getStringParameter(req, "operation_type", true));

				}

			} else if (operation.equals("do_step_25")) {
				// if export show otherwise next stage
				String operation_type = getStringSessionValue(getUniquePageId(), req, "wiz_operation_type");

				// if not set restore that from session vars
				String param_profile = getStringParameter(req, "data_profile", true);
				String param_profile_idx = getStringParameter(req, "data_profile_idx", true);

				if (param_profile.length() <=0) {
					param_profile = getStringSessionValue(getUniquePageId(), req, "wiz_profile");
					param_profile_idx = getStringSessionValue(getUniquePageId(), req, "wiz_profile_idx");
				}

				if (operation_type.equals("export")) {


					// populate provider symbol list
					List<HtPair<String, String>> allProvSymbList =	HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getStoredProvidersSymbols_ForProfile_FromHistory(param_profile);

					// if we selected on previous stage just mark this
					List<HtPair<String, String>> provSymbList =(List<HtPair<String, String>>) getObjectSessionValue(getUniquePageId(), req, "wiz_export_symb_prov_list");

					// !!
					setStringSessionValue(getUniquePageId(), req, "provider_symbol_list_to_export", createXmlParametersTable(allProvSymbList, provSymbList));

					// make this navigation visible
					setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_25_done");
					setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "do_step_2");
					setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_3");


				} else if (operation_type.equals("import")) {
					// go forward and exclude from navigation
					setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_25_done");

				} else {
					throw new HtException(getContext(), "initialize", "Invalid (export/import)operation type: " + operation_type);
				}

				// store of what we have from previous page

				if (set_param.equals("true")) {
					// expect import data flag
					setStringSessionValue(getUniquePageId(), req, "wiz_import_behav", getStringParameter(req, "imp_behav", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider", getStringParameter(req, "rt_provider", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider_idx", getStringParameter(req, "rt_provider_idx", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_oper_log", getStringParameter(req, "oper_log", true));

					setStringSessionValue(getUniquePageId(), req, "wiz_profile", param_profile);
					setStringSessionValue(getUniquePageId(), req, "wiz_profile_idx", param_profile_idx);

				}


			} else if (operation.equals("do_step_3")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_3_done");

				String operation_type = getStringSessionValue(getUniquePageId(), req, "wiz_operation_type");

				// skip the previous navigation if import
				if (operation_type.equals("export")) {
					setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "do_step_25");

					// save provider symbol list
					List<HtPair<String, String>> provSymbList = getStringPairListParameter(req, "prov_symb_list", true);

					// store as object
					setObjectSessionValue(getUniquePageId(), req, "wiz_export_symb_prov_list", provSymbList);


				} else if (operation_type.equals("import")) {
					setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "do_step_2");
				} else {
					throw new HtException(getContext(), "initialize", "Invalid (export/import)operation type: " + operation_type);
				}


				setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_step_4");


				// provide date periods that may provide provider
				StringBuilder out = new StringBuilder();

				// extract property
				String hist_provider_name = getStringSessionValue(getUniquePageId(), req, "wiz_history_provider");


				if (operation_type.equals("import")) {
					HtHistoryProviderProp prop =
									HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getHistoryProviderProperty(hist_provider_name);

					Vector<String> symbols =
									HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getSymbolsToBeImported(
									prop.getParameters(), hist_provider_name, prop.getProviderClass());

					LongParam minDate = new LongParam(-1);
					LongParam maxDate = new LongParam(-1);

					out.append("<?xml version=\"1.0\"?>");
					out.append("<rows>");


					for (int i = 0; i < symbols.size(); i++) {
						HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getMaxMinDatesSymbolsToBeImported(
										symbols.get(i),
										prop.getParameters(),
										hist_provider_name,
										prop.getProviderClass(),
										prop.getImportHourShift(),
										minDate,
										maxDate);

						out.append("<row id=\"").append(i).append("\">");
						out.append("<cell>").append(symbols.get(i)).append("</cell><cell>").append(HtDateTimeUtils.time2SimpleString_Gmt(minDate.getLong())).append("</cell><cell>").append(HtDateTimeUtils.time2SimpleString_Gmt(maxDate.getLong())).append("</cell>");
						out.append("</row>");
					}
					out.append("</rows>");
					// !!
					setStringSessionValue(getUniquePageId(), req, "import_provider_datetimes", out.toString());

				}
				

				if (set_param.equals("true")) {
					// expect import data flag
					/*
					setStringSessionValue(getUniquePageId(), req, "wiz_import_behav", getStringParameter(req, "imp_behav", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider", getStringParameter(req, "rt_provider", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider_idx", getStringParameter(req, "rt_provider_idx", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_oper_log", getStringParameter(req, "oper_log", true));

					setStringSessionValue(getUniquePageId(), req, "wiz_profile", getStringParameter(req, "data_profile", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_profile_idx", getStringParameter(req, "data_profile_idx", true));
					*/
				}

			} else if (operation.equals("do_step_4")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "step_4_done");
				setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "do_step_3");
				setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "do_finish");




				// get export import dates
				if (set_param.equals("true")) {
					setStringSessionValue(getUniquePageId(), req, "wiz_oper_dbeg", getStringParameter(req, "oper_dbeg", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_oper_dend", getStringParameter(req, "oper_dend", true));
					setStringSessionValue(getUniquePageId(), req, "wiz_date_selector_idx", getStringParameter(req, "date_selector_idx", true));
				}


			} else if (operation.equals("do_finish")) {
				setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "finish_done");


				// perform operation
				String history_provider = getStringSessionValue(getUniquePageId(), req, "wiz_history_provider");
				String operation_type = getStringSessionValue(getUniquePageId(), req, "wiz_operation_type");
				String import_behav = getStringSessionValue(getUniquePageId(), req, "wiz_import_behav");
				String rt_provider = getStringSessionValue(getUniquePageId(), req, "wiz_rt_provider");
				String oper_log = getStringSessionValue(getUniquePageId(), req, "wiz_oper_log");
				String oper_dbeg = getStringSessionValue(getUniquePageId(), req, "wiz_oper_dbeg");
				String oper_dend = getStringSessionValue(getUniquePageId(), req, "wiz_oper_dend");

				//
				boolean logenabled = oper_log.equals("true") ? true : false;

				// do not adjust from LOCAL as all is expected to be done in GMT
				long datebegin = HtDateTimeUtils.parseDateTimeParameter(oper_dbeg, false);
				long dateend = HtDateTimeUtils.parseDateTimeParameter(oper_dend, false);

				// resolve property
				HtHistoryProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getHistoryProviderProperty(history_provider);

				// this is the only parameter passed to finsih stage
				String operation_str_id = getStringParameter(req, "operation_str_id", false);
				String data_profile = getStringSessionValue(getUniquePageId(), req, "wiz_profile");

				if (operation_type.equals("export")) {
					List<HtPair<String, String>> provSymbList =(List<HtPair<String, String>>) getObjectSessionValue(getUniquePageId(), req, "wiz_export_symb_prov_list");

					
					HtCommandProcessor.instance().get_HtDatabaseProxy().remote_exportData_Nonblocking(
									operation_str_id,
									data_profile,
									history_provider,
									prop.getProviderClass(),
									prop.getParameters(),
									datebegin,
									dateend,
									provSymbList,
									prop.getExportHourShift(),
									logenabled,
									RtConfigurationManager.StartUpConst.SELECT_PAGE_DEFAULT_SIZE);


				} else if (operation_type.equals("import")) {
					// import flag
					int import_flag = -1;

					if (import_behav.equals("overwrite")) {
						import_flag = HtDatabaseProxy.IMPORT_FLAG_OVERWRITE;
					} else if (import_behav.equals("skip")) {
						import_flag = HtDatabaseProxy.IMPORT_FLAG_SKIP;
					} else if (import_behav.equals("exception")) {
						import_flag = HtDatabaseProxy.IMPORT_FLAG_EXCEPTION;
					} else if (import_behav.equals("bulk_silent")) {
						import_flag = HtDatabaseProxy.IMPORT_BULK_SILENTLY;
					}
					else {
						throw new HtException(getContext(), "initialize", "Invalid import flag: " + import_behav);
					}

					// start oper


					HtCommandProcessor.instance().get_HtDatabaseProxy().remote_importData_Nonblocking(
									operation_str_id,
									data_profile,
									import_flag,
									rt_provider,
									history_provider,
									prop.getProviderClass(),
									prop.getParameters(),
									datebegin,
									dateend,
									prop.getImportHourShift(),
									logenabled);
				} else {
					throw new HtException(getContext(), "initialize", "Invalid (export/import)operation type: " + operation_type);
				}





			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;

	}	 //

	public String prepareProviderSymbolListHtml(HttpServletRequest req) throws Exception {
		StringBuilder val5 = new StringBuilder();
		if (getStringSessionValue(getUniquePageId(), req, "wiz_operation_type").equals("export")) {
			val5.append("<td valign=top align=left class=x2 width=160px>Export symbol list</td>");

			List<HtPair<String, String>> provSymbList =	(List<HtPair<String, String>>) getObjectSessionValue(getUniquePageId(), req, "wiz_export_symb_prov_list");

			val5.append("<td align=left>");

			if (provSymbList != null) {
				val5.append("<table width=100% class=x8 >");
				for (int i = 0; i < provSymbList.size(); i++) {
					HtPair<String, String> data_i = provSymbList.get(i);
					String key = data_i.first;
					String value = data_i.second;

					val5.append("<tr><td align=left>");
					val5.append(key).append(" - ").append(value);
					val5.append("</td></tr>");
				}
				val5.append("</table>");
			}
			val5.append("<td align=left>");


		}
		return val5.toString();
	}

	private void initThisPageGlobals(HttpServletRequest req) throws Exception {

		// stage variables
		setStringSessionValue(getUniquePageId(), req, "current_dialog_stage", "");
		setStringSessionValue(getUniquePageId(), req, "next_dialog_oper", "");
		setStringSessionValue(getUniquePageId(), req, "prev_dialog_oper", "");
		setStringSessionValue(getUniquePageId(), req, "provider_list", "");
		setStringSessionValue(getUniquePageId(), req, "profile_list", "");
		setStringSessionValue(getUniquePageId(), req, "import_provider_datetimes", "");
		setStringSessionValue(getUniquePageId(), req, "provider_symbol_list_to_export", "");

		// need to check some info
		// data profiles
		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredDataProfilesList();
		if (profiles.size() <= 0) {
			throw new HtException(getContext(), "initThisPageGlobals", "No data profiels are registered!");
		}

		// RT providers
		Set<String> providers = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRegisteredRTProvidersList();

		if (providers.size() <= 0) {
			throw new HtException(getContext(), "initThisPageGlobals", "No RT providers are registered!");
		}

	}

	// this clears all parameters prepared by wizard, but this must be preserved ove prev/next calls
	private void initWizardParameters(HttpServletRequest req) throws Exception {
		setStringSessionValue(getUniquePageId(), req, "wiz_history_provider", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_history_provider_idx", "");

		setStringSessionValue(getUniquePageId(), req, "wiz_operation_type", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_import_behav", "");

		setStringSessionValue(getUniquePageId(), req, "wiz_profile", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_profile_idx", "");

		setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_rt_provider_idx", "");

		setStringSessionValue(getUniquePageId(), req, "wiz_oper_log", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_oper_dbeg", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_oper_dend", "");
		setStringSessionValue(getUniquePageId(), req, "wiz_date_selector_idx", "");


		setObjectSessionValue(getUniquePageId(), req, "wiz_export_symb_prov_list", null);
	}

	// this creates XML table and sets selected checkbozes if necessary
	private String createXmlParametersTable(List<HtPair<String, String>> data, List<HtPair<String, String>> toselect) {
		StringBuilder out = new StringBuilder();
		out.append("<?xml version=\"1.0\"?>");
		out.append("<rows>");
		for (int i = 0; i < data.size(); i++) {
			HtPair<String, String> data_i = data.get(i);
			String key = data_i.first;
			String value = data_i.second;

			boolean found = false;
			if (toselect != null && toselect.size() > 0) {
				for (int k = 0; k < toselect.size(); k++) {
					HtPair select_k = toselect.get(k);

					if (select_k.first.equals(key) && select_k.second.equals(value)) {
						found = true;
						break;
					}
				}
			}



			out.append("<row id=\"" + i + "\">");

			out.append("<cell>");
			out.append(found ? "1" : "0");
			out.append("</cell>");

			out.append("<cell>");
			out.append(key);
			out.append("</cell>");

			out.append("<cell>");
			out.append(value);
			out.append("</cell>");



			out.append("</row>");

		}

		out.append("</rows>");

		return out.toString();
	}
}
