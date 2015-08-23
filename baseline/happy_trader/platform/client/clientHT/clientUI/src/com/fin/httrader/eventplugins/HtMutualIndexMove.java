/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMutualIndexMove implements HtEventPlugin {

	private static final int STATE_UPTICK = 1;
	private static final int STATE_DOWNTICK = -1;
	private static final int STATE_NOTHING = 0;
	private StringBuilder name_m = new StringBuilder();
	private String rtProvider_m = null;
	private HashMap<String, HtRtData> previous_m = new HashMap<String, HtRtData>();
	private HashMap<String, Integer> states_m = new HashMap<String, Integer>();
	private int totalSymbols_m = -1;

	@Override
	public Integer[] returnSubscribtionEventTypes() {
		return new Integer[]{XmlEvent.ET_RtProviderTicker};
	}

	@Override
	public void initialize(Map<String, String> initdata) throws Exception {
		rtProvider_m = HtUtils.getParameterWithException(initdata, "RT_PROVIDER");
		totalSymbols_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "TOTAL_SYMBOLS"));
	}

	@Override
	public void deinitialize() throws Exception {
	}

	@Override
	public String getEventPluginId() {
		return name_m.toString();
	}

	@Override
	public void setEventPluginId(String pluginId) {
		name_m.setLength(0);
		name_m.append(pluginId);
	}

	@Override
	public void execute(XmlEvent alertData) throws Exception {
		if (alertData.getEventType() == XmlEvent.ET_RtProviderTicker) {
			HtRtData rtdata = alertData.getAsRtData();

			if (rtdata.getProvider().equals(rtProvider_m) && rtdata.getType() == HtRtData.RT_Type) {
				HtRtData prev = previous_m.get(rtdata.getSymbol());
				if (prev == null) {
					previous_m.put(rtdata.getSymbol(), rtdata);
					states_m.put(rtdata.getSymbol(), STATE_NOTHING);

					XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "Initialized previous data: " + rtdata.getSymbol());

					if (totalSymbols_m == states_m.size()) {
						XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "All symbols are ready");
						sendDrawableObject("All symbols are ready");
					}


				} else {
					if (rtdata.bid_m > prev.bid_m) {
						// uptick bid
						states_m.put(rtdata.getSymbol(), STATE_UPTICK);
					} else if (rtdata.bid_m < prev.bid_m) {
						// downtick bid
						states_m.put(rtdata.getSymbol(), STATE_DOWNTICK);

					} else {
						states_m.put(rtdata.getSymbol(), STATE_NOTHING);
					}

					if (totalSymbols_m == states_m.size()) {
						
						int state_sum = 0;
						for (Iterator<String> it = states_m.keySet().iterator(); it.hasNext();) {
							state_sum += states_m.get(it.next());
						}

						if (state_sum == states_m.size()) {
							XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "All symbols UPTICK");
							sendDrawableObject("All symbols UPTICK");
						}

						if (state_sum == -states_m.size()) {
							XmlEvent.createSimpleLog("", getEventPluginId(), CommonLog.LL_INFO, "All symbols DOWNTICK");
							sendDrawableObject("All symbols DOWNTICK");
						}
					}



				}
			}
		}
	}

	void sendDrawableObject(String text)
	{
		HtDrawableObject dobj = new HtDrawableObject();
		HtDrawableObject.Text dtxt= dobj.getAsText(true);

		dtxt.shortText_m.append("Notification");
		dtxt.text_m.append(text);

		dobj.setProvider(rtProvider_m);
		dobj.setSymbol("dummy");
		dobj.setRunName("dummy");
		dobj.setTime(HtDateTimeUtils.getCurGmtTime());

		XmlEvent event_dr = new XmlEvent();
		HtDrawableObject.createXmlEventDrawableObject(dobj, event_dr);
		RtGlobalEventManager.instance().pushCommonEvent(event_dr);

		
	}
}
