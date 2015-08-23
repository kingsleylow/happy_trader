<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerRunNames_v2" scope="request"/>
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

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

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
				<script type="text/javascript" src="js/misc/ajax_utils.js"></script>

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
			
			

						//var xmlHttp_m = getXmlHttp();

		


						function page_load()
						{
							setPageCaption(tltObj.innerText);
							goClearObj.disabled=true;
				
							initHistoryGridObject();

						}
			
						function get_selected_session_ids()
						{
							// iterate through all table and collect sessions IDS
							var res = new Array();
							if (mygrid_m==null)
								return res;
				
							var nums = mygrid_m.getRowsNum();
				
							for(i = 0; i < nums; i++) {
								var rowId = mygrid_m.getRowId(i);
								var state = mygrid_m.cellById(rowId, 0).getValue();
					
					
								if (state != 0) {
									var session_i = mygrid_m.cellById(rowId, 3).getValue();
									res.push(session_i);
						
								}
							}
				
							return res;   
						}


						function do_navigate()
						{
							//reinitHistoryObject();
							//mygrid_m.loadXML(historyBaseUrl, check_error_after_history_load);

						
							var url = '/HtBrokerRunNames_v2_HistoryProvider.jsp?'+
								"bdate="+encodeURIComponent(fmain.f_date_b.value)+
								"&edate="+encodeURIComponent(fmain.f_date_e.value) +
								"&page_uid=<%= SC.getUniquePageId() %>";

				

							makeAsynchXmlHttpRequestExternHandler(
							xmlhttp_g_m,
								-1,
							'GET',
							url,
							null,
							function(loader)
							{
								usrFun_SwitchOffLoadInProgress();
						
								if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							
									//reinitHistoryObject();
							
						
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

							
								}
								else {

									alert('No data arrived');
							
								}
							}	,

							function(errmsg)
							{
								alert('Internal error on loading data: '+errmsg);
						
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
							
									document.body.innerHTML = extractErrorMessage(loader.responseXML);
								}
								else {

									// all is ok
							
								}
							}	,

							function(errmsg)
							{
								alert('Internal error on get error status: '+errmsg);
						

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

					
							if (mygrid_m != null) {
								mygrid_m.clearAll();
							}
					

							//reinitHistoryObject();
					
			


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
					
							usrFun_SwitchOnLoadInProgress();


							do_navigate();
			
						}

				


						function initHistoryGridObject()
						{
							mygrid_m = new dhtmlXGridObject('runIdData_obj');
							mygrid_m.setImagePath("js/grid3/codebase/imgs/");
							mygrid_m.setHeader("Check,Run Name,Operation Start Time,Session Id,Order Count,Response Count,Position Record Count");
					
							mygrid_m.setInitWidths("40,260,150,240,110,110,140");
							mygrid_m.setColAlign("center,left,left,left,left,left,left");
							mygrid_m.setColTypes("ch,ro,ro,ro,ro,ro,ro");
							mygrid_m.setColSorting("str,str,str,str,str,str,str");
							mygrid_m.enableResizing("false,true,true,true,true,true,true");
					
							mygrid_m.attachEvent("onCheck", 
							function(rId,cInd,state)
							{
								var rowNum = mygrid_m.getRowsNum();
							
								// check is we have runName
								var runName = mygrid_m.cellById(rId, 1).getValue();
							
								if (runName != null && runName.length > 0) {
									// valid run name
									var idx_rId = mygrid_m.getRowIndex(rId);
								
									var next_idx = idx_rId + 1;
									while(true) {
								
								
										// exit if out of bound
										if (next_idx >= rowNum)
											break;
									
										var next_row_id = mygrid_m.getRowId(next_idx);
										if (next_row_id < 0)
											break;
									
										var	runName_i = mygrid_m.cellById(next_row_id, 1).getValue();
									
										if (runName_i != null && runName_i.length > 0) {
											break;
										} else {
											mygrid_m.cellById(next_row_id, cInd).setValue(state);
										}
									
										// advance
										next_idx++;
									}
								}
							
								//alert('hi');
							}
						);
					
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
				
						function launch_trade_report()
						{
							var launchform = document.getElementById("mForm");
							var sessions =  get_selected_session_ids();
					
							var data = AjaxHelperObject.assosiativeArrayToXmlParameter(sessions);
							launchform.data.value = data;
					
							var url = '/htBrokerRunNames_v2_TradeReport.jsp?operation=read_report&page_uid=<%=SC.getUniquePageId() %>';
							launchform.action = url;
							launchform.target="_blank";
					
							launchform.submit();
					
						}
				
						function launch_broker_dialog()
						{
							var launchform = document.getElementById("mForm");
							var sessions =  get_selected_session_ids();
					
							var data = AjaxHelperObject.assosiativeArrayToXmlParameter(sessions);
							launchform.data.value = data;
					
							var url = '/htBrokerRunNames_v2_BrokerDialog.jsp?operation=show_broker_dialog&page_uid=<%=SC.getUniquePageId() %>';
							launchform.action = url;
							launchform.target="_blank";
					
							launchform.submit();
					
						}


				

					</script>
					<style type="text/css">


						html,body{
							height:100%;
							margin:0px;
							padding:0px
						}
					</style>
					</head>
					<body id="bodyObj" onload="page_load();">

						<table width=100% height="100%">

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


							<tr height=20px bgcolor="buttonface">
								<td>

								</td>
							</tr>

							<tr height="100%" width=100%>
								<td style='position:relative'>

									<div id="runIdData_obj" style='width:99%;height:99%;border: 1px solid black;position:absolute;top:0px'></div>


								</td>
							</tr>

							<tr >
								<td height="20px" bgcolor="buttonface">
									<table width=100% class =x8>
										<tr>
											<td class="x3" align=left width="80px">
												<a href="#" onclick="launch_trade_report();">Trade Report</a>
											</td>

											<td class="x3" align=left width="120px">
												<a href="#" onclick="launch_broker_dialog();">Broker Dialog</a>
											</td>

											<td>
											</td>

											<td>
											</td>	

										</tr>
									</table>
								</td>
							</tr>



						</table>

						<form method="post" action="" id="mForm" style="display:none">
							<input type=hidden name="data" value=""></input>
						</form>	

					</body>
					</html>