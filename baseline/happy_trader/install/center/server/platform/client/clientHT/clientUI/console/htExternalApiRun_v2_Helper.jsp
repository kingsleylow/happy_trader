<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtExternalApiRun_v2_Helper" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		
		<title id='tltObj' >Run Custom Packages</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>
		
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript">


			function on_load()
			{
				var url = "/HtExternalApiRun_ServletThread.jsp?run_uid=<%= SC.getRunUid()%>";

				var iframe_elem = document.getElementById("frmContentObj");
				iframe_elem.src = url;

			}

			function insertHTMLpiece_c(data)
			{
				dataDivObj.innerHTML += data;
			}

			function tellFinished_c()
			{
				HeaderObj.innerHTML = "Finished <%= SC.getOriginalPackageName() %>";
			}


		</script>

	</head>

	<body onload="on_load();">
		<table class=x8 width=100% height=100%>

			<tr height=20px >
				<td align=left>
					<table width="100%" height="100%" class=x8 >


						<tr height=20px>
							<td style="font:larger" class="x3">
								<div id="HeaderObj" align=left>
									Running External JAR file: <%= SC.getOriginalPackageName() %>
								</div>
							</td>
						</tr>

					</table>
				</td>
			</tr>



			<!-- data -->

			<tr width=100%>
				<td align=left>


					<table class=x8 width=100% height=100% cellpadding="2" cellspacing="2">
						<tr>
							<td align=left valign=top >
								<div id="dataDivObj" class="tbl_data">
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

		<iframe id="frmContentObj" width=1px height=1px style="display:none"></iframe>

	</body>

</html>