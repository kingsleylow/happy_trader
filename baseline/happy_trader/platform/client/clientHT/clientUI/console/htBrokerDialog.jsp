<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerDialog" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj' >View Broker Dialog</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>


		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

		<!-- misc functions -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- navigation bar -->
		<script type="text/javascript" src="js/misc/navhelper.js"></script>

		<!-- main calendar program -->
		<script type="text/javascript" src="js/calendar.js"></script>

		<!-- language for the calendar -->
		<script type="text/javascript" src="js/lang/calendar-en.js"></script>

		<!-- the following script defines the Calendar.setup helper function, which makes
			 adding a calendar a matter of 1 or 2 lines of code. -->
		<script type="text/javascript" src="js/calendar-setup.js"></script>


		<!-- OLD DHTML grid -->
	
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>

		<script  src="js/grid/dhtmlxgrid_start.js"></script>
		

		

		<script>
			<%
				String jscriptArraySession_ids = SC.getStringSessionValue(SC.getUniquePageId(), request, "session_runid_list");
				if (jscriptArraySession_ids==null || jscriptArraySession_ids.length()==0) {
			%>
				var filterSessions_m = new Array();

			<% } else { %>

				var filterSessions_m =  <%= jscriptArraySession_ids %>

			<% } %>
				function page_load()
				{

					setPageCaption(tltObj.innerText);

					if (is_session_filtering()) {
						sessionShowSelectObj.innerHTML = '<a href="#" onclick="do_show_selected_sessions();">Show Selected Sessions</a>'

						sessionFilterSelectObj.innerHTML='Select Sessions';
						runnameFilterSelectObj.innerHTML ='Select Run Names';

					}
					else {
						sessionShowSelectObj.innerHTML = 'No session filter';

						sessionFilterSelectObj.innerHTML = '<a href="#" onclick="do_select_session();">Select Sessions</a>';
						runnameFilterSelectObj.innerHTML = '<a href="#" onclick="do_select_runnames();">Select Run Names</a>';

					}


					new_page_load();

				}



				function do_clear()
				{
					new_do_clear();


				}

				function do_go()
				{
					new_do_go();
				}



				function ordet(id)
				{
					var url = "/htShowOrderPlusDetails.jsp?order_uid="+id;
					window.open(url);
				}

				function brdet(id)
				{
					var url = "/htShowBrokerPlusOrder.jsp?broker_uid="+id;
					window.open(url);
				}

				function do_select_session()
				{
					window.navigate('htBrokerSessions.jsp?operation=show_default_page');
				}

				function do_select_runnames()
				{
					window.navigate('htBrokerRunNames.jsp?operation=show_default_page');
				}

				// helper function which shows available sessions
				function do_show_selected_sessions()
				{
					//window.open('/htBrokerDialog_showFilterSessions.jsp?cookie=<%= SC.getUniquePageId()%>');
					window.showModalDialog('/htBrokerDialog_showFilterSessions.jsp?cookie=<%= SC.getUniquePageId()%>', '', 'resizable:1;dialogHeight=400px;dialogWidth=400px;help:0;status:0;scroll:0');

				}

				function create_session_list() {
					var slist = "";

					for(i=0; i < filterSessions_m.length/2; i++) {
						var session_i = filterSessions_m[i*2];

						slist += session_i + ",";

					}
					return slist;
				}

				// whether there are available session for filtering
				function is_session_filtering()
				{
					return filterSessions_m.length > 0;
				}


				function do_report()
				{
					if (!is_session_filtering()) {
						alert('No selected sessions');
						return;
					}
					
					var inp_params = new Array();
				
					// get parameters
					var outparams = window.showModalDialog(
						'htTradingReport_repParams.jsp',
						inp_params, 'resizable:0;dialogHeight=390px;dialogWidth=600px;help:0;status:0;scroll:0');

					if (outparams==null)
						return;

					var url = '/htTradingReport.jsp?operation=do_report';
					mForm_Report.session_ids.value = create_session_list();
					mForm_Report.edate.value = fmain.f_date_e.value;
					mForm_Report.bdate.value = fmain.f_date_b.value;

					// whether to group via run id
					mForm_Report.do_runid_group.value = outparams['do_runid_group'];

					var use_rep_symb = outparams['use_report'];

					if (use_rep_symb==1) {
						mForm_Report.price_to_use.value = outparams['price_to_use'];
						mForm_Report.report_profit_symbol.value = outparams['report_profit_symbol'];
						mForm_Report.report_profit_provider.value = outparams['report_profit_provider'];
						mForm_Report.report_profit_profile.value = outparams['report_profit_profile'];
						mForm_Report.portfolio_combin.value = outparams['portfolio_combin'];
						mForm_Report.init_equity_sum.value = outparams['init_equity_sum'];
						mForm_Report.max_min_translate_torep_symbol.value=outparams['max_min_translate_torep_symbol'];
					}
					else {
						mForm_Report.price_to_use.value = '';
						mForm_Report.report_profit_symbol.value = '';
						mForm_Report.report_profit_provider.value = '';
						mForm_Report.report_profit_profile.value = '';
						mForm_Report.portfolio_combin.value = '0';
						mForm_Report.init_equity_sum.value = '';
						mForm_Report.max_min_translate_torep_symbol.value = 0;
					}



					var dt = new Date();
					mForm_Report.target="Trading Report "+dt.getMilliseconds();
					mForm_Report.action = url;


					mForm_Report.submit();

				}

				// --------------------------------------------------------
				// NEW CODE
				// --------------------------------------------------------


				var navigationBar_Orders_m = null;

				var navigationBar_Responses_m = null;

				var xmlHttp_Orders_m = getXmlHttp();

				var xmlHttp_Responses_m = getXmlHttp();

				var grid_Orders_m = null;

				var grid_Responses_m = null;


				function usrFun_SwitchOnLoadInProgress()
				{
					bodyObj.disabled=true;
				}

				function usrFun_SwitchOffLoadInProgress()
				{
					bodyObj.disabled=false;
				}


				function new_page_load()
				{
					goClearObj.disabled=true;

					navigationBar_Orders_m = new NavigationBar(	'Orders', "navDivObj_Orders",		<%= SC.getPageSize()%>,			<%= SC.getNumberPagesToShow()%>,
					usrFun_generUrl_Orders,
					usrFun_Navigate_Orders,
					usrFun_SwitchOnLoadInProgress,
					usrFun_SwitchOffLoadInProgress, 'p1');


					navigationBar_Orders_m.disable();


					navigationBar_Responses_m = new NavigationBar(	'Responses', "navDivObj_Responses",		<%= SC.getPageSize()%>,			<%= SC.getNumberPagesToShow()%>,
					usrFun_generUrl_Responses,
					usrFun_Navigate_Responses,
					usrFun_SwitchOnLoadInProgress,
					usrFun_SwitchOffLoadInProgress,'p2');

					navigationBar_Responses_m.disable();
				}

				function new_do_clear()
				{
					// disable go
					goBtnObj.disabled=false;
					goClearObj.disabled=true;

					fmain.f_trigger_b.disabled = false;
					fmain.f_trigger_e.disabled = false;
					fmain.f_date_b.disabled = false;
					fmain.f_date_e.disabled = false;

					reinitHistoryObject_Orders();
					reinitHistoryObject_Responses();

					navigationBar_Orders_m.disable();
					navigationBar_Responses_m.disable();
				}

				function new_do_go()
				{
					// disable go
					goBtnObj.disabled=true;
					goClearObj.disabled=false;

					fmain.f_trigger_b.disabled = true;
					fmain.f_trigger_e.disabled = true;
					fmain.f_date_b.disabled = true;
					fmain.f_date_e.disabled = true;


					navigationBar_Orders_m.enable();
					navigationBar_Responses_m.enable();

					navigationBar_Orders_m.first_page_history();
					navigationBar_Responses_m.first_page_history();


				}

				//////////////////////////////////////

				function initHistoryGridObject_Orders()
				{
				}

				function reinitHistoryObject_Orders()
				{
					if (grid_Orders_m != null) {
						grid_Orders_m.destructor();
						grid_Orders_m = null;
					}
					dataTblObj_Orders.innerHTML = '<div style="width:100%;height:100%;"></div>';
				}

				function createGridFromTable_Orders(loader)
				{
					dataTblObj_Orders.innerHTML = loader.responseText;

					initTableWithDummyWhitspaces('dataTblObj_Orders_Tbl');
					grid_Orders_m = new dhtmlXGridFromTable('dataTblObj_Orders_Tbl');
					grid_Orders_m.setImagePath("/imgs/grid/");

				}

				function usrFun_generUrl_Orders(history_pageIndex, pageSize)
				{

					var url = '/HtBrokerDialog_Orders_HistoryProvider.jsp?'+
						"bdate="+encodeURIComponent(fmain.f_date_b.value)+
						"&edate="+encodeURIComponent(fmain.f_date_e.value) +
						"&read_page_num="+history_pageIndex+
						"&read_rows_per_page="+pageSize+
						"&page_uid=<%= SC.getUniquePageId() %>";


					return url;

				}

				function usrFun_Navigate_Orders(historyBaseUrl)
				{


					reinitHistoryObject_Orders();

					var session_ids = create_session_list();
					var total_data = "session_ids="+encodeURIComponent(session_ids)+"&session_human_ids=&alg_brk_pairs=";

					makeAsynchXmlHttpRequestExternHandler(
					xmlHttp_Orders_m,
						-1,
					'POST',
					historyBaseUrl,
					total_data,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							createGridFromTable_Orders(loader);

							// status callback
							check_error_after_history_load_orders();

						}
						else {

							// status callback
							check_error_after_history_load_orders();

						}
					},

					function(errmsg)
					{
						alert('Internal error on navigating: '+errmsg);
						// status callback
						check_error_after_history_load_orders();
					}
				);




				}

				function getNumberOfRows_Orders()
				{
					return new Number(dataTblObj_Orders_rowsnum.innerText);
				}



				// checks if any error happend after history load
				function check_error_after_history_load_orders()
				{

					makeAsynchXmlHttpRequestExternHandler(
					xmlHttp_Orders_m,
						-1,
					'GET',
					'/HtReturnSessionVar_v2.jsp?response_type=xml&session_var=error_info&cookie=<%=SC.getUniquePageId() %>',
					null,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							// error message
							navigationBar_Orders_m.callback_NavigationFinished( -1, false );
							document.body.innerHTML = extractErrorMessage(loader.responseXML);
						}
						else {

							navigationBar_Orders_m.callback_NavigationFinished( getNumberOfRows_Orders() , true );


						}
					}	,

					function(errmsg)
					{
						alert('Internal error on get error status: '+errmsg);
						navigationBar_Orders_m.callback_NavigationFinished( -1, false );

					}
				);

				};


				///////////

				function initHistoryGridObject_Responses()
				{
				}

				function reinitHistoryObject_Responses()
				{
					if (grid_Responses_m != null) {
						grid_Responses_m.destructor();
						grid_Responses_m = null;
					}
					dataTblObj_Responses.innerHTML = '<div style="width:100%;height:100%;"></div>';
				}

				function createGridFromTable_Responses(loader)
				{
					dataTblObj_Responses.innerHTML = loader.responseText;
					initTableWithDummyWhitspaces('dataTblObj_Responses_Tbl');

					grid_Responses_m = new dhtmlXGridFromTable('dataTblObj_Responses_Tbl');
					grid_Responses_m.setImagePath("/imgs/grid/");


				}


				function usrFun_generUrl_Responses(history_pageIndex, pageSize)
				{

					var url = '/HtBrokerDialog_BrkResp_HistoryProvider.jsp?'+
						"bdate="+encodeURIComponent(fmain.f_date_b.value)+
						"&edate="+encodeURIComponent(fmain.f_date_e.value) +
						"&read_page_num="+history_pageIndex+
						"&read_rows_per_page="+pageSize+
						"&page_uid=<%= SC.getUniquePageId() %>";


					return url;

				}

				function usrFun_Navigate_Responses(historyBaseUrl)
				{


					reinitHistoryObject_Responses();

					var session_ids = create_session_list();
					var total_data = "session_ids="+encodeURIComponent(session_ids)+"&session_human_ids=&alg_brk_pairs=";

					makeAsynchXmlHttpRequestExternHandler(
					xmlHttp_Responses_m,
						-1,
					'POST',
					historyBaseUrl,
					total_data,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
		
							createGridFromTable_Responses(loader);

							// status callback
							check_error_after_history_load_responses();

						}
						else {

							// status callback
							check_error_after_history_load_responses();

						}
					},

					function(errmsg)
					{
						alert('Internal error on get error status: '+errmsg);
						// status callback
						check_error_after_history_load_responses();
					}
				);




				}

				function getNumberOfRows_Responses()
				{
					return new Number(dataTblObj_Responses_rowsnum.innerText);
				}



				// checks if any error happend after history load
				function check_error_after_history_load_responses()
				{


					makeAsynchXmlHttpRequestExternHandler(
					xmlHttp_Responses_m,
						-1,
					'GET',
					'/HtReturnSessionVar_v2.jsp?response_type=xml&session_var=error_info&cookie=<%=SC.getUniquePageId() %>',
					null,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							// error message
							navigationBar_Responses_m.callback_NavigationFinished( -1, false );
							document.body.innerHTML = extractErrorMessage(loader.responseXML);
						}
						else {

							navigationBar_Responses_m.callback_NavigationFinished( getNumberOfRows_Responses() , true );


						}
					}	,

					function(errmsg)
					{
						alert('Internal error on get error status: '+errmsg);
						navigationBar_Responses_m.callback_NavigationFinished( -1, false );

					}
				);

				};

		</script>
	</head>

	<body id="bodyObj" onload="page_load();">

		<table width=100% height="100%" class=x8>
			<tr height=20px> <!-- control header begin -->

				<td>
					<table height="100%" width=100% class=x8>
						<tr>

							<td width="350px" style="border:1px solid black"> <!-- first column -->

								<table width="100%" class=x8 height=100%>
									<tr valign=top>
										<td id="sessionShowSelectObj" align=left colspan=3>
										</td>
									</tr>

									<tr valign=top>
										<td height=20px width=150px id="runnameFilterSelectObj" align=left >
											<a href="#" onclick="do_select_runnames();">Select Run Names</a>
										</td>

										<td height=20px width=150px id="sessionFilterSelectObj" align=left >
											<a href="#" onclick="do_select_session();">Select Sessions</a>
										</td>

										<td ></td>
									</tr>

								</table>

							</td> <!-- first column -->

							<td width=250px style="border:1px solid black" > <!-- second column -->
								<form action="#" method="get" id="fmain">
									<table width=100% height=100% valign=top class=x8>
										<tr valign=top>
											<td align=left width=80px>
												Select From
											</td>


											<td>


												<table width=100% cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
													<tr>
														<td width=120px>
															<input style="width: 100%" class=x8 type="text" name="date" id="f_date_b" />
														</td>

														<td>
															<img src="imgs/img.gif" id="f_trigger_b" style="cursor: pointer; border: 1px solid red;" title="Date selector"
												onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
														</td>

													</tr>
												</table>


												<script type="text/javascript">
													Calendar.setup({
														inputField     :    "f_date_b",     // id of the input field
														ifFormat       :    "%d-%m-%Y %H:%M",      // format of the input field
														button         :    "f_trigger_b",  // trigger for the calendar (button ID)
														align          :    "Tl",           // alignment (defaults to "Bl")
														singleClick    :    true,
														timeFormat     :    "24",
														showsTime      :    true
													});
												</script>

											</td>
										</tr>

										<tr valign=top>
											<td align=left width=80px>
												Select To
											</td>
											<td>

												<table width=100% cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
													<tr>
														<td width=120px>
															<input style="width: 100%" class=x8 type="text" name="date" id="f_date_e" />
														</td>

														<td>
															<img src="imgs/img.gif" id="f_trigger_e" style="cursor: pointer; border: 1px solid red;" title="Date selector"
												onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
														</td>

													</tr>
												</table>


												<script type="text/javascript">
													Calendar.setup({
														inputField     :    "f_date_e",     // id of the input field
														ifFormat       :    "%d-%m-%Y %H:%M",      // format of the input field
														button         :    "f_trigger_e",  // trigger for the calendar (button ID)
														align          :    "Tl",           // alignment (defaults to "Bl")
														singleClick    :    true,
														timeFormat     :    "24",
														showsTime      :    true
													});
												</script>

											</td>
										</tr>



										<tr>
											<td  width="90px">

											</td>

											<td align=left>

											</td>
										</tr>


									</table>
								</form>
							</td> <!-- second column -->

							<td> <!-- third column -->
								<table class=x8 width=100% height=100% valign=top>
									<tr height=10px>
										<td id="goBtnObj" width="90px" align=left>
											<a class="xd" href="#" onclick="do_go();">GO ></a>
										</td>
										<td></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td id="goClearObj" width="90px" align=left>
											<a class="xd" href="#" onclick="do_clear();">Clear</a>
										</td>
										<td></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width="90px" align=left valign=top>
											<a class="xd" href="#" onclick="do_report()">Trading report</a>
										</td>

										<td valign=top>
										</td>

										<td></td>
									</tr>

									<tr>
										<td></td>
										<td></td>
										<td></td>
									</tr>

								</table>
							</td> <!-- third column -->

							<td></td>

						</tr>
					</table>

				</td>
			</tr> <!-- control header end -->




			<tr height=30px>
				<td>
					<div id="navDivObj_Orders" width="100%" height="100%">
					</div>
				</td>
			</tr>


			<tr width=100%>
				<td style='border: 1px solid black' id="dataTblObj_Orders" align="center">
					<div style="width:100%;height:100%;"></div>
				</td>
			</tr>

			<tr height=30px>
				<td>
					<div id="navDivObj_Responses" width="100%" height="100%">
					</div>
				</td>
			</tr>

			<tr width=100% >
				<td style='border: 1px solid black' id="dataTblObj_Responses" align="center">
				 <div style="width:100%;height:100%;"></div>
				</td>
			</tr>

		</table>


		<form method="post" action="" id="mForm_Report" style="display:none">
			<input type=hidden name="session_ids" value=""></input>
			<input type=hidden name="price_to_use" value=""></input>
			<input type=hidden name="report_profit_symbol" value=""></input>
			<input type=hidden name="report_profit_provider" value=""></input>
			<input type=hidden name="report_profit_profile" value=""></input>
			<input type=hidden name="portfolio_combin" value="0"></input>
			<input type=hidden name="edate" value=""></input>
			<input type=hidden name="bdate" value=""></input>
			<input type=hidden name="init_equity_sum" value=""></input>
			<input type=hidden name="do_runid_group" value="0"></input>
			<input type=hidden name="max_min_translate_torep_symbol" value="0"></input>
		</form>



	</body>
</html>

