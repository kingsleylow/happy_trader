<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerRunNames_v2_TradeReport" scope="request"/>
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

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
			<meta http-equiv="Cache-Control" content="No-cache">

				<title id='tltObj' >Trading Report</title>

				<link href="css/console.css" rel="stylesheet" type="text/css"/>

				<!-- calendar stylesheet -->
				<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

				<script type="text/javascript" src="js/misc/helper.js"></script>

				<script type="text/javascript" src="js/misc/property_table.js"></script>		

				<!-- NEW GRID -->
				<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
					<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
					<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
					<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
					<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>





					<script>
						var xgrid_m = null;
						var xGridExporter_m = null;
			
						function page_load()
						{
							xgrid_m = new dhtmlXGridFromTable('param_obj');
							xgrid_m.setImagePath("js/grid3/codebase/imgs/");
				
							xGridExporter_m = new ExportImportXGrid(xgrid_m, "<%=SC.getUniquePageId() %>" );
						}
				
						function do_export()
						{
							xGridExporter_m.exportAllRows("trade_report");
						}
			
					</script>	
					<style type="text/css">


						html,body{
							height:100%;
							margin:0px;
							padding:0px
						}
					</style>
					</head>
					<body id="bodyObj" onload="page_load();">

						<table width=100% height="100%">

							<tr>
								<td  align="center" style='position:relative'>
									<div style='width:99%;height:99%;border: 1px solid black;position:absolute;top:0px'>
										<table id='param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8' >
											<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "report_data") %>
										</table>
									</div>
								</td>
							</tr>

							<tr >
								<td height="20px" bgcolor="buttonface">
									<table width=100% class =x8>
										<tr>
											<td class="x3" align=left width="80px">
												<a href="#" onclick="do_export();">CSV Export</a>
											</td>

											<td>
											</td>

											<td>
											</td>	

										</tr>
									</table>
								</td>
							</tr>

						</table>


					</body>
					</html>
