<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtShowDetailedLog" scope="request"/>
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
		
		<title>View Detailed Log Data</title>

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
									Detailed Logging Data
								</div>
							</td>
						</tr>


						<tr height=20px>
							<td align=right class=x3 bgcolor='buttonface' width=120px>
								ID
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "id_val")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td class=x3 bgcolor='buttonface'>
								Created On
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "tlast_created_val")%>
							</td>
							<td></td>
						</tr>


						<tr height=20px>
							<td class=x3 bgcolor='buttonface'>
								Level
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "level_val")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td class=x3 bgcolor='buttonface'>
								Thread
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "thread_val")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td class=x3 bgcolor='buttonface'>
								Session
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "session_val")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td class=x3 bgcolor='buttonface'>
								Context
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "context_val")%>
							</td>
							<td></td>
						</tr>


					</table>
				</td>
			</tr>

			<tr height=20px>
				<td></td>
			</tr>

			<!--  -->
			<tr class=x3 height="20px">
				<td align="center" valign=top bgcolor='buttonface'>
					Content
				</td>
			</tr>

			<!--  data -->
			<tr width=100% height="100%">
				<td align=center>


					<table width="100%" height="100%" class=x8 >
						<tr>
							<td>
								
								<div style="overflow:hidden;width:expression(document.body.clientWidth - 40 );height:100%">
									<textarea STYLE="width:100%;height:100%" readonly="true"><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "content")%></textarea>
								</div>
							</td>

						</tr>
					</table>



				</td>
			</tr>

			<!--  data -->
			
			<tr class=x3 height=20px>
				<td align=center bgcolor='buttonface' >
				</td>
			</tr>



		</table>



	</body>
</html>
