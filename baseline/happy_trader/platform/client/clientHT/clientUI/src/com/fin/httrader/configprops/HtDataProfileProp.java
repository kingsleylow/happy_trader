/*
 * HtDataProfileProp.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.configprops;

import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.utils.HtException;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtDataProfileProp extends HtBaseProp {

	// constants
	public static final Object[] TS_YEAR = new Object[]		{new Integer(1), "Year",						new Integer[]{1}};
	public static final Object[] TS_MONTH = new Object[]	{new Integer(2), "Month",						new Integer[]{1}};
	public static final Object[] TS_WEEK = new Object[]		{new Integer(3), "Week",						new Integer[]{1}};
	public static final Object[] TS_DAY = new Object[]		{new Integer(4), "Day",							new Integer[]{1}};
	public static final Object[] TS_HOUR = new Object[]		{new Integer(5), "Hour",						new Integer[]{1, 2, 3, 4, 6, 8, 12, 24}};
	public static final Object[] TS_MINUTE = new Object[]	{new Integer(6), "Minute",					new Integer[]{1, 2, 3, 4, 5, 6, 10, 12, 15, 20, 30, 60}};
	public static final Object[] TS_NA = new Object[]			{new Integer(0), "No aggregation",	new Integer[]{}};

	/** Creates a new instance of HtDataProfileProp */
	public HtDataProfileProp() {
		type_m = HtBaseProp.ID_CONFIGURE_DATA_PROFILE;
	}
	
	public Map<String, Integer> getProvMultFactorMap()
	{
		return provMultFactor_m;
	}
	
	public Map<String, Integer> getProvTimeScaleMap()
	{
		return provTimeScale_m;
	}
	
	public Map<String, Boolean> getProvIsSubscribedMap()
	{
		return provIsSubscribed_m;
	}
	
		public Map<String, Boolean> getIsSubscribedDrawObjMap()
	{
		return provIsSubscribedDrawObj_m;
	}
	
	
	//
	@HtSaveAnnotation
	private LinkedHashMap<String, Integer> provMultFactor_m = new LinkedHashMap<String, Integer>();
	
	@HtSaveAnnotation
	private LinkedHashMap<String, Integer> provTimeScale_m = new LinkedHashMap<String, Integer>();

	// if I'm subscribed to RT/HIST data (HtRtData)
	@HtSaveAnnotation
	private LinkedHashMap<String, Boolean> provIsSubscribed_m = new LinkedHashMap<String, Boolean>();
	
	// if I'm subscribed to Level1 data
	@HtSaveAnnotation
	private LinkedHashMap<String, Boolean> provIsSubscribedLevel1_m = new LinkedHashMap<String, Boolean>();

	// if I'm subscribed to drawable object data
	@HtSaveAnnotation
	private LinkedHashMap<String, Boolean> provIsSubscribedDrawObj_m = new LinkedHashMap<String, Boolean>();

	// partition ID
	@HtSaveAnnotation
	private int partitionId_m = -1;

	public int getPartitionId()
	{
		return this.partitionId_m;
	}
	
	public void setPartitionId(int partitionId)
	{
		this.partitionId_m = partitionId;
	}
	
	

	// wheter this is subscribed to RT/HIST
	public boolean isProviderSubscribed_RTHist(String provider) {
		if (provIsSubscribed_m.containsKey(provider)) {
			return provIsSubscribed_m.get(provider).booleanValue();
		}

		return false;
	}

	public boolean isProviderSubscribed_Level1(String provider) {
		if (provIsSubscribedLevel1_m.containsKey(provider)) {
			return provIsSubscribedLevel1_m.get(provider).booleanValue();
		}

		return false;
	}

	public boolean isProviderSubscribed_DrawObj(String provider) {
		if (provIsSubscribedDrawObj_m.containsKey(provider)) {
			return provIsSubscribedDrawObj_m.get(provider).booleanValue();
		}

		return false;
	}

	public int getProviderTimeScale(String provider) {
		if (provTimeScale_m.containsKey(provider)) {
			return provTimeScale_m.get(provider).intValue();
		}

		return (Integer) TS_NA[0];
	}

	public int getProviderMultFactor(String provider) {
		if (provMultFactor_m.containsKey(provider)) {
			return provMultFactor_m.get(provider).intValue();
		}

		return -1;
	}

	public void removeProvider(String provider) {
		provMultFactor_m.remove(provider);
		provTimeScale_m.remove(provider);
		provIsSubscribed_m.remove(provider);
		provIsSubscribedLevel1_m.remove(provider);
		provIsSubscribedDrawObj_m.remove(provider);
	}

	public void validateEntry(String provider) throws Exception {
		if (isProviderSubscribed_RTHist(provider)) {
			int multFact = getProviderMultFactor(provider);
			int timeScale = getProviderTimeScale(provider);


			if (multFact <= 0 && timeScale > 0) {
				throw new HtException(getClass().getSimpleName(), "validateEntry", "Invalid multiplication factor for provider: " + provider);
			}

			if (timeScale < 0) {
				throw new HtException(getClass().getSimpleName(), "validateEntry", "Invalid time scale for provider: " + provider);
			}

			//if (partitionId_m <= 0) {
				// TODO !!!
				//throw new HtException(getClass().getSimpleName(), "validateEntry", "Invalid partition ID");
			//}

		}
	}

	public void updateProvider(
					String provider,
					int multFactor,
					int timeScale,
					boolean isSubscribed_RtHist,
					boolean isSubscribedLevel1,
					boolean isSubscribedDrawObj
		) throws Exception {
		// remove initially
		removeProvider(provider);

		if ((timeScale != (Integer) TS_YEAR[0]) &&
						(timeScale != (Integer) TS_MONTH[0]) &&
						(timeScale != (Integer) TS_WEEK[0]) &&
						(timeScale != (Integer) TS_DAY[0]) &&
						(timeScale != (Integer) TS_HOUR[0]) &&
						(timeScale != (Integer) TS_MINUTE[0]) &&
						(timeScale != (Integer) TS_NA[0])) {
			throw new HtException(getClass().getSimpleName(), "updateProvider", "Invalid time scale: " + timeScale);
		}

		provMultFactor_m.put(provider, multFactor);
		provTimeScale_m.put(provider, timeScale);
		provIsSubscribed_m.put(provider, isSubscribed_RtHist);
		provIsSubscribedLevel1_m.put(provider, isSubscribedLevel1);
		provIsSubscribedDrawObj_m.put(provider, isSubscribedDrawObj);

	}

	public static Integer[] returnAllTimeScales() {
		return new Integer[]{
							(Integer) TS_YEAR[0],
							(Integer) TS_MONTH[0],
							(Integer) TS_WEEK[0],
							(Integer) TS_DAY[0],
							(Integer) TS_HOUR[0],
							(Integer) TS_MINUTE[0],
							(Integer) TS_NA[0]
						};
	}

	public static Integer[] returnAllMultFactors(int timescale) {
		if (timescale == ((Integer)TS_YEAR[0]).intValue()) {
			return (Integer[]) TS_YEAR[2];
		} else if (timescale == ((Integer)TS_MONTH[0]).intValue()) {
			return (Integer[]) TS_MONTH[2];
		} else if (timescale == ((Integer)TS_WEEK[0]).intValue()) {
			return (Integer[]) TS_WEEK[2];
		} else if (timescale == ((Integer)TS_DAY[0]).intValue()) {
			return (Integer[]) TS_DAY[2];
		} else if (timescale == ((Integer)TS_HOUR[0]).intValue()) {
			return (Integer[]) TS_HOUR[2];
		} else if (timescale == ((Integer)TS_MINUTE[0]).intValue()) {
			return (Integer[]) TS_MINUTE[2];
		} else if (timescale == ((Integer)TS_NA[0]).intValue()) {
			return (Integer[]) TS_NA[2];
		} else {
			return null;
		}
	}

	public static String timeScaleToString(int timescale) {
		if (timescale == ((Integer)TS_YEAR[0]).intValue()) {
			return (String) TS_YEAR[1];
		} else if (timescale == ((Integer)TS_MONTH[0]).intValue()) {
			return (String) TS_MONTH[1];
		} else if (timescale == ((Integer)TS_WEEK[0]).intValue()) {
			return (String) TS_WEEK[1];
		} else if (timescale == ((Integer)TS_DAY[0]).intValue()) {
			return (String) TS_DAY[1];
		} else if (timescale == ((Integer)TS_HOUR[0]).intValue()) {
			return (String) TS_HOUR[1];
		} else if (timescale == ((Integer)TS_MINUTE[0]).intValue()) {
			return (String) TS_MINUTE[1];
		} else if (timescale == ((Integer)TS_NA[0]).intValue()) {
			return (String) TS_NA[1];
		} else {
			return null;
		}

	}

	public static int convertJavaAP2TSAp(int timescale) {
		if (timescale == ((Integer)TS_YEAR[0]).intValue()) {
			return TSAggregationPeriod.AP_Year;
		} else if (timescale == ((Integer)TS_MONTH[0]).intValue()) {
			return TSAggregationPeriod.AP_Month;
		} else if (timescale == ((Integer)TS_WEEK[0]).intValue()) {
			return TSAggregationPeriod.AP_Week;
		} else if (timescale == ((Integer)TS_DAY[0]).intValue()) {
			return TSAggregationPeriod.AP_Day;
		} else if (timescale == ((Integer)TS_HOUR[0]).intValue()) {
			return TSAggregationPeriod.AP_Hour;
		} else if (timescale == ((Integer)TS_MINUTE[0]).intValue()) {
			return TSAggregationPeriod.AP_Minute;
		} else if (timescale == ((Integer)TS_NA[0]).intValue()) {
			return TSAggregationPeriod.AP_Dummy;
		} else {
			return -1;
		}
	}
}
