<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtServerAlgorithmLoad_AlgBrkParam" scope="request"/>
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
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript" src="js/misc/property_table.js"></script>

		<script type="text/javascript">
			var xGrid_brk_param_m = null;
			var xGrid_alg_param_m = null;

			function on_close()
			{
				// properties
				// new key-value pair
				var algParams_values = new Array();
				var brkParams_values = new Array();

				var algParams_keys = new Array();
				var brkParams_keys = new Array();

				var outparams = new Array();

				for(i = 0; i < xGrid_alg_param_m.getRowsNum(); i++ ) {
					var rowId = xGrid_alg_param_m.getRowId(i);


					var key_cell = xGrid_alg_param_m.cells(rowId, 0).getValue()
					var value_cell = xGrid_alg_param_m.cells(rowId, 1).getValue()

					if (key_cell.length <= 0) {
						alert(i+" position in the algorithm parameter table contains invalid entry!");
						return;
					}

					algParams_keys[i] = key_cell;
					algParams_values[i] = value_cell;

				}
				// new key-value pair

				for(i = 0; i < xGrid_brk_param_m.getRowsNum(); i++ ) {
					var rowId = xGrid_brk_param_m.getRowId(i);


					var key_cell = xGrid_brk_param_m.cells(rowId, 0).getValue()
					var value_cell = xGrid_brk_param_m.cells(rowId, 1).getValue()

					if (key_cell.length <= 0) {
						alert(i+" position in the broker parameter table contains invalid entry!");
						return;
					}

					brkParams_keys[i] = key_cell;
					brkParams_values[i] = value_cell;
				}

				//
				outparams['op'] = 'close';
				outparams['brk_keys'] = brkParams_keys;
				outparams['alg_keys'] = algParams_keys;
				outparams['brk_values'] = brkParams_values;
				outparams['alg_values'] = algParams_values;
				outparams['see_thread_id'] = threadObj.options(threadObj.selectedIndex).value;
				outparams['propagate_thread_id'] = newThreadObj.options(newThreadObj.selectedIndex).value;


				window.returnValue = outparams;

				//

				xGrid_brk_param_m.destructor();
				xGrid_alg_param_m.destructor();
				window.close();

			}

			function on_cancel()
			{
				xGrid_brk_param_m.destructor();
				xGrid_alg_param_m.destructor();

				window.close();
			}

			function on_thread_change()
			{
				var thread_id = threadObj.options(threadObj.selectedIndex).value;
				var outparams = new Array();
				outparams['op'] = 'reload';
				outparams['see_thread_id'] = threadObj.options(threadObj.selectedIndex).value;
				outparams['page_uid']='<%=SC.getUniquePageId()%>';

				window.returnValue = outparams;
				window.close();
			}

			//
			function insert_new_row(obj)
			{
				addNewRowToXGrid(obj);
			}

			function delete_rows(obj)
			{
				deleteSelectedItems(obj);
			}

			function select_all_rows(obj)
			{
				selectXGridAllRows(obj);
			}

			function deselect_all_rows(obj)
			{
				deselectXGridAllRows(obj);
			}


		</script>

	</head>
	<body>
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>

			<tr class=x2>
				<td height=20px>
					<div align=left><%= SC.getStringSessionValue(SC.getUniquePageId(),request, "list_source") %></div>
				</td>
			</tr>

			<tr>
				<td align=left height=20px>
					<table class=x8 width=100%>
						<tr>
							<td>
								<select style="width=100%" id="threadObj" onchange="on_thread_change();">
									<option value=-1>ALL</option>
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "thread_list") %>
								</select>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr class=x2>
				<td height=20px>
					<div align=left>Select the target thread for the parameters. "ALL" means propagation for all the therads
					</div>
				</td>
			</tr>

			<tr>
				<td align=left height=20px>
					<table class=x8 width=100%>
						<tr>
							<td>
								<select style="width=100%" id="newThreadObj">
									<option value=-1>ALL</option>
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "thread_list") %>
								</select>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
			<td align=left height=20px>
				Algorithm Parameters
			</td>
			<tr>

			<tr>
				<td height=20px>

					<div id='alg_param_obj' style="width=100%;height:120px;border-style:solid; border-width:1px;">
						<script>
							xGrid_alg_param_m = new dhtmlXGridObject('alg_param_obj');

							xGrid_alg_param_m.imgURL = "imgs/grid/";
							xGrid_alg_param_m.setHeader("Name,Value");
							xGrid_alg_param_m.setInitWidths("250,*");
							xGrid_alg_param_m.setColAlign("left,left");
							xGrid_alg_param_m.setColTypes("ed,ed");
							xGrid_alg_param_m.setColSorting("str,str");
							xGrid_alg_param_m.enableMultiselect(true);
							xGrid_alg_param_m.init();
							xGrid_alg_param_m.loadXML("/HtReturnSessionVar?operation=get_session_var&session_var=alg_pair_parameters&page_uid=<%=SC.getUniquePageId()%>");


						</script>
					</div>

				</td>
			</tr>

			<tr>
				<td height=20px>
					<table width=100% class =x8>
						<tr>
							<td>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="deselect_all_rows(xGrid_alg_param_m);">Deselect All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="select_all_rows(xGrid_alg_param_m);">Select All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="insert_new_row(xGrid_alg_param_m);">Insert</a>
							</td>
							<td align=right width=80px>
								<a href="#" onclick="delete_rows(xGrid_alg_param_m);">Remove</a>
							</td>

						</tr>
					</table>
				</td>
			</tr>


			<tr>
			<td height=20px >
			</td>
			<tr>

			<tr>
			<td align=left height=20px>
				Broker Parameters
			</td>
			<tr>

			<tr>
				<td height=20px>

					<div id='brk_param_obj' style="width=100%;height:120px;border-style:solid; border-width:1px;">
						<script>
							xGrid_brk_param_m = new dhtmlXGridObject('brk_param_obj');

							xGrid_brk_param_m.imgURL = "imgs/grid/";
							xGrid_brk_param_m.setHeader("Name,Value");
							xGrid_brk_param_m.setInitWidths("250,*");
							xGrid_brk_param_m.setColAlign("left,left");
							xGrid_brk_param_m.setColTypes("ed,ed");
							xGrid_brk_param_m.setColSorting("str,str");
							xGrid_brk_param_m.enableMultiselect(true);
							xGrid_brk_param_m.init();
							xGrid_brk_param_m.loadXML("/HtReturnSessionVar?operation=get_session_var&session_var=brk_pair_parameters&page_uid=<%=SC.getUniquePageId()%>");


						</script>
					</div>


				</td>
			</tr>

			<tr>
				<td>
					<table id="brkParamManTblObj" width=100% class =x8>
						<tr>
							<td>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="deselect_all_rows(xGrid_brk_param_m);">Deselect All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="select_all_rows(xGrid_brk_param_m);">Select All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="insert_new_row(xGrid_brk_param_m);">Insert</a>
							</td>
							<td align=right width=80px>
								<a href="#" onclick="delete_rows(xGrid_brk_param_m);">Remove</a>
							</td>

						</tr>
					</table>
				</td>
			</tr>

			<tr>
			<td height=20px >
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