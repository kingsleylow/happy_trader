<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtShowBrokerPlusOrder" scope="request"/>
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
		<title>Broker Response Plus Parent Order</title>
	</head>
	<body>
		<table width=100%>
			<tr>
				<td align=left  width=200px class="ITRHomeWelcome">
					Order
				</td>
				<td align=left  width=200px class="ITRHomeWelcome">
					Response
				</td>
			</tr>
			<tr>
				<!-- first column-->
				<td style="border: 1px solid black" valign=top>
					<table width=100% class=x8 cellpadding="2" cellspacing="2" style="font-size:smaller">
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "order_data")%>
					</table>
				</td>

				<!-- second column-->
				<td style="border: 1px solid black" valign=top>
					<table width=100% class=x8 cellpadding="2" cellspacing="2" style="font-size:smaller">
						<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_data")%>

					</table>
				</td>
			</tr>
		</table>



	</body>
</html>
