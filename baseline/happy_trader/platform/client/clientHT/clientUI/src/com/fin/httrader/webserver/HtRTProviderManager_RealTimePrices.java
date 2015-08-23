/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.HtMain;
import com.fin.httrader.configprops.HtRTProviderProp;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.interfaces.HtEventListener;
import com.fin.httrader.managers.RtConfigurationManager;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.SingleReaderQueue;
import com.fin.httrader.utils.SingleReaderQueueExceedLimit;
import com.fin.httrader.utils.hlpstruct.HtLevel1Data;
import com.fin.httrader.utils.hlpstruct.HtLevel2Data;

import com.fin.httrader.utils.hlpstruct.HtLevel2Data;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.Vector;


import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.it_brain.collections.MultiTreeMap;
//import jsync.Queue;

/**
 * real time prices providers
 * @author DanilinS
 */
public class HtRTProviderManager_RealTimePrices extends HtServletsBase implements HtEventListener {

	//private Queue queue_m = new Queue();
	//private LinkedBlockingQueue queue_m = new LinkedBlockingQueue();
	SingleReaderQueue queue_m = new SingleReaderQueue("Real Time Prices Queue", true, RtConfigurationManager.StartUpConst.MAX_ELEMENTS_IN_QUEUE);

	// ------------------------------
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		HtLog.log(Level.INFO, getContext(), "initialize_Get", "Started real time prices propagation");
		ServletOutputStream ostrm = res.getOutputStream();

		try {

			List<String> providers = getStringListParameter(req, "providers", false);
			Set<String> providersset = new HashSet<String>(providers);

			// check providers
			for (Iterator<String> it = providers.iterator(); it.hasNext();) {
				String provider_i = it.next();

				boolean isRunning_i = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_queryProviderRunning(provider_i);

				// check provider property for alien
				HtRTProviderProp prop = HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getRTProviderProperty(provider_i);
				if (prop.isAlien()) {
					continue;
				}

				if (isRunning_i) {
					int status = HtCommandProcessor.instance().get_HtRtDsourceProxy().remote_getProviderStatusCode(provider_i);
					if (BackgroundJobStatus.STATUS_OK != status) {
						throw new HtException(getContext(), "initialize_Get", "Invalid provider status: " + provider_i + " - " + BackgroundJobStatus.getStatusString(status));
					}
				}
			}

			initialize(res);

			ArrayList list = new ArrayList();
			XmlEvent event = null;


			while (true) {
				//event = (XmlEvent) queue_m.get(RtConfigurationManager.StartUpConst.HEARTBEAT_TIMEOUT_SEC * 1000);
				//event = (XmlEvent) queue_m.poll(RtConfigurationManager.StartUpConst.HEARTBEAT_TIMEOUT_SEC * 1000,  TimeUnit.MILLISECONDS);


				queue_m.get(list, RtConfigurationManager.StartUpConst.RTPRICE_MAN_HEARTBEAT_TIMEOUT_SEC * 1000);

				for (Iterator it = list.iterator(); it.hasNext();) {

					event = (XmlEvent) it.next();

					if (event != null) {

						if (event.getType() == XmlEvent.DT_Ticker && event.getEventType() == XmlEvent.ET_RtProviderTicker) {

							// ours
							HtRtData rtdata = event.getAsRtData();
							if (rtdata.getType() == HtRtData.RT_Type) {
								crackXmlEvent_Ticker_RT(providersset, res, ostrm, rtdata);
							} else if (rtdata.getType() == HtRtData.HST_Type) {
								//HtLog.log(Level.INFO, getContext(), "initialize_Get", "RT ticker: " + rtdata.toString());
								crackXmlEvent_Ticker_Hist(providersset, res, ostrm, rtdata);
							}
						} else if (event.getType() == XmlEvent.DT_Level1Data && event.getEventType() == XmlEvent.ET_Level1Data) {

							// ours
							crackXmlEvent_Level1(providersset, res, ostrm, event.getAsLevel1Data());
						} else if (event.getType() == XmlEvent.DT_Level2Data && event.getEventType() == XmlEvent.ET_Level2Data) {
							crackXmlEvent_Level2(providersset, res, ostrm, event.getAsLevel2Data());
						} else {
							throw new HtException(getContext(), "initialize_Get", "Wrong rt price data type");
						}

						event = null;

					} else {
						sendHeartBeat(res);
					}

				}

				if (list.size() == 0) {
					sendHeartBeat(res);
				}
			}


		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "initialize_Get", "Exception in real time prices propagation servlet: \"" + e.getMessage() + "\"");

			insertErrorData(res, ostrm, "Exception in real time prices propagation servlet: \"" + e.getMessage() + "\"");

		}

		deinitialize();

		HtLog.log(Level.INFO, getContext(), "initialize_Get", "Finished real time prices propagation");
		return true;
	}

	// ------------------------------
	private void initialize(HttpServletResponse res) throws Exception {
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_RtProviderTicker, 20, this);
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_Level1Data, 20, this);
		RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).subscribeForEvent(XmlEvent.ET_Level2Data, 20, this);

	}

	private void deinitialize() {



		// uninitialize
		try {
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_RtProviderTicker, 20, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_Level1Data, 20, this);
			RtGlobalEventManager.instance().resolveListenerThread(RtGlobalEventManager.instance().MAIN_THREAD).unsubscribeForEvent(XmlEvent.ET_Level2Data, 20, this);
		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "deinitialize", "Error in unsubscribing from real time prices: \"" + e.getMessage() + "\"");
		}

		queue_m.releaseMonitoring();

	}

	public void onEventArrived(XmlEvent event) throws SingleReaderQueueExceedLimit {


		queue_m.put(event);

	}

	private void insertErrorData(HttpServletResponse res, ServletOutputStream ostrm, String data) {
		try {


			StringBuilder line = new StringBuilder();
			String[] st = data.split("[\n\r]");

			for (int i = 0; i < st.length; i++) {
				String w = st[i].replaceAll("\\\\", "\\\\\\\\");
				w = w.replaceAll("[']", "\\\\'");

				ostrm.print("<script type='text/javascript'>");
				ostrm.print("window.parent.insertErrorData('");
				ostrm.print(w);
				ostrm.print("');");
				ostrm.print("</script>\n");
				res.flushBuffer();

			}

			// funsih
			ostrm.print("<script type='text/javascript'>");
			ostrm.print("window.parent.finishInsertingErrorData();");
			ostrm.print("</script>\n");
			res.flushBuffer();

		} catch (Throwable e) {
			// if we can't do...
			HtLog.log(Level.WARNING, getContext(), "insertErrorData", "Can't make output to our jsp layer: \"" + e.getMessage() + "\"");

		}
	}

	private void crackXmlEvent_Level2(Set<String> providers, HttpServletResponse res, ServletOutputStream ostrm, HtLevel2Data level2data) throws Exception {
		String provider_i = level2data.getProvider();
		String symbol_i = level2data.getSymbol();

		HtLevel2Data.Htlevel2Entry[] pr = level2data.getData();
		
		//HtLog.log(Level.INFO, getContext(), "crackXmlEvent_Level2", "best bid idx: " + level2data.getBestBidIndex());
		//HtLog.log(Level.INFO, getContext(), "crackXmlEvent_Level2", "best ask idx: " + level2data.getBestAskIndex());
		
		
		StringBuilder obj = new StringBuilder();
		obj.append("{ ");
		if (pr != null) {
			for (int i = 0; i < pr.length; i++) {
				obj.append("\"");
				obj.append(i);
				obj.append("\":");
				obj.append("[\"");
				obj.append(pr[i].getPrice());
				obj.append("\",\"");
				obj.append(pr[i].getType());
				obj.append("\",\"");
				obj.append(pr[i].getVolume());
				obj.append("\",\"");
				obj.append(pr[i].isSelf() ? "1" : "0");
				obj.append("\",\"");
				obj.append(level2data.getBestBidIndex()==i?"1":"0");
				obj.append("\",\"");
				obj.append(level2data.getBestAskIndex()==i?"1":"0");
				obj.append("\"],");
			}
		}

		obj.setLength(obj.length() - 1);
		obj.append(" }");


		if (providers.contains(provider_i)) {
			ostrm.print("<script type='text/javascript'>");
			ostrm.print("window.parent.insert_level2_data('");
			ostrm.print(provider_i);
			ostrm.print("','");
			ostrm.print(symbol_i);
			ostrm.print("','");
			ostrm.print(HtDateTimeUtils.time2String_Gmt(level2data.getTime()));
			ostrm.print("','");
			ostrm.print(obj.toString());
			ostrm.print("');");
			ostrm.print("</script>\n");
		}

	}

	private void crackXmlEvent_Level1(Set<String> providers, HttpServletResponse res, ServletOutputStream ostrm, HtLevel1Data rtdata) throws Exception {
		String provider_i = rtdata.getProvider();
		String symbol_i = rtdata.getSymbol();

		if (providers.contains(provider_i)) {
			ostrm.print("<script type='text/javascript'>");
			ostrm.print("window.parent.insert_level1_data('");
			ostrm.print(provider_i);
			ostrm.print("','");
			ostrm.print(symbol_i);
			ostrm.print("','");
			ostrm.print(HtDateTimeUtils.time2String_Gmt(rtdata.time_m));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.best_bid_price_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.best_bid_volume_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.best_ask_price_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.best_ask_volume_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.open_interest_m, 5, true));
			ostrm.print("');");
			ostrm.print("</script>\n");
			res.flushBuffer();
		}

	}

	private void crackXmlEvent_Ticker_RT(Set<String> providers, HttpServletResponse res, ServletOutputStream ostrm, HtRtData rtdata) throws Exception {

		String provider_i = rtdata.getProvider();
		String symbol_i = rtdata.getSymbol();
		if (providers.contains(provider_i)) {
			// propagate
			ostrm.print("<script type='text/javascript'>");
			ostrm.print("window.parent.insert_rt_data('");
			ostrm.print(provider_i);
			ostrm.print("','");
			ostrm.print(symbol_i);
			ostrm.print("','");
			ostrm.print(HtDateTimeUtils.time2String_Gmt(rtdata.time_m));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.bid_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.ask_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.volume_m, 5, true));
			ostrm.print("',");
			ostrm.print(rtdata.color_m);
			ostrm.print(");");
			ostrm.print("</script>");
			res.flushBuffer();


		}
	}

	private void crackXmlEvent_Ticker_Hist(Set<String> providers, HttpServletResponse res, ServletOutputStream ostrm, HtRtData rtdata) throws Exception {

		String provider_i = rtdata.getProvider();
		String symbol_i = rtdata.getSymbol();
		if (providers.contains(rtdata.getProvider())) {
			// propagate
			ostrm.print("<script type='text/javascript'>");
			ostrm.print("window.parent.insert_hist_data('");
			ostrm.print(provider_i);
			ostrm.print("','");
			ostrm.print(symbol_i);
			ostrm.print("','");
			ostrm.print(HtDateTimeUtils.time2String_Gmt(rtdata.time_m));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.open_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.high_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.low_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.close_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.open2_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.high2_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.low2_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.close2_m, 5, true));
			ostrm.print("','");
			ostrm.print(HtUtils.formatPriceValue(rtdata.volume_m, 5, true));
			ostrm.print("');");
			ostrm.print("</script>");
			res.flushBuffer();


		}
	}

	private void sendHeartBeat(HttpServletResponse res) throws Exception {
		// send heart beat to avoid hanging
		res.getOutputStream().print("<script type=\"text/javascript\">");
		res.getOutputStream().print("</script>");
		res.flushBuffer();
	}

	@Override
	public String getListenerName() {
		return " listener: [ " + getContext() + " ] ";
	}
}
