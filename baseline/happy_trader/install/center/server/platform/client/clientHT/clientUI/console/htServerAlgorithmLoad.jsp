<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServerAlgorithmLoad" scope="request"/>
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

		<link href="css/console.css" rel="stylesheet" type="text/css"/>



		<title id='tltObj' >Manage Server Thread</title>

		<!-- DHTML grid -->
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<!--
		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>
		-->
		
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>


		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript">
			var xGrid_threads_m = null;
			var xGrid_alg_brk_pairs_m = null;
			var xGrid_loaded_pairs = null;

			// this function calls dialog to send messages to the defined therad & alg broker pair
			function do_send_message(thread_id, alg_brk_pair, broker_seq_num)
			{

				var url = "\htBrokerCommand.jsp?operation=initial_load&target_thread="+encodeURIComponent(thread_id) +
					"&alg_brk_pair="+encodeURIComponent(alg_brk_pair)+
					"&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText)+
					"&broker_seq_num=" + encodeURIComponent(broker_seq_num) + 
					"&generate_new_page_uid=true";

				mForm.action = url;
				mForm.submit();
			}

			function do_send_custom_message(thread_id, alg_brk_pair, brok_seq_num)
			{
				var url = "\htBrokerCommand_SendCustomEvent.jsp?alg_brk_pair=" + encodeURIComponent(alg_brk_pair)+
						"&server_id=" + encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText)+
						"&target_thread=" + encodeURIComponent(thread_id);

				window.open(url);
			}

			function init_grids()
			{
				xGrid_alg_brk_pairs_m = new dhtmlXGridFromTable('alg_brk_pairs_obj');
				xGrid_alg_brk_pairs_m.setImagePath("/imgs/grid/");

				//
				xGrid_threads_m = new dhtmlXGridFromTable('thread_table_obj');
				//xGrid_threads_m.setImagePath("/imgs/grid/");
				xGrid_threads_m.setImagePath("js/grid3/codebase/imgs/");
				xGrid_threads_m.attachEvent("onRowSelect", on_threads_grid_row_selected);



				// // // ------

				xGrid_loaded_pairs = new dhtmlXGridObject('loaded_pairs_for_thread_obj');

				//xGrid_loaded_pairs.imgURL = "imgs/grid/";
				xGrid_loaded_pairs.setImagePath("js/grid3/codebase/imgs/");
				xGrid_loaded_pairs.setHeader("Algorithm/Broker Pair,Send Message 1,Send Message 2, Send Custom Message");
				xGrid_loaded_pairs.setInitWidths("170,100, 100,150");
				xGrid_loaded_pairs.setColAlign("left,left,left,left");
				xGrid_loaded_pairs.setColTypes("ro,ro,ro,ro");
				xGrid_loaded_pairs.setColSorting("str,str,str,str");

				xGrid_loaded_pairs.init();

			}

			// when we chnage navigation in thread grid
			function on_threads_grid_row_selected(rowid, cellind)
			{
				xGrid_loaded_pairs.clearAll( false );
				thread_id = xGrid_threads_m.cells(rowid, 0 ).getValue();
				var loadedargs = eval(xGrid_threads_m.cells(rowid, 5 ).getValue());

				for(i = 0; i < loadedargs.length; i++) {
					var send_message_cell = "<a href=\"#\" onclick=\"do_send_message(" + thread_id +",'" + loadedargs[i] +"',1)\">Broker 1</a>";
					var send_message_cell_2 = "<a href=\"#\" onclick=\"do_send_message(" + thread_id +",'" + loadedargs[i] +"',2)\">Broker 2</a>";
					var send_custom_message_cell = "<a href=\"#\" onclick=\"do_send_custom_message(" + thread_id +",'" + loadedargs[i] +"')\">Click</a>";
					var entry = new Array( loadedargs[i], send_message_cell, send_message_cell_2, send_custom_message_cell );
					xGrid_loaded_pairs.addRow(i+1, entry);
				}
			}


			function on_load()
			{
				setPageCaption(tltObj.innerText);

				// check id no running servers at all
				if (serversObj.options.length <=0) {
					// disable everything excepting add server option
					existServersTable.style.display = "none";
					noServersTable.style.display="inline";

				}
				else {
					init_grids();
				}
			}

			function do_stop_rt(thread_id)
			{
				if (window.confirm('Are you sure to stop RT activity of the thread: '+thread_id+' ?')==true) {
					var url = "/htServerAlgorithmLoad.jsp?operation=do_stop_rt";
					url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
					url+= "&thread_id="+thread_id;
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_start_rt(thread_id)
			{
				var def_name = "RT_";
				def_name += dateToString(new Date());

				var result = window.showModalDialog('htServerAlgorithmLoad_startRT.jsp', def_name, 'resizable:0;dialogHeight=400px;dialogWidth=400px;help:0;status:0;scroll:0');

				if(result==null) {
					return;
				}


				var url = "/htServerAlgorithmLoad.jsp?operation=do_start_rt";
				url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				url+="&thread_id="+thread_id;
				url+="&run_name="+encodeURIComponent(result['run_name']);
				url+="&sim_prof_pass="+encodeURIComponent(result['sim_prof_pass']);
				url+="&sim_profile_name="+encodeURIComponent(result['sim_profile_name']);

				mForm.run_comment.value=result['run_comment'];
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function do_start_simulation(thread_id)
			{
				var def_name = "SIMULATION_";
				def_name += dateToString(new Date());

				// modal dialog
				// need to get alot of parameters
				var server_id = encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				var outparams = window.showModalDialog(
				'htServerAlgorithmLoad_startSimulationDialog.jsp?server_id='+server_id+'&thread_id='+thread_id,
				def_name, 'resizable:0;dialogHeight=350px;dialogWidth=600px;help:0;status:0;scroll:0');

				if (outparams==null)
					return;

				var url = "/htServerAlgorithmLoad.jsp?operation=do_start_simulation";
				url += "&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				url += "&thread_id="+thread_id;
				url += "&run_name="+encodeURIComponent(outparams['run_name']);
				url += "&hist_context_uid="+encodeURIComponent(outparams['hist_context_uid']);
				url += "&single_sim_run="+encodeURIComponent(outparams['single_sim_run']);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				
				mForm.action = url;
				mForm.submit();

			}

			function do_stop_simulation(thread_id)
			{
				if (window.confirm('Are you sure to stop simulation of the thread: '+thread_id+' ?')==true) {
					var url = "/htServerAlgorithmLoad.jsp?operation=do_stop_simulation";
					url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
					url+= "&thread_id="+thread_id;
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_reload_algorithm_parameters(alg_id)
			{

				var server_id = encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				var alg_pair = encodeURIComponent(alg_id);

				// need to get alot of parameters
				var outparams = window.showModalDialog(
				'htServerAlgorithmLoad_AlgBrkParam.jsp?server_id='+server_id+'&alg_pair='+alg_pair,
				"", 'resizable:0;dialogHeight=700px;dialogWidth=800px;help:0;status:0;scroll:0');

				// no data
				if (outparams==null)
					return;

				var operation =  outparams['op'];
				while (operation=='reload') {

					var outparams = window.showModalDialog(
					'htServerAlgorithmLoad_AlgBrkParam.jsp?server_id='+server_id+'&alg_pair='+alg_pair+
						'&thread_id='+encodeURIComponent(outparams['see_thread_id'])+
						'&page_uid='+encodeURIComponent(outparams['page_uid']) ,
					"Update algorithm/broker parameters", 'resizable:0;dialogHeight=700px;dialogWidth=800px;help:0;status:0;scroll:0');

					if (outparams==null)
						return;

					operation =  outparams['op'];
				}

				var brkParams_keys =  outparams['brk_keys'];
				var algParams_keys = outparams['alg_keys'];
				var brkParams_values =  outparams['brk_values'];
				var algParams_values = outparams['alg_values'];

				var url = "/htServerAlgorithmLoad.jsp?operation=reload_algorithms&alg_pair="+alg_pair;
				url+="&server_id="+server_id;
				url+="&thread_id="+encodeURIComponent(outparams['propagate_thread_id']);
				url+="&brk_params=";

				for(i = 0; i < brkParams_keys.length; i++) {
					url += encodeURIComponent(brkParams_keys[i]) + "," + encodeURIComponent(brkParams_values[i]) +",";
				}

				url+="&alg_params=";
				for(i = 0; i < algParams_keys.length; i++) {
					url += encodeURIComponent(algParams_keys[i]) + "," +  encodeURIComponent(algParams_values[i]) + ",";
				}
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function do_load_algorithm(alg_id, rowId)
			{

				var def_name = "INITIAL_LOAD_";
				def_name += dateToString(new Date());

				var result = window.showModalDialog('htServerAlgorithmLoad_enterRunName.jsp', def_name, 'resizable:0;dialogHeight=350px;dialogWidth=330px;help:0;status:0;scroll:0');

				if(result==null) {
					return;
				}

				var priority  = xGrid_alg_brk_pairs_m.cells(rowId, 3).getValue();
				var url = "/htServerAlgorithmLoad.jsp?operation=load_algorithms&alg_pair="+encodeURIComponent(alg_id);
				url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				url+="&priority="+encodeURIComponent(priority);

				url+="&run_name="+encodeURIComponent(result['run_name']);
				mForm.run_comment.value=result['run_comment'];
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function do_unload_algorithm(alg_id, rowId)
			{

				if (window.confirm("Are you sure to unload the pair: "+alg_id+"?")==true) {
					var priority  = xGrid_alg_brk_pairs_m.cells(rowId, 3).getValue();
					var url = "/htServerAlgorithmLoad.jsp?operation=unload_algorithms&alg_pair="+encodeURIComponent(alg_id);
					url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
					url+="&priority="+encodeURIComponent(priority);
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_refresh()
			{
				var url = "/htServerAlgorithmLoad.jsp?operation=do_refresh";
				url += "&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function do_start_thread()
			{
				var url = "/htServerAlgorithmLoad.jsp?operation=do_start_thread";
				url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);

				var spar = get_selected_algs();
				if (spar.length <=0) {
					alert('One or more algorithm-broker pairs must be selected!');
					return;
				}

				url += "&selected_pairs=" + spar;
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function do_stop_thread(thread_id)
			{
				if (window.confirm("Are you sure stop the thread: "+thread_id+"?")==true) {
					var url = "/htServerAlgorithmLoad.jsp?operation=do_stop_thread";
					url += "&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);
					url += "&thread_id=" + encodeURIComponent(thread_id);
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}


			function get_selected_algs()
			{


				var result="";
				var rowNum = xGrid_alg_brk_pairs_m.getRowsNum();

				for(i = 0; i < rowNum; i++ ) {
					var rowId = xGrid_alg_brk_pairs_m.getRowId(i);
					if (xGrid_alg_brk_pairs_m.getColType(0)=='ro') {
						cb_cell = xGrid_alg_brk_pairs_m.cells(rowId, 0 );
						var cb_cell_value = cb_cell.getValue();

						if (cb_cell_value != null && cb_cell_value != '') {

							var str_cb_cell_value = (new String(cb_cell_value)).toUpperCase( );
							if (str_cb_cell_value.toUpperCase().indexOf("CHECKED") >=0) {
								//ours
								var value = xGrid_alg_brk_pairs_m.cells(rowId, 1).getValue();
								result += value +",";
							}

						}


					}
					else {
						alert('The column is not of "ro" type');
						return undefined;
					}
				}

				return result;
			}



		</script>
	</head>

	<body onload="on_load();">

		<table id="noServersTable" style="display:none" width=100% class=x8>
			<tr>
				<td align=left>
					No running trading servers
				</td>
			</tr>
		</table>

		<table id="existServersTable" class=x8 width=100%>

			<tr>
				<td colspan=5>
					<table class=x2 width=100%>
						<tr>
							<td align=left>
								Select Running Trading Server
							</td>
						</tr>

						<tr>
							<td>
								<select id="serversObj" style="width:100%" onchange="do_refresh();">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "running_server_list") %>
								</select>
							</td>
						</tr>

					</table>
				</td>
			</tr>


			<tr>
				<td align="left">
					Available Algorithm/Broker Pairs
				</td>
			</tr>

			<tr>
				<td style='border: 1px solid black' align="center">

					<table  id='alg_brk_pairs_obj' gridWidth='100%' gridHeight='250px' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "algorithm_list") %>
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
										<td align="left" width="100px">
											Thread Status
										</td>
										<td align="left" width="150px">
											<a href="#" onclick="do_start_thread();">Start Thread</a>
										</td>


										<td>
										</td>
									</tr>
								</table>



							</td>

							<td align=left>
								Send Message
							</td>

						</tr>

						<tr>
							<td width="60%" style='border: 1px solid black' align="center">

								<table  id='thread_table_obj' gridWidth='100%' gridHeight='170px' lightnavigation='false' class='x8'>
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "thread_list") %>
								</table>

							</td>

							<td align="center">
								<div id='loaded_pairs_for_thread_obj' style="width:100%;height:170px;border-style:solid; border-width:1px;"></div>

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
					<a href="#" onclick="do_refresh();">Refresh</a>
				</td>
			</tr>



		</table>


		<form method="post" action="" id="mForm" style="display:none">
			<input type=hidden name="run_comment" value=""></input>
			<input type=hidden name="prov_symbols" value=""></input>
		</form>

	</body>
</html>
