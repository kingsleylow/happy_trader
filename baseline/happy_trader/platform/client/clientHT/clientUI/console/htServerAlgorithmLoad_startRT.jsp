<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServerAlgorithmLoad_startRT" scope="request"/>
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
			function on_load()
			{
				var args = window.dialogArguments;
				if (args != null) {
					runNameObject.value = args;
				}
			}

			function on_close()
			{
				var result  = new Array();
				result['run_name'] = runNameObject.value;
				result['run_comment'] = runNameCommentObj.innerText;

				if (simProfileListObj.selectedIndex==0) {
					// no passed simulation profile
					result['sim_prof_pass']=0;
				}
				else {
					result['sim_prof_pass']=1;
					result['sim_profile_name']=simProfileListObj.options(simProfileListObj.selectedIndex).innerText;
				}

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
				<td align=left  class=x63>
					Please enter run name:
				</td>
			</tr>

			<tr>
				<td align=left >
					<input style='width=100%' id="runNameObject" type=text ></input>
				</td>
			</tr>

			<tr>
				<td align=left  class=x63>
					Please enter run comments
				</td>
			</tr>

			<tr>
				<td align=left height="110px">
					<textarea style="width:100%;height:100%" id="runNameCommentObj" class="x4"></textarea>
				</td>
			</tr>

			<tr>
				<td align=left  class=x63>
					Please enter simulation profile name
				</td>
			</tr>

			<tr>
				<td align=left width=190px>
					<select style='width:100%' id='simProfileListObj'>
						<option>No profile</option>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sim_profile_list_only") %>
					</select>
				</td>
			</tr>


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
			</tr>
		</table>
	</body>
</html>