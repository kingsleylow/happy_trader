<%
String package_uid = request.getParameter("package_uid");
String package_name = request.getParameter("package_name");
String status = request.getParameter("status");


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
				var url = "/HtReadFile.jsp?target_read=external_package&package_uid=<%= package_uid %>";
				// ajax

				makeAsynchXmlHttpRequestExternHandler(
				getXmlHttp(),
					-1,
				'GET',
				url,
				null,
				function(loader)
				{
					if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {

						dataDivObj.innerHTML = loader.responseText;

					}
					else {

						alert('No data arrived');
					
					}
				}	,

				function(errmsg)
				{
					alert('Internal error on loading data: '+errmsg);
					
				}
			);

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
									External JAR file: [<%= package_name %>] [<%= status%>]
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



	</body>

</html>