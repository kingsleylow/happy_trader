<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerRunNames" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
}
else if (methodName.equalsIgnoreCase("POST")) {

		SC.initialize_Post(request,response);
}
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj' >View Broker Runs</title>

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

		<!-- OLD DHTML grid -->
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		-->

		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>


		<!-- JQUERY -->
		<!--
		<script src="js/jtip/jquery.js" type="text/javascript"></script>
		<script src="js/jtip/jquery.hoverIntent.js" type="text/javascript"></script>
		<script src="js/jtip/jquery.cluetip.js" type="text/javascript"></script>
		<link rel="stylesheet" href="css/jtip/jquery.cluetip.css" type="text/css" />
		-->


		<script>

			var xmlhttp_g_m = getXmlHttp();

			var mygrid_m = null;

			var navigationBar_m = null;

			var xmlHttp_m = getXmlHttp();

			/*
			function update_cluetip()
			{
				$('div.basic').cluetip(

				{
					height: '200px',
					width:  '300px',
					ajaxCache: true,
					positionBy: 'auto',
					dropShadow:  false,
					arrows:      true,
					sticky: true,
					mouseOutClose: true,
					closePosition: 'title',
					closeText: '<img src="/imgs/jtip/cross.png" alt="close" />'

				}
			);
			}
			*/


			function page_load()
			{
				setPageCaption(tltObj.innerText);
				goClearObj.disabled=true;

				navigationBar_m = new NavigationBar(	'Run Names', "navDibObj",
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
					/*
					var url = '/HtBrokerRunNames_HistoryProvider.jsp?'+
						"bdate="+encodeURIComponent(fmain.f_date_b.value)+
						"&edate="+encodeURIComponent(fmain.f_date_e.value) +
						"&read_page_num="+history_pageIndex+
						"&read_rows_per_page="+pageSize+
						"&page_uid=<%= SC.getUniquePageId() %>";


					return url;
					 */
				
					return null;

					

				}

				function usrFun_Navigate(historyBaseUrl, history_pageIndex, pageSize)
				{
					//reinitHistoryObject();
					//mygrid_m.loadXML(historyBaseUrl, check_error_after_history_load);

						
					var url = '/HtBrokerRunNames_HistoryProvider.jsp?'+
						"bdate="+encodeURIComponent(fmain.f_date_b.value)+
						"&edate="+encodeURIComponent(fmain.f_date_e.value) +
						"&read_page_num="+history_pageIndex+
						"&read_rows_per_page="+pageSize+
						"&page_uid=<%= SC.getUniquePageId() %>";

				

					makeAsynchXmlHttpRequestExternHandler(
					xmlhttp_g_m,
						-1,
					'GET',
					url,
					null,
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							
							reinitHistoryObject();
							

							// parse
							dhtmlxError.catchError("ALL",
							function(type, desc, erData)
							{
								// error occured
								alert('Error occured of type: ' + type + "\ndescription: " + desc + "\nstatus: " + erData[0].status);
								check_error_after_history_load();
							}
						);

							mygrid_m.parse(loader.responseXML);
							dhtmlxError.catchError("ALL", null );

							navigationBar_m.callback_NavigationFinished( mygrid_m.getRowsNum(), true );
						}
						else {

							alert('No data arrived');
							navigationBar_m.callback_NavigationFinished( -1, false );
						}
					}	,

					function(errmsg)
					{
						alert('Internal error on loading data: '+errmsg);
						navigationBar_m.callback_NavigationFinished( -1, false );
					}
				);

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

					makeAsynchXmlHttpRequestExternHandler(
					xmlhttp_g_m,
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

							// all is ok
							navigationBar_m.callback_NavigationFinished( mygrid_m.getRowsNum(), true );
							update_cluetip();
						}
					}	,

					function(errmsg)
					{
						alert('Internal error on get error status: '+errmsg);
						navigationBar_m.callback_NavigationFinished( -1, false );

					}
				);

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
					mygrid_m = new dhtmlXGridObject('runIdData_obj');
					mygrid_m.setImagePath("js/grid3/codebase/imgs/");
					mygrid_m.setHeader("Select, Run Name, Operation Started, Comment, User Data, RUN_NAME");
					
					mygrid_m.setInitWidths("40,270,180,350,320,0");
					mygrid_m.setColAlign("center,left,left,left,left,left");
					mygrid_m.setColTypes("ch,ro,ro,ed,txt,ro");
					mygrid_m.setColSorting("str,str,str,str,str,str");
					mygrid_m.enableResizing("true,true,true,true,true, false");
					
					mygrid_m.attachEvent("onEditCell", hndl_cell_edit);
					mygrid_m.attachEvent("onRowSelect", hndl_row_select);

					mygrid_m.setColumnColor('window,window,window,window,yellow,window');




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


				function navigate_broker_detail_run_name(run_name)
				{


										
					var url = '/htBrokerDetailRunNames.jsp?operation=show_default_page&run_name_filter='+encodeURIComponent(run_name);
					window.navigate(url);


				}

				
				// opens modal with some details
				function open_info_window(run_name)
				{
					var url = '/htBrokerRunNames_showOperationsNumber.jsp?run_name='+encodeURIComponent(run_name);

					window.showModalDialog(
					url,
					'',
					'resizable:0;dialogHeight=200px;dialogWidth=300px;help:0;status:0;scroll:0' );
				}
				 

				function do_broker_dialog()	{
					var url = '/htBrokerDialog.jsp?operation=pass_runs&generate_new_page_uid=true&cookie_broker_runs_set=<%=SC.getUniquePageId() %>';

					mForm.action = url;
					mForm.submit();
				}

				// this function saves checked run names to session var
				function save_runname( runname )
				{
					var url = "/HtCreateSetAsSessionVar.jsp?cookie=<%=SC.getUniquePageId() %>"+
						"&set_name=broker_runs_set&operation=add_value&set_value="+encodeURIComponent(runname);

					xmlHttp_m.open('POST', url, false);
					xmlHttp_m.send(null);

				}

				function clear_runname( runname )
				{

					var url = "/HtCreateSetAsSessionVar.jsp?cookie=<%=SC.getUniquePageId() %>"+
						"&set_name=broker_runs_set&operation=remove_value&set_value="+encodeURIComponent(runname);

					xmlHttp_m.open('POST', url, false);
					xmlHttp_m.send(null);

				}

				// on edit handler

				function hndl_row_select(rowId,ind_cell)
				{
					if (ind_cell == 2) {
						var run_name = mygrid_m.cellById(rowId,5).getValue();
						open_info_window(run_name);
					}
				}

				function hndl_cell_edit(stage,rowId,cellInd,nValue,oValue)
				{
					if (cellInd == 0) {

						// check box
						if (stage==1) {
							// our check box
							var val_to_send= mygrid_m.cellById(rowId,0).getValue();
							var val_runname= mygrid_m.cellById(rowId,5).getValue();
							if (val_to_send==1) {
								save_runname(val_runname);
							}
							else {
								clear_runname(val_runname);
							}

						}
					}
					else if (cellInd == 4) {
						// user data
						if (stage==1) {
							mygrid_m.setUserData(rowId, "d", 1);
						}
					}

					return true;
				}


				function do_update_user_data()
				{
					var result = "";
					for(i = 0; i < mygrid_m.getRowsNum(); i++ ) {
						var rowId = mygrid_m.getRowId(i);
						var user_tag = mygrid_m.getUserData(rowId, "d");

						// user_data
						if (user_tag != null && user_tag==1) {

							var runname = mygrid_m.cells(rowId, 5).getValue();
							var user_data = mygrid_m.cells(rowId, 4).getValue();

							result += runname+","+user_data+",";
							mygrid_m.setUserData(rowId, "d", 0);

						}
					}

					//
					var url = '/htBrokerRunNames.jsp?operation=update_user_data&page_uid=<%=SC.getUniquePageId() %>';
					var total_data = "user_data="+encodeURIComponent(result);

					makeAsynchXmlHttpRequestExternHandler(
					xmlHttp_m,
						-1,
					'POST',
					url,
					total_data,
					function(loader)
					{

					},

					function(errmsg)
					{
						alert('Internal error on updating user data: '+errmsg);

					}
				);

				}

				function do_select_clear_user_data()
				{
					for(i = 0; i < mygrid_m.getRowsNum(); i++ ) {
						var rowId = mygrid_m.getRowId(i);
						mygrid_m.setUserData(rowId, "d", 1);

						mygrid_m.cells(rowId, 4).setValue('');

					}
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
					<div id="runIdData_obj" style="width:100%;height:100%;border-style:solid; border-width:1px;"></div>
				</td>
			</tr>

			<tr width=100%>
				<td align="center">
					<table class=x8 width=100%>
						<tr>
							<td height=20px align=left width="200px">
								<a href='#' onclick="do_broker_dialog()" >Select runs and return back</a>
							</td>

							<td height=20px align=left width="200px">
								<a href='#' onclick="do_update_user_data();" >Save User Data</a>
							</td>

							<td height=20px align=left width="200px">
								<a href='#' onclick="do_select_clear_user_data();" >Clear All User Data</a>
							</td>

							<td height=20px align=left>

							</td>

						</tr>

					</table>

				</td>
			</tr>


		</table>

		<!-- data grid-->


		<form method="post" action="" id="mForm" style="display:none">
			<input type=hidden name="run_ids" value=""></input>
		</form>



	</body>
</html>