<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtExternalApiRun_v2_EditParameters" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
else if (methodName.equalsIgnoreCase("GET")) {
		SC.initialize_Get(request,response);
}
%>

<%
String op = request.getParameter("operation");
if (op != null) {
	
if (methodName.equalsIgnoreCase("GET") || 
				(	methodName.equalsIgnoreCase("POST") && op.equalsIgnoreCase("initial_load")) ||
				(	methodName.equalsIgnoreCase("POST") && op.equalsIgnoreCase("initial_load_template"))
) {
	// make this output
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj' >Edit Parameters</title>



		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<link href="css/tbl.css" rel="stylesheet" type="text/css"/>

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>
		<script type="text/javascript" src="js/misc/ajax_utils.js"></script>

		<script type="text/javascript">

			var ajax_caller_m = new AjaxCaller(-1);
			function on_load()
			{
				

			}
			
			function start_long_op()
			{
				document.body.disabled=true;
			}

			function stop_long_op()
			{
				document.body.disabled=false;
			}
						
			function do_download_file()
			{
				mForm.action = "/htExternalApiRun_v2_EditParameters.jsp?operation=download";
				
				mForm.work_uid.value = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "work_uid") %>";
				mForm.package_name.value = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "package_name") %>";
				mForm.xml_content.value = document.getElementById('dataObjObj').innerText;

				mForm.submit();
			}
			
<%
	if (SC.getStringSessionValue(SC.getUniquePageId(),request, "is_template").equalsIgnoreCase("true")) {
%>
			function do_save_to_template()
			{
								
				parameters = new Array();
				parameters['work_uid'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "work_uid") %>";
				parameters['package_name'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "package_name") %>";
				parameters['xml_content'] = document.getElementById('dataObjObj').innerText;
				
				var url = '/htExternalApiRun_v2_EditParameters.jsp?operation=save_to_template';
				var data = "total_data="+AjaxHelperObject.assosiativeArrayToXmlParameter(parameters);

							
				
				ajax_caller_m.makeCall("POST", url, data,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					
					if (ajax_caller_m.isResponseOk()) {
						alert('Template: ' + '<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "package_name") %>' +' updated successfully');
					}
					else {
						alert('Error occured: ' + ajax_caller_m.createErrorString());
					}

					
					
				},

				function()
				{
					start_long_op();
				}
				,
				function()
				{

					stop_long_op();
					
				}

			);
			}
<%
	}
%>


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
									Editing parameters for JAR file: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "package_name") %>
								</div>
							</td>
						</tr>
						
						<tr height=20px>
							<td style="font:larger" class="x3">
								<div align=left>
									Module version: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "version") %>
								</div>
							</td>
						</tr>

					</table>
				</td>
			</tr>

			<tr width=100%>
				<td align=left>


					<table class=x8 width=100% height=100% cellpadding="2" cellspacing="2">
						<tr>
							<td align=left valign=top >
								<textarea id="dataObjObj" STYLE="width:100%;height:100%" WRAP="off"><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "xml") %></textarea>
							</td>

						</tr>
					</table>



				</td>
			</tr>

			<tr >
				<td colspan="4" height="20px" bgcolor="buttonface">
					<table width=100% class =x8>
						<tr>
							<td class="x3" align=left width="160px">
								<a href="#" onclick="do_download_file();">Download Updated File</a>
							</td>
<%
	if (SC.getStringSessionValue(SC.getUniquePageId(),request, "is_template").equalsIgnoreCase("true")) {
%>
							<td class="x3" align=left width="160px">
								<a href="#" onclick="do_save_to_template();">Update Template</a>
							</td>
<%
	}
%>

							<td>
							</td>


						</tr>
					</table>
				</td>
			</tr>


		</table>

	</body>
	<form method="post" action="" id="mForm" style="display:none">
		<input type=hidden name="work_uid" value=""></input>
		<input type=hidden name="package_name" value=""></input>
		<input type=hidden name="xml_content" value=""></input>
	</form>

</html>

<%
	}
}
%>