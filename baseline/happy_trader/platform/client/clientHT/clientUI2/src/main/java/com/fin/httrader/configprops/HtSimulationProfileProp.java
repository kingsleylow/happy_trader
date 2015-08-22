/*
 * HtSimulationProfileProp.java
 *
 *
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.hlpstruct.TSGenerateTickMethod;
import com.fin.httrader.hlpstruct.TickUsage;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSimulationProfileProp extends HtBaseProp {

	public static class HtSimulationProfileBaseParams {

		public int genTickMethod_m = TSGenerateTickMethod.TG_Dummy;
		// aggregation period
		public int aggrPeriod_m = TSAggregationPeriod.AP_Dummy;
		// multiplication factor
		public int multFactor_m = -1;
		// tick split number
		public int tickSplitNumber_m = -1;
		// either Bid or Ask
		public int tickUsage_m = TickUsage.PC_USE_DUMMY;
	};

	// describes symbol related property
	public static class SymbolRelatedProperty {
		// in price units

		public SymbolRelatedProperty() 
		{
		}

		public void initByDefault() {
			simulationSpread_m = 0;
			simulationSlippage_m = 0;
			minStopLossDistance_m = 0;
			minTakeProfitDistance_m = 0;
			minLimitOrderDistance_m = 0;
			minStopOrderDistance_m = 0;
			comission_m = 0;
			comissionPct_m = 0;
			signDigits_m = 4;
			pointValue_m = 1;
			executePosExactPrices_m = true;
		}
		
		public double simulationSpread_m = -1;
		// in price units
		public double simulationSlippage_m = -1;
		// in price units
		public double minStopLossDistance_m = -1;
		// in price units
		public double minTakeProfitDistance_m = -1;
		// in price units
		public double minLimitOrderDistance_m = -1;
		// in price units
		public double minStopOrderDistance_m = -1;
		
		// just abs comission
		public double comission_m = -1;
		
		// comission PCT
		public double comissionPct_m = -1;
		
		// significant digits after comma
		public int signDigits_m = -1;
		// point value
		public double pointValue_m = -1;
		// whether to execute exact price
		public boolean executePosExactPrices_m = true;
	}

	/** Creates a new instance of HtSimulationProfileProp */
	public HtSimulationProfileProp() {
		type_m = HtBaseProp.ID_CONFIGURE_SIMUL_PROF;
	}
	
	
	// registered providers symbols <-> cookie
	@HtSaveAnnotation
	public LinkedHashMap<String, LinkedHashMap<String, Uid>> registeredProvSymbols_m = new LinkedHashMap<String, LinkedHashMap<String, Uid>>();
	
	
	// these are common parameters for all the symbols
	// generate tick method
	@HtSaveAnnotation
	public int genTickMethod_m = TSGenerateTickMethod.TG_Dummy;
	
	
	// aggregation period
	@HtSaveAnnotation
	public int aggrPeriod_m = TSAggregationPeriod.AP_Dummy;
	
	
	// multiplication factor
	@HtSaveAnnotation
	public int multFactor_m = -1;
	
	
	// tick split number
	@HtSaveAnnotation
	public int tickSplitNumber_m = -1;
	
	
	// either Bid or Ask
	@HtSaveAnnotation
	public int tickUsage_m = TickUsage.PC_USE_DUMMY;
	
	
	// data for each symbol
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> simulationSpread_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> simulationSlippage_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> minStopLossDistance_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> minTakeProfitDistance_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> minLimitOrderDistance_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> minStopOrderDistance_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> comissions_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> comissionsPct_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Integer> signDigits_m = new LinkedHashMap<Uid, Integer>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Double> pointValues_m = new LinkedHashMap<Uid, Double>();
	
	@HtSaveAnnotation
	public LinkedHashMap<Uid, Boolean> executePosExactPrices_m = new LinkedHashMap<Uid, Boolean>();

	public Set<Uid> getRegisteredUids() {
		// just the simpliest way
		LinkedHashSet result = new LinkedHashSet();
		result.addAll(comissions_m.keySet());

		return result;
	}

	public void clear() {
		genTickMethod_m = TSGenerateTickMethod.TG_Dummy;
		aggrPeriod_m = TSAggregationPeriod.AP_Dummy;
		multFactor_m = -1;
		tickSplitNumber_m = -1;
		tickUsage_m = TickUsage.PC_USE_DUMMY;


		simulationSpread_m.clear();
		simulationSlippage_m.clear();
		minStopLossDistance_m.clear();
		minTakeProfitDistance_m.clear();
		minLimitOrderDistance_m.clear();
		minStopOrderDistance_m.clear();
		comissions_m.clear();
		comissionsPct_m.clear();
		signDigits_m.clear();
		pointValues_m.clear();
		executePosExactPrices_m.clear();
		registeredProvSymbols_m.clear();
	}

	// validate
	public void validate() throws Exception {
		if (!TSGenerateTickMethod.getAllGenerateTickMethods().contains(genTickMethod_m)) {
			throw new HtException(getClass().getSimpleName(), "isValid", "Generate tick method is invalid");
		}

		if (!TSAggregationPeriod.getAllAggregationPeriods().contains(aggrPeriod_m)) {
			throw new HtException(getClass().getSimpleName(), "isValid", "Aggregation period is invalid");
		}


		if (!TickUsage.getAllTickUsageTypes().contains(tickUsage_m)) {
			throw new HtException(getClass().getSimpleName(), "isValid", "Tick usage is invalid");
		}

		if (multFactor_m < 0) {
			throw new HtException(getClass().getSimpleName(), "isValid", "Multiplication factor is invalid");
		}

		if (tickSplitNumber_m < 4 && genTickMethod_m == TSGenerateTickMethod.TG_Uniform) {
			throw new HtException(getClass().getSimpleName(), "isValid", "Tick split number is invalid, for TG_Uniform method please use more or equal 4");
		}

		List<HtPair<String, String>> lst = getRegisteredProviderSymbols();

		for (int i = 0; i < lst.size(); i++) {
			String provider = lst.get(i).first;
			String symbol = lst.get(i).second;

			SymbolRelatedProperty prop = getSymbolProperty(resolveProviderSymbol(provider, symbol));
			if (prop.comission_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property comission is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.minLimitOrderDistance_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property min limit order distance is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.minStopLossDistance_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property min stop loss distance is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.minStopOrderDistance_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property min stop order distance is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.minTakeProfitDistance_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property min take profit distance is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.pointValue_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property point value is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.simulationSlippage_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property simulation slippage is invalid for: " +
								provider + " - " + symbol);
			}

			if (prop.simulationSpread_m < 0) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property simulation spread is invalid for: " +
								provider + " - " + symbol);
			}

			if ((prop.signDigits_m != -1) &&
							(prop.signDigits_m <= 0 || prop.signDigits_m >= 9)) {
				throw new HtException(getClass().getSimpleName(), "isValid", "In provider - symbol property significant digit is invalid for: " +
								provider + " - " + symbol);
			}

		}

	}

	public List<HtPair<String, String>> getRegisteredProviderSymbols() {
		List<HtPair<String, String>> result = new ArrayList<HtPair<String, String>>();

		for (Iterator<String> it = registeredProvSymbols_m.keySet().iterator(); it.hasNext();) {
			String provider = it.next();
			LinkedHashMap<String, Uid> symbolsMap = registeredProvSymbols_m.get(provider);

			for (Iterator<String> it2 = symbolsMap.keySet().iterator(); it2.hasNext();) {
				String symbol = it2.next();
				result.add(new HtPair<String, String>(provider, symbol));

			}
		}

		return result;
	}

	public boolean isExistProviderSymbol(String provider, String symbol) {

		if (registeredProvSymbols_m.containsKey(provider)) {
			LinkedHashMap<String, Uid> symbolsMap = registeredProvSymbols_m.get(provider);

			if (symbolsMap.containsKey(symbol)) {
				return true;
			}
		}


		return false;

	}

	public Uid resolveProviderSymbol(String provider, String symbol) {
		Uid cookie = new Uid();
		LinkedHashMap<String, Uid> symbolsMap = null;
		if (registeredProvSymbols_m.containsKey(provider)) {
			symbolsMap = registeredProvSymbols_m.get(provider);
		} else {
			symbolsMap = new LinkedHashMap<String, Uid>();
			registeredProvSymbols_m.put(provider, symbolsMap);
		}

		if (symbolsMap.containsKey(symbol)) {
			cookie.fromUid(symbolsMap.get(symbol));
		} else {
			cookie.generate();
			symbolsMap.put(symbol, cookie);

			// set empty entry
			setSymbolproperty(cookie, new SymbolRelatedProperty());
		}

		return cookie;
	}

	public void removeProviderSymbol(String provider, String symbol) throws Exception {
		if (registeredProvSymbols_m.containsKey(provider)) {
			LinkedHashMap<String, Uid> symbolsMap = registeredProvSymbols_m.get(provider);

			if (symbolsMap.containsKey(symbol)) {

				// remove from property list
				removeSymbolProperty(symbolsMap.get(symbol));

				symbolsMap.remove(symbol);
				if (symbolsMap.size() == 0) {
					registeredProvSymbols_m.remove(provider);
				}
			}
		}
	}

	//
	public SymbolRelatedProperty getSymbolProperty(Uid uid) throws Exception {
		SymbolRelatedProperty p = new SymbolRelatedProperty();

		if (simulationSpread_m.containsKey(uid)) {
			p.simulationSpread_m = simulationSpread_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (simulationSlippage_m.containsKey(uid)) {
			p.simulationSlippage_m = simulationSlippage_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (minStopLossDistance_m.containsKey(uid)) {
			p.minStopLossDistance_m = minStopLossDistance_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (minTakeProfitDistance_m.containsKey(uid)) {
			p.minTakeProfitDistance_m = minTakeProfitDistance_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (minLimitOrderDistance_m.containsKey(uid)) {
			p.minLimitOrderDistance_m = minLimitOrderDistance_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (minStopOrderDistance_m.containsKey(uid)) {
			p.minStopOrderDistance_m = minStopOrderDistance_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (comissions_m.containsKey(uid)) {
			p.comission_m = comissions_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}
		
		if (comissionsPct_m.containsKey(uid)) {
			p.comissionPct_m = comissionsPct_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (signDigits_m.containsKey(uid)) {
			p.signDigits_m = signDigits_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (pointValues_m.containsKey(uid)) {
			p.pointValue_m = pointValues_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		if (executePosExactPrices_m.containsKey(uid)) {
			p.executePosExactPrices_m = executePosExactPrices_m.get(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "getSymbolProperty", "No symbol property contained");
		}

		return p;
	}

	public void setSymbolproperty(Uid uid, SymbolRelatedProperty p) {
		simulationSpread_m.put(uid, p.simulationSpread_m);
		simulationSlippage_m.put(uid, p.simulationSlippage_m);
		minStopLossDistance_m.put(uid, p.minStopLossDistance_m);
		minTakeProfitDistance_m.put(uid, p.minTakeProfitDistance_m);
		minLimitOrderDistance_m.put(uid, p.minLimitOrderDistance_m);
		minStopOrderDistance_m.put(uid, p.minStopOrderDistance_m);
		comissions_m.put(uid, p.comission_m);
		comissionsPct_m.put(uid, p.comissionPct_m);
		signDigits_m.put(uid, p.signDigits_m);
		pointValues_m.put(uid, p.pointValue_m);
		executePosExactPrices_m.put(uid, p.executePosExactPrices_m);

	}

	public void removeSymbolProperty(Uid uid) throws Exception {
		if (simulationSpread_m.containsKey(uid)) {
			simulationSpread_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (simulationSlippage_m.containsKey(uid)) {
			simulationSlippage_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (minStopLossDistance_m.containsKey(uid)) {
			minStopLossDistance_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (minTakeProfitDistance_m.containsKey(uid)) {
			minTakeProfitDistance_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (minLimitOrderDistance_m.containsKey(uid)) {
			minLimitOrderDistance_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (minStopOrderDistance_m.containsKey(uid)) {
			minStopOrderDistance_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (comissions_m.containsKey(uid)) {
			comissions_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}
		
		if (comissionsPct_m.containsKey(uid)) {
			comissionsPct_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (signDigits_m.containsKey(uid)) {
			signDigits_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (pointValues_m.containsKey(uid)) {
			pointValues_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}

		if (executePosExactPrices_m.containsKey(uid)) {
			executePosExactPrices_m.remove(uid);
		} else {
			throw new HtException(getClass().getSimpleName(), "removeSymbolProperty", "No symbol property contained");
		}


	}
}
