<!--
This page is going to place new order and expect response arrived from DB
-->
<!-- This page will show requests to broker and related responses-->
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerCommand_PlaceOrder" scope="request"/>
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

		<title>Place Order for session: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "cur_session_id") %></title>

		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- main calendar program -->
		<script type="text/javascript" src="js/calendar.js"></script>

		<!-- language for the calendar -->
		<script type="text/javascript" src="js/lang/calendar-en.js"></script>

		<!-- the following script defines the Calendar.setup helper function, which makes
			 adding a calendar a matter of 1 or 2 lines of code. -->
		<script type="text/javascript" src="js/calendar-setup.js"></script>

		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>

		<script type="text/javascript" src="js/misc/ajax_utils.js"></script>

		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>


		<script type="text/javascript">
			// price const
			var PC_ZERO =			0; // price is zero
			var PC_BEST_BID = 1; // best bid
			var PC_BEST_ASK = 2;  // best ask
			var PC_INPUT =		3;  // use price as it is in the input field

			var POLL_TIMEOUT_MSEC = 1000;
			var START_POLL_CAPTION = "Start Polling";
			var STOP_POLL_CAPTION = "Stop Polling";


			var ajax_caller_m = new AjaxCaller(-1);
			var responseUrl_m = null;
			var parentOrderId_m = null;

			var historyGrid_m = null;

			function on_load()
			{

				startStopPollingResponse.value=START_POLL_CAPTION;

				historyGrid_m = new dhtmlXGridObject("broker_resp_div_obj");


				historyGrid_m.setImagePath("js/grid3/codebase/imgs/");
				historyGrid_m.setEditable(false);
				historyGrid_m.setHeader("Session ID, Session create time, Session connect string, Is simulation, Response ID, Issue time, Operation time, Parent Order ID, Broker comment, Broker internal code, Broker result code, Broker result detail code, Broker position ID, Broker Order ID, Operation price, Operation STOP price, Operation TP price, Operation SL price, Operation volume, Operation Remaining Volume, RT provider, Symbol, Expiration time, Validity flag, Position Type, Comissions,Context");
				historyGrid_m.setInitWidths("200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200");
				historyGrid_m.setColTypes("ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro");
				historyGrid_m.setColAlign("left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left,left");
				historyGrid_m.setColSorting("na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na,na");
				historyGrid_m.enableResizing("true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true");
				historyGrid_m.enableTooltips("false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false");

				historyGrid_m.init();
			}

			function start_long_op()
			{
				document.body.disabled=true;
				sendOrderBtn.disabled = true;
				
				
			}

			function stop_long_op()
			{
				document.body.disabled=false;
				sendOrderBtn.disabled = false;
				
			}

			function start_stop_polling_response()
			{
				if (window.event.srcElement.value == START_POLL_CAPTION) {
					start_polling_response();
				}
				else if (window.event.srcElement.value == STOP_POLL_CAPTION) {
					stop_polling_response();
				}
			}
			
			function start_polling_response()
			{
				
				responseUrl_m ="\htBrokerCommand_PlaceOrder.jsp?operation=poll_broker_response&page_uid=<%=SC.getUniquePageId() %>" +
					"&session_id=<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "session_id") %>";


				if (radioPollCriteriaObj[0].checked) {
					if (orderIdObj.value == null || orderIdObj.value.length <=0) {
						alert('Order UID must be set');
						return;
					}

					responseUrl_m += "&parent_order_uid=" + encodeURIComponent(orderIdObj.value);
				}
				else if (radioPollCriteriaObj[1].checked) {

					if (brokerOrderIdObj.value == null || brokerOrderIdObj.value.length <= 0) {
						alert('Broker Order ID must be set');
						return;
					}
					responseUrl_m += "&broker_order_id=" + encodeURIComponent(brokerOrderIdObj.value);
				}
				else if (radioPollCriteriaObj[2].checked) {
					
					if (brokerPositionIdObj.value == null || brokerPositionIdObj.value.length <= 0) {
						alert('Broker Position ID must be set');
						return;
					}
					responseUrl_m += "&broker_position_id=" + encodeURIComponent(brokerPositionIdObj.value);
				}

				startStopPollingResponse.value=STOP_POLL_CAPTION;
				sendOrderBtn.disabled = true;

				setTimeout("do_polling_response()",POLL_TIMEOUT_MSEC);

			}

			function stop_polling_response()
			{
				sendOrderBtn.disabled = false;
				responseUrl_m = null;
				parentOrderId_m = null;

				startStopPollingResponse.value=START_POLL_CAPTION;
				
			}

			function do_polling_response()
			{
				if (responseUrl_m != null) {
					// continue
					setTimeout("do_polling_response()",POLL_TIMEOUT_MSEC);

					// make actions
					ajax_caller_m.makeCall("POST", responseUrl_m, '',
					function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
					{
						if (ajax_caller_m.isResponseOk()) {

							historyGrid_m.clearAll();
							historyGrid_m.parse(content);
						
						}
						else {
							alert('Exception on polling: '+ajax_caller_m.createErrorString()+', stopping...');
							stop_polling_response();
						}
					},

					function()
					{

					}
					,
					function()
					{


					}

				);
					
				}
			}

			
			
			function on_close()
			{

			
			}

			function do_invoke()
			{
				var response = create_response();
				
				var data = "total_data="+AjaxHelperObject.assosiativeArrayToXmlParameter(response, "request");
							
				var url = '\htBrokerCommand_PlaceOrder.jsp?operation=send_order&page_uid=<%=SC.getUniquePageId() %>';
				
				ajax_caller_m.makeCall("POST", url, data,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					
					parentOrderId_m=content;
					orderIdObj.value = parentOrderId_m;
					if (ajax_caller_m.isResponseOk()) {
						alert('Order was successfully sent, ID: ' + parentOrderId_m);
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

			function priceTypeChanged()
			{
				if (priceSelectObj.options(priceSelectObj.selectedIndex).value==PC_INPUT) {
					priceObj.disabled=false;
				}
				else
					priceObj.disabled = true;
			}

			function stopPriceTypeChanged()
			{
				if (stopPriceSelectObj.options(stopPriceSelectObj.selectedIndex).value==PC_INPUT) {
					stopPriceObj.disabled=false;
				}
				else
					stopPriceObj.disabled = true;
			}

			function create_response()
			{
				parameters = new Array();
				parameters['session_id'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "session_id") %>";
				parameters['alg_brk_pair'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_brk_pair") %>";
				parameters['server_id'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>";
				parameters['target_thread'] = "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>";
				parameters['broker_seq_num']=	"<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>";
				
				parameters['order_type'] = parseInt(orderTypeDivObj.innerText,16);

				parameters['volume'] = volumeObj.value;
				parameters['stop_loss'] = slObj.value;
				parameters['take_profit'] = tpObj.value;
				parameters['price'] = priceObj.value;
				parameters['stop_price'] = stopPriceObj.value;
				parameters['price_type'] = priceSelectObj.options(priceSelectObj.selectedIndex).value;
				parameters['stop_price_type'] = stopPriceSelectObj.options(stopPriceSelectObj.selectedIndex).value;

				parameters['validity'] = validityPriceSelectObj.options(validityPriceSelectObj.selectedIndex).value;
				parameters['expiration']  = date_expirdate.value;

				parameters['comment'] = commentObj.value;
				parameters['method'] = methodObj.value;
				parameters['place'] = placeObj.value;
				parameters['additional_info'] = additionalInfoObj.value;

				parameters['symbol'] = symbolObj.value;
				//parameters['symbol_denom']  =symbDenomObj.value;
				parameters['provider']  =prividerObj.value;

				parameters['brk_order_id']  =brkOrderIdObj.value;
				parameters['brk_pos_id']  =brkPosIdObj.value;
				parameters['context_str'] = contextObj.value;
				


				return parameters;
			}

			function on_ordertype_checkbox_change()
			{
				var orderType = parseInt(orderTypeDivObj.innerText,16);
				
				var input_obj = window.event.srcElement;
				var checkbox_val = parseInt(input_obj.value);
				if (input_obj.checked) {
					 
					orderType = orderType | checkbox_val;
					
				} else {
					orderType = orderType & (~checkbox_val);
					
				}

				
				orderTypeDivObj.innerText = orderType.toString(16);
			}

			
		</script>

	</head>
	<body onload="on_load();">
		<table cellpadding="1" cellspacing="1" width="100%" height="100%" border="0" class=x8>
			<tr>
				<td width="400px" style='border: 1px solid black'>
					<table cellpadding="1" cellspacing="1" width="100%" height="100%" border="0" class=x8>



						<tr>
							<td width=160px align=left height="20px">
							Select Order Type
							</td>

							<td>

								<div style="height:80;overflow:auto;border: 1px solid black">
									<table cellpadding=1 cellspacing=1 width='100%' height='100%' class=x8>
										<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "all_order_types") %>
									</table>
								</div>

							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Order Type
							</td>

							<td>
								<div style='border: 1px solid black' id="orderTypeDivObj">0</div>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Broker Order ID
							</td>

							<td>
								<input id='brkOrderIdObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Broker Position ID
							</td>

							<td>
								<input id='brkPosIdObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Volume
							</td>

							<td>
								<input id='volumeObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Stop Loss
							</td>

							<td>
								<input id='slObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Take Profit
							</td>

							<td>
								<input id='tpObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Price
							</td>

							<td>

								<table width="100%" height="100%" class="x8">
									<tr>
										<td>
											<input id='priceObj' style='width:100%'></input>
										</td>
										<td width="120px">
											<select style="width:100%" id="priceSelectObj" onchange="priceTypeChanged();">
												<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "available_price_types") %>
											</select>
										</td>
									</tr>
								</table>
							</td>

						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Stop Price
							</td>

							<td height="20px">

								<table width="100%" height="100%" class="x8">
									<tr>
										<td>
											<input id='stopPriceObj' style='width:100%'></input>
										</td>
										<td width="120px">
											<select style="width:100%" id="stopPriceSelectObj" onchange="stopPriceTypeChanged();">
												<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "available_price_types") %>
											</select>
										</td>
									</tr>
								</table>
							</td>

						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Validity
							</td>

							<td>
								<select style="width:100%" id="validityPriceSelectObj">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "available_validity_types") %>
								</select>
							</td>
						</tr>

						<tr valign=top>
							<td align=left width=100px height="20px">
							Expiration Date
							</td>

							<td>

								<table width=100% cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
									<tr>
										<td width=120px>
											<input style="width: 100%" class=x8 type="text" name="date_expirdate" id="f_date_e" />
										</td>

										<td>
											<img src="imgs/img.gif" id="f_trigger_expirdate" style="cursor: pointer; border: 1px solid red;" title="Date selector"
													 onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
										</td>

									</tr>
								</table>


								<script type="text/javascript">
									Calendar.setup({
										inputField     :    "date_expirdate",     // id of the input field
										ifFormat       :    "%d-%m-%Y %H:%M",      // format of the input field
										button         :    "f_trigger_expirdate",  // trigger for the calendar (button ID)
										align          :    "Tl",           // alignment (defaults to "Bl")
										singleClick    :    true,
										timeFormat     :    "24",
										showsTime      :    true
									});
								</script>

							</td>
						</tr>

						<!-- -->

						<tr>
							<td width=160px align=left height="20px">
							Comment
							</td>

							<td>
								<input id='commentObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Symbol
							</td>

							<td>
								<input id='symbolObj' style='width:100%'></input>
							</td>
						</tr>

						

						<tr>
							<td width=160px align=left height="20px">
							Provider
							</td>

							<td>
								<input id='prividerObj' style='width:100%'></input>
							</td>
						</tr>

						<!-- -->
						<tr>
							<td width=160px align=left height="20px">
							Method
							</td>

							<td>
								<input id='methodObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Place
							</td>

							<td>
								<input id='placeObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left height="20px">
							Additional Info
							</td>

							<td>
								<input id='additionalInfoObj' style='width:100%'></input>
							</td>
						</tr>
						
						<tr>
							<td width=160px align=left height="20px">
							Context (String only)
							</td>

							<td>
								<input id='contextObj' style='width:100%'></input>
							</td>
						</tr>

						<tr>
							<td width=160px align=left>
							</td>

							<td>
							</td>
						</tr>


					</table>
				</td>

				<!-- broker response list -->
				<td  style='border: 1px solid black'>
					<table cellpadding="1" cellspacing="1"  width="100%" height="100%" class="x8">
						<tr>
							<td height="20px" style='border: 1px solid black'>
								Server ID: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "server_id") %>
							</td>

							<td height="20px" style='border: 1px solid black' >
								Thread ID: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "target_thread") %>
							</td>

							<td height="20px" style='border: 1px solid black'>
								Alg/Brk Pair: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_brk_pair") %>
							</td>

							<td height="20px" style='border: 1px solid black'>
								Session ID: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "session_id") %>
							</td>
							
							<td height="20px" style='border: 1px solid black'>
								Broker #: <%= SC.getStringSessionValue(SC.getUniquePageId(),request, "broker_seq_num") %>
							</td>
						</tr>

						<tr>
							<td colspan="5>
								<div id="broker_resp_div_obj" style='height:100%; width:100%; '></div>
							</td>
						</tr>

						<tr>
							<td height="20px" colspan="5" style='border: 1px solid black'>
								<table cellpadding="1" cellspacing="1"  width="100%" height="100%" class="x8">
									<tr>
										<td width="130px" align="left">
											<INPUT type=radio name="radioPollCriteriaObj" CHECKED>Issued Order UID</input>
										</td>

										<td width="260px" align="left">
											<input type="text" id="orderIdObj"  value="" style="width:100%"></input>
										</td>

										<td>
										</td>
									<tr>
										
									<tr>
										<td width="130px" align="left">
											<INPUT type=radio name="radioPollCriteriaObj" >Broker Order ID</input>
										</td>

										<td width="260px" align="left">
											<input type="text" id="brokerOrderIdObj"  value="" style="width:100%"></input>
										</td>

										<td>
										</td>
									<tr>

									<tr>
										<td width="130px" align="left">
											<INPUT type=radio name="radioPollCriteriaObj" >Broker Position ID</input>
										</td>

										<td width="260px" align="left">
											<input type="text" id="brokerPositionIdObj"  value="" style="width:100%"></input>
										</td>

										<td>
										</td>
									<tr>

								</table>
							</td>
						</tr>

					</table>
					
				</td>
				<!-- -->

			</tr>

			<tr>
				<td align="left" style='border: 1px solid black' colspan="2">

					<table width="100%" class="x8">
						<tr>
							<td align="left" width="10px">
								<input id="sendOrderBtn" type="button" onclick="do_invoke()" value="Send Order"></input>
							</td>
							<td align="left" width="10px">
								<input id="startStopPollingResponse"  type="button" onclick="start_stop_polling_response();" value=""></input>
							</td>

							<td>
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