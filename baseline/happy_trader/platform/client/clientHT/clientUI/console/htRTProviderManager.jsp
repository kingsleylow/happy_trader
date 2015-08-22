<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtRTProviderManager" scope="request"/>
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

		<title id='tltObj'>Real-Time Provider Manager</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- OLD DHTML grid -->
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

		<script type="text/javascript" src="js/misc/xgrid_utils.js"></script>

		<script type="text/javascript">
			var xGrid_Providers_m = null;
			function do_start_provider(provider)
			{
				if (window.confirm('Are you sure to start RT provider: ' + provider)==true) {
					var url = "/htRTProviderManager.jsp?operation=start_provider&provider_id="+encodeURIComponent(provider);
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_stop_provider(provider)
			{
				if (window.confirm('Are you sure to stop RT provider: ' + provider)==true) {
					var url = "/htRTProviderManager.jsp?operation=stop_provider&provider_id="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}
			}

			function do_refresh()
			{
				var url = "/htRTProviderManager.jsp?operation=refresh";
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function on_load()
			{
				setPageCaption(tltObj.innerText);

				//initTableWithDummyWhitspaces('rt_prov_tbl');
				xGrid_Providers_m = new dhtmlXGridFromTable('rt_prov_tbl');
				xGrid_Providers_m.setImagePath("js/grid3/codebase/imgs/");
				
			}


			function do_start_rt_show()
			{
				var providers = new Array();
			
				for(i = 0; i < xGrid_Providers_m.getRowsNum(); i++ )
				{
					var is_checked = XgridHelperObject.get_checkbox_status2(xGrid_Providers_m.cells( xGrid_Providers_m.getRowId(i), 3 ).getValue());

					if (is_checked) {
						providers.push( xGrid_Providers_m.cells( xGrid_Providers_m.getRowId(i), 0 ).getValue() );
					}
	
				}

				var data = getCommaSeparatedList(providers);
				
				if (providers.length <=0) {
					alert('Please select at least one provider');
					return;
				}
				var url = '/htRTProviderManager_RealTimePricesHelper.jsp?providers='+encodeURIComponent(data);
				window.open(url,'_blank', 'titlebar:0');

				//window.showModelessDialog(url, '', 'resizable:1;dialogWidth:400px;dialogHeight:500px');


			}


		</script>
	</head>

	<body onload='on_load();'>

		<table width="100%" height="100%" class=x8>

			<tr>
				<td height="20px" align="left">
					Available RT Providers
				</td>
			</tr>

			<tr>
				<td align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
					<table id='rt_prov_tbl' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "provider_list") %>
					</table>
				</div>
				</td>
			</tr>

			<tr>
				<td height=20px bgcolor="buttonface">
					<table width="100%" height="100%" class=x8>
						<tr>
							<td width="95px" align="left" class="x3">
								<a href="#" onclick="do_start_rt_show();">Show RT Data</a>
							</td>


							<td width="50x" align="left" class="x3">
								<a href="#" onclick="do_refresh();">Refresh</a>
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

		</table>

		<form method="post" action="" id="mForm" style="display:none">
		</form>

	</body>
</html>
