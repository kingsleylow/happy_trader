<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtShowOrderPlusDetails" scope="request"/>
<jsp:setProperty name="SC" property="*"/>
<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
}
%>


<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
		<meta http-equiv="Cache-Control" content="No-cache" />

		<title>Order and Dependent Responses</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>

		<!-- OLD DHTML grid -->
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>
		-->
		
		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css" />
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>

		<script  src="js/misc/helper.js"></script>


		<script>
			var historyGrid_m = null;

			function initHistoryGridObject()
			{


				initTableWithDummyWhitspaces('bresp_finData');
				historyGrid_m = new dhtmlXGridFromTable("bresp_finData");
				historyGrid_m.setImagePath("js/grid3/codebase/imgs/");

			}

			function on_load()
			{
				initHistoryGridObject();
			}
			

		</script>
		<!--
		<style type="text/css">
		

			html,body{
				height:100%;
				margin:0px;
				padding:0px
			}
		</style>
		-->

	</head>
	<body onload="on_load()">

		<table width="100%" height="100%" class="x8" cellpadding="2" cellspacing="2">

			<tr>
				<td height='20px' class="ITRHomeWelcome">
					<div align='left'>
						Order
					</div>
				</td>
			</tr>

			<tr>
				<td height="250px" style='border: 1px solid black;' valign="top" align="center">
				
					<table width="100%" class="x8" cellpadding="2" cellspacing="2" style="font-size:smaller">
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "order_data")%>
					</table>
				
				</td>
			</tr>


			<tr>
				<td height="20px" class="ITRHomeWelcome">
					<div align="left">
						Responses List
					</div>
				</td>
			</tr>

			<tr>
				<td valign="top" align="center">
					
					
						<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
					
						<table gridWidth="100%" gridHeight="100%" id="bresp_finData" lightnavigation="false" >
							<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "bresp_history_data") %>
						</table>
						
					</div>
		 
				</td>
			</tr>

		</table>



	</body>
</html>
