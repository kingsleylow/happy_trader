/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.OperationDetailResult;
import com.fin.httrader.hlpstruct.OperationResult;
import com.fin.httrader.hlpstruct.OrderType;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtBaseEntry;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtBrokerDialogData;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtOrderEntry;
import com.fin.httrader.utils.hlpstruct.brokerDialog.HtOrderResponseEntry;
import java.io.IOException;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtBrokerRunNames_v2_BrokerDialog extends HtServletsBase {

	public HtBrokerRunNames_v2_BrokerDialog() {
	}

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
			return false;
		}
		
		return true;
	}

	public boolean initialize_Post(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			// generate new Uid and set initial values
			setUniquePageId(getStringParameter(req, "page_uid", false));

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("show_broker_dialog")) {
				String data = this.getStringParameter(req, "data", false);
				XmlParameter params = XmlHandler.deserializeParameterStatic(data);

				Set<String> sessions = new HashSet<String>();
				Set<String> keys = params.getKeys();

				for (Iterator<String> it = keys.iterator(); it.hasNext();) {
					sessions.add(params.getString(it.next()));
				}

				readOrders(req, res, sessions);

				if (sessions.isEmpty()) {
					throw new HtException(getContext(), "initialize_Post", "Please select valid broker sessions");
				}


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
	void readOrders(HttpServletRequest req, HttpServletResponse res, Set<String> sessions) throws Exception {
		HtBrokerDialogData rdata = new HtBrokerDialogData();
		HtCommandProcessor.instance().get_HtDatabaseProxy().remote_getOrdersAndResponseTree(sessions, rdata);

		// create XML base on that
		//int cnt  =0;
		StringBuilder r = new StringBuilder("<tree id=\"0\">");
		List<HtBaseEntry> elist = rdata.getDataList();
		for (int i = 0; i < elist.size(); i++) {
			HtBaseEntry e_i = elist.get(i);
			int id_main = (i + 1) * 10000;



			if (e_i.getType() == HtBaseEntry.TYPE_ORDER) {
				r.append("<item").append(" id=\"").append(id_main).append("\">");

				HtOrderEntry oe_i = (HtOrderEntry) e_i;

				r.append("<itemtext>");
				r.append("<![CDATA[<table>");
				r.append(createHtmlOrderEntryHeader("#00FFFF"));
				r.append(createHtmlOrderEntryRow(oe_i,"#00FFFF"));
				r.append("</table>]]>");
				r.append("</itemtext>");

				// nested
				List<HtOrderResponseEntry> children = oe_i.getChildDataList();
				if (!children.isEmpty()) {
					int id_kid = id_main + 1;
					r.append("<item").append(" id=\"").append(id_kid).append("\">");

					r.append("<itemtext>");
					r.append("<![CDATA[<table>");
					r.append(createHtmlResponseEntryHeader( "BlanchedAlmond"));
					for (int j = 0; j < children.size(); j++) {
						HtOrderResponseEntry roe_j = children.get(j);
						r.append(createHtmlResponseEntryRow(roe_j, "BlanchedAlmond"));
					}

					r.append("</table>]]>");
					r.append("</itemtext>");

					r.append("</item>");
				}
				r.append("</item>");

			} else if (e_i.getType() == HtBaseEntry.TYPE_ORDER_RESPONSE) {
				
				HtOrderResponseEntry roe_i = (HtOrderResponseEntry) e_i;
				int id_kid = id_main + 1;
				
				// main item
				r.append("<item text=''").append(" id=\"").append(id_main).append("\">");
			

				// orphans
				r.append("<item").append(" id=\"").append(id_kid).append("\">");

				r.append("<itemtext>");
				r.append("<![CDATA[<table>");
				
				r.append(createHtmlResponseEntryHeader( "BlanchedAlmond"));
				r.append(createHtmlResponseEntryRow(roe_i, "BlanchedAlmond"));
				
				r.append("</table>]]>");
				r.append("</itemtext>");

				r.append("</item>");
				
				// -- end orphan
		

				r.append("</item>");
				
				
				

			}

		}

		r.append("</tree>");

		setStringSessionValue(getUniquePageId(), req, "report_data", r.toString());
	}
	
	private String createHtmlOrderEntryHeader(String color) {
		StringBuilder r = new StringBuilder();
		
		r.append("<tr style='font-weight:bold;font-size:smaller' bgcolor='").append(color).append("'><td>").
								append("Session ID").append("</td><td>").
								append("Order ID").append("</td><td>").
								append("Provider").append("</td><td>").
								append("Symbol").append("</td><td>").
								append("Issue Time").append("</td><td>").
								append("Operation Time").append("</td><td>").
								append("Order Type").append("</td><td>").
								append("</td></tr>");

		return r.toString();

	}
	
	private String createHtmlOrderEntryRow( HtOrderEntry oe_i ,String color) {
		StringBuilder r = new StringBuilder();
		
		r.append("<tr style='font-size:smaller' bgcolor='").append(color).append("'><td>").
								append(oe_i.sessionId_m).append("</td><td>").
								append("<a target='_blank' href='/htShowOrderPlusDetails.jsp?order_uid=").append(oe_i.id_m).append("'>").append(oe_i.id_m).
								append("</a>").append("</td><td>").
								append(oe_i.provider_m).append("</td><td>").
								append(oe_i.symbol_m).append("</td><td>").
								append(HtDateTimeUtils.time2String_Gmt(oe_i.orderIssueTime_m)).append("</td><td>").
								append(HtDateTimeUtils.time2String_Gmt(oe_i.orderOperationTime_m)).append("</td><td>").
								append(OrderType.getTypeName(oe_i.orderType_m)).append("</td><td>").
								append("</td></tr>");

		return r.toString();

	}

	private String createHtmlResponseEntryHeader( String color) {
		StringBuilder r = new StringBuilder();
		
		r.append("<tr style='font-weight:bold;font-size:smaller' bgcolor='").append(color).append("'><td>").
						append("Session ID").append("</td><td>").
						append("Response ID").append("</td><td>").
						append("Provider").append("</td><td>").
						append("Symbol").append("</td><td>").
						append("Issue Time").append("</td><td>").
						append("Operation Time").append("</td><td>").
						append("Order Type").append("</td><td>").
						append("Result Code").append("</td><td>").
						append("Result Detail Code").append("</td><td>").
						append("</td></tr>");

		return r.toString();

	}
	
	private String createHtmlResponseEntryRow(HtOrderResponseEntry ro, String color) {
		StringBuilder r = new StringBuilder();
		
		r.append("<tr style='font-size:smaller' bgcolor='").append(color).append("'><td>").
						append(ro.sessionId_m).append("</td><td>").
						append("<a target='_blank' href='/htShowBrokerPlusOrder.jsp?broker_uid=").append(ro.id_m).append("'>").append(ro.id_m).append("</a>").append("</td><td>").
						append(ro.provider_m).append("</td><td>").
						append(ro.symbol_m).append("</td><td>").
						append(HtDateTimeUtils.time2String_Gmt(ro.responseIssueTime_m)).append("</td><td>").
						append(HtDateTimeUtils.time2String_Gmt(ro.responseIssueTime_m)).append("</td><td>").
						append(OrderType.getTypeName(ro.orderType_m)).append("</td><td>").
						append(OperationResult.getOperationResultName(ro.resultCode_m)).append("</td><td>").
						append(OperationDetailResult.getOperationDetailresultName(ro.resultDetailCode_m)).append("</td><td>").
						append("</td></tr>");

		return r.toString();

	}
}
