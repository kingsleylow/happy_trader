/*
 * HtSimulationProfile.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.configprops.HtSimulationProfileProp;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.hlpstruct.TSGenerateTickMethod;
import com.fin.httrader.hlpstruct.TickUsage;
import com.fin.httrader.interfaces.ReturnValueEnterface;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.webserver.GridDescriptor.GridDataSet;
import com.fin.httrader.webserver.GridDescriptor.GridFieldDescriptor;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSimulationProfile extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/**
	 * Creates a new instance of HtSimulationProfile
	 */
	public HtSimulationProfile() {
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		// this is only intended to return session variables
		try {

			// generate new UID
			this.generateUniquePageId();



			// this is initial load
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
			setStringSessionValue(getUniquePageId(), req, "profile_list", "");
			setStringSessionValue(getUniquePageId(), req, "simul_params", "");

			setStringSessionValue(getUniquePageId(), req, "tick_usage", "");
			setStringSessionValue(getUniquePageId(), req, "tick_split_num", "");
			setStringSessionValue(getUniquePageId(), req, "mult_factor", "");
			setStringSessionValue(getUniquePageId(), req, "data_aggr_periods", "");
			setStringSessionValue(getUniquePageId(), req, "gen_ticks_methods", "");


			String operation = getStringParameter(req, "operation", false);


			if (operation.equalsIgnoreCase("refresh_page")) {
				readProfileProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("apply_changes")) {
				applyChanges(req);
				readProfileProperty(req, null, true);
			} else if (operation.equalsIgnoreCase("add_new_profile")) {

				String new_profile = addNewProfile(req);
				readProfileProperty(req, new_profile, false);
			} else if (operation.equalsIgnoreCase("delete_profile")) {

				deleteProfile(req);
				readProfileProperty(req, null, false);
			} else {
				throw new HtException(getContext(), "initialize", "Invalid operation: " + operation);
			}




		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			
			return false;
		}
		
		
		return true;

	}
	//

	private String addNewProfile(HttpServletRequest req) throws Exception {

		// new provider name
		String new_profile = getStringParameter(req, "new_profile", false);

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileList();
		if (profiles.contains(new_profile)) {
			throw new HtException(getContext(), "addNewProfile", "Simulation Profile already exists: \"" + new_profile + "\"");
		}


		HtSimulationProfileProp prop = new HtSimulationProfileProp();
		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setSimulationProfileProperty(new_profile, prop);

		return new_profile;
	}

	private void deleteProfile(HttpServletRequest req) throws Exception {
		String cur_profile = getStringParameter(req, "cur_profile", false);

		Set<String> profiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileList();
		if (!profiles.contains(cur_profile)) {
			throw new HtException(getContext(), "deleteProfile", "Simulation Profile does not exist: \"" + cur_profile + "\"");
		}

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_removeSimulationProfileProperty(cur_profile);

	}

	private void applyChanges(HttpServletRequest req) throws Exception {
		String cur_profile = getStringParameter(req, "cur_profile", false);
		HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(cur_profile);


		// add new properties
		// RT Provider, Symbol,Simulation Spread,Slippage,Min S/L Distance,Min T/P Distance,
		// Min Limit Order Distance,Min Stop Order Distance, Comission, Significant Digits,Point Value
		//Set<Map<String, String>> rawprops = getStringMappedTableParameters(req, "properties", true);


		String all_sim_prperties_s = getStringParameter(req, "properties", true);
		XmlParameter xmlparameter = XmlHandler.deserializeParameterStatic(all_sim_prperties_s);


		prop.clear();

		//List<String> columns = xmlparameter.getStringList("columns");

		XmlParameter value_data_2 = xmlparameter.getXmlParameter("data");
		List<XmlParameter> rows_data = value_data_2.getXmlParameterList("rows");

		for (int i = 0; i < rows_data.size(); i++) {
			XmlParameter row_i = rows_data.get(i);
			//Map<String, String> row = it.next();

			HtSimulationProfileProp.SymbolRelatedProperty p = new HtSimulationProfileProp.SymbolRelatedProperty();


			String provider = row_i.getXmlParameter("RT Provider").getString("value");
			String symbol = row_i.getXmlParameter("Symbol").getString("value");

			p.simulationSpread_m = Double.valueOf(row_i.getXmlParameter("Simulation Spread").getString("value"));
			p.simulationSlippage_m = Double.valueOf(row_i.getXmlParameter("Slippage").getString("value"));
			p.minStopLossDistance_m = Double.valueOf(row_i.getXmlParameter("Min S/L Distance").getString("value"));
			p.minTakeProfitDistance_m = Double.valueOf(row_i.getXmlParameter("Min T/P Distance").getString("value"));
			p.minLimitOrderDistance_m = Double.valueOf(row_i.getXmlParameter("Min Limit Order Distance").getString("value"));
			p.minStopOrderDistance_m = Double.valueOf(row_i.getXmlParameter("Min Stop Order Distance").getString("value"));
			p.comission_m = Double.valueOf(row_i.getXmlParameter("Comission Abs").getString("value"));
			p.comissionPct_m = Double.valueOf(row_i.getXmlParameter("Comission Pct").getString("value"));
			p.signDigits_m = Integer.valueOf(row_i.getXmlParameter("Significant Digits").getString("value"));
			p.pointValue_m = Double.valueOf(row_i.getXmlParameter("Point Value").getString("value"));


			p.executePosExactPrices_m = (Integer.valueOf(row_i.getXmlParameter("Execution Exact Price").getString("value")) == 1 ? true : false);

			Uid uid = prop.resolveProviderSymbol(provider, symbol);
			prop.setSymbolproperty(uid, p);

		}

		prop.genTickMethod_m = (int) getIntParameter(req, "gen_tick_method", false);
		prop.aggrPeriod_m = (int) getIntParameter(req, "data_aggr_period", false);
		prop.multFactor_m = (int) getIntParameter(req, "mult_factor", false);
		prop.tickSplitNumber_m = (int) getIntParameter(req, "tick_split_num", false);
		prop.tickUsage_m = (int) getIntParameter(req, "tick_usage", false);

		// validate
		prop.validate();

		HtCommandProcessor.instance().get_HtConfigurationProxy().remote_setSimulationProfileProperty(cur_profile, prop);
	}

	private void readProfileProperty(HttpServletRequest req, String cur_profile_passed, boolean read_param) throws Exception {
		// list of providers
		Set<String> simprofiles = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileList();

		// list is empty
		if (simprofiles.size() <= 0) {
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
			cur_profile = (String) simprofiles.toArray()[0];
		}


		setStringSessionValue(getUniquePageId(), req, "profile_list", HtWebUtils.createOptionList(cur_profile, simprofiles, false));

		// read parameter

		GridDataSet dataset = new GridDataSet(
						new GridFieldDescriptor[]{
							new GridFieldDescriptor("RT Provider", "ed"),
							new GridFieldDescriptor("Symbol", "ed"),
							new GridFieldDescriptor("Simulation Spread", "ed"),
							new GridFieldDescriptor("Slippage", "ed"),
							new GridFieldDescriptor("Min S/L Distance", "ed"),
							new GridFieldDescriptor("Min T/P Distance", "ed"),
							new GridFieldDescriptor("Min Limit Order Distance", "ed"),
							new GridFieldDescriptor("Min Stop Order Distance", "ed"),
							new GridFieldDescriptor("Comission Abs", "ed"),
							new GridFieldDescriptor("Comission Pct", "ed"),
							new GridFieldDescriptor("Significant Digits", "ed"),
							new GridFieldDescriptor("Point Value", "ed"),
							new GridFieldDescriptor("Execution Exact Price", "ch")
						});



		HtSimulationProfileProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getSimulationProfileProperty(cur_profile);


		List<HtPair<String, String>> provSymbols = prop.getRegisteredProviderSymbols();
		for (int i = 0; i < provSymbols.size(); i++) {

			String provider = provSymbols.get(i).first;
			String symbol = provSymbols.get(i).second;

			Uid uid = prop.resolveProviderSymbol(provider, symbol);
			HtSimulationProfileProp.SymbolRelatedProperty p = prop.getSymbolProperty(uid);


			//
			//RT Provider, Symbol,Simulation Spread,Slippage,Min S/L Distance,Min T/P Distance, Min Limit Order Distance,Min Stop Order Distance, Comission, Significant Digits,Point Value,Execution Exact Price

			dataset.addRow();
			dataset.setRowValue(i, 0, provider);
			dataset.setRowValue(i, 1, symbol);
			dataset.setRowValue(i, 2, HtUtils.formatPriceValue(p.simulationSpread_m, p.signDigits_m, true));
			dataset.setRowValue(i, 3, HtUtils.formatPriceValue(p.simulationSlippage_m, p.signDigits_m, true));
			dataset.setRowValue(i, 4, HtUtils.formatPriceValue(p.minStopLossDistance_m, p.signDigits_m, true));
			dataset.setRowValue(i, 5, HtUtils.formatPriceValue(p.minTakeProfitDistance_m, p.signDigits_m, true));
			dataset.setRowValue(i, 6, HtUtils.formatPriceValue(p.minLimitOrderDistance_m, p.signDigits_m, true));
			dataset.setRowValue(i, 7, HtUtils.formatPriceValue(p.minStopOrderDistance_m, p.signDigits_m, true));
			dataset.setRowValue(i, 8, HtUtils.formatPriceValue(p.comission_m, 4, false));
			dataset.setRowValue(i, 9, HtUtils.formatPriceValue(p.comissionPct_m, 4, false));
			dataset.setRowValue(i, 10, String.format("%d", p.signDigits_m));
			dataset.setRowValue(i, 11, HtUtils.formatPriceValue(p.pointValue_m, p.signDigits_m, true));
			dataset.setRowValue(i, 12, p.executePosExactPrices_m ? "1" : "0");

		}


		setStringSessionValue(getUniquePageId(), req, "simul_params", HtWebUtils.createHTMLForDhtmlGrid(dataset));

		// fill in the other parameters
		//





		setStringSessionValue(getUniquePageId(), req, "tick_usage",
						HtWebUtils.createOptionList(prop.tickUsage_m, TickUsage.getAllTickUsageTypes(), new ReturnValueEnterface() {

			@Override
			public Object getValue(Object key) {
				return TickUsage.getTickUsageName((Integer) key);
			}

			@Override
			public Object getAttributeValue(String attributeName, Object key) {
				throw new UnsupportedOperationException("Not supported yet.");
			}
		}));



		//


		setStringSessionValue(getUniquePageId(), req, "tick_split_num", String.valueOf(prop.tickSplitNumber_m > 0 ? prop.tickSplitNumber_m : 5));
		setStringSessionValue(getUniquePageId(), req, "mult_factor", String.valueOf(prop.multFactor_m > 0 ? prop.multFactor_m : 1));

		//



		setStringSessionValue(getUniquePageId(), req, "data_aggr_periods",
						HtWebUtils.createOptionList(prop.aggrPeriod_m, TSAggregationPeriod.getAllAggregationPeriods(), new ReturnValueEnterface() {

			@Override
			public Object getValue(Object key) {
				return TSAggregationPeriod.getAggregationPeriodName((Integer) key);
			}

			@Override
			public Object getAttributeValue(String attributeName, Object key) {
				throw new UnsupportedOperationException("Not supported yet.");
			}
		}));




		//


		setStringSessionValue(getUniquePageId(), req, "gen_ticks_methods",
						HtWebUtils.createOptionList(prop.genTickMethod_m, TSGenerateTickMethod.getAllGenerateTickMethods(), new ReturnValueEnterface() {

			@Override
			public Object getValue(Object key) {
				return TSGenerateTickMethod.getGenerateTickMethodNameName((Integer) key);
			}

			@Override
			public Object getAttributeValue(String attributeName, Object key) {
				throw new UnsupportedOperationException("Not supported yet.");
			}
		}));

	}
}
