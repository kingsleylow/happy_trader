<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtStartStopServer_AllowedEvents" scope="request"/>
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

			function on_close()
			{
				// prepare parameters
				// subscribed events
				var subscr_events = new Array();
				var cnt = 0;
				for(i = 2; i <= defEventObj.rows.length; i++) 
				{
					var row_i = defEventObj.rows(i-1);

					var event_cell = row_i.cells(0);
					var check_cell = row_i.cells(1);
					var cb = check_cell.all(0);

					if (cb.checked) {
						subscr_events[cnt] = event_cell.internal_event_type;
						cnt++;
					}

				}

				var log_level = loglevelObj.options(loglevelObj.selectedIndex).value;

				//
				var results = new Array();
				results['log_level'] = log_level;
				results['subscr_events'] = subscr_events;

				window.returnValue = results;
				window.close();
			}

			function on_cancel()
			{
				window.close();
			}
		</script>
	</head>
	<body>
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>

			<tr class=x2>
				<td height=20px>
					<div align=left>Select server log level</div>
				</td>
			</tr>

			<tr>
				<td align=left height=20px>
					<table class=x8 width=100%>
						<tr>
							<td>
								<select style="width:100%" id="loglevelObj" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "server_log_levels") %>
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

			<tr class=x2>
				<td height=20px>
					<div align=left>Select Allowed Events</div>
				</td>
			</tr>

			<tr>
				<td height=20px>
					<div style="width:100%;height:120px;overflow-y:scroll;border-style:solid; border-width:1px;">
						<table cellpadding="2px" cellspacing="2px" id="defEventObj" width=100% class=x8>
							<tr class=x1y>
								<td width=100px align=left>
									Event type
								</td>

								<td width=100px  align=left>
									Is Subscribed
								</td>

								<td>
								</td>
							</tr>
							<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "event_subscr_list") %>
						</table>
					</div>
				</td>
			</tr>


			<tr>
				<td height=20px>
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


		</table>
	</body>
</html>