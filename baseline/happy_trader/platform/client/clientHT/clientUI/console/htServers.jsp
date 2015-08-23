<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServers" scope="request"/>
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

		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">
		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>
		-->

		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>

		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>
		<script type="text/javascript" src="js/misc/ajax_utils.js"></script>

		<!-- MODAL -->
		<link  type="text/css" rel="stylesheet" href="js/modal/modalhelper.css" />
		<script type="text/javascript" src="js/modal/modalhelper.js"></script>

		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>
		
		
		
		<script  src="js/misc/json2.js"></script>
		

		<title id='tltObj' >Trading Servers Administration</title>
		<script type="text/javascript">

			var xGrid_subscr_events_m = null;
			var modalObj_m = null;
			

			function start_long_op()
			{
				document.body.disabled=true;
			}

			function stop_long_op()
			{
				document.body.disabled=false;
			}
						

			function on_load()
			{

				setPageCaption(tltObj.innerText);

				// check id no servers at all
				if (serversObj.options.length <=0) {
					// disable everything excepting add server option
					existServersTable.style.display = "none";
					noServersTable.style.display="inline";

				}
				else {
					// normal
					xGrid_subscr_events_m = new dhtmlXGridFromTable('param_obj');
					xGrid_subscr_events_m.setImagePath("js/grid3/codebase/imgs/");
				}

				if (<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "use_spawner") %>==true) {
					spawnerUseObj.checked = true;
				}
				
				on_spawner_check();
				modalObj_m = new ModalDialog();

				
			}

			function getXGridSelectedRows_local(xGrid, cbId )
			{
				var result="";
				var rowNum = xGrid.getRowsNum();

				for(i = 0; i < rowNum; i++ ) {
					var rowId = xGrid.getRowId(i);

					cb_cell = xGrid.cells(rowId, cbId );


					var cb_cell_value = cb_cell.getValue();

					var event_type = xGrid.cells(rowId,2).getValue();

					if (cb_cell_value.toUpperCase().indexOf('CHECKED')>=0) {
						result += event_type + ",1,";
					}
					else {
						result += event_type + ",0,";
					}


				}

				return result;
			};


			function add_new_server()
			{
				var url = "/htServers.jsp?operation=add_new_server&new_server=";

				var server_name = window.showModalDialog('htServers_addNewServer.jsp', "", 'resizable:0;dialogHeight=150px;dialogWidth=330px;help:0;status:0;scroll:0');

				if(server_name==null)
					return;

				if (server_name.length <=0 ) {
					alert("You must enter valid server name");
					return;
				}

				url += encodeURIComponent(server_name);
				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}

			function delete_server()
			{
				if (window.confirm('Are you sure to delete the server: '+get_cur_server())==true) {
					var url = "/htServers.jsp?operation=delete_server&cur_server="+get_cur_server();


					url += '&page_uid=<%=SC.getUniquePageId()%>';

					mForm.action = url;
					mForm.submit();
				}

			}

			function get_cur_server()
			{
				if (serversObj.options.length > 0)
					return encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);

				return "";
			}

			function refresh_page()
			{
				var url = "/htServers.jsp?operation=refresh_page&cur_server="+get_cur_server();


				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}

			function allpy_changes()
			{
				var url = "/htServers.jsp?operation=apply_changes&cur_server=" + get_cur_server();



				if (cmdPortObj.value.length <=0) {
					alert('Invalid command port value');
					return;
				}

						

				if (serverHostObj.value ==null || serverHostObj.value.length <=0) {
					alert('Enter valid host name');
					return;
				};

				if (spawnerPortObj.value == null || spawnerPortObj.value.length <=0) {
					spawnerPortObj.value = -1;
				}

				

				url += "&use_spawner=" + (spawnerUseObj.checked ? "true":"false");
				url += "&server_host=" + encodeURIComponent(serverHostObj.value);
				url += "&cmd_port=" + encodeURIComponent(cmdPortObj.value);
				url += "&spawner_port=" + encodeURIComponent(spawnerPortObj.value);
				url += "&srv_log_level=" + encodeURIComponent(srvLogLevelObj.options(srvLogLevelObj.selectedIndex).value);
				url += "&srv_path=" + encodeURIComponent(srvPathObj.value);
				url += "&def_log_level="+encodeURIComponent(logLevelsObj.options(logLevelsObj.selectedIndex).value);
				url += "&subscribed_events=";
				//


				var tabledat = getXGridSelectedRows_local(xGrid_subscr_events_m, 0, new Array(2) );
				url += tabledat;
				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}

			function on_spawner_check()
			{
				if (spawnerUseObj.checked) {
					serverHostObj.disabled = false;
					spawnerPortObj.disabled = false;
				}
				else {
					serverHostObj.disabled = true;
					spawnerPortObj.disabled = true;
				}
			}

			function lib_item_clicked(exe_path)
			{
				srvPathObj.value = exe_path;
				modalObj_m.hide();
			}
			
			function str_replace(search, replace, subject) {
				return subject.split(search).join(replace);
			}
						
			function click_fun(entry, tag)
			{
				return "onclick=\"lib_item_clicked('"+str_replace("\\", "\\\\", entry)+"')\"";
				
			}
			
			function populate_executable()
			{
				var json_srvlist = <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "available_executables")%>;
				var data = ModalDialog.createDivContentFromList(json_srvlist, click_fun, null, " ");
				
				modalObj_m.show(500,300,data);
			}
		
			

		

		</script>
		<style type="text/css">

						.pointer { 
							cursor: pointer 
						} 

						html,body{
							height:100%;
							margin:0px;
							padding:0px
						}


		</style>
	</head>

	<body onload="on_load();">

		<table id="noServersTable" style="display:none" width=100% class=x8>
			<tr>
				<td align=left>
					<a href="#" onclick="add_new_server();" >Add New Trading server</a>
				</td>
			</tr>
		</table>

		<table id="existServersTable" width=100% height="100%" class=x8>

			<tr class="x2">
				<td align="left" height="20px">
					Select Trading Server
				</td>
			</tr>

			
			<tr>
				<td height="20px">
					<table class="x8" width="100%">
						<tr>
							<td>
								<select id="serversObj" style="width:100%" onchange="refresh_page();">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_list") %>
								</select>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height=20px bgcolor="buttonface">
					<table width=100% class=x8>
						<tr>
							<td>
							</td>



							<td width=110px align=left>
								<a href="#" onclick="delete_server();">Delete Server</a>
							</td>

							<td width=110px align=left>
								<a href="#" onclick="add_new_server();" >Add New Server</a>
							</td>


						</tr>
					</table>
				</td>
			</tr>




			<tr>
				<td align=left height="20px">
					<table width=100% class=x8>
						<tr>


							<td width=30px align=right>
								Port
							</td>

							<td width=50px align=left>
								<input id="cmdPortObj" value='<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "cmdport") %>' style="width:100%"></input>
							</td>


							<td width=20px>
							</td>

							<td width=130px align=left>
								Default Logging Level
							</td>

							<td align=left width=200px>
								<select id="logLevelsObj" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "log_level_options")%>
								</select>
							</td>

							<td>
							</td>

						</tr>

					</table>
				</td>
			</tr>

			<!-- -->

			<tr>
				<td height="20px">

					<table width=100% class=x8>
						<tr>
							<td align=left width=100px>
								<input type="checkbox" id="spawnerUseObj" onclick="on_spawner_check();">Use spawner</input>
							</td>

							<td width=30px align=right>
								Host
							</td>

							<td width=170px align=left>
								<input id="serverHostObj" value='<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "serverhost") %>' style="width:100%"/>
							</td>

							<td width=90px align=right>
								Spawner port
							</td>

							<td width=50px align=left>
								<input id="spawnerPortObj" value='<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "spawnerport") %>' style="width:100%"/>
							</td>



							<td>
							</td>


						</tr>
					</table>

				</td>
			</tr>
			<!-- -->

			<tr>
				<td height="20px">

					<table width=100% class=x8>

						<!--
						<tr>
							<td align=left width=160px>
								Available Executables
							</td>

							<td align=left>
								<select id="availableSrvPathsObj" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "available_executables")%>
								</select>

							</td>


							<td>
							</td>

						</tr>
						-->

						<tr>
							<td align=left width=160px>
								Server Path
							</td>

							<td align=left>
								<input id="srvPathObj" value="<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "srv_path")%>" style="width:100%"/>
							</td>

							<td width="30px">
								<a href="#" onclick="populate_executable();">Populate</a>
							</td>


							<td>
							</td>

						</tr>

					</table>

				</td>
			</tr>

			<tr>
				<td height="20px">

					<table width=100% class=x8>
						<tr>
							<td align=left width=150px>
								Server Internal Log Level
							</td>

							<td align=left width="120px">
								<select id="srvLogLevelObj" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "srv_log_levels")%>
								</select>

							</td>

							<td>
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
					Default Trading Server Events
				</td>
			</tr>

			<tr>
				<td style='border: 1px solid black' align="center">
					<table id='param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "event_subscr_list") %>
					</table>
				</td>
			</tr>



			<tr >
				<td height="20px" bgcolor="buttonface">
					<table width=100% class =x8>
						<tr>
							<td class="x3" align=left width="38px">
								<a href="#" onclick="allpy_changes();">Apply</a>
							</td>

							<td>
							</td>


						</tr>
					</table>
				</td>
			</tr>


		</table>



		<form method="post" action="" id="mForm" style="display:none">
		</form>

	</body>
</html>
