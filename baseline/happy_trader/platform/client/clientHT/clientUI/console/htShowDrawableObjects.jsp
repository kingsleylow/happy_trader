<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtShowDrawableObjects" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
		SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<%
if (methodName.equalsIgnoreCase("GET")) {
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj'>View Drawable Objects</title>

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

		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>

		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>


		<script>

			var xmlhttp_m = getXmlHttp();
			
			var historyGrid_m = null;

			var navigationBar_m = null;

			var pricesXGrid_m = null;

			var drawTypes_m = <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "all_obj_types") %>;

			var symbolsForEachProfile_m = <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "symbols_for_each_profile")%>;

			var ajax_caller_m = new AjaxCaller(-1);

			function get_last_runname_for_drawable()
			{
				var profilename = profilNameObj.options(profilNameObj.selectedIndex).value;
				var url = "/htShowDrawableObjects.jsp?operation=get_last_runname&profilename=" + encodeURIComponent(profilename);
				
				ajax_caller_m.makeCall("POST", url, '',
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					if (ajax_caller_m.isResponseOk()) {
						fmain.runNameFilterObj.value = content;
					}
					else {
						document.body.innerHTML = wrapErrorToHTML(ajax_caller_m.createErrorString());
					}
				},

				function()
				{
					usrFun_SwitchOnLoadInProgress();
				}
				,
				function()
				{
					// stop modal
					usrFun_SwitchOffLoadInProgress();

				}

				);

			}

			function onclick_typeObj()
			{

				var selItems = new Array();
				// selected items
				for(i=0; i < fmain.typeObj.options.length;i++) {
					var id = new Number(fmain.typeObj.options.item(i).value);
					selItems[ id.toString() ] = 1;
				}

				// clear
				fmain.typeObj.length = 0;

				var param = new Array();
				param['comment_text'] = 'Select object types';
				param['items'] = drawTypes_m;
				param['selected_items'] = selItems;


				var result = window.showModalDialog(
				'htHelperCheckboxSelector.jsp',
				param,
				'resizable:0;dialogHeight=400px;dialogWidth=400px;help:0;status:0;scroll:0'
			);

				if (result != null) {
					for (var i in result)
					{
						var oOption = document.createElement("OPTION");
						oOption.text = result[i];
						oOption.value = i;

						fmain.typeObj.options.add(oOption);
					}

					if (result.length==0)
						addNoFilterType();


				}
				else {
					addNoFilterType();
				}

			}

			function addNoFilterType()
			{
				// all objects
				var oOption = document.createElement("OPTION");
				oOption.text = 'No Filter';
				oOption.value = -1;

				fmain.typeObj.options.add(oOption);
			}

			function  initialPopulateTypeObject()
			{
				fmain.typeObj.length = 0;
				for (var i in drawTypes_m) {
					var oOption = document.createElement("OPTION");
					oOption.text = drawTypes_m[i];
					oOption.value = i;

					fmain.typeObj.options.add(oOption);

				}
			}


			function create_prices_xgrid()
			{
				pricesXGrid_m = new dhtmlXGridObject("provSymbMap_divObj");


				pricesXGrid_m.setImagePath("js/grid3/codebase/imgs/");
				pricesXGrid_m.setHeader("Provider, Symbol, Checked");
				pricesXGrid_m.setInitWidths("140,120,60");
				pricesXGrid_m.setColTypes("ro,ro,ch");
				pricesXGrid_m.setColAlign("left,left,center");
				pricesXGrid_m.setColSorting("str,str,na");
				pricesXGrid_m.enableResizing("true,true,true");
				pricesXGrid_m.enableTooltips("false,false,false");

				pricesXGrid_m.init();

			}



			function on_change_profile()
			{
				pricesXGrid_m.clearAll(false);
				var profilename = profilNameObj.options(profilNameObj.selectedIndex).value;
				var provider_symbol = symbolsForEachProfile_m[profilename];

				if (provider_symbol != null && provider_symbol != undefined) {

					for(i = 0; i < provider_symbol.length/2; i++) {
						var provider_i = provider_symbol[i*2];
						var symbol_i = provider_symbol[i*2+1];

						var entry = new Array(provider_i,symbol_i,0);
						pricesXGrid_m.addRow(i+1, entry);

					}

				};

			}

			function prepareTypeFilterList()
			{
				var res = "";

				if (fmain.typeObj.options.length==1 && fmain.typeObj.options.item(0).value==-1) {
					// return
					return res;
				}
				for(i=0; i < fmain.typeObj.options.length;i++) {
					res += fmain.typeObj.options.item(i).value+",";
				}

				return res;

			}

			function prepareProviderSymbolList(is_url_encode)
			{
				var url = "";
				var rowNum = pricesXGrid_m.getRowsNum();

				for(i = 0; i < rowNum; i++ ) {
					var rowId = pricesXGrid_m.getRowId(i);

					cb_cell = pricesXGrid_m.cells(rowId, 2 );


					var cb_cell_value = cb_cell.getValue();

					if (cb_cell_value==1) {
						// checked
						var provider_i = pricesXGrid_m.cells(rowId, 0).getValue();
						var symbol_i = pricesXGrid_m.cells(rowId, 1).getValue();

						if (is_url_encode != undefined && is_url_encode)
							url += encodeURIComponent(provider_i)+","+encodeURIComponent(symbol_i)+",";
						else
							url += provider_i+","+symbol_i+",";

					} // if checked


				}



				return url;
			}

			function page_load()
			{
				setPageCaption(tltObj.innerText);

				initialPopulateTypeObject();

				create_prices_xgrid();
				on_change_profile();


				goClearObj.disabled=true;

				navigationBar_m = new NavigationBar(	'Drawable Objects', "navDibObj",		<%= SC.getPageSize()%>,			<%= SC.getNumberPagesToShow()%>,
				usrFun_generUrl,
				usrFun_Navigate,
				usrFun_SwitchOnLoadInProgress,
				usrFun_SwitchOffLoadInProgress		);


				navigationBar_m.disable();

			}

			function usrFun_generUrl(history_pageIndex, pageSize)
			{
				return null;
			}

			function ifAllSymbolsSelected()
			{

				var rowNum = pricesXGrid_m.getRowsNum();
				var selectedNum = 0;
				for(i = 0; i < rowNum; i++ ) {
					var rowId = pricesXGrid_m.getRowId(i);
					cb_cell = pricesXGrid_m.cells(rowId, 2 );
					var cb_cell_value = cb_cell.getValue();

					if (cb_cell_value==1) {
						selectedNum ++;
					}
				}

				return (selectedNum==rowNum);
			}

			function createUrl(history_pageIndex, pageSize)
			{
				var url = '/HtShowDrawableObjects_HistoryProvider.jsp?'+
					"bdate="+encodeURIComponent(fmain.f_date_b.value)+
					"&edate="+encodeURIComponent(fmain.f_date_e.value) +
					"&type="+prepareTypeFilterList()+
					"&short_text_filter="+encodeURIComponent(fmain.shortTextFilterObj.value) +
					"&long_text_filter="+encodeURIComponent(fmain.longTextFilterObj.value) +
					"&run_name_filter="+encodeURIComponent(fmain.runNameFilterObj.value) +
					"&profile="+ encodeURIComponent(profilNameObj.options.item(profilNameObj.selectedIndex).innerText) +
					"&read_page_num="+history_pageIndex+
					"&read_rows_per_page="+pageSize+
					"&page_uid=<%= SC.getUniquePageId() %>";

				return url;
			}

			function createUrl2(history_pageIndex, pageSize)
			{
				var url = '/HtShowDrawableObjects_CreateWLStructure.jsp?'+
					"bdate="+encodeURIComponent(fmain.f_date_b.value)+
					"&edate="+encodeURIComponent(fmain.f_date_e.value) +
					"&type="+prepareTypeFilterList()+
					"&short_text_filter="+encodeURIComponent(fmain.shortTextFilterObj.value) +
					"&long_text_filter="+encodeURIComponent(fmain.longTextFilterObj.value) +
					"&run_name_filter="+encodeURIComponent(fmain.runNameFilterObj.value) +
					"&profile="+ encodeURIComponent(profilNameObj.options.item(profilNameObj.selectedIndex).innerText) +
					"&read_page_num="+history_pageIndex+
					"&read_rows_per_page="+pageSize+
					"&page_uid=<%= SC.getUniquePageId() %>";

				return url;
			}

			function usrFun_Navigate(historyBaseUrl, history_pageIndex, pageSize)
			{
				// history_pageIndex, pageSize can be ignored or historyBaseUrl can be ignored
				//reinitHistoryObject();
				//historyGrid_m.loadXML(historyBaseUrl, check_error_after_history_load);

				var url = createUrl(history_pageIndex, pageSize);

				var data = "";
				if (ifAllSymbolsSelected()) {
					url += "&all_if_no_symb=true";
				}
				else {
					url += "&all_if_no_symb=false";
					data = "&ps=" + prepareProviderSymbolList(true);
				}

				makeAsynchXmlHttpRequestExternHandler(
				xmlhttp_m,
					-1,
				'POST',
				url,
				data,
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

						historyGrid_m.parse(loader.responseXML);
						dhtmlxError.catchError("ALL", null );

						navigationBar_m.callback_NavigationFinished( historyGrid_m.getRowsNum(), true );
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


			}; // end of fun


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
				xmlhttp_m,
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



			};


			function do_clear()
			{
				//


				profilNameObj.disabled=false;
				pricesXGrid_m.setEditable(true);


				fmain.f_trigger_b.disabled = false;
				fmain.f_trigger_e.disabled = false;
				fmain.f_date_b.disabled = false;
				fmain.f_date_e.disabled = false;

				fmain.longTextFilterObj.disabled = false;
				fmain.shortTextFilterObj.disabled = false;
				fmain.runNameFilterObj.disabled = false;

				fmain.typeObj.disabled = false;


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

				profilNameObj.disabled=true;
				pricesXGrid_m.setEditable(false);

				fmain.f_trigger_b.disabled = true;
				fmain.f_trigger_e.disabled = true;
				fmain.f_date_b.disabled = true;
				fmain.f_date_e.disabled = true;

				fmain.longTextFilterObj.disabled = true;
				fmain.shortTextFilterObj.disabled = true;
				fmain.runNameFilterObj.disabled = true;

				fmain.typeObj.disabled = true;


				// disable go
				goBtnObj.disabled=true;
				goClearObj.disabled=false;


				navigationBar_m.enable();
				navigationBar_m.first_page_history();
			}

			function initHistoryGridObject()
			{
				historyGrid_m = new dhtmlXGridObject("finData");


				historyGrid_m.setImagePath("js/grid3/codebase/imgs/");
				//historyGrid_m.setEditable(false);

				// 38


				historyGrid_m.setHeader("Run Name, Id, Provider, Symbol,Creation Time,Time, Type, Name, LineIndic_Name1, LineIndic_Color1, LineIndic_Value1, LineIndic_Name2, LineIndic_Color2, LineIndic_Value2, LineIndic_Name3, LineIndic_Color3, LineIndic_Value3, LineIndic_Name4, LineIndic_Color4, LineIndic_Value4, LineIndic_Name5, LineIndic_Color5, LineIndic_Value5, LineIndic_Name6, LineIndic_Color6, LineIndic_Value6, LineIndic_Name7, LineIndic_Color7, LineIndic_Value7, Print_Bid, Print_Ask, Print_Volume, Print_Color, Print_Operation, OHLC_Open, OHLC_High, OHLC_Low, OHLC_Close, OHLC_Volume, Text_Priority, Text_Short, Text_Content, Best_Bid_Price, Best_Ask_Price, Best_Bid_Volume, Best_Ask_Volume");
				historyGrid_m.setInitWidths("190,140,140,140,160,160,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,340, 140,140,140,140");
				historyGrid_m.setColTypes("txt,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,txt,ro,ro,ro,ro");
				historyGrid_m.setColAlign("left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left");
				historyGrid_m.setColSorting("na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na");
				historyGrid_m.enableResizing("true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true");
				historyGrid_m.enableTooltips("false,false,false,false, false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false");


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

			// check and unchek all symbols
			function check_all_symbols(to_check)
			{
				var rowNum = pricesXGrid_m.getRowsNum();
				for(i = 0; i < rowNum; i++ ) {
					cb_cell = pricesXGrid_m.cells( pricesXGrid_m.getRowId(i), 2 ).setValue(to_check);
				}
			}

			// this function helps to download all data
			// to see it in another application
			function do_download_xml()
			{

				var url = createUrl2(-1, -1);
				var data="";

				if (ifAllSymbolsSelected()) {
					url += "&all_if_no_symb=true";
				}
				else {
					url += "&all_if_no_symb=false";
					data = prepareProviderSymbolList(false);
				}


				mForm.target="_self";
				mForm.action = url;
				mForm.ps.value = data;

				mForm.submit();


			}

			function do_open_viewer()
			{
				window.open("/htWebStartHelper.jsp?application_name=HtCharting");
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

								<table id="PriceSelectorTable_Obj" width="100%" class=x8 >

									<tr>
										<td align=left >
											Data Profile
										</td>

										<td width=60% align=left>
											<select id = "profilNameObj" class="x4" style="width:100%" onchange="on_change_profile()">
												<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "all_profiles") %>
											</select>
										</td>

										<td></td>
									</tr>


									<tr>
										<td colspan=3 align=left width="400px">
											<div id="provSymbMap_divObj" style="width:100%;height:200px;border-style:solid; border-width:1px;">
											</div>
										</td>
									</tr>

								</table>

							</td> <!-- first column -->

							<td width=250px style="border:1px solid black" > <!-- second column -->
								<form action="#" method="get" id="fmain">
									<table width=100% height=100% valign=top class=x8>

										<tr valign=top>

											<td height="20px" align=left width=80px>
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
											<td height="20px" align=left width=80px>
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
											<td  height="20px" width="90px">
												<div align='left'>
													Object Type
												</div>
											</td>

											<td align=left height="20px">
												<table class="x8" width="100%">
													<tr>
														<td>
															<select id="typeObj" style="width:100%" class=x8">
															</select>
														</td>
														<td align="left" width="40px">
															<a href="#" onclick="onclick_typeObj();">Select</a>
														</td>
													</tr>
												</table>
											</td>

										</tr>


										<tr>
											<td  height="20px" width="90px">
												<div align='left'>
													Short Text
												</div>
											</td>

											<td align=left height="20px">
												<table class="x8" width="100%">
													<tr>
														<td>
															<input id="shortTextFilterObj" style="width:100%" type="text" value="" size="50" maxLength="50"></input>
														</td>

													</tr>
												</table>
											</td>

										</tr>

										<tr>
											<td  height="20px" width="90px">
												<div align='left'>
													Long Text
												</div>
											</td>

											<td align=left height="20px">
												<table class="x8" width="100%">
													<tr>
														<td>
															<input id="longTextFilterObj" style="width:100%" type="text" value="" size="50" maxLength="50"></input>
														</td>

													</tr>
												</table>
											</td>

										</tr>

										<tr>
											<td  height="20px" width="90px">
												<div align='left'>
													Run Name
												</div>
											</td>

											<td align=left height="20px">
												<table class="x8" width="100%">
													<tr>
														<td>
															<input id="runNameFilterObj" style="width:100%" type="text" value="" size="50" maxLength="50"></input>
														</td>

														<td width="20px">
															<a href="#" onclick="get_last_runname_for_drawable();">Last</a>
														</td>

													</tr>
												</table>
											</td>

										</tr>


										<tr>
											<td align="left" height="20px" width="90px">
												<a href="#" onclick="check_all_symbols(1);">Check all</a>
											</td>

											<td align="left">
												<a href="#" onclick="check_all_symbols(0);">Uncheck all</a>
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
										<td id="goBtnObj" width="180px" align=left>
											<a class="xd" href="#" onclick="do_go();">GO ></a>
										</td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td id="goClearObj" width="180px" align=left>
											<a class="xd" href="#" onclick="do_clear();">Clear</a>
										</td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
									</tr>

									<tr height=10px>
										<td id="goDownloadXML" width="180px" align=left>
											<a class="xd" href="#" onclick="do_open_viewer();">Open Viewer</a>
										</td>
										<td></td>
									</tr>

									<tr height=10px>
										<td width=10px></td>
										<td></td>
									</tr>


									<tr height=10px>
										<td id="goDownloadXML" width="180px" align=left>
											<a class="xd" href="#" onclick="do_download_xml();">Download XML for the Viewer</a>
										</td>
										<td></td>
									</tr>

									<tr>
										<td width=10px></td>
										<td></td>
									</tr>


								</table>
							</td> <!-- third column -->

							<td></td>

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



			<tr height="100%" width=100%>
				<td align="center">


					<div  style='height:100%; width:100%; border: 1px solid black'>
						<table  class=x8  width=100% height=100%>
							<tr>
								<td align="center">
									<div id="finData" style='height:100%; width:100%; '></div>
								</td>
							</tr>
						</table>

					</div>

				</td>
			</tr>

		</table>



	</body>
	<!-- form to get an external HTML-->
	<form method="post" action="" id="mForm" name="mForm" style="display:none">
		<input type=hidden name="ps" value=""></input>
	</form>

</html>
<%
}
%>