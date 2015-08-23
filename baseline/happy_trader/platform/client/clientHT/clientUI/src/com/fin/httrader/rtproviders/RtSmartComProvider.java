/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import com.fin.httrader.eventplugins.HtSmartComLoader;
import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.hlpstruct.TSAggregationPeriod;
import com.fin.httrader.interfaces.HtRealTimeProvider;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class RtSmartComProvider implements HtRealTimeProvider {

  private BackgroundJobStatus status_m = new BackgroundJobStatus();
  private String name_m = null;
  private long timeShift_m = 0;
  boolean initialized_m = false;
  private HashSet<String> symbols_m = new HashSet<String>();
  private String server_id_m = null;
  private String alg_brk_pair_m = null;
  private static final int TIMEOUT_SEC = 10;
  // this is for tick history
  private long historyTickRequestFromDate_m = -1;
  private long historyTickRequestCount_m = -1;
  private boolean requestHistoryTicks_m = false;
  // this is for bars history
  private long historyBarsRequestFromDate_m = -1;
  private long historyBarsRequestCount_m = -1;
  private String historyBarsRequestAggregationPeriod_m = TSAggregationPeriod.getAggregationPeriodName(TSAggregationPeriod.AP_Dummy);
  private boolean requestHistoryBars_m = false;
  // by default
  private boolean subscribePrints_m = true;
  private boolean subscribeLevel1_m = false;
  private boolean subscribeLevel2_m = false;
  private int broker_seq_num_m = -1;
  private int requestMyTradesPeriodSec_m = -1;
  private int historyBarsMultFactor_m = -1;

  private String getContext() {
	return RtSmartComProvider.class.getSimpleName();
  }
  // ------------------------------

  @Override
  public void initialize(Map<String, String> initdata) throws Exception {
	if (initialized_m) {
	  throw new HtException(getContext(), "initialize", "Already initialized");
	}

	//

	// message recepient
	server_id_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_SERVER_ID");
	alg_brk_pair_m = HtUtils.getParameterWithException(initdata, "SMARTCOM_ALG_BRK_PAIR");
	broker_seq_num_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "BROKER_SEQ_NUM"));

	String symbolFile = HtUtils.getParameterWithException(initdata, "SYMBOL_FILE");
	symbols_m.addAll(HtFileUtils.readSymbolListFromFile(symbolFile,1));

	historyBarsMultFactor_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "REQUEST_BARS_HISTORY_MULT_FACTOR"));
	
	// if we need tick history
	requestHistoryTicks_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "REQUEST_HISTORY_TICKS"));
	requestHistoryBars_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "REQUEST_HISTORY_BARS"));

	historyTickRequestCount_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "REQUEST_TICK_HISTORY_COUNT"));

	String historyTickRequestFromDate_s = HtUtils.getParameterWithoutException(initdata, "REQUEST_TICK_HISTORY_FROM_DATETIME");
	historyTickRequestFromDate_m = -1;

	if (historyTickRequestFromDate_s.length() > 0) {
	  historyTickRequestFromDate_m = HtDateTimeUtils.parseDateTimeParameter(historyTickRequestFromDate_s, false);
	} else {
	  // defaults to beginning if the day
	  historyTickRequestFromDate_m = HtDateTimeUtils.getCurGmtTime_DayBegin();

	}

	// if we need bars history
	historyBarsRequestCount_m = HtUtils.parseInt(HtUtils.getParameterWithoutException(initdata, "REQUEST_BARS_HISTORY_COUNT"));

	String historyBarsRequestFromDate_s = HtUtils.getParameterWithoutException(initdata, "REQUEST_BARS_HISTORY_FROM_DATETIME");
	historyBarsRequestFromDate_m = -1;

	if (historyBarsRequestFromDate_s.length() > 0) {
	  historyBarsRequestFromDate_m = HtDateTimeUtils.parseDateTimeParameter(historyBarsRequestFromDate_s, false);
	} else {
	  // defaults to beginning if the day
	  historyBarsRequestFromDate_m = HtDateTimeUtils.getCurGmtTime_DayBegin();

	}

	//

	historyBarsRequestAggregationPeriod_m = HtUtils.getParameterWithoutException(initdata, "REQUEST_BARS_HISTORY_AGGR_PERIOD");


	subscribePrints_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "SUBSCRIBE_PRINTS"));
	subscribeLevel1_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "SUBSCRIBE_LEVEL1"));
	subscribeLevel2_m = HtUtils.parseBoolean(HtUtils.getParameterWithException(initdata, "SUBSCRIBE_LEVEL2"));
	requestMyTradesPeriodSec_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "REQUEST_MY_TRADES_POLL_SEC"));


	// receive ticks history
	if (requestHistoryTicks_m) {
	  XmlParameter xmlparam_print_history = new XmlParameter();
	  xmlparam_print_history.setStringList("symbol_list", new ArrayList<String>(symbols_m));
	  xmlparam_print_history.setInt("history_tick_request_count", historyTickRequestCount_m);
	  xmlparam_print_history.setDate("history_tick_request_from_date", historyTickRequestFromDate_m);

	  String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // broadcast
			  broker_seq_num_m,
			  "get_print_history",
			  XmlHandler.serializeParameterStatic(xmlparam_print_history),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out);

	  XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider requested tick history");

	}

	// receive bars history
	if (requestHistoryBars_m) {
	  XmlParameter xmlparam_bars_history = new XmlParameter();
	  xmlparam_bars_history.setStringList("symbol_list", new ArrayList<String>(symbols_m));
	  xmlparam_bars_history.setInt("bars_request_count", historyBarsRequestCount_m);
	  xmlparam_bars_history.setDate("bars_request_from_date", historyBarsRequestFromDate_m);
	  xmlparam_bars_history.setInt("mult_factor", historyBarsMultFactor_m);
	  xmlparam_bars_history.setInt("aggr_period", TSAggregationPeriod.getAggregationPeriodFromName(historyBarsRequestAggregationPeriod_m));


	  String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // broadcast
			  broker_seq_num_m,
			  "get_bar_history",
			  XmlHandler.serializeParameterStatic(xmlparam_bars_history),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out);

	  XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider requested bars history");

	}

	// send event
	if (subscribePrints_m) {
	  // need to subscribe
	  XmlParameter xmlparam_subscribe_prints = new XmlParameter();
	  xmlparam_subscribe_prints.setStringList("symbol_list", new ArrayList<String>(symbols_m));


	  String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // broadcast
			  broker_seq_num_m,
			  "subscribe_prints",
			  XmlHandler.serializeParameterStatic(xmlparam_subscribe_prints),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out);

	  XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider subscribed for prints");
	}

	if (subscribeLevel1_m) {
	  XmlParameter xmlparam_subscribe_level1 = new XmlParameter();
	  xmlparam_subscribe_level1.setStringList("symbol_list", new ArrayList<String>(symbols_m));

	  String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // broadcast
			  broker_seq_num_m,
			  "subscribe_level1",
			  XmlHandler.serializeParameterStatic(xmlparam_subscribe_level1),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out);

	  XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider subscribed for level1");
	}

	if (subscribeLevel2_m) {
	  XmlParameter xmlparam_subscribe_level2 = new XmlParameter();
	  xmlparam_subscribe_level2.setStringList("symbol_list", new ArrayList<String>(symbols_m));

	  String out = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // broadcast
			  broker_seq_num_m,
			  "subscribe_level2",
			  XmlHandler.serializeParameterStatic(xmlparam_subscribe_level2),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out);

	  XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider subscribed for level2");
	}

	if (requestMyTradesPeriodSec_m > 0) {
	  XmlParameter xmlparam_subscribe_get_my_trades = new XmlParameter();
	  xmlparam_subscribe_get_my_trades.setInt("poll_period_sec", requestMyTradesPeriodSec_m);

	  // request regular polling
	  String out2 = HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
			  server_id_m,
			  alg_brk_pair_m,
			  -1, // delegate to all threads
			  broker_seq_num_m,
			  "subscribe_get_my_trades",
			  XmlHandler.serializeParameterStatic(xmlparam_subscribe_get_my_trades),
			  TIMEOUT_SEC);

	  HtSmartComLoader.checkSmartComStatusString(out2);
	}



	initialized_m = true;

	status_m.initializeOk();
  }

  @Override
  public void deinitialize() throws Exception {
	if (!initialized_m) {
	  throw new HtException(getContext(), "deinitialize", "Not initialized");
	}

	// need to deinitialize
	// need to subscribe


	// send event
	if (subscribePrints_m) {
	  try {
		XmlParameter xmlparam_unsubscribe_prints = new XmlParameter();
		xmlparam_unsubscribe_prints.setStringList("symbol_list", new ArrayList<String>(symbols_m));

		HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
				server_id_m,
				alg_brk_pair_m,
				-1, // broadcast
				broker_seq_num_m,
				"unsubscribe_prints",
				XmlHandler.serializeParameterStatic(xmlparam_unsubscribe_prints),
				TIMEOUT_SEC);

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider unsubscribed from prints");
	  } catch (Throwable e) {
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider unsubscribing from prints error: " + e.getMessage());
	  }
	}

	if (subscribeLevel1_m) {
	  try {
		XmlParameter xmlparam_unsubscribe_level1 = new XmlParameter();
		xmlparam_unsubscribe_level1.setStringList("symbol_list", new ArrayList<String>(symbols_m));

		HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
				server_id_m,
				alg_brk_pair_m,
				-1, // broadcast
				broker_seq_num_m,
				"unsubscribe_level1",
				XmlHandler.serializeParameterStatic(xmlparam_unsubscribe_level1),
				TIMEOUT_SEC);

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider unsubscribed from level1");

	  } catch (Throwable e) {
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider unsubscribing from level1 error: " + e.getMessage());
	  }
	}

	if (subscribeLevel2_m) {
	  try {
		XmlParameter xmlparam_unsubscribe_level2 = new XmlParameter();
		xmlparam_unsubscribe_level2.setStringList("symbol_list", new ArrayList<String>(symbols_m));

		HtCommandProcessor.instance().get_HtServerProxy().remote_SendCustomEvent_toBroker(
				server_id_m,
				alg_brk_pair_m,
				-1, // broadcast
				broker_seq_num_m,
				"unsubscribe_level2",
				XmlHandler.serializeParameterStatic(xmlparam_unsubscribe_level2),
				TIMEOUT_SEC);

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider unsubscribed from level2");
	  } catch (Throwable e) {
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider unsubscribing from level2 error: " + e.getMessage());
	  }
	}

	initialized_m = false;
  }

  @Override
  public String getProviderId() {
	return name_m;
  }

  @Override
  public void setProviderId(String providerId) {
	name_m = providerId;
  }

  @Override
  public void setTimeShift(long timeshift) {
	timeShift_m = timeshift;
  }

  @Override
  public long getTimeShift() {
	return timeShift_m;
  }

  @Override
  public BackgroundJobStatus returnProviderStatus() {
	return status_m;
  }

  @Override
  public void returnAvailableSymbolList(List<String> symbols) {
	symbols.clear();
	symbols.addAll(symbols_m);
  }
}
