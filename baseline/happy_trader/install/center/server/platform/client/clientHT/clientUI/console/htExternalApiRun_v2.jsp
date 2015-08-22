<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtExternalApiRun_v2" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
		SC.initialize_Get(request,response);
}
else if (methodName.equalsIgnoreCase("POST")) {
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
		<title id='tltObj' >Run Custom Packages</title>


		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script type="text/javascript" src="js/misc/helper.js"></script>


		<!-- NEW GRID -->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>

		<script type="text/javascript" src="js/misc/xgrid_utils.js"></script>


		<!-- AJAX CALLER -->
		<script type="text/javascript" src="js/misc/base64.js"></script>
		<script type="text/javascript" src="js/misc/ajax_caller.js"></script>

		<!-- AJAX UTILS -->
		<script type="text/javascript" src="js/misc/ajax_utils.js"></script>




		<script type="text/javascript">

			var ajax_caller_m = new AjaxCaller(-1);
			
			function custom_date_sort(a,b,order)
			{
				
			}
			

			function on_load()
			{
				setPageCaption(tltObj.innerText);

				xGrid_subscr_events_m = new dhtmlXGridFromTable('ext_run_pack_obj');
				xGrid_subscr_events_m.setImagePath("js/grid3/codebase/imgs/");
				xGrid_subscr_events_m.setColSorting("na,na,na,na");

				xGrid_subscr_events_m
			};


			function do_run_template()
			{
				
				if (mForm.template_jar_obj.options.length <=0) {
					alert('Nothing to run');
					return;
				}

				mForm2.action = "/htExternalApiRun_v2_Helper.jsp?operation=load_as_template&template_name=" +
					encodeURIComponent(getTemplateNameFromList());

				mForm2.target = "Executing template jar..." + (new Date()).getTime();


				// begins that process
				mForm2.submit();
			}
			
			function do_run()
			{
				mForm.package_data_file_path.value = mForm.package_data.value;

				if (mForm.package_data.value.length <=0) {
					alert("Please enter a jar package!");
					return;
				}

				mForm.action = "/htExternalApiRun_v2_Helper.jsp?operation=load_locally";
				mForm.target = "Executing jar..." + (new Date()).getTime();

				// begins that process
				mForm.submit();
			}

			function do_upload_as_template()
			{
				if (mForm.package_data.value.length <=0) {
					alert("Please enter a jar package!");
					return;
				}
				
				var f = pathToFile(mForm.package_data.value);
				
				var result = window.showModalDialog('htExternalApiRun_v2_enterTemplateName.jsp', f.name, 'resizable:0;dialogHeight=150px;dialogWidth=400px;help:0;status:0;scroll:0');
				if (!result)
				  return;
				
				var new_file_name = result+ "." + f.extension;
				
				
				mForm.package_data_file_path.value = mForm.package_data.value;
				
				mForm.target="upload_target";
				mForm.action = "/HtUploadFile.jsp?operation=upload_template_package&new_file_name="+encodeURIComponent(new_file_name);
				

				mForm.submit();
				
			}

			function on_frame_load()
			{
				// this text data is output
				

				var body = frames['upload_target'].document.getElementsByTagName("body")[0];
				var txtdata = body.getElementsByTagName("textarea")[0].innerText;
				
				alert(txtdata);
				do_refresh();
				
			}


			function do_edit_parameters_template()
			{
				// calls the window for editing parameters
				if (mForm.template_jar_obj.options.length <=0) {
					alert('Nothing to edit');
					return;
				}

				mForm2.action = "/htExternalApiRun_v2_EditParameters.jsp?operation=initial_load_template&template_name="+
					encodeURIComponent(getTemplateNameFromList());
				mForm2.target="_blank";

				mForm2.submit();
			}
			

			function do_edit_parameters()
			{
				// calls the window for editing parameters
				mForm.package_data_file_path.value = mForm.package_data.value;
				if (mForm.package_data.value.length <=0) {
					alert("Please enter a jar package!");
					return;
				}

				mForm.action = "/htExternalApiRun_v2_EditParameters.jsp?operation=initial_load";
				mForm.target="_blank";

				mForm.submit();
			}

			
			function do_refresh()
			{
				window.navigate('/htExternalApiRun_v2.jsp');
			}

			function open_log(uid, package_name, status)
			{
				
				var url = "/htExternalApiRun_v2_ReadLogFile.jsp?package_uid="+encodeURIComponent(uid)
					+"&package_name="+encodeURIComponent(package_name)+"&status="+encodeURIComponent(status);

				window.open(url);
				
			}

			
			function load_jar_file(uid, file_name)
			{
				
				var url = "/HtReadFile.jsp?package_uid="+encodeURIComponent(uid)+"&target_read=external_package_jar";
				window.open(url, '_blank');

			
			}

			function do_unselect_all()
			{
				
				for(i = 0; i < xGrid_subscr_events_m.getRowsNum(); i++ ) {
					xGrid_subscr_events_m.cells( xGrid_subscr_events_m.getRowId(i), 5 ).setValue(XgridHelperObject.CHECKBOX_UNCHEKED);
				}
			}

			function do_select_all()
			{
				for(i = 0; i < xGrid_subscr_events_m.getRowsNum(); i++ ) {
					xGrid_subscr_events_m.cells( xGrid_subscr_events_m.getRowId(i), 5 ).setValue(XgridHelperObject.CHECKBOX_CHEKED);
				}
			}

			function do_delete()
			{
				var uid_list = new Array();

				var idx = 0;
				for(i = 0; i < xGrid_subscr_events_m.getRowsNum(); i++ ) {
					var is_checked = XgridHelperObject.get_checkbox_status2(xGrid_subscr_events_m.cells( xGrid_subscr_events_m.getRowId(i), 5 ).getValue());
					if (is_checked) {
						uid_list[idx++] = xGrid_subscr_events_m.cells( xGrid_subscr_events_m.getRowId(i), 0 ).getValue();
					}
				}
				
				if (window.confirm('Are you sure to remove the selected items?' )==false) {
					return;
				}


				var data = "total_data="+AjaxHelperObject.assosiativeArrayToXmlParameter(uid_list);
				var url = '/htExternalApiRun_v2.jsp?operation=remove_entries&page_uid=<%=SC.getUniquePageId() %>';

				// make actions
				ajax_caller_m.makeCall("POST", url, data,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					if (ajax_caller_m.isResponseOk()) {

						alert(content);

						// refresh
						do_refresh();

					}
					else {
						alert('Exception on operation: '+ajax_caller_m.createErrorString());
							
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
			
			function getTemplateNameFromList()
			{
				//_file_name
				//var tname = mForm.template_jar_obj.options(mForm.template_jar_obj.selectedIndex).innerText;
				
				var frm = document.getElementById("mForm");
				if (frm == null)
					return null;
				
				var optionVar = frm.template_jar_obj.options(mForm.template_jar_obj.selectedIndex);
				var tname = optionVar._file_name;
				if (tname == null || tname == undefined)
					return null;
				
				//alert(tname);
		
				return tname;
			}

			function do_delete_template()
			{
				if (mForm.template_jar_obj.options.length <=0) {
					alert('Nothing to remove');
					return;
				}
				
				if (window.confirm('Are you sure to remove the template: ' + getTemplateNameFromList())==false) {
					return;
				}

				var url = '/htExternalApiRun_v2.jsp?operation=delete_template&page_uid=<%=SC.getUniquePageId() %>'+
					'&template_name=' + encodeURIComponent(getTemplateNameFromList());

				// make actions
				ajax_caller_m.makeCall("POST", url, null,
				function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
				{
					if (ajax_caller_m.isResponseOk()) {

						alert(content);

						// refresh
						do_refresh();

					}
					else {
						alert('Exception on operation: '+ajax_caller_m.createErrorString());

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



		</script>
	</head>

	<body onload="on_load();">

		<table width=100% height="100%">
			<form method="post" action="" name="mForm" id="mForm" ENCTYPE="multipart/form-data">

				<iframe id="upload_target" name="upload_target" src="" style="width:0;height:0;border:0px solid #fff;display:none;">
				</iframe>

				<tr class=x8>
					<td align=left height="20px">
						Upload an external package to run
					</td>

					<td></td>
					<td></td>
					<td></td>
				</tr>


				<tr class=x3>

					<td align=left width=100px height=20px>
						<input type="hidden" name="package_data_file_path" value="" style="display:none">
						<input type="file" value="" name="package_data" size='80' >
					</td>

					<td width="100px" align="left">
						<table class="x8" width="100%">
							<tr>

								<td width=20px align=left>
									<input type="button" onclick="do_run();" value="Run"></input>
								</td>
								<td width=20px align=left>
									<input type="button" onclick="do_edit_parameters();" value="Edit Parameters"></input>
								</td>
								<td width=20px align=left>
									<input type="button" onclick="do_upload_as_template();" value="Upload as Template"></input>
								</td>
								
								<td>
								</td>
							</tr>
						</table>
					</td>

					<td></td>
					<td></td>
					


				</tr>


				<tr class=x8>
					<td align=left height="20px">
						Select a jar from template storage
					</td>

					<td></td>
					<td></td>
					<td></td>
				</tr>


				<tr class=x3 height="20px">
					<td>
						<select style="width:100%" name="template_jar_obj" id="template_jar_obj" ><%= SC.getStringSessionValue(SC.getUniquePageId(), request, "template_jars") %></select>
					</td>


					<td width="100px" align="left">
						<table class="x8" width="100%">
							<tr>

								<td width=20px align=left>
									<input type="button" onclick="do_run_template();" value="Run"></input>
								</td>
								<td width=20px align=left>
									<input type="button" onclick="do_edit_parameters_template();" value="Edit Parameters"></input>
								</td>

								<td width=20px align=left>
									<input type="button" onclick="do_delete_template();" value="Delete Template"></input>
								</td>

								<td>
								</td>
							</tr>
						</table>
					</td>


										
					<td></td>
					<td></td>
				</tr>

				<tr class=x8>
					<td height=20px></td>
					<td></td>
					<td></td>
					<td></td>
				</tr>

				<tr class=x8>
					<td align=left colspan="4">

						<div style="
								 border: 1px solid black;
								 overflow:auto;
								 width:expression(document.body.clientWidth - 40 );
								 height:100%
								 ">
							<table  id='ext_run_pack_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "external_run_packages") %>
							</table>
						</div>

					</td>
				</tr>

				<tr >
					<td colspan="4" height="20px" bgcolor="buttonface">
						<table width=100% class =x8>
							<tr>
								<td class="x3" align=left width="38px">
									<a href="#" onclick="do_refresh();">Refresh</a>
								</td>

								<td class="x3" align=left width="45px">
									<a href="#" onclick="do_delete();">Delete</a>
								</td>

								<td class="x3" align=left width="67px">
									<a href="#" onclick="do_select_all();">Select All</a>
								</td>

								<td class="x3" align=left width="80px">
									<a href="#" onclick="do_unselect_all();">Unselect All</a>
								</td>

								<td>
								</td>


							</tr>
						</table>
					</td>
				</tr>

			</form>
		</table>


		<form method="post" action="" id="mForm2" style="display:none">
		</form>

	</body>


</html>
<%
}
%>



