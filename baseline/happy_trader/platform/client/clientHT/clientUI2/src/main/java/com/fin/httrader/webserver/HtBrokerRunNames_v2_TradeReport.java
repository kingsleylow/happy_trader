/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.hlpstruct.OrderValidity;
import com.fin.httrader.hlpstruct.PriceConst;
import com.fin.httrader.hlpstruct.TradeDirection;
import com.fin.httrader.interfaces.HtCommonRowSelector;
import com.fin.httrader.interfaces.HtCommonRowSelector.CommonHeader;
import com.fin.httrader.interfaces.HtCommonRowSelector.CommonRow;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtStringByteConverter;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.LongParam;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.IOException;
import java.util.*;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames_v2_TradeReport extends HtServletsBase {

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new Uid and set initial values
			this.generateUniquePageId();



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return true;
		}
		
		
		return false;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {



			// generate new Uid and set initial values
			setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("read_report")) {

				String data = this.getStringParameter(req, "data", false);
				XmlParameter params = XmlHandler.deserializeParameterStatic(data);

				Set<String> sessions = new HashSet<String>();
				Set<String> keys = params.getKeys();

				for (Iterator<String> it = keys.iterator(); it.hasNext();) {
					sessions.add(params.getString(it.next()));
				}
				
				if (sessions.isEmpty())
					throw new HtException(getContext(), "initialize_Post", "Please select valid broker sessions");

				//Map<String, Set<String>> sessRunIds = HtCommandProcessor.instance().get_HtDatabaseProxy().remote_resolveSessionRunIdsViaSessions(sessions);

				final HttpServletResponse fres = res;
				final HttpServletRequest freq = req;
				final StringBuilder out = new StringBuilder();

				final LongParam id = new LongParam();

				HtCommonRowSelector dataselector = new HtCommonRowSelector() {

					@Override
					public void newHeaderArrived(CommonHeader pheader) throws Exception {

						sendInit_XmlHeader(out);

						for (int i = 1; i <= pheader.getSize(); i++) {



							//
							if (pheader.getFieldName(i).equalsIgnoreCase("broker_issue_time")) {
								pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
							}

							if (pheader.getFieldName(i).equalsIgnoreCase("pos_open_time")) {
								pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
							}

							if (pheader.getFieldName(i).equalsIgnoreCase("pos_close_time")) {
								pheader.addFormatter(i, HtCommonFieldFormatter_GmtTime.getInstance());
							}

							if (pheader.getFieldName(i).equalsIgnoreCase("trade_direction")) {
								pheader.addFormatter(i, new HtCommonRowSelector.HtCommonFieldFormatter() {

									@Override
									public String format(CommonRow row, String sval) {
										return TradeDirection.getTradeDirectionName(HtUtils.parseInt(sval));
									}
								});
							}


							sendInit_XmlField(out, pheader.getDisplayName(i));

						} // end loop

						sendInit_XmlFooter(out);
					}

					@Override
					public void newRowArrived(CommonRow row) throws Exception {
						id.setLong(id.getLong() + 1);
						sendXmlRow(out, id.getLong(), row);
					}

					@Override
					public void onSelectionFinish(long cntr) throws Exception {
						sendXmlFooter(out, id.getLong());
					}
				};

				// get data
				HtCommandProcessor.instance().get_HtDatabaseProxy().remote_showClosePos_Callback(sessions, dataselector);

				setStringSessionValue(getUniquePageId(), req, "report_data", out.toString());

			} else if (operation.equalsIgnoreCase("save_csv")) {
			} else {
				throw new HtException(getContext(), "initialize_Post", "Invalid operation");
			}

		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}

	/**
	 * Helpers
	 */
	private void sendInit_XmlHeader(StringBuilder out) throws Exception {
		out.append("<tr class='x3' bgcolor='buttonface'>");
	}

	private void sendInit_XmlField(StringBuilder out, String fieldname) throws Exception {
		out.append("<td type='ro' NOWRAP=true align=left>");
		out.append(fieldname);
		out.append("</td>");

	}

	private void sendInit_XmlFooter(StringBuilder out) throws Exception {
		out.append("</tr>");
	}

	private void sendXmlFooter(StringBuilder out, long lastid) throws Exception {
	}

	private void sendXmlRow(StringBuilder out, long id, HtCommonRowSelector.CommonRow row) throws Exception {

		out.append("<tr align=left internal_id='");
		out.append(id);
		out.append("'>");

		for (int k = 1; k <= row.getHeader().getSize(); k++) {
			out.append("<td NOWRAP=true>");

			//String fieldName = row.getHeader().getFieldName(k);
			out.append(row.getFormattedValue(k));
			//res.getOutputStream().print(row.getFormattedValue(k));
			out.append("</td>");
		}

		out.append("</tr>");
	}
}
