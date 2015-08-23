<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerSessions" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
		SC.initialize_Get(request,response);
}
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj' >View Broker Sessions</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>


		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

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

		<!-- DHTML grid -->
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>

		<script>

			var mygrid_m = null;

			var navigationBar_m = null;

			var xmlHttp_m = getXmlHttp();

			function page_load()
			{



				setPageCaption(tltObj.innerText);
				goClearObj.disabled=true;

				navigationBar_m = new NavigationBar(	'Sessions', "navDibObj",
			<%= SC.getPageSize()%>,
			<%= SC.getNumberPagesToShow()%>,
					usrFun_generUrl,
					usrFun_Navigate,
					usrFun_SwitchOnLoadInProgress,
					usrFun_SwitchOffLoadInProgress		);


					navigationBar_m.disable();

					if ('<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "operation")%>'=='show_default_page') {
						do_go();
					}

				}



				function usrFun_generUrl(history_pageIndex, pageSize)
				{
					var url = '/HtBrokerSessions_HistoryProvider.jsp?'+
						"bdate="+encodeURIComponent(fmain.f_date_b.value)+
						"&edate="+encodeURIComponent(fmain.f_date_e.value) +
						"&read_page_num="+history_pageIndex+
						"&read_rows_per_page="+pageSize+
						"&page_uid=<%= SC.getUniquePageId() %>";


					return url;

				}

				function usrFun_Navigate(historyBaseUrl)
				{
					reinitHistoryObject();
					mygrid_m.loadXML(historyBaseUrl, check_error_after_history_load);
				}

				function usrFun_SwitchOnLoadInProgress()
				{
					bodyObj.disabled=true;
				}

				function usrFun_SwitchOffLoadInProgress()
				{
					bodyObj.disabled=false;
				}

				// checks if any error happend after history load
				function check_error_after_history_load()
				{

					makeAsynchXmlHttpRequest(
						-1,
					'GET',
					'/HtReturnSessionVar_v2.jsp?response_type=xml&session_var=error_info&cookie=<%=SC.getUniquePageId() %>',
					null,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							// error message
							navigationBar_m.callback_NavigationFinished( -1, false );
							document.body.innerHTML = extractErrorMessage(loader.responseXML);
						}
						else {

							navigationBar_m.callback_NavigationFinished( mygrid_m.getRowsNum(), true );
						}
					}	,

					function(errmsg)
					{
						alert('Internal error on get error status: '+errmsg);
						navigationBar_m.callback_NavigationFinished( -1, false );

					}		);

				};


				function do_clear()
				{
					//


					fmain.f_trigger_b.disabled = false;
					fmain.f_trigger_e.disabled = false;
					fmain.f_date_b.disabled = false;
					fmain.f_date_e.disabled = false;


					// disable go
					goBtnObj.disabled=false;
					goClearObj.disabled=true;

					//


					reinitHistoryObject();
					navigationBar_m.disable();


				}





				function do_go()
				{
					// disable



					fmain.f_trigger_b.disabled = true;
					fmain.f_trigger_e.disabled = true;
					fmain.f_date_b.disabled = true;
					fmain.f_date_e.disabled = true;


					// disable go
					goBtnObj.disabled=true;
					goClearObj.disabled=false;


					navigationBar_m.enable();
					navigationBar_m.first_page_history();
				}


				function initHistoryGridObject()
				{
					mygrid_m = new dhtmlXGridObject('sessionIdData_obj');

					mygrid_m.imgURL = "imgs/grid/";
					mygrid_m.setHeader("Select,Session ID,Create Time,Connect String,Simulation");
					mygrid_m.setInitWidths("40,270,200,200,100");
					mygrid_m.setColAlign("center,left,left,left,center");
					mygrid_m.setColTypes("ch,ro,ro,ro,ro");
					mygrid_m.setColSorting("str,str,str,str,str");
					mygrid_m.setOnEditCellHandler(hndl_cell_edit);

					mygrid_m.init();
				}

				function reinitHistoryObject()
				{
					if (mygrid_m != null) {
						mygrid_m.destructor();
						mygrid_m = null;
					}

					initHistoryGridObject();
				}


				
				function do_broker_dialog()
				{

					//var colArr = new Array();
					//colArr[0] = 1;
					//var sids = getXGridSelectedRows(mygrid_m,0, colArr);
					var url = '/htBrokerDialog.jsp?operation=pass_sessions&generate_new_page_uid=true&cookie_broker_sessions_set=<%=SC.getUniquePageId() %>';

					//mForm.session_ids.value = sids;

					mForm.action = url;
					mForm.submit();

				}

				// this function saves checked run names to session var
				function save_session( session_val )
				{
					var url = "/HtCreateSetAsSessionVar.jsp?cookie=<%=SC.getUniquePageId() %>"+
					"&set_name=broker_sessions_set&operation=add_value&set_value="+encodeURIComponent(session_val);

					
					xmlHttp_m.open('POST', url, false);
					xmlHttp_m.send(null);


				}

				function clear_session( session_val )
				{

					var url = "/HtCreateSetAsSessionVar.jsp?cookie=<%=SC.getUniquePageId() %>"+
					"&set_name=broker_sessions_set&operation=remove_value&set_value="+encodeURIComponent(session_val);

					xmlHttp_m.open('POST', url, false);
					xmlHttp_m.send(null);

				}

				// on edit handler
				function hndl_cell_edit(stage,rowId,cellInd)
				{
						if (cellInd == 0) {
							if (stage==1) {
							// our check box
								var val_to_send= mygrid_m.cellById(rowId,0).getValue();
								var val_session= mygrid_m.cellById(rowId,1).getValue();
								if (val_to_send==1) {
									save_session(val_session);
								}
								else {
									clear_session(val_session);
								}

							}
						}


					return true;
				}



		</script>

	</head>
	<body id="bodyObj" onload="page_load();">

		<table width=100% height="100%" class=x8>

			<tr> <!-- control header begin -->
				<td>


					<table height="100%" width=100% class=x8>
						<tr>


							<td width="350px" style="border:1px solid black"> <!-- first column -->

								<table width="100%" class=x8 height=100%>

									<tr valign=top>
										<td align=left height=20px width=100%>
										</td>
										<td></td>
									</tr>

									<tr valign=top>
										<td align=left>
										</td>

										<td></td>
									</tr>




									<tr>
										<td>
										</td>
										<td>
										</td>
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
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td id="goClearObj" width="90px" align=left>
											<a class="xd" href="#" onclick="do_clear();">Clear</a>
										</td>
										<td></td>
									</tr>

									<tr>
										<td></td>
									</tr>

								</table>
							</td> <!-- third column -->

							<td>
							</td>

						</tr>
					</table>

				</td>
			</tr> <!-- control header end -->


			<tr height=30px>
				<td>
					<!--  navigation footer -->
					<div id="navDibObj" width="100%" height="100%">
					</div>
				</td>
			</tr>

			<tr height="100%" width=100%>
				<td align="center">

					<div id="sessionIdData_obj" style="width:100%;height:100%;border-style:solid; border-width:1px;"></div>

				</td>
			</tr>

			<tr width=100%>
				<td align="center">
					<table class=x8 width=100%>
						<tr>
							<td height=20px align=left>
								<a href='#' onclick="do_broker_dialog()" >Select sessions and return back</a>

							</td>
						</tr>
					</table>

				</td>
			</tr>


		</table>

		<!-- data grid-->


		<form method="post" action="" id="mForm" style="display:none">
			<input type=hidden name="session_ids" value=""></input>
		</form>

	</body>
</html>