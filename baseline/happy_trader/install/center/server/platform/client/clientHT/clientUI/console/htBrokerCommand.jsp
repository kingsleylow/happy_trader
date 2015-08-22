<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerCommand" scope="request"/>
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

		<title id='tltObj' >Broker Command Dialog</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- DHTML grid -->
		
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

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
		


		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript" src="js/misc/property_table.js"></script>

		<script>
			
			var mygrid_m = null;
			var myposgrid_m = null;
			var xPosGridExporter_m = null;


			function do_place_order()
			{

					var url = '\htBrokerCommand_PlaceOrder.jsp?session_id='+encodeURIComponent(curConnectObj.innerText) +
						"&alg_brk_pair=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
						"&server_id=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>')+
						"&target_thread=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>') +
						"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>');
						
					window.open(url);
				
				
			}

			

			function do_refresh()
			{
				var url = "\htBrokerCommand.jsp?operation=initial_refresh_load&target_thread="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>') +
					"&alg_brk_pair="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
					"&server_id="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>') +
					"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>');
				
				submit_form(url);
				
			}

			function page_load()
			{
				// create mygrid_m
				initTableWithDummyWhitspaces('sessions_tab');
				mygrid_m = new dhtmlXGridFromTable('sessions_tab');
				//mygrid_m.setImagePath("/imgs/grid/");
				mygrid_m.setImagePath("js/grid3/codebase/imgs/");
				//mygrid_m.setOnRowSelectHandler(doOnRowSelected);
				mygrid_m.attachEvent("onRowSelect", 
				  function(id, ind){
					curConnectObj.innerText = mygrid_m.cells(id, 0 ).getValue();
				  }
				);
				
				mygrid_m.enableMultiselect(true);
				


				// position grid
				initTableWithDummyWhitspaces('orders_tab');
				myposgrid_m = new dhtmlXGridFromTable('orders_tab');
				//myposgrid_m.setImagePath("/imgs/grid/");
				myposgrid_m.setImagePath("js/grid3/codebase/imgs/");
				myposgrid_m.enableMultiselect(true);

				xPosGridExporter_m = new ExportImportXGrid(myposgrid_m, "<%=SC.getUniquePageId() %>" );



				setPageCaption(tltObj.innerText);
				restore_form();
			}

			function do_rebounce_connection(session_id, run_name, connect_string, is_connected)
			{
				var url = "\htBrokerCommand.jsp?operation=rebounce_connection&target_thread="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>') +
					"&alg_brk_pair="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
					"&server_id="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>') +
					"&session_id="+encodeURIComponent(session_id)+
					"&run_id="+encodeURIComponent(run_name) +
					"&connect_string="+encodeURIComponent(connect_string) +
					"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>')+
					"&is_connected="+(is_connected ? "1":"0");


				submit_form(url);
			}

			function do_delete_connection(session_id, run_name, connect_string, is_connected)
			{
				var url = "\htBrokerCommand.jsp?operation=delete_connection&target_thread="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>') +
					"&alg_brk_pair="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
					"&server_id="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>') +
					"&session_id="+encodeURIComponent(session_id)+
					"&run_id="+encodeURIComponent(run_name) +
					"&connect_string="+encodeURIComponent(connect_string) +
					"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>')+
					"&is_connected="+(is_connected ? "1":"0");


				submit_form(url);
			}

			function create_new_connection()
			{
				var run_name = "INTERFACE_" + dateToString(new Date());
				var connect_string = "";
				var comment_string = "";

				var params = new Array();
				params['run_name'] = run_name;
				params['connect_string'] = connect_string;

				var result = window.showModalDialog('htBrokerCommand_createNewConnection.jsp', params, 'resizable:0;dialogHeight=400px;dialogWidth=330px;help:0;status:0;scroll:0');
				if (result==null) {
					return;
				}

				connect_string = result['connect_string'];
				run_name = result['run_name'];
				comment_string = result['comment'];

				//
				var url = "\htBrokerCommand.jsp?operation=create_new_connection&target_thread="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "target_thread") %>') +
					"&alg_brk_pair="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
					"&server_id="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>') +
					"&run_id="+encodeURIComponent(run_name) +
					"&connect_string="+encodeURIComponent(connect_string)+
					"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>')+
					"&comment_string="+encodeURIComponent(comment_string);

				submit_form(url);
			}

			

			function query_positions()
			{
				if (curConnectObj.innerText.length <=0) {
					alert('Select valid session');
					return;
				}

				var session_id = curConnectObj.innerText;
				var url = "\htBrokerCommand.jsp?operation=query_active_orders&target_thread="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "target_thread") %>') +
					"&alg_brk_pair="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
					"&server_id="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>') +
					"&session_id="+encodeURIComponent(session_id)+
					"&broker_seq_num=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>');


				if (allPosObj.checked)
					url+= "&show_history_orders=true";
				else
					url+= "&show_history_orders=false";

				submit_form(url);

			}

			function do_send_custom_event()
			{
				var url = "\htBrokerCommand_SendCustomEvent.jsp?alg_brk_pair=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>')+
						"&server_id=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>')+
						"&target_thread=" + encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>');

				window.open(url);
					
				
			}


			// submit form with data to store
			function submit_form(url)
			{
				var surl = url;
				surl += "&cur_session_id="+encodeURIComponent(curConnectObj.innerText);
				surl += "&checked_all_orders_query="+encodeURIComponent(allPosObj.checked);
				surl += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = surl;
				mForm.submit();
			}

			// restore some values
			function restore_form()
			{
				curConnectObj.innerText = '<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "cur_session_id") %>';
				var  allPosObj_checked = new String('<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "checked_all_orders_query") %>');
				if (allPosObj_checked.toLowerCase()=='true')
					allPosObj.checked = true;
				else if (allPosObj_checked.toLowerCase()=='false')
					allPosObj.checked = false;
			}

			function export_result()
			{
				var name = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>" +
					"_"+"<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>";

				xPosGridExporter_m.exportSelectedRows(name);
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
	<body onload="page_load();">
		<table width=100% height="100%" class=x8>

			<tr>
				<td height="20px">
					<table class=x8 width=100%>
						<td align=left width=105px>
							The target thread:
						</td>

						<td align=left class=x2 width=60px>
							<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>
						</td>

						<td align=left width=120px>
							The target algorithm:
						</td>

						<td align=left class=x2 width=140px>
							<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_alg_brk_pair") %>
						</td>

						<td>
						</td>
						
					</table>
				</td>

			</tr>


			<tr>
				<td align=left height=20px>
					Available sessions
				</td>
			</tr>

			<tr>
				<td valign="top" align="center" style='position:relative'>
				  <div  style='width:99%;height:99%;border: 1px solid black;position:absolute;top:0px'>
					<table  id='sessions_tab' gridWidth='100%' gridHeight='120px' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "session_list") %>
					</table>
				  </div>	
				</td>
			</tr>


			<tr>
				<td align=right bgcolor="buttonface" height="20px">
					<a href="#" onclick="create_new_connection();" >Create New Connection</a>
				</td>
			</tr>

			<tr>
				<td align=left height="20px">
				<td>
			</tr>

			<tr>
				<td bgcolor="buttonface" height=20px>
					<table width=100% height="100%" class=x8>
						<tr>

							<td align=left  width=230px>
								Broker operations against connection:
							</td>
							<td align=left width=250px>
								<div style="width:245px;border-style:solid; border-width:1px" id="curConnectObj"></div>
							</td>

							<td>
							</td>

							<td align=right width="200px">
								<input id='allPosObj' type=checkbox>Include Historical Positions</input>
							</td>


						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td valign="top" align="center" style='position:relative'>
				  <div  style='width:99%;height:99%;border: 1px solid black;position:absolute;top:0px'>
					<table  id='orders_tab' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "position_list") %>
					</table>
				  </div>
				</td>
			</tr>

			<tr>
				<td height="20px" align=left width=100% bgcolor="buttonface">

					<table class=x8 width="100%">
						<tr>

							<td>
							</td>

							<td align=right width=150px>
								<a href='#' onclick='export_result();' >Export to File</a>
							</td>

							<td align=right width=150px>
								<a href='#' onclick='query_positions();' >Query Positions</a>
							</td>

							<td align=right width=150px>
								<a href='#' onclick='do_place_order();' >Place New Order</a>
							</td>

							<td align=right width=150px>
								<a href='#' onclick='do_send_custom_event();' >Send Custom Event</a>
							</td>

							

						</tr>
					</table>
				</td>
			</tr>

			
			<tr>
				<td height="20px" class="x3" align=left width="40px" bgcolor="buttonface">
					<a href="#" onclick="do_refresh();">Refresh</a>
				</td>
			</tr>

			<tr>
				<td align=left height=20px>

				</td>
			</tr>

		</table>


	</body>

	<form method="post" action="" id="mForm" style="display:none">
		<input type=hidden name="custom_event_data" value=""></input>
	</form>

	

</html>