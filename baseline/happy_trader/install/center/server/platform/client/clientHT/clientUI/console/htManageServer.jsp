<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtManageServer" scope="request"/>
<jsp:setProperty name="SC" property="*"/>
<%  SC.initialize(request,response); %>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<title id='tltObj' >Run-time Server Management</title>

		<script type="text/javascript">
			var generatePageMilisec_m = '<%=  SC.getStringSessionValue(SC.getContext(),request, "time_load_page")%>';

			function on_load()
			{
				setPageCaption(tltObj.innerText);

				// check id no running servers at all
				if (serversObj.options.length <=0) {
					// disable everything excepting add server option
					existServersTable.style.display = "none";
					noServersTable.style.display="inline";

				}
			}

			function do_refresh()
			{
				var url = "/HtManageServer?operation=do_refresh";
				url+="&server_id="+encodeURIComponent(serversObj.options(serversObj.selectedIndex).innerText);


				mForm.action = url;
				mForm.submit();
			}


		</script>
	</head>
	<body onload="on_load();">

		<table id="noServersTable" style="display:none" width=100% class=x8>
			<tr>
				<td align=left>
					No running trading servers
				</td>
			</tr>
		</table>

		<table id="existServersTable" class=x8 width=100%>

			<tr>
				<td colspan=5>
					<table class=x2 width=100%>
						<tr>
							<td align=left>
								Select Running Trading Server
							</td>
						</tr>

						<tr>
							<td>
								<select id="serversObj" style="width:100%" onchange="do_refresh();">
									<%= SC.getStringSessionValue(SC.getContext(),request, "running_server_list") %>
								</select>
							</td>
						</tr>

					</table>
				</td>
			</tr>

			<tr>
				<td height=20px>
				</td>
			</tr>

			<tr>
				<td height=20px>
					<a href="#" onclick="do_refresh();">Refresh</a>
				</td>
			</tr>
		</table>

		<form method="post" action="" id="mForm" style="display:none">
		</form>

	</body>
</html>

