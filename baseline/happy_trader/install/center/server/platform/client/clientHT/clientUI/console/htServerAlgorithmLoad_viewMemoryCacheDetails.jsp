<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServerAlgorithmLoad_viewMemoryCacheDetails" scope="request"/>
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

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
  
		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>


		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script>
			var grid_m = null;

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
		</script>

	</head>

	<body onload="on_load();">
		<table width="100%" height="100%" class=x8>

			<tr class="x2">
				<td align=left height="20px">
					<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "description") %>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					<table bgcolor="buttonface" id="simProfilePropTab_obj" width=100% class=x8 >
						<tr>
							<td align=left width=190px>
								Aggregation Period
							</td>

							<td align=left width=260px id="aper_obj">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sprop_aggr_per") %>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Generate Ticks method
							</td>

							<td align=left width=260px id="gtick_meth_obj">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sprop_gen_tick_method") %>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Multiplication Factor
							</td>

							<td align=left width=260px id="mult_fact_obj">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sprop_mult_factor") %>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px >
								Ticks Split Number
							</td>

							<td align=left width=260px id="ticks_split_num_obj">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sprop_ticks_split_number") %>
							</td>

							<td>
							</td>
						</tr>

						<tr>
							<td align=left width=190px>
								Ticks Usage
							</td>

							<td align=left width=260px id="ticks_usage_obj">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sprop_ticks_usage") %>
							</td>

							<td>
							</td>
						</tr>

					</table>
				</td>
			</tr>

			<tr>
				<td height=20px id="date_obj">
					<table width=100% height="100%" cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
						<tr>

							<td width="160px" class="x8" height="20px">
								<div align="left" style="width:100%">Date Time Span </div>
							</td>

							<td width="130px">
								<input readonly="true" style="width:100%" class=x8 type="text" name="date" id="f_date_b"
									value="<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "min_total_time") %>">
								</input>
							</td>


							<td width="30px" height="20px">
								&nbsp;
							</td>

							<td width="130px" bgcolor="buttonface">
								<input readonly="true" style="width: 100%" class=x8 type="text" name="date" id="f_date_e"
									value="<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "max_total_time") %>">
								</input>
							</td>

							<td height="20px">
								&nbsp;
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

			<tr height="100%" width=100%>
				<td align="center">
					<div id="symbol_list_obj" style="width:100%;height:100%;border-style:solid; border-width:1px;"></div>
				</td>
			</tr>

		</table>
	</body>

</html>