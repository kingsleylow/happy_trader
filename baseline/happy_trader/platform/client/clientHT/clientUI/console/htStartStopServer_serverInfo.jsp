<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtStartStopServer_serverInfo" scope="request"/>
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
		<title>Server Information</title>
	</head>
	<body>
		<table class=x8 width=100% height="100%">

			<tr>
				<td align=left class=x18 height="20px">
					Trading server: <%= SC.getStringSessionValue(SC.getUniquePageId(), request, "trading_server_name") %>
				</td>
			</tr>

			<tr>
				<td height=20px>
				</td>
			</tr>

			<tr>
				<td height=20px>
					<table class=x8 width=100%>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "trading_server_parameters") %>
					</table>
				</td>
			</tr>

			

			<tr>
				<td>
				</td>
			</tr>


		</table>
	</body>
</html>