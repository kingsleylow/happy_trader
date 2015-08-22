<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtAlgLibPair_v2" scope="request"/>
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

				<!--
				<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">
				<script  src="js/grid/dhtmlxcommon.js"></script>
				<script  src="js/grid/dhtmlxgrid.js"></script>
				<script  src="js/grid/dhtmlxgridcell.js"></script>
				<script  src="js/grid/dhtmlxgrid_start.js"></script>
				-->


				<!-- NEW GRID -->
				
				<!--

					<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
					<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
					<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
					<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
					<script  src="js/grid3/codebase/ext/dhtmlxgrid_start.js"></script>
				-->
				<!-- DHTML XGRID -->
				<link rel="stylesheet" type="text/css" href="js/d/codebase/dhtmlxgrid.css">
				<script type="text/javascript" src="js/d/codebase/dhtmlxcommon.js"></script>
				<script type="text/javascript" src="js/d/codebase/dhtmlxgrid.js"></script>
				<script type="text/javascript" src="js/d/codebase/dhtmlxgridcell.js"></script>
				<script type="text/javascript" src="js/d/codebase/ext/dhtmlxgrid_start.js"></script>

					<script type="text/javascript" src="js/misc/helper.js"></script>

					<script type="text/javascript" src="js/misc/property_table.js"></script>

					<!-- AJAX CALLER -->
					<script type="text/javascript" src="js/misc/base64.js"></script>
					<script type="text/javascript" src="js/misc/ajax_caller.js"></script>
					<script type="text/javascript" src="js/misc/ajax_utils.js"></script>



					<link id="webfx-tab-style-sheet" type="text/css" rel="stylesheet" href="js/tabpane/css/tab.webfx.css" />
					<script type="text/javascript" src="js/tabpane/js/tabpane.js"></script>


					<!-- MODAL -->
					<link  type="text/css" rel="stylesheet" href="js/modal/modalhelper.css" />
					<script type="text/javascript" src="js/modal/modalhelper.js"></script>
				
					<title id='tltObj' >Algorithm/Broker Pair Administration</title>


					<script type="text/javascript">

						var xGrid_alg_param_m = null;
						var xGrid_brk_param_m = null;
						var xGrid_brk_param2_m = null;

						var xGrid_alg_paramExporter_m = null;
						var xGrid_brk_paramExporter_m = null;
						var xGrid_brk2_paramExporter_m = null;
						var ajax_caller_m = new AjaxCaller(-1);
			
						var tabpaneobj_m = null;
						var modalObj_m = null;

						function start_long_op()
						{
							document.body.disabled=true;
						}

						function stop_long_op()
						{
							document.body.disabled=false;
						}
						
						
						
						function lib_item_clicked(lib_path, trg_elem)
						{
							var trg_elem_obj = document.getElementById(trg_elem);
							if (trg_elem_obj == null)
							{
								alert('Internal error on populating library name');
								return;
							}
							
							trg_elem_obj.value = lib_path;
							modalObj_m.hide();
						}
						
						function str_replace(search, replace, subject) {
							return subject.split(search).join(replace);
						}
						
						function populateDivWithLibs(content, trg_elem)
						{
							
								
							
							var libs_arr = JSON.parse(content);
							var data = ModalDialog.createDivContentFromList(libs_arr, on_click_fun, trg_elem);
							
							modalObj_m.show(500, 300, data);
							
						}
						
						function on_click_fun(entry, trg_elem)
						{
							return "onclick=\"lib_item_clicked('"+str_replace("\\", "\\\\", entry)+"','"+trg_elem+"')\"";
						}
			
					

						function do_invoke_files_read(operation, trg_elem)
						{
							var url = '/HtAlgLibPair_v2_GetDlls.jsp?operation='+operation;
							
				
							ajax_caller_m.makeCall("GET", url, null,
							function(is_timeout, http_status, http_status_text, custom_code, custom_string, content)
							{
					
								if (ajax_caller_m.isResponseOk()) {
									//alert(content);
									if (content != null) {
										
										try {
											//alert(content);
											populateDivWithLibs(content, trg_elem);
										}
										catch(e)
										{
											alert('When trying to show the list exception happened: [' + e.description + ']');
										}
									}
						
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
			

						function on_load()
						{
				
							setPageCaption(tltObj.innerText);

							// check id no providers at all
							if (pairsObj.options.length <= 0) {
								// disable everything excepting add provider option
								existPairTable.style.display = "none";
								noPairTable.style.display="inline";

							} else 	{
					
								xGrid_alg_param_m = new dhtmlXGridFromTable(document.getElementById('alg_param_obj'));
								xGrid_alg_param_m.setImagePath("js/grid3/codebase/imgs/");
								xGrid_alg_param_m.enableMultiselect(true);
								xGrid_alg_paramExporter_m	= new ExportImportXGrid(xGrid_alg_param_m, "<%=SC.getUniquePageId() %>" );
				

								xGrid_brk_param_m = new dhtmlXGridFromTable(document.getElementById('brk_param_obj'));
								xGrid_brk_param_m.setImagePath("js/grid3/codebase/imgs/");
								xGrid_brk_param_m.enableMultiselect(true);
								xGrid_brk_paramExporter_m	= new ExportImportXGrid(xGrid_brk_param_m, "<%=SC.getUniquePageId() %>" );
					
								xGrid_brk_param2_m = new dhtmlXGridFromTable(document.getElementById('brk_param_obj_2'));
								xGrid_brk_param2_m.setImagePath("js/grid3/codebase/imgs/");
								xGrid_brk_param2_m.enableMultiselect(true);
								xGrid_brk2_paramExporter_m	= new ExportImportXGrid(xGrid_brk_param2_m, "<%=SC.getUniquePageId() %>" );
					
								//setupAllTabs();
								//var tabPaneObjectObj = document.getElementById( "tabPaneObject" );
								//tabpaneobj_m = new WebFXTabPane( tabPaneObjectObj, false );
					
							
				
								modalObj_m = new ModalDialog();


							}

							if (<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "is_broker_here")%>==1) {
								document.getElementById('Brk1Enabled').checked=true;
							}
							else {
								document.getElementById('Brk1Enabled').checked=false;
							}
				
							if (<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "is_broker_here_2")%>==1) {
								document.getElementById('Brk2Enabled').checked=true;
							}
							else {
								document.getElementById('Brk2Enabled').checked=false;
							}
				
							selectenabledbrokers();


				
				
						}

						function add_new_pair()
						{
							var url = "/htAlgBrkPair_v2.jsp?operation=add_new_pair&new_pair=";

							var pair_name = window.showModalDialog('htAlgBrkPair_addNewPair.jsp', "", 'resizable:0;dialogHeight=150px;dialogWidth=330px;help:0;status:0;scroll:0');

							if(pair_name==null)
								return;

							if (pair_name.length <=0 ) {
								alert("You must enter valid pair name");
								return;
							}

							url += encodeURIComponent(pair_name);
							url += "&page_uid=<%=SC.getUniquePageId() %>";

							mForm.action = url;
							mForm.submit();
						}

						function delete_pair()
						{
							if (window.confirm('Are you sure to remove the pair: ' + get_cur_pair())==true) {
								var url = "/htAlgBrkPair_v2.jsp?operation=delete_pair&cur_pair="+get_cur_pair();
								url += "&page_uid=<%=SC.getUniquePageId() %>";

								mForm.action = url;
								mForm.submit();
							}

						}

						function get_cur_pair()
						{
							if (pairsObj.options.length > 0)
								return encodeURIComponent(pairsObj.options(pairsObj.selectedIndex).innerText);

							return "";
						}

						function refresh_page()
						{
							var url = "/htAlgBrkPair_v2.jsp?operation=refresh_page&cur_pair="+get_cur_pair();
							url += "&page_uid=<%=SC.getUniquePageId() %>";

							mForm.action = url;
							mForm.submit();
						}

						//
						function allpy_changes()
						{
							var url = "/htAlgBrkPair_v2.jsp?operation=apply_changes&cur_pair=" + get_cur_pair();

							// new key-value pair
							
											
							var form = document.getElementById('mForm');
							form.alg_param.value = xGrid_alg_paramExporter_m.serializeAllRowsAsXmlParameter();
							form.brk_param.value = xGrid_brk_paramExporter_m.serializeAllRowsAsXmlParameter();
							form.brk_param2.value = xGrid_brk2_paramExporter_m.serializeAllRowsAsXmlParameter();
			
							form.alg_path.value = algPathObj.value;

							// if broker is set
							if (document.getElementById('Brk1Enabled').checked) {

								form.brk_path.value = document.getElementById('brkPathObj').value;
							}
				
							if (document.getElementById('Brk2Enabled').checked) {

								form.brk_path2.value = document.getElementById('brkPathObj_2').value;
							}

							url += "&page_uid=<%=SC.getUniquePageId() %>";

							mForm.action = url;
							mForm.submit();
						}

						function export_param(obj, postfx)
						{
							obj.exportSelectedRows(get_cur_pair()+'_'+postfx);
						}

						function import_param(obj)
						{
							obj.importRows();
				
						}

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


						function selectenabledbrokers()
						{
				
				
				
							if (Brk1Enabled.checked) {
								document.getElementById('brk1TableObject').style.display='inline';
							}
							else {
								document.getElementById('brk1TableObject').style.display='none';
							}
				
							if (Brk2Enabled.checked) {
								document.getElementById('brk2TableObject').style.display='inline';
							}
							else {
								document.getElementById('brk2TableObject').style.display='none';
							}
					
						}
			
			
			

						function select_path(inpObj)
						{

							var rsv = selectLocalPath();
							if (rsv.length > 0)
								inpObj.value = rsv;

						}

					
			
						function populate_brk_dll()
						{
							do_invoke_files_read('brk_dll_list', 'brkPathObj');
							
						}

						function populate_alg_dll()
						{
							
							do_invoke_files_read('alg_dll_list', 'algPathObj');
						}
			
						function populate_brk_2_dll()
						{
							do_invoke_files_read('brk_dll_list', 'brkPathObj_2');
						}
						
					</script>
					</head>

					<body onload="on_load();">

						<table id="noPairTable" style="display:none;width:100%;height:20px"  class=x8>
							<tr>
								<td align=left>
									<a href="#" onclick="add_new_pair();" >Add New Algorithm Broker Pair</a>
								</td>
							</tr>
						</table>

						<table id="existPairTable" style="width:100%;height:100%" class=x8>

							<tr class="x2">
								<td align="left" height="20px">
									Select Algorithm Broker Pair
								</td>
							</tr>

							<tr>
								<td height="20px">
									<table class="x8" width="100%">
										<tr>
											<td>
												<select id="pairsObj" style="width:100%" onchange="refresh_page();">
													<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "pair_list") %>
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
											<td align="left" width=80px>
												<input type="checkbox" id="Brk1Enabled" onclick="selectenabledbrokers()">Broker 1</input>
											</td>
											<td align="left" width="80px">
												<input type="checkbox" id="Brk2Enabled" onclick="selectenabledbrokers()">Broker 2</input>
											</td>

											<td>

											</td>

											<td width=80px align=left>
												<a href="#" onclick="delete_pair();">Delete Pair</a>
											</td>

											<td width=80px align=left>
												<a href="#" onclick="add_new_pair();" >Add New pair</a>
											</td>


										</tr>
									</table>
								</td>
							</tr>

							

							<tr>   
								<td  style='vertical-align:top;'>
									<div class="tab-pane" id="tabPaneObject" style='width:100%;height:420px;' >
										
										<!-- alg page -->
										<div class="tab-page" id="alg_pane_obj" style="width:95%;height:85%;border: 1px solid black;">
											<h2 class="tab">Algorithm</h2>	

											<table width="100%" height="100%" class=x8>
												<tr>
													<td height="20px">
														<table width=100% class=x8>
															<tr>
																<td align=left width=200px>
																	Algorithm Parameters
																</td>
																<td align=left width=75px>
																	Library Path
																</td>

																<td align=left>
																	<input id="algPathObj" value="<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "alg_path")%>" style="width:100%"/>
																</td>

																<td width="30px" align="left">
																	<a href="#" onclick="populate_alg_dll();">Populate</a>
																</td>



															</tr>
														</table>
													</td>
												</tr>


												<tr>
													<td align="center" style='vertical-align:top;height:300px'>
														<div  style='width:99%;height:99%;border: 1px solid black;'>
															<table id='alg_param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
																<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "alg_pair_parameters") %>
															</table>
														</div>
													</td>
												</tr>

												<tr>
													<td height="20px" bgcolor="buttonface">
														<table width=100% class =x8 >
															<tr>
																<td>
																</td>
																<td align=right width=100px>
																	<a href="#" onclick="import_param(xGrid_alg_paramExporter_m);">Import From File</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="export_param(xGrid_alg_paramExporter_m, 'alg');">Export to File</a>
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
											</table>	
										</div>
															

										<!-- broker1 page-->
										<div class="tab-page" id="broker1_pane_obj" style="width:95%;height:85%;border: 1px solid black;">
											<h2 class="tab">Broker 1</h2>	

											<table width="100%" height="100%" class=x8 id="brk1TableObject">
												<tr>
													<td height="20px">
														<table  width=100% class=x8>
															<tr>
																<td align=left width=200px>
																	Broker 1 Parameters
																</td>
																<td id="brkParamHeaderObj" align=left width=75px>
																	Library Path
																</td>

																<td align=left>
																	<input id="brkPathObj" value="<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "brk_path")%>" style="width:100%"/>
																</td>

																<td width="30px" align="left">
																	<a id="brk_populate_obj" href="#" onclick="populate_brk_dll();">Populate</a>
																</td>


															</tr>
														</table>
													</td>
												</tr>

												<tr id="brk_param_obj_row">
												  	<td align="center" style='vertical-align:top;height:300px'>
														<div style='width:99%;height:99%;border: 1px solid black;'>
															<table id='brk_param_obj' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
																<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "brk_pair_parameters") %>
															</table>
														</div>
													</td>
												</tr>


												<tr>
													<td height="20px" bgcolor="buttonface">
														<table id="brkParamManTblObj" width=100% class =x8 >
															<tr>
																<td>
																</td>

																<td align=right width=100px>
																	<a href="#" onclick="import_param(xGrid_brk_paramExporter_m);">Import From File</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="export_param(xGrid_brk_paramExporter_m, 'brk');">Export to File</a>
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
											</table>
										</div>
															

										<!-- broker 2 page-->
										<div class="tab-page" id="broker2_pane_obj" style="width:95%;height:85%;border: 1px solid black;">
											<h2 class="tab">Broker 2</h2>	

											<table width="100%" height="100%" class=x8 id="brk2TableObject">
												<tr>
													<td height="20px">
														<table  width=100% class=x8>
															<tr>
																<td align=left width=200px>
																	Broker 2 Parameters
																</td>
																<td id="brkParamHeaderObj_2" align=left width=75px>
																	Library Path
																</td>

																<td align=left>
																	<input id="brkPathObj_2" value="<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "brk_path2")%>" style="width:100%"/>
																</td>

																<td width="30px" align="left">
																	<a id="brk_populate_obj_2" href="#" onclick="populate_brk_2_dll();">Populate</a>
																</td>


															</tr>
														</table>
													</td>
												</tr>
																

												<tr id="brk_param_obj_row_2">
													<td align="center" style='vertical-align:top;height:300px'>
														<div style='width:99%;height:99%;border: 1px solid black;'>
															<table id='brk_param_obj_2' gridWidth='100%' gridHeight='100%' lightnavigation='false' class='x8'>
																<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "brk_pair_parameters2") %>
															</table>
														</div>
													</td>
												</tr>


												<tr>
													<td height="20px" bgcolor="buttonface">
														<table id="brkParamManTblObj_2" width=100% class =x8 >
															<tr>
																<td>
																</td>

																<td align=right width=100px>
																	<a href="#" onclick="import_param(xGrid_brk2_paramExporter_m);">Import From File</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="export_param(xGrid_brk2_paramExporter_m, 'brk2');">Export to File</a>
																</td>


																<td align=right width=80px>
																	<a href="#" onclick="deselect_all_rows(xGrid_brk_param2_m);">Deselect All</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="select_all_rows(xGrid_brk_param2_m);">Select All</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="insert_new_row(xGrid_brk_param2_m);">Insert</a>
																</td>

																<td align=right width=80px>
																	<a href="#" onclick="delete_rows(xGrid_brk_param2_m);">Remove</a>
																</td>

															</tr>
														</table>
													</td>
												</tr>
											</table>
										</div>
										<!-- -->
											
									</div>

								</td>
							</tr>
															
							

							<tr>
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



						</table>



						<form method="post" action="" id="mForm" style="display:none">
							<input type=hidden name="alg_param" value=""></input>
							<input type=hidden name="brk_param" value=""></input>
							<input type=hidden name="brk_param2" value=""></input>
							<input type=hidden name="alg_path" value=""></input>
							<input type=hidden name="brk_path" value=""></input>
							<input type=hidden name="brk_path2" value=""></input>
						</form>



					</body>
					</html>

