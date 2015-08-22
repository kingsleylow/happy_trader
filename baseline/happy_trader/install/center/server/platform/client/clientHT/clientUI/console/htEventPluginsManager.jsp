<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtEventPluginsManager" scope="request"/>
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
		<title id='tltObj'>Event Plugin Manager</title>

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- DHTML grid -->
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


		<script type="text/javascript">
			var mygrid_m = null;
			function do_start_plugin(plugin)
			{
				if (window.confirm('Are you sure to start event plugin: ' + plugin)==true) {
					var url = "/htEventPluginsManager.jsp?operation=start_plugin&plugin_id="+encodeURIComponent(plugin);
					url += '&page_uid=<%=SC.getUniquePageId()%>';

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_stop_plugin(plugin)
			{
				if (window.confirm('Are you sure to stop event plugin: ' + plugin)==true) {
					var url = "/htEventPluginsManager.jsp?operation=stop_plugin&plugin_id="+encodeURIComponent(plugin);
					url += '&page_uid=<%=SC.getUniquePageId()%>';

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_refresh()
			{
				var url = "/htEventPluginsManager.jsp?operation=refresh";
				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}

			function on_load()
			{
				setPageCaption(tltObj.innerText);
				initTableWithDummyWhitspaces('param_obj');

				mygrid_m = new dhtmlXGridFromTable('param_obj');
				mygrid_m.setImagePath("js/grid3/codebase/imgs/");
			}
		</script>
	</head>

	<body onload='on_load();'>
		<table class=x8 width=100% height="100%">

			<tr>
				<td align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
						<table id='param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
							<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "plugin_list") %>
						</table>
					</div>
				</td>
			</tr>

			

			<tr>
				<td class="x3" height=20px bgcolor="buttonface">
					<div align="left"><a href="#" onclick="do_refresh();">Refresh</a></div>
				</td>
			</tr>

			<tr>
				<td height=20px>

				</td>
			</tr>

		</table>

		<form method="post" action="" id="mForm" style="display:none">
		</form>
	</body>
</html>
