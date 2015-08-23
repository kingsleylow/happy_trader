/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.interfaces.HtRunNameObjectSelector;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.HtRunNameEntry;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames_v2_HistoryProvider extends HtServletsBase {
	
	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/xml; charset=windows-1251");
		forceNoCache(res);

		try {
			
			// set this page UID
			setUniquePageId(getStringParameter(req, "page_uid", false));
			clearErrorXmlInfo(req);

			String begDateStr = getStringParameter(req, "bdate", true);
			String endDateStr = getStringParameter(req, "edate", true);

			long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
			long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);

			final HttpServletResponse fres = res;
			final HttpServletRequest freq = req;
			final ServletOutputStream ostrm = res.getOutputStream();
			
			final LongParam id = new LongParam();
			final LongParam color = new LongParam();
			color.setLong(0);
			
			HtRunNameObjectSelector dataselector = new HtRunNameObjectSelector()
			{

				@Override
				public void onRunNameEntryArrived(HtRunNameEntry rentry) throws Exception {
					
					sendXmlRow(color, id, ostrm,rentry);
				}

				@Override
				public void onFinish(long cntr) throws Exception {
					
				}
			};
							
			sendXmlHeader(ostrm);
		
			HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showBrokerRunNames_v2_Callback(
							begDate,
							endDate,
							dataselector
					
							);
			
			
			sendXmlFooter(ostrm);
		
		} catch (Throwable e) {

			//writeHtmlErrorToOutput(res, e);
			// must be in a separate place
			initErrorXmlInfo(req,e);


		}
	}
	
	/**
	 * Helpers
	 */
	
	private void sendXmlHeader(ServletOutputStream ostrm) throws Exception {
		ostrm.print("<?xml version=\"1.0\"?>");
		ostrm.print("<rows>");

	}

	private void sendXmlFooter(ServletOutputStream ostrm) throws Exception {
		ostrm.print("</rows>");
	}

	private void sendXmlRow(LongParam color, LongParam id, ServletOutputStream ostrm, HtRunNameEntry rentry) throws Exception {
	
		if (color.getLong() == 0) {
			color.setLong(1);
		}else {
			color.setLong(0);
		}
		
		for(int i = 0; i < rentry.sessions_m.size(); i++) {
			HtRunNameEntry.HtSessionEntry se = rentry.sessions_m.get(i);
			
			id.setLong( id.getLong() + 1);
			
			ostrm.print("<row id=\"");
			ostrm.print(id.getLong());
			ostrm.print("\">");
		
			ostrm.print("<cell>");
			ostrm.print(0);
			ostrm.print("</cell>");
			
			ostrm.print("<cell>");
			ostrm.print(i==0 ? rentry.runName_m : "");
			ostrm.print("</cell>");
			
			ostrm.print("<cell>");
			ostrm.print(i==0 ? HtDateTimeUtils.time2String_Gmt( rentry.opFirstTime_m ) : "");
			ostrm.print("</cell>");
			
			
			ostrm.print("<cell>");
			ostrm.print(se.sessionId_m != null ? se.sessionId_m: "");
			ostrm.print("</cell>");
			
			ostrm.print("<cell>");
			ostrm.print(se.opCount_m);
			ostrm.print("</cell>");
			
			ostrm.print("<cell>");
			ostrm.print(se.respCount_m);
			ostrm.print("</cell>");
			
					
			ostrm.print("<cell>");
			ostrm.print(se.closePosCount_m);
			ostrm.print("</cell>");
			
			
					
			ostrm.print("</row>");
			
		}
	}
}
