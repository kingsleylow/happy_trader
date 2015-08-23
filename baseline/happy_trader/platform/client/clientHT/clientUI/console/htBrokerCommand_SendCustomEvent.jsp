<% if (request.getMethod().equalsIgnoreCase("GET")) { %><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><% } %>
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerCommand_SendCustomEvent" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<%
if (methodName.equalsIgnoreCase("GET")) {
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>

				
		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>

		<script type="text/javascript" src="js/misc/ajax_utils.js"></script>

		<title>Send Custom Data</title>
		<script type="text/javascript">
			var ajax_caller_m = new AjaxCaller(-1);
			function on_load()
			{
				
			}

			
			function on_click_cb()
			{
				if (non_block_mode_chb.checked) {
					// disable response
					document.getElementById('RespTiemoutObj').disabled = true;
					document.getElementById('RespTiemoutObj').style.backgroundColor = 'grey';


					document.getElementById('dataRespObj').disabled=true;
					document.getElementById('dataRespObj').style.backgroundColor = 'grey';
					
					
				}
				else {
					document.getElementById('RespTiemoutObj').disabled = false;
					document.getElementById('RespTiemoutObj').style.backgroundColor = 'window';
					
					// enable response
					document.getElementById('dataRespObj').disabled=false;
					document.getElementById('dataRespObj').style.backgroundColor = 'window';
					
					
				}
			}
			

			function on_send()
			{
				parameters = new Array();
			
				parameters['alg_brk_pair'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_brk_pair") %>";
				parameters['server_id'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>";
				parameters['target_thread'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>";

				parameters['custom_name'] = document.getElementById('eventNameObject').value;
				parameters['custom_data'] = document.getElementById('dataStrObj').innerText;
				
				parameters['time_out_sec'] = "-1";
				var RespTiemoutObj_obj = document.getElementById('RespTiemoutObj');
				if (RespTiemoutObj_obj.selectedIndex != 0) {
				  parameters['time_out_sec'] = RespTiemoutObj_obj.options(RespTiemoutObj_obj.selectedIndex).value;
				}
				

				parameters['no_wait_call'] = (non_block_mode_chb.checked ? 'true':'false');

				var data = "total_data="+AjaxHelperObject.assosiativeArrayToXmlParameter(parameters);

				var url = '\htBrokerCommand_SendCustomEvent.jsp?operation=send_custom_event&page_uid=<%=SC.getUniquePageId() %>';
			
				ajax_caller_m.makeCall("POST", url, data,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{

					// content
					if (ajax_caller_m.isResponseOk()) {
						if (!non_block_mode_chb.checked) {
							document.getElementById('dataRespObj').innerText = content;
							alert('Event was sent successfully in BLOCKING mode!');
						}
						else {
							document.getElementById('dataRespObj').innerText = content;
							alert('Event was sent successfully in NON-BLOCKING mode!');
						}

					}
					else {
						// should error happen need to decode this
						alert('Error occured: [' + ajax_caller_m.createErrorString()+']');
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

			function start_long_op()
			{
				document.body.disabled=true;
			}

			function stop_long_op()
			{
				document.body.disabled=false;
			}

		
		</script>
		<style type="text/css">
			html,body{
				height:100%;
				margin:0px;
				padding:0px
			}
		</style>

	</head>
	<body onload="on_load();">
		<table cellpadding="2" cellspacing="2" width="100%" height="100%" border="0" class=x8>
			<tr>
			  <td align=left  height="20px">
				<table class=x63>
				  <tr>
					<td bgcolor=buttonface>Target Algorithm/Broker Pair</td>
					<td class=x5z><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_brk_pair") %></td>
					<td bgcolor=buttonface>Server ID</td>
					<td class=x5z><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %></td>
					<td bgcolor=buttonface>Target Thread</td>
					<td class=x5z><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %></td>
					<td></td>
				  </tr>
				</table>  
			  </td>
			</tr>
		  
			<tr>
			  <td align=left  class=x63 height="20px" bgcolor='buttonface'>
					Enter event name:
				</td>
			</tr>

			<tr>
				<td align=left height="20px">
					<input style='width:100%' id="eventNameObject" type=text ></input>
				</td>
			</tr>


			<tr>
				<td align=left  class=x63 height="20px" bgcolor='buttonface'>
					Enter data
				</td>
			</tr>

			<tr>
				<td valign=top align=left height=200px>
					<textarea style="overflow:scroll;wrap:off;width:100%;height:200px" id="dataStrObj" class="x4"></textarea>
				</td>
			</tr>

			<tr>
				<td align=left  class=x63 height="20px" bgcolor='buttonface'>
					Response
				</td>
			</tr>

			<tr>
				<td style='border: 1px solid black;' valign=top align=left id="dataRespObj">
					<!-- <textarea readonly="true" style="overflow:scroll;wrap:off;width:100%;height:100%" id="dataRespObj" class="x4"></textarea> -->
				</td>
			</tr>


			<tr>
				<td height="20px">
					<table width=100% class=x8>
						<tr>
							<td align="left" width="180px">
								<input type="checkbox" onclick="on_click_cb()" id="non_block_mode_chb">Do not wait for a response</input>
							</td>

							<td width=20px>
							</td>
						  
							<td width=130px>
							  Response Timeout (Sec)
							</td>
						  
						  
							<td width=100px>
							  <select id="RespTiemoutObj">
								<option>
								  Default (<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "def_resp_timeout") %>)
								</option>  
								<option>
								  20
								</option>  
								<option>
								  30
								</option>  
								<option>
								  60
								</option>  
								<option>
								  120
								</option> 
							  </select>
							</td>
						  
							<td align=right>
								<a href='#' onclick="on_send();">Send</a>
							</td>


							
						</tr>
					</table>

				</td>
			</tr>


		</table>
	</body>
</html>

<%
}
%>