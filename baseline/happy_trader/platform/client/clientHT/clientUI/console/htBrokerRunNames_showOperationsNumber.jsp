<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerRunNames_showOperationsNumber" scope="request"/>
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
		<title></title>
	</head>
	<body>
		<table cellpadding="2" cellspacing="2" width="100%"  border="0" class=x8>

			<tr>
				<td align=left  class=x63>
					<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "run_name") %>
				</td>

			</tr>

			<tr>
				<td align=left >
					The number of sessions per that run: <b><%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sessions_number") %></b>
				</td>

			</tr>

			<tr>
				<td align=left >
					The number of requests to broker: <b><%= SC.getStringSessionValue(SC.getUniquePageId(), request, "orders_number") %></b>
				</td>

			</tr>

			<tr>
				<td align=left >
					The number of responses from broker: <b><%= SC.getStringSessionValue(SC.getUniquePageId(), request, "responses_from_broker") %></b>
				</td>

			</tr>

		</table>

	</body>
</html>