<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtWelcome" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<script type="text/javascript" src="js/misc/helper.js"></script>
		<title id='tltObj'>Welcome page</title>
		<style>
			.parentDisable {
				z-index:999;
				width:100%;
				height:100%;
				display:none;
				position:absolute;
				top:0;
				left:0;
				background-color: #ccc;
				color: #aaa;
				opacity: 1;
				filter: alpha(opacity=100);
			}
			#popup {
				width:600;
				height:100;
				position:absolute;
				top:200px;
				left:200px;
				color: #000;
				background-color: #fff;
			}
		</style>


		<script>
			function pop() {
				document.getElementById('mainDiv').style.display='block';
				return false
			}
			function hide() {
				document.getElementById('mainDiv').style.display='none';
				return false
			}


			function on_load()
			{
				//setPageCaption(tltObj.innerText);

			}

			function advertize()
			{
				pop();
			}

		</script>
	</head>

	<body onload='on_load();'>
		<div id="mainDiv" class="parentDisable">
			<table border="0" id="popup" class="x8">

				<tr>
					<td align="left" colspan="2">
						<br>
						1. Create and manage real-time and history data sources
						<br>
						2. Create and manage algorithm code executing per tick arrivals or other event-based data
						<br>
						3. Create and manage broker related code responsible for communicating with a broker side. This can be not only a full automatic trading (using API) but half manual or completely manual trading mode. Architecturally this matches the server architecture.
						<br>
						4. Fast back testing as algorithm code/broker code to be implemented in C++.
						<br>
						5. Full logging capabilities including alerts
						<br>
						6. Algorithm code can be executed in different processes or even different machines thus allowing clusterizing and high reliability. Crash in a separate trading process will not cause the whole platform to die.
						<br>
						7. Trading/simulation results are stored in DB and can be analyzed including drawing charts.
						<br>
						
					</td>
				</tr>

				<tr>
					<td align="left">
						<a href="mailto:klavlin@gmail.com">klavlin@gmail.com</a>
					</td>
					<td>
						<a href="#" onClick="return hide()">Close</a>
					</td>
				</tr>

				<tr>
					<td>
					</td>
				</tr>
			</table>
		</div>


		<table width="100%" height="100%" border="0">
			<tr height=valign="middle">
				<td class="ITRHomeWelcome">
					<div align=center><a href="#" onclick="advertize()">Welcome to Happy Trader</a></div>
				</td>
			</tr>

			<tr bgcolor="buttonface" height="20px">
				<td class="x8">
					<div align=center>Version: <%= SC.getVersionInfo() %></div>
				</td>
			</tr>

			<tr bgcolor="buttonface">
				<td height="20px" class="x8">
					<div align="center" style="font-size:smaller">For any queries/issues please contact the author <a href="mailto:klavlin@gmail.com">klavlin@gmail.com</a></div>
				</td>
			</tr>


		</table>
	</body>
</html>