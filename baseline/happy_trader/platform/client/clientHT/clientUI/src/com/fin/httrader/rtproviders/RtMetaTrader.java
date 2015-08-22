/*
 * RtMetaTrader.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.rtproviders;

import au.com.bytecode.opencsv.CSVReader;

import com.fin.httrader.hlpstruct.BackgroundJobStatus;
import com.fin.httrader.managers.RtGlobalEventManager;
import com.fin.httrader.managers.RtRealTimeProviderManager;

import com.fin.httrader.interfaces.*;
import com.fin.httrader.utils.hlpstruct.CommonLog;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import java.io.File;
import java.io.FileReader;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;
import java.util.TreeMap;
import jsync.Event;

/**
 *
 * @author victor_zoubok
 */
public class RtMetaTrader extends Thread implements HtRealTimeProvider {

	// event to shutdown

	private Event toShutdown_m = new Event();

	// if errored
	private BackgroundJobStatus status_m = new BackgroundJobStatus();

	private Event started_m = new Event();
	private HashMap<String, String> initData_m = new HashMap<String, String>();
	private StringBuilder name_m = new StringBuilder();
	//private RtProviderCallback callback_m = null;
	boolean initialized_m = false;
	private Process child_m = null;
	// helpers to parse data stream
	private int codeInt_m = -1;
	private ByteBuffer code_m = null;
	private ByteBuffer topic_m = null;
	private ByteBuffer item_m = null;
	private ByteBuffer value_m = null;
	private ByteBuffer errormsg_m = null;
	private int tokenNum_m = 0;
	private boolean canRead_m = false;

	// this is found symbol list
	private Set<String> symbolList_m = new HashSet<String>();
	
	// this will 
	
	// read from settings
	//private StringBuilder symbol_m = new StringBuilder();
	// formatter class
	//
	final private SimpleDateFormat format_m = new SimpleDateFormat("yyyy/MM/dd HH:mm");
	private String DDEExecutable_m = null;
	//
	// when parsingquote
	private StringBuilder dateTimeBuffer_m = new StringBuilder();
	private StringBuilder bidBuffer_m = new StringBuilder();
	private StringBuilder askBuffer_m = new StringBuilder();
	// map containing previous times of passed symbols
	private HashMap<String, Long> previousTimes_m = new HashMap<String, Long>();
	// timeshift
	private long timeShift_m = 0;

	private String getContext()
	{
		return getClass().getSimpleName();
	}

	//
	public RtMetaTrader() throws Exception {
		// allocate enogh memeory
		topic_m = ByteBuffer.allocate(255);
		item_m = ByteBuffer.allocate(255);
		value_m = ByteBuffer.allocate(255);
		errormsg_m = ByteBuffer.allocate(255);
		code_m = ByteBuffer.allocate(32);

		format_m.setTimeZone(TimeZone.getTimeZone("GMT"));
	}

	public void run() {
		// thread run finction


		InputStream in = null;

		//XmlEvent rtDataEvent = new XmlEvent();
		//XmlEvent messageEvent = new XmlEvent();

		try {

			//if (callback_m==null)
			//throw new HtException(getContext(), "run", "The context is not initialized");

			//HtLog.log(Level.INFO, getContext(), "run", "Working thread is initialized", null);

			//callback_m.onSubscriptionBegin(this, RtProviderCallback.OK, null);
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider started");

			// start specila helper process
			child_m = Runtime.getRuntime().exec(DDEExecutable_m);
			//child_m = Runtime.getRuntime().exec("cmd");

			synchronized (child_m) {
				in = child_m.getInputStream();
			}

			//HtLog.log(Level.INFO, getContext(), "run", "Helper process initialized");

			previousTimes_m.clear();
			initStreamProcessing();

			// thread is really started
			started_m.signal();

			// tries to read and send history
			// we actually subscribed for streaming
			readAllSymbolsHistory();
			
			//byte[] arrB = new byte[1];
			while (true) {

				try {
					int c = in.read();
					// cannot read from buffer
					if (c == -1) {
						break;
					}

					//arrB[0] = (byte)c;
					//String converted = new String(arrB, "US-ASCII");

					if (processStreamSymbol((byte) c)) {
						// line finished

						// error occured
						if (codeInt_m == 1) {
							// notify
							String errorS = new String(errormsg_m.array(), 0, errormsg_m.position(), "US-ASCII");
							//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, errorS);
							XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "DDE Client returned error: " + errorS);
							status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, "DDE Client returned error: " + errorS);

							break;
						}

						HtRtData rtdata = parseLine();
						//callback_m.onNewDataArrived(this, rtdata, "");
						XmlEvent rtDataEvent = new XmlEvent();
						RtRealTimeProviderManager.createXmlEventRtData(rtdata, rtDataEvent);
						RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);

					}

					if (toShutdown_m.waitEvent(0)) {
						break;
					}


				//
				} catch (Throwable e) {
					//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, e.getMessage());
					XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider inside loop exception: " + e.getMessage());

				
					status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, "Provider inside loop exception: " + e.getMessage());
					
					
					//HtLog.log(Level.WARNING, getContext(), "run", "Inside processing loop exception happened: " + e.getMessage());
				}

			} // main loop



		} catch (Throwable e) {
			//callback_m.onErrorHappened(this, RtProviderCallback.FAILURE, e.getMessage());
	
			XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_ERROR, "Provider outside loop exception: " + e.getMessage());

	
			status_m.setUp(BackgroundJobStatus.STATUS_ERRORED, "Provider outside loop exception: " + e.getMessage());
			

			//HtLog.log(Level.WARNING, getContext(), "run", "Outside loop exception happened: " + e.getMessage());
		}

		// kill this forcibly
		try {
			if (in != null) {
				in.close();
			}

			synchronized (child_m) {
				if (child_m != null) {
					child_m.destroy();
					child_m = null;
				}
			}
		} catch (Throwable e) {
		}
		

		//callback_m.onSubscriptionEnd(this, RtProviderCallback.OK, null);
		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Provider working thread finishing...");

		//HtLog.log(Level.INFO, getContext(), "run", "Working thread finishing...", null);
	}

	public void initialize(Map<String, String> initdata) throws Exception {
		if (initialized_m) {
			throw new HtException(getContext(), "initialize", "Already initialized");
		}

		initData_m.clear();
		initData_m.putAll(initdata);
		//callback_m = callback;

		// some initialization
		DDEExecutable_m = prepareDDEClientCall();


		initialized_m = true;

		// clear events
		toShutdown_m.reset();
		started_m.reset();

	
		status_m.initializeOk();
		

		// start the thread
		start();

		// wait until it is started
		// wiat 7 secs
		if (!started_m.waitEvent(7000))
			throw new HtException(getContext(), "initialize", "Cannot start provider: " + this.getProviderId());

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "Started RT Provider: "+this.getProviderId());
	}

	public void deinitialize() throws Exception {
		if (!initialized_m) {
			throw new HtException(getContext(), "deinitialize", "Not initialized");
		}

		initialized_m = false;

		// need to kill the process stream if any
		try {
			synchronized (child_m) {
				if (child_m != null) {
					child_m.destroy();
					child_m = null;
				}
			}
		} catch (Throwable e) {
		}
		

		toShutdown_m.signal();


		// wait until thread finishes
		join();
		//HtLog.log(Level.INFO, getContext(), "deinitialize", "Working thread finished");
	}

	public String getProviderId() {
		return name_m.toString();
	}

	public void setProviderId(String providerId) {
		name_m.setLength(0);
		name_m.append(providerId);
	}

	public void setTimeShift(long timeshift) {
		timeShift_m = timeshift;
	}

	public long getTimeShift() {
		return timeShift_m;
	}

	public BackgroundJobStatus returnProviderStatus() {
		// return error state
		return status_m;

	}

	/*
	 * Helpers
	 */

	
	
	// finite state machine to parse input data from helper DDE client
	private void initStreamProcessing() {

		// allocate enogh memeory
		topic_m.clear();
		item_m.clear();
		value_m.clear();
		errormsg_m.clear();
		code_m.clear();

		tokenNum_m = 0;
		canRead_m = false;
		codeInt_m = -1;
	}

	// returns
	private boolean processStreamSymbol(byte c) throws Exception {
		boolean finishline = false;

		if (c == '|') {
			// special sign to reset everything
			initStreamProcessing();
		} else if (c == '<') {
			tokenNum_m++;
			canRead_m = true;
		} else if (c == '>') {
			canRead_m = false;

			// proccess symbols
			if (tokenNum_m == 1) {
				codeInt_m = Integer.valueOf(new String(code_m.array(), 0, code_m.position(), "US-ASCII"));
			} else if (tokenNum_m == 2) {
				if (codeInt_m == 1) {
					// eroor happens
					tokenNum_m = 0;
					finishline = true;
				}
			} else if (tokenNum_m == 3) {
			} else if (tokenNum_m == 4) {
				// finish this line
				if (codeInt_m == 0) {
					// all ok
					tokenNum_m = 0;
					finishline = true;
				}

			}
		} else {
			if (canRead_m) {
				if (tokenNum_m == 1) {
					// status code
					code_m.put(c);
				} else if (tokenNum_m == 2) {
					if (codeInt_m == 0) {
						topic_m.put(c);
					} else if (codeInt_m == 1) {
						errormsg_m.put(c);
					}

				} else if (tokenNum_m == 3) {
					if (codeInt_m == 0) {
						item_m.put(c);
					}

				} else if (tokenNum_m == 4) {
					if (codeInt_m == 0) {
						value_m.put(c);
					}

				}
			} // end canRead_m
		}

		return finishline;

	}
	; // end of function

	private HtRtData parseLine() throws Exception {
		// will prepare new data
		HtRtData rtdata = new HtRtData();
		rtdata.setProvider(getProviderId());

		String itemS = new String(item_m.array(), 0, item_m.position(), "US-ASCII");
		rtdata.setSymbol(itemS);

		rtdata.type_m = HtRtData.RT_Type;

		String topicS = new String(topic_m.array(), 0, topic_m.position(), "US-ASCII");
		if (topicS.equalsIgnoreCase("QUOTE")) {
			// this is our topic
			// 2007/04/23 12:40 2.0005 2.0008

			// will use simple loop

			String valueToParse = new String(value_m.array(), 0, value_m.position(), "US-ASCII");

			dateTimeBuffer_m.setLength(0);
			bidBuffer_m.setLength(0);
			askBuffer_m.setLength(0);

			int token = 1;
			for (int i = 0; i < valueToParse.length(); i++) {
				char cc = valueToParse.charAt(i);
				if (cc == ' ') {
					++token;
				}

				if (token == 1 || token == 2) {
					dateTimeBuffer_m.append(cc);
				} else if (token == 3) {
					// bid
					bidBuffer_m.append(cc);
				} else if (token == 4) {
					// ask
					askBuffer_m.append(cc);
				}
			}
			// parse the stuff
			rtdata.ask_m = Double.valueOf(askBuffer_m.toString());
			rtdata.bid_m = Double.valueOf(bidBuffer_m.toString());

			//dateTimeBuffer_m.append(",GMT");
			Date dat = format_m.parse(dateTimeBuffer_m.toString());

			Calendar now = Calendar.getInstance();
			long passedTime = dat.getTime() + now.get(Calendar.SECOND) * 1000 + now.get(Calendar.MILLISECOND);

			//String parsedBack = HtUtils.getGmtTime(passedTime);

			// need to apply shift to have uniqueness
			if (previousTimes_m.containsKey(itemS)) {
				long prevTime = previousTimes_m.get(itemS);

				if (passedTime <= prevTime) {
					// incerement 1 mislisecond in case of collisions
					rtdata.time_m = prevTime + 1;
				} else {
					rtdata.time_m = passedTime;
				}
			} else {
				rtdata.time_m = passedTime;
			}


			// store this
			previousTimes_m.put(itemS, rtdata.time_m);

		} else {
			throw new HtException(getContext(), "parseLine", "Invalid DDE topic: " + topicS);
		}

		return rtdata;
	}

	private String prepareDDEClientCall() throws Exception {
		// this prepares the call to DDE client

		//ddestdoutclient.exe DONOTCLOSEONERROR OUTENABLE MT4 QUOTE EURUSD QUOTE GBPUSD QUOTE AUDUSD

		StringBuilder callStr = new StringBuilder();
		String path = initData_m.get("ddeclient_path");

		if (path == null) {
			throw new HtException(getContext(), "prepareDDEClientCall", "parameter \"ddeclient_path\" is not set");
		}

		File fPath = new File(path);
		if (!fPath.exists()) {
			throw new HtException(getContext(), "prepareDDEClientCall", "Cannot find DDE client executable : " + path);
		}

		callStr.append(path);
		callStr.append(" DONOTCLOSEONERROR OUTENABLE MT4 ");

		// process symbol lsit
		for (Iterator<String> it = initData_m.keySet().iterator(); it.hasNext();) {
			String key = it.next();

			if (key.indexOf("symbol") != -1) {
				String val = initData_m.get(key);

				callStr.append("QUOTE");
				callStr.append(" ");
				callStr.append(val);
				callStr.append(" ");

				symbolList_m.add( val );
			}

		}

		// then if we fail just find
		String symbolFileName = initData_m.get("file_data_list");

		if (symbolFileName != null) {
			CSVReader reader = new CSVReader(new FileReader(symbolFileName), ',');
			String[] nextLine;

			while ((nextLine = reader.readNext()) != null) {
				callStr.append("QUOTE");
				callStr.append(" ");
				callStr.append(nextLine[0]);
				callStr.append(" ");

				symbolList_m.add( nextLine[0] );
			}


			reader.close();
		}

		return callStr.toString();
	}

	// this reads all symbols history  and sends them as tickers
	private void readAllSymbolsHistory() throws Exception
	{

		Map<Long, List<HtRtData> > historyCache = new TreeMap<Long, List<HtRtData>>();

		for(Iterator<String> it = symbolList_m.iterator(); it.hasNext();) {
			String symbol_i = it.next();

			StringBuilder fileName = new StringBuilder(HtSystem.getSystemTempPath());
			fileName.append(File.separatorChar);
			fileName.append(symbol_i);
			fileName.append("_history.dat");

			File f_i = new File(fileName.toString());

			if (f_i.exists()) {

				// need to lock this file and read it
				FileChannel channel = new RandomAccessFile(f_i, "rw").getChannel();

				// Use the file channel to create a lock on the file.
				// This method blocks until it can retrieve the lock.
				FileLock lock = channel.lock();

				
				
				byte[] ctmb = new byte[4];
				byte[] openb = new byte[8];
				byte[] highb = new byte[8];
				byte[] lowb = new byte[8];
				byte[] closeb = new byte[8];
				byte[] volub = new byte[8];
				
				// read content
				try {
					while(true) {
						ByteBuffer buf= ByteBuffer.allocate(4 + 8*5);

						int rd = channel.read(buf);

						if (rd<=0)
							break;


						byte[] buf_arr = buf.array();
						System.arraycopy(buf_arr, 0, ctmb, 0, 4);
						System.arraycopy(buf_arr, 4*1, openb, 0, 8);
						System.arraycopy(buf_arr, 4*1 + 8, highb, 0, 8);
						System.arraycopy(buf_arr, 4*1 + 8*2, lowb, 0, 8);
						System.arraycopy(buf_arr, 4*1 + 8*3, closeb, 0, 8);
						System.arraycopy(buf_arr, 4*1 + 8*4, volub, 0, 8);

						HtRtData rtdata_i = new HtRtData();
						rtdata_i.setSymbol(symbol_i);
						rtdata_i.setProvider(getProviderId());
						rtdata_i.type_m = HtRtData.HST_Type;

						rtdata_i.time_m = (long) HtMathUtils.convertByteToInt4(ctmb) * 1000;

						// ask prices
						/*
						rtdata_i.open_m = HtMathUtils.readDoubleValueFromBytes(openb);
						rtdata_i.close_m = HtMathUtils.readDoubleValueFromBytes(closeb);
						rtdata_i.high_m = HtMathUtils.readDoubleValueFromBytes(highb);
						rtdata_i.low_m = HtMathUtils.readDoubleValueFromBytes(lowb);
						 */

						// again
						// in MT history we work only with BID prices
						// bid prices
						rtdata_i.open2_m = HtMathUtils.readDoubleValueFromBytes(openb);
						rtdata_i.close2_m = HtMathUtils.readDoubleValueFromBytes(closeb);
						rtdata_i.high2_m = HtMathUtils.readDoubleValueFromBytes(highb);
						rtdata_i.low2_m = HtMathUtils.readDoubleValueFromBytes(lowb);

						rtdata_i.volume_m = HtMathUtils.readDoubleValueFromBytes(volub);

						// need to collect
						List<HtRtData> data_list = null;
						if (historyCache.containsKey(rtdata_i.time_m)) {
							data_list = historyCache.get(rtdata_i.time_m);
						}
						else {
							data_list = new ArrayList<HtRtData>();
							historyCache.put(rtdata_i.time_m, data_list);
						}

						data_list.add( rtdata_i );


					}

				}
				catch(Throwable e)
				{
					throw new HtException(getContext(), "readAllSymbolsHistory", "Exception: " + e.getMessage());
				}
				finally {
					// release
					lock.release();
					channel.close();
				}


				
				
			}
			
		}

		// send out this list
		int cnt = 0;
		for(Iterator<Long> it = historyCache.keySet().iterator(); it.hasNext(); ) {
			List<HtRtData> list_i = historyCache.get(it.next());

			for(int i = 0; i < list_i.size(); i++) {
				XmlEvent rtDataEvent = new XmlEvent();
				RtRealTimeProviderManager.createXmlEventRtData(list_i.get(i), rtDataEvent);
				RtGlobalEventManager.instance().pushCommonEvent(rtDataEvent);

				cnt++;
			}
		}

		XmlEvent.createSimpleLog("", getProviderId(), CommonLog.LL_INFO, "AS historical data was send: " + cnt + " HIST tickers");
	}

	@Override
	public void returnAvailableSymbolList(List<String> symbols) {
		throw new UnsupportedOperationException("Not supported yet.");
	}

	
	
}
