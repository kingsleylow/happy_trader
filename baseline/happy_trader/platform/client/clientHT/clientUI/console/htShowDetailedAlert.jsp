<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtShowDetailedAlert" scope="request"/>
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
		<title>View Detailed Alert Data</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>
		
	</head>
	<body>

		<table width=100% height=100%  class=x8 >



			<tr height=20px >
				<td align=left>
					<table width=100% height=100% class=x8 >


						<tr height=20px>
							<td colspan=3 style="font:larger" class="x3">
								<div align=left>
									Detailed Alert Data
								</div>
							</td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								ID
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "id")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Alert Time
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alert_time")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Thread
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "thread")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Priority
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "priority")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Session ID
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "source")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Run Name
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "run_name")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Algorithm Broker Pair
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_brk_pair")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Small Data
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "small_data")%>
							</td>
							<td></td>
						</tr>



					</table>
				</td>
			</tr>

			<tr height=20px >
				<td></td>
			</tr>

			<!--  -->
			<tr class=x3 height=20px>
				<td align=center bgcolor='buttonface' >
					Alert Data
				</td>

			</tr>

			<!-- data -->

			<tr width=100% height="100%">
				<td align=center>


					<table width="100%" height="100%" class=x8 >
						<tr>
							<td>

								
								<div align="left" class="tbl_data">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "data")%>
								</div>

								
							</td>
						</tr>
					</table>



				</td>
			</tr>

			<!-- data -->

			<tr class=x3 height=20px>
				<td align=center bgcolor='buttonface' >
				</td>
			</tr>


		</table>



	</body>
</html>