<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<title></title>
		<script type="text/javascript">
			function on_load()
			{
				var args = window.dialogArguments;
				if (args != null) {
					runNameObject.value = args['run_name'];
					connectStrObject.value = args['connect_string'];
				}
			}

			function on_close()
			{
				if (connectStrObject.value.length <=0) {
					alert('Please enter a valid connection string');
					return;
				}

				var result = new Array();
				result['run_name'] = runNameObject.value;
				result['connect_string'] = connectStrObject.value;
				result['comment'] = commentStrObj.innerText;


				window.returnValue = result;
				window.close();
			}

			function on_cancel()
			{
				window.returnValue = null;
				window.close();
			}
		</script>

	</head>
	<body onload="on_load();">
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>
			<tr>
				<td align=left  class=x63 height="20px">
					Enter new connection run name:
				</td>
			</tr>

			<tr>
				<td align=left height="20px">
					<input style='width:100%' id="runNameObject" type=text ></input>
				</td>
			</tr>

			<tr>
				<td height="20px">
				</td>
			</tr>

			<tr>
				<td align=left  class=x63 height="20px">
					Enter new connection string:
				</td>
			</tr>

			<tr>
				<td align=left height="20px">
					<input style='width:100%' id="connectStrObject" type=text ></input>
				</td>
			</tr>

			<tr>
				<td align=left  class=x63 height="20px">
					Enter comment
				</td>
			</tr>

			<tr>
				<td align=left>
					<textarea style="width:100%;height:100%" id="commentStrObj" class="x4"></textarea>
				</td>
			</tr>


			<tr>
				<td height="20px">
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
			</tr>

			
		</table>
	</body>
</html>