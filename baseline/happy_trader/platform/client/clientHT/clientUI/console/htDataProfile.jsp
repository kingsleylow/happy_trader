<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtDataProfile" scope="request"/>
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
		<title id='tltObj' >Data Profile Administration</title>


		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<!--
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



		<script type="text/javascript">

			var xGrid_param_m = null;
			var profileOperationResult_m = <%= SC.getStringSessionValue(SC.getUniquePageId(), request, "profile_operation_result") %>;

			function on_load()
			{
				setPageCaption(tltObj.innerText);

				// check id no providers at all
				if (profilesObj.options.length <=0) {
					// disable everything excepting add provider option
					existProfilesTable.style.display = "none";
					noProfilesTable.style.display="inline";

				}else {
					// normal
					xGrid_param_m = new dhtmlXGridFromTable('param_obj');
					xGrid_param_m.setImagePath("js/grid3/codebase/imgs/");
				}
				
				
				if (profileOperationResult_m != 0) {
					// eroor happaned
					var textData = document.getElementById("warningObj").innerText;
					alert("You had problems with operation:\n" + textData);
				}
			}

			function synch_profile()
			{
				var url = "/htDataProfile.jsp?operation=synch_profile&cur_profile="+get_cur_profile() + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}
			
				
			function add_new_profile()
			{
				var url = "/htDataProfile.jsp?operation=add_new_profile&new_profile=";

				var profile_name = window.showModalDialog('htDataProfile_addNewProfile.jsp', "", 'resizable:0;dialogHeight=150px;dialogWidth=330px;help:0;status:0;scroll:0');

				if(profile_name==null)
					return;

				if (profile_name.length <=0 ) {
					alert("You must enter valid data profile name");
					return;
				}

				url += encodeURIComponent(profile_name);
				url += "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function delete_profile()
			{
				if (window.confirm('Are you sure to remove the profile: ' + get_cur_profile())==true) {
					var url = "/htDataProfile.jsp?operation=delete_profile&cur_profile="+get_cur_profile() + "&page_uid=<%=SC.getUniquePageId() %>";

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
				var url = "/htDataProfile.jsp?operation=refresh_page&cur_profile="+get_cur_profile() + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function change_timescale(provider, sObj)
			{
				// when we change tim scale for the provider need to reflect available mult factors


				var url="/htDataProfile.jsp?operation=update_temp&cur_profile="+get_cur_profile()+
					"&new_time_scale="+ encodeURIComponent(sObj.options(sObj.selectedIndex).value) +
					"&cur_provider="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function change_subscribed_rthist(provider, cbObj)
			{
				var url="/htDataProfile.jsp?operation=update_temp&cur_profile="+get_cur_profile()+
					"&new_subscribed="+cbObj.checked+
					"&cur_provider="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function change_subscribed_level1(provider, cbObj)
			{
				var url="/htDataProfile.jsp?operation=update_temp&cur_profile="+get_cur_profile()+
					"&new_subscribed_level1="+cbObj.checked+
					"&cur_provider="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function change_subscribed_drawobj(provider, cbObj)
			{
				var url="/htDataProfile.jsp?operation=update_temp&cur_profile="+get_cur_profile()+
					"&new_subscribed_drawobj="+cbObj.checked+
					"&cur_provider="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function change_multfactor(provider, mbObj)
			{
				var url="/htDataProfile.jsp?operation=update_temp&cur_profile="+get_cur_profile()+
					"&new_mult_factor="+encodeURIComponent(mbObj.options(mbObj.selectedIndex).value) +
					"&cur_provider="+encodeURIComponent(provider) + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();
			}

			function allpy_changes()
			{
				var url = "/htDataProfile.jsp?operation=apply_changes&cur_profile=" + get_cur_profile() + "&page_uid=<%=SC.getUniquePageId() %>";

				mForm.action = url;
				mForm.submit();

			}

		</script>
	</head>

	<body onload="on_load();">
		
		<div id="warningObj" style="display:none" width="1px" height="1px">
			<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "profile_operation_result_string") %>
		</div>

		<table id="noProfilesTable" style="display:none" width=100% height="100%" class=x8>
			<tr>
				<td height="20px" align=left>
					<a href="#" onclick="add_new_profile();" >Add New Data Profile</a>
				</td>
			</tr>
				<tr>
				<td>

				</td>
			</tr>

		</table>

		<table id="existProfilesTable" width=100% height="100%" class=x8>



			<tr class="x2">
				<td align="left" height="20px">
					Select Data Profile
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

							<td width=110px align=left>
								<a href="#" onclick="synch_profile();">Synch with DB</a>
							</td>


							<td width=110px align=left>
								<a href="#" onclick="delete_profile();">Delete Profile</a>
							</td>

							<td width=110px align=left>
								<a href="#" onclick="add_new_profile();" >Add New Profile</a>
							</td>


						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height="20px">
					<table class='x8' width="100%" height="100%">
						<tr>
							<td align="left" width="90px">
								Partition ID
							</td>
							<td align="left">
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "partition_id") %>
							</td>
							<td></td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td height=20px align=left>
					Parameters
				</td>
			</tr>

			<tr>
				<td  align="center">
					<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
					<table id='param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
						<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "profile_parameters") %>
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

	</body>
</html>

