<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtStartStopServer_MemoryCacheManagement" scope="request"/>
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

<%
if (methodName.equalsIgnoreCase("GET")) {
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<title>Memory Cache Management</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- MISC -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- CALENDAR -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />
		<script type="text/javascript" src="js/calendar.js"></script>
		<script type="text/javascript" src="js/lang/calendar-en.js"></script>
		<script type="text/javascript" src="js/calendar-setup.js"></script>

		<!-- JSON2 -->
		<script type="text/javascript" src="js/misc/json2.js"></script>

		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>

		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>

		<script type="text/javascript">

			var grid_m = null;
			var ajax_caller_m = new AjaxCaller(-1);
			var curentProfileParams_m = null;

			function on_load()
			{
				start_long_op();

				
				// claendar setup
				Calendar.setup({
					inputField     :    "f_date_b",     // id of the input field
					ifFormat       :    "%d-%m-%Y %H:%M:%S",      // format of the input field
					button         :    "f_trigger_b",  // trigger for the calendar (button ID)
					align          :    "Tl",           // alignment (defaults to "Bl")
					singleClick    :    true,
					timeFormat     :    "24",
					showsTime      :    true
				});

				Calendar.setup({
					inputField     :    "f_date_e",     // id of the input field
					ifFormat       :    "%d-%m-%Y %H:%M:%S",      // format of the input field
					button         :    "f_trigger_e",  // trigger for the calendar (button ID)
					align          :    "Tl",           // alignment (defaults to "Bl")
					singleClick    :    true,
					timeFormat     :    "24",
					showsTime      :    true
				});
				


				grid_m = new dhtmlXGridObject('symbol_list_obj');
				grid_m.setImagePath("js/grid3/codebase/imgs/");
				grid_m.setHeader("Provider,Symbol,Start Time,End Time");

				grid_m.setInitWidths("160,100,120,120");
				grid_m.setColAlign("left,left,left,left");
				grid_m.setColTypes("ro,ro,ro,ro");
				grid_m.setColSorting("str,str,str,str");
				grid_m.enableResizing("true,true,true,true");

				grid_m.init();
				grid_m.load(
					"/HtReturnSessionVar_v2.jsp?response_type=xml&session_var=symbol_data&cookie=<%=SC.getUniquePageId() %>",
					function () {

						stop_long_op();
					},
					"xml"
				);

			}

			function on_change_sim_profile()
			{
				// read simulation profile data
				if (simProfileList_obj.selectedIndex > 0) {
					simProfilePropTab_obj.style.display = 'inline';
					read_profile_properties(simProfileList_obj.options(simProfileList_obj.selectedIndex).innerText);
				}
				else {
					simProfilePropTab_obj.style.display = 'none';
					unmark_all_rows();

				}
			}

			function read_profile_properties(profile_name)
			{

				var url = "/htStartStopServer_MemoryCacheManagement.jsp?operation=read_sim_profile_data&page_uid=<%= SC.getUniquePageId() %>&sim_profile_name="+encodeURIComponent(profile_name);

				ajax_caller_m.makeCall("POST", url, '',
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					if (ajax_caller_m.isResponseOk()) {
						process_profile_response(content);
					}
					else {
						document.body.innerHTML = wrapErrorToHTML(ajax_caller_m.createErrorString());
					}
				},

				function()
				{
					start_long_op();
				}
				,
				function()
				{
					// stop modal
					stop_long_op();

				}

				);
			}

			function start_long_op()
			{
				document.body.disabled=true;
			}

			function stop_long_op()
			{
				document.body.disabled=false;
			}

			function process_profile_response(content)
			{
				// populate with results

				var data_arr = JSON.parse(content);

				//var result_arr = eval(content);

				aper_obj.innerText = data_arr['sprop_aggr_per'];
				gtick_meth_obj.innerText = data_arr['sprop_gen_tick_method'];
				mult_fact_obj.innerText = data_arr['sprop_mult_factor'];
				ticks_split_num_obj.innerText = data_arr['sprop_ticks_split_number'];
				ticks_usage_obj.innerText = data_arr['sprop_ticks_usage'];

				curentProfileParams_m = new Array();
				curentProfileParams_m['sprop_aggr_per_int'] = data_arr['sprop_aggr_per_int'];
				curentProfileParams_m['sprop_gen_tick_method_int'] = data_arr['sprop_gen_tick_method_int'];
				curentProfileParams_m['sprop_ticks_usage_int'] = data_arr['sprop_ticks_usage_int'];
				curentProfileParams_m['sprop_mult_factor'] = data_arr['sprop_mult_factor'];
				curentProfileParams_m['sprop_ticks_split_number'] = data_arr['sprop_ticks_split_number'];

				var providers_arr =  JSON.parse(data_arr['providers_result']);
				var symbols_arr = JSON.parse(data_arr['symbols_result']);

				select_profile_matches(providers_arr, symbols_arr);
			}

			function unmark_all_rows()
			{
				var rowsCount = grid_m.getRowsNum();
				for(i=0; i < rowsCount; i++) {
					var row_id = grid_m.getRowId(i);
					grid_m.setRowTextNormal(row_id);
				}
			}

			function select_profile_matches(providers_arr, symbols_arr )
			{
				// first unmark all rows
				var rowsCount = grid_m.getRowsNum();
				unmark_all_rows();

				var search_arr = new Array();
				for(i = 0; i < providers_arr.length; i++) {
					var provider_i = providers_arr[i];
					var symbol_i = symbols_arr[i];
					var key = provider_i + symbol_i;
					search_arr[key] = 1;
				}

				var cnt = 0;
				for(i=0; i < rowsCount; i++) {
					var row_id = grid_m.getRowId(i);
					var provider_i = grid_m.cellByIndex(i, 0).getValue();
					var symbol_i = grid_m.cellByIndex(i, 1).getValue();
					//
					var key = provider_i + symbol_i;

					if (search_arr[key] == 1) {
						cnt++;
						grid_m.setRowTextBold(row_id);
						grid_m.setUserData(row_id, "selected", 1);
					}
					else {
						grid_m.setUserData(row_id, "selected", 0);
					}

				}

				sel_size_obj.innerText = 'Selected size: '+cnt;
			}

			function generate_ticks()
			{
				if (simProfileList_obj.selectedIndex==0) {
					alert("Please select simulation profile name from the list");
					return;
				}

				if (id_str.value.length<=0) {
					alert("Please enter small string ID");
					return;
				}

				var profile_name = simProfileList_obj.options(simProfileList_obj.selectedIndex).innerText;

				// go through all symbols
				var prov_symbol_list = new Array();

				var rowsCount = grid_m.getRowsNum();
				for(i = 0; i < rowsCount; i++) {
					var row_id = grid_m.getRowId(i);

					if ( grid_m.getUserData(row_id, "selected") == 1) {
						var e = new Array(grid_m.cellByIndex(i, 0).getValue(), grid_m.cellByIndex(i, 1).getValue() );
						prov_symbol_list.push( e );
					}
				}

				var url = "/htStartStopServer_MemoryCacheManagement.jsp?server_id="+
					encodeURIComponent("<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "trading_server_name") %>")+
					"&operation=create_sim_ticks&page_uid=<%= SC.getUniquePageId() %>&sim_profile_name="+encodeURIComponent(profile_name)+
					"&bdate="+encodeURIComponent(f_date_b.value)+
					"&edate="+encodeURIComponent(f_date_e.value)+
					"&id_string="+encodeURIComponent(id_str.value);
				
				var data = "plist="+JSON.stringify(prov_symbol_list);

				ajax_caller_m.makeCall("POST", url, data,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{

					if (ajax_caller_m.isResponseOk()) {
						// all is ok
						alert("Simulation cache UID:" + content);
					}
					else {
						document.body.innerHTML = wrapErrorToHTML(ajax_caller_m.createErrorString());
					}
				},

				function()
				{
					start_long_op();
				}
				,
				function()
				{
					// stop modal
					stop_long_op();
				}

			);


			}

		</script>
	</head>
	<body onload="on_load();">
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>

			<tr class=x3>
				<td align=left height="20px">
					Trading server: <%= SC.getStringSessionValue(SC.getUniquePageId(), request, "trading_server_name") %>
				</td>
			</tr>

			<tr class=x3>
				<td align=left width=260px>
					<select style='width:100%' id='simProfileList_obj' onchange="on_change_sim_profile()">
						<option value="DEFAULT">Select Simulation Profile...</option>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sim_profile_list") %>
					</select>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<table bgcolor="buttonface" id="simProfilePropTab_obj" width=100% class=x8 style="display:none">
						<tr>
							<td align=left width=190px>
								Aggregation Period
							</td>

							<td align=left width=260px id="aper_obj">
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Generate Ticks method
							</td>

							<td align=left width=260px id="gtick_meth_obj">
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Multiplication Factor
							</td>

							<td align=left width=260px id="mult_fact_obj">
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Ticks Split Number
							</td>

							<td align=left width=260px id="ticks_split_num_obj">
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px>
								Ticks Usage
							</td>

							<td align=left width=260px id="ticks_usage_obj">
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=260px>
								<input id="id_str" value=""></input>
							</td>

							<td align=left width=190px>
								<input type="button" onclick="generate_ticks()" value="Generate Ticks" ></input>
							</td>

							<td>
							</td>

							<td>
							</td>
						</tr>

					</table>
				</td>
			</tr>

			<tr class=x8>
				<td align=left height="20px">
					All size: <%= SC.getStringSessionValue(SC.getUniquePageId(), request, "symbol_list_size") %>
				</td>
			</tr>

			<tr class=x8>
				<td align=left height="20px" id="sel_size_obj">
					Selected size: 0
				</td>
			</tr>
		
			<tr>
				<td height=20px id="date_obj">
					<table width=100% height="100%" cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
						<tr>

							<td width="160px" class="x8" height="20px">
								<div align="left" style="width:100%">Set up cache time period</div>
							</td>

							<td width="130px">
								<input style="width:100%" class=x8 type="text" name="date" id="f_date_b" 
									value="<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "min_total_time") %>">
								</input>
							</td>

							
							<td width="10px">
								<img src="imgs/img.gif" id="f_trigger_b" style="cursor: pointer; border: 1px solid red;" title="Date selector"
									onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
							</td>

							<td width="30px" height="20px">
								&nbsp;
							</td>

							<td width="130px" bgcolor="buttonface">
								<input style="width: 100%" class=x8 type="text" name="date" id="f_date_e"
									value="<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "max_total_time") %>">
								</input>
							</td>

							<td width="10px">
								<img src="imgs/img.gif" id="f_trigger_e" style="cursor: pointer; border: 1px solid red;" title="Date selector"
									onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
							</td>
							

							<td height="20px">
								&nbsp;
							</td>

						</tr>
					</table>

				</td>
			</tr>

			

			<tr height="100%" width=100%>
				<td align="center">
					<div id="symbol_list_obj" style="width:100%;height:100%;border-style:solid; border-width:1px;"></div>
				</td>
			</tr>


		</table>
	</body>
</html>

<%
}
%>