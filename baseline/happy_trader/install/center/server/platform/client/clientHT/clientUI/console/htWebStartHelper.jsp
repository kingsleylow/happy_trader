<%@ page contentType="text/html;charset=windows-1251"%>

<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtWebStartHelper" scope="request"/>
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
		<title id='tltObj'>Java Web Start</title>
		<script src="http://java.com/js/deployJava.js"></script>
    <script>
			
			//window.resizeTo(300, 200);
			deployJava.createWebStartLaunchButton("<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "url_to_load") %>");
			
		</script>
	</head>
	<body>
		<table class="x3" width="100%" height="100%">
			<tr>
				<td width="250px" height="20px"align="left" valign="center" bgcolor="lightsteelblue">
					<a id="aLinkObj" href="<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "url_to_load") %>">Click to start "<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "application_name") %>"</a>
				</td>
				<td>
				</td>
			</tr>
			<tr>
				<td>
				</td>
				<td>
				</td>	
			</tr>
		</table>
	</body>
</html>