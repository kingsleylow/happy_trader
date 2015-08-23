<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtHistoryProviders" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<html>
	<head>


		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<title id='tltObj' >History Providers Administration</title>

		<!-- DHTML grid -->
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>
	
		-->
		
			<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<script type="text/javascript" src="js/misc/property_table.js"></script>

		

		<script type="text/javascript">

			var xGrid_hst_param_m = null;

			var xGridExporter_m = null;


			function on_load()
			{
				setPageCaption(tltObj.innerText);

				// check id no providers at all
				if (providersObj.options.length <=0) {
					// disable everything excepting add provider option
					existProvidersTable.style.display = "none";
					noProvidersTable.style.display="inline";

				}
				else {
					// normal
					xGrid_hst_param_m = new dhtmlXGridFromTable('param_obj');
					xGrid_hst_param_m.setImagePath("js/grid3/codebase/imgs/");
					xGrid_hst_param_m.enableMultiselect(true);

					xGridExporter_m = new ExportImportXGrid(xGrid_hst_param_m, "<%=SC.getUniquePageId() %>" );


				}
			}


			function add_new_provider()
			{
				var url = "/htHistoryProviders.jsp?operation=add_new_provider&new_provider=";

				var provider_name = window.showModalDialog('htHistoryProviders_addNewProvider.jsp', "", 'resizable:0;dialogHeight=150px;dialogWidth=330px;help:0;status:0;scroll:0');

				if(provider_name==null)
					return;

				if (provider_name.length <=0 ) {
					alert("You must enter valid provider name");
					return;
				}

				url += encodeURIComponent(provider_name);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function delete_provider()
			{
				if (window.confirm('Are you sure to remove the provider ' + get_cur_provider())==true) {
					var url = "/htHistoryProviders.jsp?operation=delete_provider&cur_provider="+get_cur_provider();
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}

			}

			function get_cur_provider()
			{
				if (providersObj.options.length > 0)
					return encodeURIComponent(providersObj.options(providersObj.selectedIndex).innerText);

				return "";
			}

			function refresh_page()
			{


				var url = "/htHistoryProviders.jsp?operation=refresh_page&cur_provider="+get_cur_provider();
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function allpy_changes()
			{
				var url = "/htHistoryProviders.jsp?operation=apply_changes&cur_provider=" + get_cur_provider();

				// new key-value pair
						
				//mForm.properties.value = getXGridAllRows(xGrid_hst_param_m, new Array(0,1));
				mForm.properties.value = xGridExporter_m.serializeAllRowsAsXmlParameter();
				url += "&export_hour_shift="+encodeURIComponent(expHourShiftObj.options(expHourShiftObj.selectedIndex).innerText);
				url += "&import_hour_shift="+encodeURIComponent(impHourShiftObj.options(impHourShiftObj.selectedIndex).innerText);

				var provider_class = providersClassObj.options(providersClassObj.selectedIndex).innerText;
				if (providersClassObj.options(providersClassObj.selectedIndex).invalid=='true') {
					alert("The provider class: \"" + provider_class + "\" is not registered. Please change it.");
					return;
				}

				url += "&provider_class=" + encodeURIComponent(provider_class);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

			

				mForm.action = url;
				mForm.submit();
			}

			function export_param()
			{
				xGridExporter_m.exportSelectedRows(get_cur_provider());

			}

			function import_param()
			{
				xGridExporter_m.importRows();
			}


			function insert_new_row()
			{
				addNewRowToXGrid(xGrid_hst_param_m);
			}

			function delete_rows()
			{
				deleteSelectedItems(xGrid_hst_param_m);
			}

			function select_all_rows()
			{
				selectXGridAllRows(xGrid_hst_param_m);
			}

			function deselect_all_rows()
			{
				deselectXGridAllRows(xGrid_hst_param_m);
			}


		</script>
	</head>

	<body onload="on_load();">

		<table id="noProvidersTable" style="display:none" width=100% height="20px" class=x8>
			<tr>
				<td align=left>
					<a href="#" onclick="add_new_provider();" >Add New History Provider</a>
				</td>
			</tr>
		</table>

		<table id="existProvidersTable" width="100%" height="100%" class=x8>

			<tr class="x2">
				<td align="left" height="20px">
					Select History Provider
				</td>
			</tr>

			<tr>
				<td height="20px">
					<table class="x8" width="100%">
						<tr>
							<td>
								<select id="providersObj" style="width:100%" onchange="refresh_page();">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "provider_list") %>
								</select>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height=20px bgcolor="buttonface">
					<table width=100% class=x8>
						<tr>
							<td>
							</td>



							<td width=110px align=left>
								<a href="#" onclick="delete_provider();">Delete Provider</a>
							</td>

							<td width=110px align=left>
								<a href="#" onclick="add_new_provider();" >Add New Provider</a>
							</td>


						</tr>
					</table>
				</td>
			</tr>


			<tr >
				<td height="20px">
					<table width=100% class=x8>
						<tr>
							<td align=left width=80px>
								Class Name
							</td>

							<td width=450px align=left>
								<select  id="providersClassObj" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "provider_classes") %>
								</select>
							</td>

							<td>
							</td>

						</tr>
					</table>

				</td>
			</tr>



			<tr >
				<td  height=20px>
					<table width=100% class =x8>
						<tr>

							<td align=left width=100px>
								Export Hour Shift
							</td>

							<td align=left width=60px>
								<select id="expHourShiftObj" style="width:100%" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "export_hours") %>
								</select>

							</td>

							<td width=20px>
							</td>


							<td align=left width=100px>
								Import Hour Shift
							</td>

							<td align=left width=60px>
								<select id="impHourShiftObj" style="width:100%" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "import_hours") %>
								</select>

							</td>

							

							<td>
							</td>


						</tr>
					</table>
				</td>
			</tr>




			<tr >
				<td  align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">

					<table id='param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "provider_parameters") %>
					</table>
					</div>
				</td>
			</tr>




			<tr >
				<td height="20px" bgcolor="buttonface">
					<table width=100% class =x8>
						<tr>
							<td class="x3" align=left width="38px">
								<a href="#" onclick="allpy_changes();">Apply</a>
							</td>

							<td>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="import_param();">Import From File</a>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="export_param();">Export to File</a>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="deselect_all_rows();">Deselect All</a>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="select_all_rows();">Select All</a>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="insert_new_row();">Insert</a>
							</td>

							<td  align=right width=95px>
								<a href="#" onclick="delete_rows();">Remove</a>
							</td>


						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height=20px>
				</td>
			</tr>

		</table>


		<form method="post" action="" id="mForm" style="display:none">
			<input type=hidden name="properties" value=""></input>
		</form>


	</body>
</html>
