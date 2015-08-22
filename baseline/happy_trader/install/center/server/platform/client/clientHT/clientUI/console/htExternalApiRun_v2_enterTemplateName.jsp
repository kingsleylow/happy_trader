<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<title></title>
		<script type="text/javascript">
			function on_close()
			{
				window.returnValue = templateNameObject.value;
				window.close();
			}

			function on_cancel()
			{
				window.close();
			}
			
			function on_load()
			{
			  var args = window.dialogArguments;
			  if (args != null) {
				templateNameObject.value = args;
			  }
			}
			
		</script>

	</head>
	<body onload="on_load()">
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>
			<tr>
			<td align=left  class=x63>
				Please enter new template name file:
			</td>
			<tr>

			<tr>
			<td align=left >
				<input style='width=100%' id="templateNameObject" type=text ></input>
			</td>
			<tr>


			<tr>
			<td>
				<table width=100% class=x8>
					<tr>
						<td>
						</td>
						<td align=right width=40px>
							<a href='#' onclick="on_cancel();">Cancel</a>
						</td>


						<td align=right width=40px>
							<a href='#' onclick="on_close();">OK</a>
						</td>
					</tr>
				</table>

			</td>
			<tr>
		</table>
	</body>
</html>