<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<base target="_self">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>
		
		<script type="text/javascript" src="js/misc/helper.js"></script>
		
		<script type="text/javascript">

			function on_load()
			{
			}
			
			function on_close()
			{
				window.returnValue = mForm.textAreaObj.innerText;
				window.close();

			}

			function on_cancel()
			{
				window.returnValue = null;
				window.close();
			}

			function do_send_file()
			{
				mForm.target="upload_target";
				mForm.package_data_file_path.value = mForm.package_data.value;
				
				
				if (mForm.package_data.value.length <=0) {
					alert("Please enter a file to upload");
					return;
				}

				// begins that process
				mForm.submit();

			}

			function on_frame_load()
			{
				var body = frames['upload_target'].document.getElementsByTagName("body")[0];
				var txtdata = body.getElementsByTagName("textarea")[0].innerText;
				mForm.textAreaObj.innerText = txtdata;
	
			}

		</script>
	</head>
	<body onload="on_load();">
		
		<table width="100%" height="100%" >
			<form method="post" action="/HtReturnFileContent.jsp?wrap_to_html=true&init_fun=on_frame_load" name="mForm" id="mForm" ENCTYPE="multipart/form-data">

				<tr class="x3">
					<td align="left" height="20px">
						Enter a file content
					</td>
				</tr>

				<tr>
					<td>
						<table width="100%" height="100%" class=x8>
							<tr>
								
								<td>
									<div style='border:0px solid black;overflow:hidden;width:expression(document.body.clientWidth - 10 );height:100%;'>
										<textarea wrap="off" style="overflow:auto;width:100%;height:100%;" ID="textAreaObj"></textarea>
									</div>
								</td>

								<td>
								<iframe id="upload_target" name="upload_target" src="" style="width:0;height:0;border:0px solid #fff;display:none;">
								</iframe>
								</td>

						</tr>
						</table>
					</td>
				</tr>

				<tr>
					<td align=left height="20px">
						<table width=100% height="100%" class=x8>
							<tr>
								<td>
									<input type="hidden" name="package_data_file_path" value="" style="display:none">
									<input type="file" value="" name="package_data" style="width:100%" >
								</td>
							</tr>
							<tr>
								<td align="left">
									<a href='#' onclick="do_send_file();">Read File</a>
								</td>
							</tr>
						</table>
					</td>
				</tr>

				<tr>
					<td height=20px>
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

			</form>
		</table>
		
		

	</body>

</html>