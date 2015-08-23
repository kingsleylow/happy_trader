<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtDeployedWebApplications" scope="request"/>
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
		<title id='tltObj' >Web Applications</title>

		<!-- DHTML grid -->
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>
	`	-->
		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>


		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript">
			var xGrid_webapps_m = null;
			function on_load()
			{
				init_grids();
			}

			function init_grids()
			{
				xGrid_webapps_m = new dhtmlXGridFromTable('webapp_table_obj');
				xGrid_webapps_m.setImagePath("js/grid3/codebase/imgs/");
				//xGrid_webapps_m.setImagePath("/imgs/grid/");
			}

			/*
			function start_webapp(name)
			{
				var url = "/htWebStartHelper.jsp?application_name="+encodeURIComponent(name);
				//window.open(url);

				window.showModalDialog(url, "", 'resizable:0;dialogHeight=150px;dialogWidth=330px;help:0;status:0;scroll:0');
			}
			*/
			
			function launch_app(url)
			{
				window.open(url, '_blank');
			}
			
			function launch_page_url(url)
			{
			  window.open(url, '_blank');
			}

		</script>
	</head>
	<body onload="on_load();">
		<table class=x8 width=100% height="100%">
			<tr>
				<td align="left" height="20px">
					Available web applications
				</td>
				</tr>

			
			<tr>
				<td  align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
					<table id='webapp_table_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "webapp_list_data") %>
					</table>
					</div>
				</td>
			</tr>

			<tr>
				<td height=20px bgcolor="buttonface">
				</td>
			</tr>
			

		</table>
	</body>
</html>