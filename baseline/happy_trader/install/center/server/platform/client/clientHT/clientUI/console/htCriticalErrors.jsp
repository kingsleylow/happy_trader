<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtCriticalErrors" scope="request"/>
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
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>

		<!-- DHTML grid -->

		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		-->

		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>
	

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<title id='tltObj' >Critical Errors List</title>

		<script>

			var mygrid_m = null;



			function page_load()
			{
				setPageCaption(tltObj.innerText);
				
				
				mygrid_m = new dhtmlXGridFromTable('error_obj');
				mygrid_m.setImagePath("js/grid3/codebase/imgs/");
				mygrid_m.enableMultiselect(true);


			}

			function clear_critical_errors()
			{
				mForm.action = "/htCriticalErrors.jsp?operation=clear_errors&page_uid=<%=SC.getUniquePageId()%>";
				mForm.submit();
			}



		</script>
	</head>
	<body onload="page_load();">
		<table width=100% height="100%" class=x8>

			<tr>
				<td align=left height=20px>
					Critical Error List
				</td>
			</tr>

			<tr height="100%" width=100%>
				<td align="center">

					<table width="100%" height="100%" class=x8 >
						<tr>
							<td align="center">
								<div id='sessions_tab' style="
										 border: 1px solid black;
										 overflow:auto;
										 width:expression(document.body.clientWidth - 40 );
										 height:100%
										 ">
									<table id='error_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
											<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "error_data") %>
									</table>
								</div>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr >
				<td height="20px" bgcolor="buttonface">
					<table width=100% class =x8>
						<tr>
							<td class="x3" align=left width="60px">
								<a href="#" onclick="clear_critical_errors();">Clear List</a>
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