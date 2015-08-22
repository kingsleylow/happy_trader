<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtSimulationProfile" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
if (SC.dispatchJspCall(request,response)) {
%>


<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>
		<title id='tltObj' >Simulation Profiles Administration</title>

				

		<!-- helper -->

		
		<!-- OLD GRID
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">
		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
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



			var xGrid_sim_profile_m = null;

			var xGridExporter_m = null;


		

			// //
			function on_load()
			{
				setPageCaption(tltObj.innerText);
			
				
				// check id no profiles at all
				if (profilesObj.options.length <=0) {
					// disable everything excepting add server option
					existProfilesTable.style.display = "none";
					noProfilesTable.style.display="inline";

				}
				else {
						xGrid_sim_profile_m = new dhtmlXGridFromTable('sim_param_obj');
						xGrid_sim_profile_m.setImagePath("js/grid3/codebase/imgs/");
						xGrid_sim_profile_m.enableMultiselect(true);
					
						xGridExporter_m = new ExportImportXGrid(xGrid_sim_profile_m, "<%=SC.getUniquePageId() %>" );

				}

				
			}

			function add_new_profile()
			{
				var url = "/htSimulationProfile.jsp?operation=add_new_profile&new_profile=";

				var profile_name = window.showModalDialog('htSimulationProfile_addNewProfile.jsp', "", 'resizable:0;dialogHeight=400px;dialogWidth=330px;help:0;status:0;scroll:0');

				if(profile_name==null)
					return;

				if (profile_name.length <=0 ) {
					alert("You must enter valid simulation profile name");
					return;
				}

				url += encodeURIComponent(profile_name);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function delete_profile()
			{
				if (window.confirm('Are you sure to delete the simulation profile: '+get_cur_profile())==true) {
					var url = "/htSimulationProfile.jsp?operation=delete_profile&cur_profile="+get_cur_profile();
					url += "&page_uid=<%=SC.getUniquePageId() %>";

					mForm.action = url;
					mForm.submit();
				}

			}

			function get_cur_profile()
			{
				if (profilesObj.options.length > 0)
					return encodeURIComponent(profilesObj.options(profilesObj.selectedIndex).innerText);

				return "";
			}

			function refresh_page()
			{
				var url = "/htSimulationProfile.jsp?operation=refresh_page&cur_profile="+get_cur_profile();
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function apply_changes()
			{

				var url = "/htSimulationProfile.jsp?operation=apply_changes&cur_profile="+get_cur_profile();

				// pass the whole table
				//url += "&properties=" + encodeURIComponent(serializeXGridToParameter(xGrid_sim_profile_m, new Array(0,1,2,3,4,5,6,7,8,9,10,11) ));
				url += "&gen_tick_method=" + encodeURIComponent(genTicksMethodObj.options(genTicksMethodObj.selectedIndex).value);
				url += "&data_aggr_period=" +  encodeURIComponent(dataAggrPeriodsObj.options(dataAggrPeriodsObj.selectedIndex).value);
				url += "&mult_factor=" + encodeURIComponent(MultFactorObj.value);
				url += "&tick_split_num=" + encodeURIComponent(TickSplitNumObj.value);
				url += "&tick_usage="+ encodeURIComponent(tickUsageObj.options(tickUsageObj.selectedIndex).value);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mFormSaver.properties.value = xGridExporter_m.serializeAllRowsAsXmlParameter();

				
				mFormSaver.action = url;
				mFormSaver.submit();
			}

			function export_param()
			{
				xGridExporter_m.exportSelectedRows(get_cur_profile());
			}


			function delete_rows()
			{
				deleteSelectedItems(xGrid_sim_profile_m);
			}

			function select_all_rows()
			{
				selectXGridAllRows(xGrid_sim_profile_m);
			}

			function deselect_all_rows()
			{
				deselectXGridAllRows(xGrid_sim_profile_m);
			}

			function insert_new_row()
			{
				 addNewRowToXGrid(xGrid_sim_profile_m);
			}

		</script>
	</head>

	<body onload="on_load();">

		<table id="noProfilesTable" style="display:none" width=100%  height="20px" class=x8>
			<tr>
				<td align=left>
					<a href="#" onclick="add_new_profile();" >Add New Simulation Profile</a>
				</td>
			</tr>
		</table>

		<table id="existProfilesTable" width=100%  height="100%" class=x8>

		

			<tr class="x2">
				<td align="left" height="20px">
					Select Simulation Profile
				</td>
			</tr>


			<tr>
				<td height="20px">
					<table class="x8" width="100%">
						<tr>
							<td>
								<select id="profilesObj" style="width:100%" onchange="refresh_page();">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "profile_list") %>
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



							<td width=170px align=left>
								<a href="#" onclick="delete_profile();">Delete Simulation Profile</a>
							</td>

							<td width=170px align=left>
								<a href="#" onclick="add_new_profile();" >Add New Simulation Profile</a>
							</td>


						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td  height=20px>
					<table width=100% class =x8>
						<tr>
							<td align=left width=180px>
								Ticks Generation Method
							</td>

							<td align=left width=160px>
								<select id="genTicksMethodObj" style="width:100%" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "gen_ticks_methods") %>
								</select>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td  height=20px>
					<table width=100% class =x8>
						<tr>
							<td align=left width=180px>
								Data Aggregation Period
							</td>

							<td align=left width=160px>
								<select id="dataAggrPeriodsObj" style="width:100%" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "data_aggr_periods") %>
								</select>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td  height=20px>
					<table width=100% class =x8>
						<tr>
							<td align=left width=180px>
								Multiplication Factor
							</td>

							<td align=left width=160px>
								<input id="MultFactorObj" style="width:100%"
											 value='<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "mult_factor") %>'>
								</input>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td  height=20px>
					<table width=100% class =x8>
						<tr>
							<td align=left width=180>
								Ticks Split Number
							</td>

							<td align=left width=160px>
								<input id="TickSplitNumObj" style="width:100%"
											 value='<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "tick_split_num") %>'>
								</input>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td  height=20px>
					<table width=100% class =x8>
						<tr>
							<td align=left width=180px>
								Tick Usage
							</td>

							<td align=left width=160px>
								<select id="tickUsageObj" style="width:100%" >
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "tick_usage") %>
								</select>
							</td>

							<td>
							</td>
						</tr>
					</table>
				</td>

			</tr>

			<tr>
				<td height=20px>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					Parameters
				</td>
			</tr>

			<tr>
				<td align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
					<table id='sim_param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "simul_params") %>
					</table>
					</div>

					
				</td>
			</tr>

			<tr >
				<td height="20px" bgcolor="buttonface">
					<table width=100% class =x8>
						<tr>
							<td class="x3" align=left width="38px">
								<a href="#" onclick="apply_changes();">Apply</a>
							</td>

							<td>
							</td>

							<td align=right width=95px>
								<a href="#" onclick="export_param();">Export to File</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="deselect_all_rows();">Deselect All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="select_all_rows();">Select All</a>
							</td>

							<td align=right width=80px>
								<a href="#" onclick="insert_new_row();">Insert</a>
							</td>
							<td align=right width=80px>
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

		</form>

		<form method="post" action="" id="mFormSaver" style="display:none">
			<input type=hidden name="properties" value=""></input>
		</form>
	</body>
</html>

<%
}
%>