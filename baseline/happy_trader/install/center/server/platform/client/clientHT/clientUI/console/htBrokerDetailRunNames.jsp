<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerDetailRunNames" scope="request"/>
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

		<title id='tltObj' >View Session Operations</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

		<!-- helper library -->
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

			var historyGrid_m = null;

			var navigationBar_m = null;

			function page_load()
			{
				setPageCaption(tltObj.innerText);


				goClearObj.disabled=true;

				navigationBar_m = new NavigationBar(	'Session Operations', "navDibObj",		<%= SC.getPageSize()%>,			<%= SC.getNumberPagesToShow()%>,
				usrFun_generUrl,
				usrFun_Navigate,
				usrFun_SwitchOnLoadInProgress,
				usrFun_SwitchOffLoadInProgress		);


				navigationBar_m.disable();

				// whether to show default
				if ('<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "operation")%>'=='show_default_page') {
					//set up default filter
					runNameFilterObj.value = "<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "run_name_filter")%>";

					do_go();
				}


			}


			//////////////////////////////////////////////////////////////////////////
			function usrFun_generUrl(history_pageIndex, pageSize)
			{
				var url = '/HtBrokerDetailRunNames_HistoryProvider.jsp?'+
					"bdate="+encodeURIComponent(fmain.f_date_b.value)+
					"&edate="+encodeURIComponent(fmain.f_date_e.value) +
					"&run_name_filter="+encodeURIComponent(runNameFilterObj.value)+
					"&read_page_num="+history_pageIndex+
					"&read_rows_per_page="+pageSize+
					"&page_uid=<%= SC.getUniquePageId() %>";

				return url;

			}

			function usrFun_Navigate(historyBaseUrl)
			{
				reinitHistoryObject();
				historyGrid_m.loadXML(historyBaseUrl, check_error_after_history_load);
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

						navigationBar_m.callback_NavigationFinished( historyGrid_m.getRowsNum(), true );
					}
				}	,

				function(errmsg)
				{
					alert('Internal error on get error status: '+errmsg);
					navigationBar_m.callback_NavigationFinished( -1, false );

				}
			);



			}


			function do_clear()
			{
				//


				fmain.f_trigger_b.disabled = false;
				fmain.f_trigger_e.disabled = false;
				fmain.f_date_b.disabled = false;
				fmain.f_date_e.disabled = false;


				runNameFilterObj.disabled = false;

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


				runNameFilterObj.disabled = true;


				// disable go
				goBtnObj.disabled=true;
				goClearObj.disabled=false;


				navigationBar_m.enable();
				navigationBar_m.first_page_history();
			}





			function initHistoryGridObject()
			{
				historyGrid_m = new dhtmlXGridObject("dataTblObj");

				historyGrid_m.imgURL = "imgs/grid/";
				historyGrid_m.setHeader("Session ID,Run Name,Algorithm Broker Pair,Thread,Operation Time,Operation");

				historyGrid_m.setInitWidths("270,270,150,80,150,150");
				historyGrid_m.setColAlign("left,left,left,left,left,left");
				historyGrid_m.setColTypes("ro,ro,ro,ro,ro,ro");
				historyGrid_m.setColSorting("str,str,str,str,str,str");

				historyGrid_m.init();

				historyGrid_m.init();

			}

			function reinitHistoryObject()
			{
				if (historyGrid_m != null) {
					historyGrid_m.destructor();
					historyGrid_m = null;
				}

				initHistoryGridObject();
			}

		</script>

	</head>
	<body id="bodyObj" onload="page_load();">

		<table width=100% height="100%" class=x8>

			<tr>
				<td>


					<table height="100%" width=100% class=x8>
						<tr>


							<td width="350px" style="border:1px solid black">

								<table height=100% width="100%" class=x8 >



									<tr valign=top>
										<td align=left height=20px width=100%>
											Run Name
										</td>
										<td></td>
									</tr>

									<tr valign=top>
										<td align=left>
											<input style="width:100%"  id="runNameFilterObj" type="text" value="" ></input>
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

							</td>


							<td width=250px style="border:1px solid black" >
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
											<td width="90px">
											</td>

											<td align=left>
											</td>
										</tr>

										<tr>
											<td width="90px">
											</td>

											<td align=left>
											</td>
										</tr>



									</table>
								</form>
							</td>

							<td>
								<table class=x8 width=100% height=100% valign=top>
									<tr height=10px>
										<td id="goBtnObj" width="110px" align=left>
											<a class="xd" href="#" onclick="do_go();">GO ></a>
										</td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=110px></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td id="goClearObj" width="110px" align=left>
											<a class="xd" href="#" onclick="do_clear();">Clear</a>
										</td>
										<td></td>
									</tr>

																	
									<tr>
										<td width=110px></td>
										<td></td>
									</tr>


								</table>
							</td>

							<td>
							</td>


						</tr>
					</table>

				</td>
			</tr> <!-- control header end -->



			<!--  begin navigation footer -->
			<tr height=30px>
				<td>
					<div id="navDibObj" width="100%" height="100%">
					</div>
				</td>
			</tr>
			<!--  end navigation footer -->

			<!--  data2 -->

			<tr height="100%" width=100%>
				<td align="center">


					<div  style='height:100%; width:100%; border: 1px solid black'>
						<table  class=x8  width=100% height=100%>
							<tr>
								<td align="center">
									<div id="dataTblObj" style='height:100%; width:100%; '></div>
								</td>
							</tr>
						</table>

					</div>

				</td>
			</tr>


			<!--  data2 -->


		</table>




	</body>
</html>