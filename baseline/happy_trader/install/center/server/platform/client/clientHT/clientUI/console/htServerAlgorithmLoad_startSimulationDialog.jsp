<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServerAlgorithmLoad_startSimulationDialog" scope="request"/>
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
		<title>Start Simulation</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- DHTML grid -->
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>

		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>



		<script type="text/javascript">


			function on_load()
			{
				var args = window.dialogArguments;
				if (args != null) {
					runNameObject.value = args;
				}

			}

			function on_close()
			{

				if (simulation_uid.options.length <=0) {
					alert('No cached simulation ticks available!');

					window.returnValue = null;
					window.close();
					return;
				}

				var outparams = new Array();
				outparams['run_name'] = runNameObject.value;
				outparams['run_comment'] = runNameCommentObj.innerText;
				outparams['single_sim_run'] = (inpSingleTickObj.checked ? 'true':'false');
				outparams['hist_context_uid']=simulation_uid.options(simulation_uid.selectedIndex).value;

				window.returnValue = outparams;
				window.close();

			}

			function on_cancel()
			{
				window.close();
			}

			function show_simulation_cache_details()
			{

				if (simulation_uid.options.length <=0) {
					alert('No cached simulation ticks available!');
					return;
				}
				// show our data
				var url = "/htServerAlgorithmLoad_viewMemoryCacheDetails.jsp?";
				url += "&server_id="+encodeURIComponent("<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>");
				url += "&hist_context_uid=" + encodeURIComponent(simulation_uid.options(simulation_uid.selectedIndex).value);

				
				window.open(url);

			}


		</script>

	</head>
	<body onload="on_load();">

		<table cellpadding="2" cellspacing="2" width="100%" height="100%" class=x8>

			<tr class=x2>
				<td height=20px align=left>


					<table class=x8 width=100%>
						<tr>
							<td height=20px align=left class=x2 width=270px>
								Thread "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "thread_id") %>" will start history simulation:
							</td>
							<td align=left>
								<input style='width:100%' id="runNameObject" type=text ></input>
							</td>

						</tr>

						<tr>

							<tr>
								<td align=left  class=x63>
									Enter run comments
								</td>

								<td>
								</td>
							</tr>

							<tr>
								<td align=left height="110px" colspan=2>
									<textarea style="width:100%;height:100%" id="runNameCommentObj" class="x4"></textarea>
								</td>

							</tr>

						</tr>
					</table>
				</td>
			</tr>


			<tr>
				<td height=20px align=left>
					<table width=100% class=x8>
						<tr>

							<td align=left width=390px>
								<select id="simulation_uid" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "simulation_uid_list") %>
								</select>
								
							</td>

							<td align="left">
								<a href="#" onclick="show_simulation_cache_details()">Check</a>
							</td>
						</tr>
					</table>
				</td>
			</tr>


			<tr>
				<td height=20px align=left>
					<table width=100% class=x8>
						<tr>

							<td align=left width=550px>
								<input id='inpSingleTickObj' type=checkbox>Only the first and the last ticks will be passed (only the single cycle over history data)</input>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>
			</tr>


			<tr>
				<td height=20px>
					<table width=100% class=x8>
						<tr>
							<td>
							</td>

							<td align=right width=40px>
								<a href='#' onclick="on_cancel();">Cancel</a>
							</td>

							<td align=right width=40px>
								<a href='#' onclick="on_close();">OK</a>
							</td>
						</tr>
					</table>

				</td>
			</tr>


		</table>
	</body>

</html>
