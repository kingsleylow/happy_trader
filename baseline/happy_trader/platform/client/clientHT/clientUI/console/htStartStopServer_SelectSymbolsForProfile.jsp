<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtStartStopServer_SelectSymbolsForProfile" scope="request"/>
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
		
		<!-- helper -->
		<script type="text/javascript" src="js/misc/helper.js"></script>

		<title>Select Symbols</title>
		<script type="text/javascript">
			function on_close()
			{
				var outparams = new Array();
				outparams['profile_data_out'] = textAreaObj.innerText;
				
				window.returnValue = outparams;
				window.close();

				
			}

			function on_validate()
			{
				// need to determin the nuber of columns, trim
				// and if necessary - if one column - add provider name
				var csvArray = CSVToArray(textAreaObj.innerText);

				
				var new_text = "";
				var current_provider = "";
				
				for(i = 0; i < csvArray.length; i++) {
					var row_arr = csvArray[i];

					// just trim whatever we have
					var symbol_i = "";
					if (row_arr.length == 1) {
						symbol_i = row_arr[0].replace(/^\s*/, "").replace(/\s*$/, "");
					}
					else if (row_arr.length==2) {
						current_provider = row_arr[0].replace(/^\s*/, "").replace(/\s*$/, "");
						symbol_i = row_arr[1].replace(/^\s*/, "").replace(/\s*$/, "");
					}
					else {
						alert('Invalid format, must be at least one column!');
						return;
					}

					if (current_provider.length <=0) {
						alert('Need to have a provider name in the first column');
						return;
					}

					if (i<(csvArray.length-1))
						new_text += current_provider+","+ symbol_i+"\n";
					else
						new_text += current_provider+","+ symbol_i;
				}

				// set up
				textAreaObj.innerText = new_text;
				alert('Finish validation, processed: ' + csvArray.length + ' symbol(s)');

			}

			function on_load()
			{
				var args = window.dialogArguments;
				if (args != null) {
										
					if (args['force_read']==false) {
						// use already passed
						textAreaObj.innerText = "";
						var out = "";
						var arraySymb = args['profile_data'];
						var i = 0;
						while(i < arraySymb.length) {
							if (i < (arraySymb.length-1))
								out+= arraySymb[i++] + "," +arraySymb[i++]+"\n";
							else
								out+= arraySymb[i++] + "," +arraySymb[i++];
						}
						
						textAreaObj.innerText = out;
					}

					// now just calculate
					
					hdrObj.innerText = "Edit the list of symbols for the profile: " + args['profile_name'];
					
				}
			}

			function on_cancel()
			{
				window.returnValue = null;
				window.close();
			}

			function on_reset()
			{
				// reser
				var outparams = new Array();
				outparams['profile_data_out'] = null;

				window.returnValue = outparams;
				window.close();
			}

			function on_clear()
			{
				textAreaObj.innerText = "";
			}

		</script>
	</head>
	<body onload="on_load();">
		<table cellpadding="2" cellspacing="2" width="100%" height="100%" border="0" class=x8>
			<tr>
				<td id="hdrObj" align="left" height=20px class=x63>
					Edit the list of symbols for the profile:
				</td>
			</tr>

			<tr>
				<td height=100%>
						<table width=100% height="100%" class=x8>
							<tr>
								<td>
									<div style='border:0px solid black;overflow:hidden;width:expression(document.body.clientWidth - 10 );height:100%;'>
										<textarea wrap="off" style="overflow:auto;width:100%;height:100%;" ID="textAreaObj"><%= SC.getStringSessionValue(SC.getUniquePageId(), request, "provider_symbols") %></textarea>
									</div>
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

							<td align=right width=60px>
								<a href='#' onclick="on_validate();">Validate</a>
							</td>

							<td align=right width=60px>
								<a href='#' onclick="on_clear();">Clear</a>
							</td>


							<td align=right width=60px>
								<a href='#' onclick="on_reset();">Reset</a>
							</td>

							<td align=right width=60px>
								<a href='#' onclick="on_cancel();">Cancel</a>
							</td>


							<td align=right width=60px>
								<a href='#' onclick="on_close();">OK</a>
							</td>
						</tr>
					</table>

				</td>
			</tr>

		</table>
	</body>
</html>
