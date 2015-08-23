<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtStartStopServer" scope="request"/>
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

		<title id='tltObj'>Start Stop Trading Servers</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />



		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>


		<!-- main calendar program -->
		<script type="text/javascript" src="js/calendar.js"></script>

		<!-- language for the calendar -->
		<script type="text/javascript" src="js/lang/calendar-en.js"></script>

		<!-- the following script defines the Calendar.setup helper function, which makes
			 adding a calendar a matter of 1 or 2 lines of code. -->
		<script type="text/javascript" src="js/calendar-setup.js"></script>

		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>


		<script type="text/javascript">

			var xGrid_srv_hist_data_m = null;
			var xGrid_srv_list_m = null;
			var xGrid_srv_providers_m = null;

			var date_beg_m = "";
			var date_end_m = "";

			// symbols for each profile
			var symbolsForEachProfile_m = <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "symbols_for_each_profile")%>;



			// this function raps our data to Java
			function createProfileProviderSymbolMap()
			{
				var out = '<?xml version="1.0" encoding="ISO-8859-1"?>';
				out += '<ent name="">';
				out += '<xmlparlist name="history_data">';


				var selectedProfiles = getSelectedProfiles();
				for(var profile_i in selectedProfiles)
				{

					out += '<ent name="'+profile_i+'">';



					var profsymblist_arr = symbolsForEachProfile_m[profile_i];
					out += '<par name="profile_values" type="4104">';

					if (profsymblist_arr != null ) {


						// simple array
						var i = 0;
						while(i < profsymblist_arr.length) {

							out += '<val>';
							out += profsymblist_arr[i++];
							out += '</val>';

						}

					}

					out += '</par>';

					out += '</ent>';

				}


				out += '</xmlparlist>';
				out += '</ent>';


				return out;
			}

			function getSelectedProfiles()
			{
				var result = new Array();
				var rowNum = xGrid_srv_hist_data_m.getRowsNum();
				for(i = 0; i < rowNum; i++ ) {
					var rowId = xGrid_srv_hist_data_m.getRowId(i);
					var cb_cell_value = xGrid_srv_hist_data_m.cellById(rowId, 0).getValue();

					if (cb_cell_value != null && cb_cell_value==1) {
						result[ xGrid_srv_hist_data_m.cellById(rowId, 1).getValue() ] = 1;
					}

				};

				return result;
			}

			function getXGridSelectedRows_local(xGrid, cbId, colArr )
			{
				var result="";
				var rowNum = xGrid.getRowsNum();


				for(i = 0; i < rowNum; i++ ) {
					var rowId = xGrid.getRowId(i);


					var cb_cell = xGrid.cellById(rowId, cbId);
					var cb_cell_value = cb_cell.getValue();


					if (cb_cell_value != null && cb_cell_value==1) {

						// checked
						for(k = 0; k < colArr.length;k++) {
							if (colArr[k] >= 0) {
								var value = xGrid.cellById(rowId, colArr[k]).getValue();
								result += value + ",";
							}
						}
					}


				}



				return result;
			};

			function on_load()
			{
				setPageCaption(tltObj.innerText);
				adjust_period_selection();

				// need to set up to avoid problems with checkboxes
				window.dhx_globalImgPath="js/grid3/codebase/imgs/";

				// server list
				xGrid_srv_list_m = new dhtmlXGridFromTable('srvList_TableObj_Tbl');
				xGrid_srv_list_m.setImagePath("js/grid3/codebase/imgs/");


				//
				xGrid_srv_hist_data_m = new dhtmlXGridFromTable('ProvSymbList_TableObj_Tbl');
				xGrid_srv_hist_data_m.setImagePath("js/grid3/codebase/imgs/");

				// RT Providers to subscrive
				xGrid_srv_providers_m = new dhtmlXGridFromTable('SubscribeProviders_TableObj_Tbl');
				xGrid_srv_providers_m.setImagePath("js/grid3/codebase/imgs/");


				// adjust manually selected lists
				adjustSelectionInHistoryTableForAllProfiles();
				adjustItemsNumberForProfile();

			}

			function do_start_server(server)
			{

				var url = "/htStartStopServer.jsp?operation=start_server&server_id="+encodeURIComponent(server);

				// pass parameters
				url += "&use_cached_data="+reuseCachedDataObj.checked;
				var val = periodSelectionObj.options(periodSelectionObj.selectedIndex).value;
				url += "&period_value="+periodSelectionObj.selectedIndex;

				var no_select_at_all = 'false';
				if (val=='s1') {

					date_beg_m = f_date_b.value;
				}
				else if (val=='s2')
				{
					date_end_m = f_date_e.value;
				}
				else if (val=='s12')
				{
					date_beg_m = f_date_b.value;
					date_end_m = f_date_e.value;
				}
				else if (val=='no_select')
				{
					no_select_at_all = 'true';
				}

				url+="&no_select_at_all="+no_select_at_all;
				url+="&load_data_dbeg="+encodeURIComponent(date_beg_m);
				url+="&load_data_dend="+encodeURIComponent(date_end_m);

				// is launch process
				url+= "&do_launch_process=" + isLaunchProcessObj.checked;
				url+= '&page_uid=<%=SC.getUniquePageId()%>';

				mForm_LaunchServer.action = url;
				mForm_LaunchServer.subscribed_providers.value = getXGridSelectedRows_local(xGrid_srv_providers_m,0,new Array(-1,1));
				//mForm_LaunchServer.historical_data.value = getXGridSelectedRows_local(xGrid_srv_hist_data_m,0, new Array(-1,1));

				mForm_LaunchServer.historical_data.value = createProfileProviderSymbolMap();
				//alert(mForm_LaunchServer.historical_data.value);
				mForm_LaunchServer.submit();
			}

			function do_shutdown_server(server)
			{
				if (window.confirm('Are you sure to normally shutdown the trading server process?')==true) {
					var url = "/htStartStopServer.jsp?operation=shutdown_server&server_id="+encodeURIComponent(server);
					url += '&page_uid=<%=SC.getUniquePageId()%>';

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_server_info(server)
			{
				window.open('htStartStopServer_serverInfo.jsp?server_id='+encodeURIComponent(server));
			}

			function do_show_internal_log(server)
			{
				window.open('HtReadFile.jsp?target_read=cpp_server&server_id='+encodeURIComponent(server), '_blank');
			}

			function do_kill_server(server)
			{
				if (window.confirm('Are you sure to kill the trading server process?')==true) {
					var url = "/htStartStopServer.jsp?operation=kill_server&server_id="+encodeURIComponent(server);
					url += '&page_uid=<%=SC.getUniquePageId()%>';

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_refresh()
			{
				var url = "/htStartStopServer.jsp?operation=refresh";
				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}





			function adjust_period_selection()
			{
				var val = periodSelectionObj.options(periodSelectionObj.selectedIndex).value;

				hide_date_time();

				date_beg_m = "";
				date_end_m = "";

				var now = new Date();


				if (val=='1d') {
					// subtrcat 1 day from now
					var mbeg = now.getMilliseconds() - 1*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);

				}
				else if (val=='3d') {
					var mbeg = now.getMilliseconds() - 3*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1w') {
					// substruct one 7 days
					var mbeg = now.getMilliseconds() - 7*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1m') {
					//subtruct one month
					var mbeg_date = new Date();
					var month = mbeg_date.getMonth();

					month--;
					if (month < 0) {
						var year = mbeg_date.getYear();
						year--;
						mbeg_date.setYear(year);
						mbeg_date.setMonth(0);
					}
					else {
						mbeg_date.setMonth(month);
					}

					date_beg_m = dateToString(mbeg_date);

				}
				else if (val=='6m') {
					var mbeg_date = new Date();
					var month = mbeg_date.getMonth();

					month = month - 6;
					if (month < 0) {
						var year = mbeg_date.getYear();
						year--;

						mbeg_date.setYear(year);
						mbeg_date.setMonth(11+month);
					}
					else {
						mbeg_date.setMonth(month);
					}

					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1y') {
					var mbeg_date = new Date();
					var year = mbeg_date.getYear();
					year--;
					mbeg_date.setYear(year);

					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='whole') {
					// implicitely it is assumed - just pass empty strings
				}
				else if (val=='s1') {
					show_first_date_time();


				}
				else if (val=='s2') {
					show_second_date_time();


				}
				else if (val=='s12') {
					show_both_date_time();

				}

				//alert(date_beg_m + ' - ' + date_end_m);


			}

			function hide_date_time()
			{
				//dateTimeObj.style.display='none';

				f_date_e.disabled=true;
				f_trigger_e.disabled=true;

				f_date_b.disabled=true;
				f_trigger_b.disabled=true;

				date_obj_e1.style.background = 'gray';
				date_obj_e2.style.background = 'gray';

				date_obj_b1.style.background = 'gray';
				date_obj_b2.style.background = 'gray';

			}

			function show_first_date_time()
			{
				//dateTimeObj.style.display='inline';

				f_date_e.disabled=true;
				f_trigger_e.disabled=true;

				f_date_b.disabled=false;
				f_trigger_b.disabled=false;

				date_obj_e1.style.background = 'gray';
				date_obj_e2.style.background = 'gray';

				date_obj_b1.style.background = '';
				date_obj_b2.style.background = '';
			}

			function show_second_date_time()
			{
				//dateTimeObj.style.display='inline';

				f_date_e.disabled=false;
				f_trigger_e.disabled=false;

				f_date_b.disabled=true;
				f_trigger_b.disabled=true;

				date_obj_e1.style.background = '';
				date_obj_e2.style.background = '';

				date_obj_b1.style.background = 'gray';
				date_obj_b2.style.background = 'gray';

			}

			function show_both_date_time()
			{
				//dateTimeObj.style.display='inline';

				f_date_e.disabled=false;
				f_trigger_e.disabled=false;

				f_date_b.disabled=false;
				f_trigger_b.disabled=false;

				date_obj_e1.style.background = '';
				date_obj_e2.style.background = '';

				date_obj_b1.style.background = '';
				date_obj_b2.style.background = '';
			}

			function do_log_event_level(server)
			{
				var outparams = window.showModalDialog(
				'htStartStopServer_AllowedEvents.jsp?server_id='+
					encodeURIComponent(server),
				"", 'resizable:0;dialogHeight=350px;dialogWidth=500px;help:0;status:0;scroll:0');


				if (outparams==null)
					return;

				var log_level = outparams['log_level'];
				var subscr_events = outparams['subscr_events'];

				var url = "/htStartStopServer.jsp?operation=do_reload_log_events";
				url += "&server_id="+encodeURIComponent(server);
				url += "&new_log_level="+encodeURIComponent(log_level);
				url += "&new_subscr_events=";
				for(i=0; i < subscr_events.length; i++) {
					url+=subscr_events[i] + ",";
				}

				url += '&page_uid=<%=SC.getUniquePageId()%>';


				mForm.action = url;
				mForm.submit();

			}


			// this function calls dilog to select symbols
			function do_select_symbols(profile)
			{
				var input = new Array();
				input['profile_name'] = profile;

				// local map

				var provSymbols = symbolsForEachProfile_m[profile]
				input['profile_data'] = provSymbols;

				if (provSymbols==null || provSymbols==undefined)
				{
					input['force_read'] = true;
					var outparams = window.showModalDialog(
					'htStartStopServer_SelectSymbolsForProfile.jsp?operation=do_read_for_profile&profile_id='+encodeURIComponent(profile),
					input, 'resizable:1;dialogHeight=350px;dialogWidth=500px;help:0;status:0;scroll:0');
				}
				else
				{
					input['force_read'] = false;
					var outparams = window.showModalDialog(
					'htStartStopServer_SelectSymbolsForProfile.jsp',
					input, 'resizable:1;dialogHeight=350px;dialogWidth=500px;help:0;status:0;scroll:0');
				}


				if (outparams != null)
				{
					var outparams_data = outparams['profile_data_out'];

					if (outparams_data==null || outparams_data==undefined) {
						symbolsForEachProfile_m[profile] = null;
						selectRowInHistoryTable(profile, 'window');
						adjustItemsNumberForProfile( profile );
						return;
					}

					if (outparams_data.length <=0)
						return;


					// append to another map
					var profileArray = new Array();
					var csvArray = CSVToArray(outparams_data);

					// simple array
					for(i = 0; i < csvArray.length; i++) {
						var arr1 = csvArray[i];
						if (arr1.length != 2) {
							alert('The rows must be in the format: provider,symbol');
							return;
						}

						for(j = 0; j < arr1.length; j++) {
							// trim
							profileArray.push( arr1[j].replace(/^\s*/, "").replace(/\s*$/, "") );
						}
					}



					symbolsForEachProfile_m[profile] = profileArray;
					selectRowInHistoryTable(profile, 'buttonface');

				}

				adjustItemsNumberForProfile( profile );
			}

			// used in onload
			function adjustSelectionInHistoryTableForAllProfiles()
			{

				for(var profile_i in symbolsForEachProfile_m ) {
					if (symbolsForEachProfile_m[profile_i]==null || symbolsForEachProfile_m[profile_i]==undefined)	{
						selectRowInHistoryTable(profile_i, 'window');
					} else {
						selectRowInHistoryTable(profile_i, 'buttonface');
					}
				}

			}

			//



			

			function adjustItemsNumberForProfile(profile)
			{
				for(i = 0; i < xGrid_srv_hist_data_m.getRowsNum(); i++ )
				{
					var rowId = xGrid_srv_hist_data_m.getRowId(i);

					var profile_i = xGrid_srv_hist_data_m.cellById(rowId, 1).getValue();
					if (profile != undefined ) {
						if (profile_i.toString() != profile.toString())
							continue;
					}
					var itemList = symbolsForEachProfile_m[profile_i];

					// then we need to update the column with the number of items
					var itemList = symbolsForEachProfile_m[profile_i];
					var url_s = "";
					if (itemList == null || itemList==undefined) {
						url_s = "<a href='#' onclick='do_select_symbols(\"" + profile_i + "\")'>"+"DEFAULT"+"</a>";

					} else {
						var ilength = itemList.length / 2;
						url_s = "<a href='#' onclick='do_select_symbols(\"" + profile_i + "\")'>"+"SELECTED "+ilength+ " ITEM(S)"+"</a>";
					}

					xGrid_srv_hist_data_m.cellById(rowId, 2).setValue(url_s);
				}
			}
			
			function selectRowInHistoryTable(profile, colorval)
			{
				// find
				for(i = 0; i < xGrid_srv_hist_data_m.getRowsNum(); i++ ) {
					var rowId = xGrid_srv_hist_data_m.getRowId(i);

					var cb_cell_value = xGrid_srv_hist_data_m.cellById(rowId, 1).getValue();
					if (cb_cell_value.toString() == profile.toString()) {
						// change the backgorund
						for(var k = 0; k < xGrid_srv_hist_data_m.getColumnsNum(); k++ ) {
							xGrid_srv_hist_data_m.cellById(rowId, k).setBgColor(colorval);
						}
					}
				}
			}

			function do_memory_cache_management(server_id)
			{
				window.open('htStartStopServer_MemoryCacheManagement.jsp?server_id='+encodeURIComponent(server_id));
			}


		</script>
	</head>

	<body onload='on_load();'>

		<table class="x8" width="100%" >
			<tr>
				<td>

					<table width=100% class=x8>
						<tr>
							<td align="left">
								Available Trading Servers
							</td>
						</tr>

						<tr>
							<td style='border: 1px solid black' align="center">
								<table  id='srvList_TableObj_Tbl' width='100%' height='170px' lightnavigation='false'>
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "server_list") %>
								</table>

							</td>
						</tr>
					</table>

				</td>
			</tr>

			<tr>
				<td height=20px>
				</td>
			</tr>

			<tr>
				<td height=20px>
					<table width=100% class=x8>

						<tr>
							<td align=left>

								<table height="100%" width="100%" class="x8">
									<tr>
										<td align="left" width="200px">
											Available history data to load
										</td>


										<td>
										</td>
									</tr>
								</table>

							</td>

							<td align=left>
								RT Providers to Subscribe
							</td>

						</tr>

						<tr>
							<td width="70%" style='border: 1px solid black' align="center">

								<table  id='ProvSymbList_TableObj_Tbl' gridWidth='100%' gridHeight='170px' lightnavigation='false' class='x8'>
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "server_data_history_table") %>
								</table>

							</td>

							<td style='border: 1px solid black' align="center">
								<table id='SubscribeProviders_TableObj_Tbl' gridWidth='100%' gridHeight='170px' lightnavigation='false'>
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "server_rt_providers") %>
								</table>
							</td>

						</tr>
					</table>
				</td>
			</tr>



			<tr>
				<td height=20px>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<input checked=true type=checkbox id="isLaunchProcessObj">Launch process</input>
					<script>
	<%
			if (SC.getStringSessionValue(SC.getUniquePageId(), request, "is_launch_process").length() > 0 ) {
			if (SC.getStringSessionValue(SC.getUniquePageId(), request, "is_launch_process").equalsIgnoreCase("true")) {
	%>
		isLaunchProcessObj.checked = true;
	<%
			} else {
	%>
		isLaunchProcessObj.checked = false;
	<%
			};
			};
	%>
					</script>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					Select date time period to load
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<select id="periodSelectionObj" onchange="adjust_period_selection();">
						<option value="1d">1 day before</option>
						<option value="3d">3 days before</option>
						<option value="1w">1 week before</option>
						<option value="1m">1 month before</option>
						<option value="6m">6 months before</option>
						<option value="1y">1 year before</option>
						<option value="whole">Whole History Provider period</option>
						<option value="s1">Select first date</option>
						<option value="s2">Select last date</option>
						<option value="s12">Select both dates</option>
						<option value="no_select">Use only cached data</option>
					</select>

					<script>
			<%
		if (SC.getStringSessionValue(SC.getUniquePageId(), request, "period_value_index").length() > 0 ) {
			%>

				periodSelectionObj.selectedIndex = <%=SC.getStringSessionValue(SC.getUniquePageId(), request, "period_value_index")%>;
			<%
		}
			%>


					</script>

				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<table id="dateTimeObj" width=100% class=x8>
						<tr>
							<td align=left width=80px>
								Start Date
							</td>

							<td>
								<table width=100% cellspacing="1" cellpadding="1" style="border-collapse: collapse;">
									<tr>
										<td width=120px id="date_obj_b1">
											<input  style="width: 100%" class=x8 type="text" name="date" id="f_date_b" />
										</td>

										<td width="10px" id="date_obj_b2">
											<img  src="imgs/img.gif" id="f_trigger_b" style="cursor: pointer; border: 1px solid red;" title="Date selector"
								 onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
										</td>

										<td>
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

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=80px>
								End Date
							</td>

							<td>

								<table  width=100% cellspacing="1" cellpadding="1" style="border-collapse: collapse;">
									<tr>
										<td width=120px id="date_obj_e1">
											<input  style="width: 100%" class=x8 type="text" name="date" id="f_date_e" />
										</td>

										<td width="10px" id="date_obj_e2">
											<img  src="imgs/img.gif" id="f_trigger_e" style="cursor: pointer; border: 1px solid red;" title="Date selector"
								 onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
										</td>

										<td>
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

							<td>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<input type=checkbox id="reuseCachedDataObj">Do not remove cache after a server sutdown</input>
					<script>
	<%
			if (SC.getStringSessionValue(SC.getUniquePageId(), request, "is_reuse_cached_data").length() > 0 ) {
			if (SC.getStringSessionValue(SC.getUniquePageId(), request, "is_reuse_cached_data").equalsIgnoreCase("true")) {
	%>
		reuseCachedDataObj.checked = true;
	<%
			} else {
	%>
		reuseCachedDataObj.checked = false;
	<%
			};
			}
	%>
					</script>
				</td>
			</tr>

			<tr>
				<td height=20px>
					<a href="#" onclick="do_refresh()">Refresh</a>
				</td>
			</tr>


		</table>


		<form method="post" action="" id="mForm" style="display:none">
		</form>

		<form method="post" action="" id="mForm_LaunchServer" style="display:none">
			<input type=hidden name="historical_data" value=""></input>
			<input type=hidden name="subscribed_providers" value=""></input>
		</form>



	</body>
</html>
