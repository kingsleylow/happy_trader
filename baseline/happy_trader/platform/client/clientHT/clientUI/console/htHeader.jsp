<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtHeader" scope="request"/>
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

		<script type="text/javascript">
			function navigate_logout()
			{
				var tFrame = top;
				if (tFrame != null) {
					tFrame.navigate("/htLoginHelper.jsp?operation=logout");
				}


			}
		</script>

	</head>
	<body>
		<table width="100%" border="0">
			<tr>
				<td colspan="4"><img src="imgs/pbs.gif">
					<span class="ITRHomeWelcome">
						Happy Trader Management Console
						<img src="imgs/pbs.gif">
					</span>
				</td>

				<td width="1%">&nbsp;</td>

				<td width="2%">
					<table cellpadding="0" cellspacing="0" border="0">
						<tr>
							<td valign="bottom"><a href="htWelcome.jsp" class="xy" target="basefrm" >Home</a></td>
							<td valign="bottom">&nbsp;&nbsp;</td>
							<td valign="bottom"><a onclick='navigate_logout();' href="#" class="xy">Logout</a></td>
							<td valign="bottom">&nbsp;&nbsp;</td>
						</tr>
					</table>
				</td>

			</tr>
		</table>

		<% if (SC.areCriticalErrors()) {%>
		<table width="100%" border="0" class="ITRHomeWelcome">
			<tr>
				<td height=20px bgcolor=red>
					You have critical errors!
				</td>
			</tr>
		</table>

		<% }%>

		<table cellpadding="0" cellspacing="0" border="0" width="100%" id="table1Main" >

			<tr>

				<td width="100%">
					<table cellpadding="0" cellspacing="0" border="0" width="100%">
						<tr>
							<td><img src="imgs/cghes.gif" width="34" height="8"></td>
							<td width="100%" style="background-image:url(imgs/cghec.gif);"></td>
							<td><img src="imgs/cghee.gif" width="5" height="8"></td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td width="100%">

					<table cellpadding="0" cellspacing="0" border="0" width="100%">

						<tr>
							<td width="110px" class="x8" >
								<div align="left">You are logged as</div>
							</td>

							<td class="x3">
								<div align="left"><%=SC.getUserName()%></div>
							</td>
						</tr>
					</table>


				</td>
			</tr>

		</table>


	</body>
</html>
