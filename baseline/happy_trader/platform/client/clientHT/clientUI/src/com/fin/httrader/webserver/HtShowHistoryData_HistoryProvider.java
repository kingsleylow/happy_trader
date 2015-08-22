/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.RtDataOperation;
import com.fin.httrader.hlpstruct.RtDataPrintColor;
import com.fin.httrader.interfaces.HtDataSelection;
import com.fin.httrader.managers.RtDatabaseManager;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.*;
import com.fin.httrader.utils.hlpstruct.*;

import java.io.IOException;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtShowHistoryData_HistoryProvider extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {


			res.setContentType("text/xml; charset=windows-1251");
			forceNoCache(res);

			


			try {

				setUniquePageId(getStringParameter(req, "page_uid", false));
				clearErrorXmlInfo(req);

				String begDateStr = getStringParameter(req, "bdate", true);
				String endDateStr = getStringParameter(req, "edate", true);

				long begDate = HtDateTimeUtils.parseDateTimeParameter(begDateStr, false);
				long endDate = HtDateTimeUtils.parseDateTimeParameter(endDateStr, false);

				// true if show all provider/symbols
				boolean allIfMapEmpty = getBooleanParameter(req, "all_if_no_symb", true);

				// paging
				// -1 means all
				int page_num = (int) getIntParameter(req, "read_page_num", false);
				int read_rows_per_page = (int) getIntParameter(req, "read_rows_per_page", false);

				String profile_name = getStringParameter(req, "profile_name", true);
				List<HtPair<String, String>> provSymbMapPage = getStringPairListParameter(req, "ps", true);
				//provSymbMapPage.clear();

			

				int dignum = (int) getIntParameter(req, "dig_num", false);


				// load history data
				final HttpServletResponse fres = res;
				final LongParam id = new LongParam();
				final int sdigits_f = dignum;
				final ServletOutputStream ostrm = res.getOutputStream();


				// instantiate an anonymous class
				HtDataSelection dataselector = new HtDataSelection() {

					@Override
					public void onSelectionFinish(long cnt) throws Exception {
					
					}


				@Override
				public void newRowArrived(long ttime, String tsymbol, String tprovider, int ttype, int aflag, double tbid, double task, double topen, double thigh, double tlow, double tclose, double topen2, double thigh2, double tlow2, double tclose2, double tvolume, long tid, int color, int operation) throws Exception {
					id.setLong(id.getLong() + 1);
						sendXmlRow(sdigits_f, id.getLong(), ostrm, ttime, tsymbol, tprovider, ttype, aflag, tbid, task, topen, thigh, tlow, tclose, topen2, thigh2, tlow2, tclose2, tvolume, tid, color, operation);
				}
				};

				// do load
				// execute that
				// read all as one chunk
				sendXmlHeader(ostrm);

				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showData_Callback_Paging(
								profile_name,
								provSymbMapPage,
								allIfMapEmpty,
								begDate,
								endDate,
								page_num,
								read_rows_per_page,
								dataselector);

				sendXmlFooter(ostrm);
			} catch (Throwable e) {

				initErrorXmlInfo(req, e);

			}

	}

//
	private void sendXmlHeader(ServletOutputStream ostrm) throws Exception {
		ostrm.print("<?xml version=\"1.0\" encoding=\"windows-1251\"?>");
		ostrm.print("<rows>");

	}

	private void sendXmlFooter(ServletOutputStream ostrm) throws Exception {
		ostrm.print("</rows>");
	}

	private void sendXmlRow(
					int sdigits_f,
					long id,
					ServletOutputStream ostrm,
					long ttime,
					String tsymbol,
					String tprovider,
					int ttype,
					int aflag,
					double tbid,
					double task,
					double topen,
					double thigh,
					double tlow,
					double tclose,
					double topen2,
					double thigh2,
					double tlow2,
					double tclose2,
					double tvolume,
					long tid,
					int color,
					int operation
				
				) throws Exception {


		ostrm.print("<row id=\"");
		ostrm.print(id);
		ostrm.print("\">");

		ostrm.print("<cell>");
		ostrm.print(HtDateTimeUtils.time2String_Gmt(ttime));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(tsymbol);
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(tprovider);
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(ttype == HtRtData.RT_Type ? "R/T" : "HIST");
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(aflag == RtDatabaseManager.HST_DATA_CLOSED ? "CLOSED" : "NON-CLOSED");
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tbid, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(task, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tvolume, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(topen, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(thigh, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tlow, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tclose, sdigits_f, false));
		ostrm.print("</cell>");

		// go with BID
		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(topen2, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(thigh2, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tlow2, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(HtUtils.formatPriceValue(tclose2, sdigits_f, false));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(RtDataPrintColor.getColorTypeName(color));
		ostrm.print("</cell>");

		ostrm.print("<cell>");
		ostrm.print(RtDataOperation.getOperationTypeName(operation));
		ostrm.print("</cell>");

		
		ostrm.print("</row>");
	}
}
