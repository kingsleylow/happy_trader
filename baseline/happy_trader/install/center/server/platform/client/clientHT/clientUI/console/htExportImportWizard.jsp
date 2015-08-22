<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtExportImportWizard" scope="request"/>
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
		<title id='tltObj'>Export/Import Wizard</title>

		<!-- DHTML grid -->
		<!--
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		-->
		<link rel="STYLESHEET" type="text/css" href="js/grid3/codebase/dhtmlxgrid.css">
		<script  src="js/grid3/codebase/dhtmlxcommon.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgrid.js"></script>
		<script  src="js/grid3/codebase/dhtmlxgridcell.js"></script>
		

		<!-- calendar stylesheet -->
		<link rel="stylesheet" type="text/css" media="all" href="css/calendar-win2k-cold-1.css" title="win2k-cold-1" />

		<!-- main calendar program -->
		<script type="text/javascript" src="js/calendar.js"></script>

		<!-- language for the calendar -->
		<script type="text/javascript" src="js/lang/calendar-en.js"></script>

		<!-- the following script defines the Calendar.setup helper function, which makes
			 adding a calendar a matter of 1 or 2 lines of code. -->
		<script type="text/javascript" src="js/calendar-setup.js"></script>

		<script type="text/javascript" src="js/misc/helper.js"></script>


		<script type="text/javascript">

			
			var stage_m = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "current_dialog_stage")%>';
			var next_oper_m = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "next_dialog_oper")%>';
			var prev_oper_m = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "prev_dialog_oper")%>';

			// parameters that were passed by previous steps
			var operationType_m  = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_operation_type")%>';
			var selectedProvider_m = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_history_provider")%>';
			var importBehavior_m = '<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_import_behav")%>';
			var rtProvider_m = '<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_rt_provider")%>';
			var profile_m = '<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_profile")%>';
			var operLog_m = '<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_log")%>';
			var oper_dbeg_m = '<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dbeg")%>';
			var oper_dend_m = '<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dend")%>';
			var date_selector_idx_m ='<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_date_selector_idx")%>';

			var xGrid_symb_to_export_m = null;

			// temp date var
			var date_beg_m = "";
			var date_end_m = "";

			function create_xgrid_table_import_provider_datetimes()
			{
				var mygrid = new dhtmlXGridObject('import_provider_datetimes');

				mygrid.setImagePath("js/grid3/codebase/imgs/");
				mygrid.setHeader("Symbol, Begin Date, Finish Date");
				mygrid.setInitWidths("200,200,200");
				mygrid.setColAlign("left,left,left");
				mygrid.setColTypes("ro,ro,ro");
				mygrid.setColSorting("str,str,str")
				mygrid.init();
				mygrid.loadXML("HtReturnSessionVar_v2.jsp?response_type=xml&session_var=import_provider_datetimes&cookie=<%=SC.getUniquePageId()%>");
				return mygrid;
			}

			function create_xgrid_table_provider_symbol_list_to_export()
			{
				var mygrid = new dhtmlXGridObject('provider_symbol_list_to_export');

				mygrid.setImagePath("js/grid3/codebase/imgs/");
				mygrid.setHeader("Check, RT Provider, Symbol");
				mygrid.setInitWidths("60,200,200");
				mygrid.setColAlign("center,left,left");
				mygrid.setColTypes("ch,ro,ro");
				mygrid.setColSorting("str,str,str");
				mygrid.init();
				mygrid.loadXML("HtReturnSessionVar_v2.jsp?response_type=xml&session_var=provider_symbol_list_to_export&cookie=<%=SC.getUniquePageId()%>");


				return mygrid ;
			}

			

			// this function selects from that table only this provider
			function xgrid_table_provider_symbol_list_to_export_select_single_provider(is_select)
			{
				var rowId_selected = xGrid_symb_to_export_m.getSelectedRowId();
				if (rowId_selected == null) {
					alert('Please select a row!');
					return;
				}

				var data_profile_name = xGrid_symb_to_export_m.cells(rowId_selected, 1).getValue();

				for(i = 0; i < xGrid_symb_to_export_m.getRowsNum(); i++ ) {
					var rowId = xGrid_symb_to_export_m.getRowId(i);

					var data_profile_i  = xGrid_symb_to_export_m.cells(rowId, 1).getValue();
					if (data_profile_i==data_profile_name) {
						if (is_select)
							xGrid_symb_to_export_m.cells(rowId, 0).setValue(1);
						else
							xGrid_symb_to_export_m.cells(rowId, 0).setValue(0);
					}
				}

			}

			function xgrid_table_provider_symbol_list_to_export_select_all(is_select)
			{
				for(i = 0; i < xGrid_symb_to_export_m.getRowsNum(); i++ ) {
					var rowId = xGrid_symb_to_export_m.getRowId(i);
					if (is_select)
							xGrid_symb_to_export_m.cells(rowId, 0).setValue(1);
					else
							xGrid_symb_to_export_m.cells(rowId, 0).setValue(0);
				}
			}

			// ----------------
			
			function on_load()
			{
				setPageCaption(tltObj.innerText);

				if (stage_m=='start_done') {
					adjust_navig_bar(true, false);

					switch_on_view_table(start_done_view);

				}
				else if (stage_m=='step_1_done') {
					adjust_navig_bar(false, false);

					switch_on_view_table(step_1_done_view);
				}
				else if (stage_m=='step_2_done') {
					adjust_navig_bar(false, false);

					switch_on_view_table(step_2_done_view);

					if (operationType_m=='export') {
						// adjust switch on off impoprt properties
						importPropObj.style.display='none';
					}

				}
				else if (stage_m=='step_25_done') {
					// export symbols here, this stage is availbale only if user selected export

					if (operationType_m == 'import')
					{
						// redirecting
						var url = "/htExportImportWizard.jsp?operation=do_step_3";
						url += '&page_uid=<%=SC.getUniquePageId()%>';

						mForm.action = url;
						mForm.submit();

						return;
					}

					/*
			if (operationType_m != 'export') {
		alert('Internal exception! - there must be export operation here');
			}
					 */

					adjust_navig_bar(false, false);
					switch_on_view_table(step_25_done_view);

					xGrid_symb_to_export_m = create_xgrid_table_provider_symbol_list_to_export();


				}
				else if (stage_m=='step_3_done') {
					// this is the last stage
					adjust_navig_bar(false, false);

					switch_on_view_table(step_3_done_view);

					if (operationType_m=='export') {
						// adjust switch on off impoprt properties
						importSymbolsPeriodObj.style.display='none';
					}

					// initial adjust period
					adjust_period_selection();

					if (operationType_m=='import') {
						// adjust grid for import
						create_xgrid_table_import_provider_datetimes();
						
					}

				}
				else if (stage_m=='step_4_done') {
					// this is the last stage
					adjust_navig_bar(false, true);

					switch_on_view_table(step_4_done_view);
				}
				else if (stage_m=='finish_done') {
					adjust_navig_bar(true, true);

					switch_on_view_table(finish_done_view);
				}
				else {
					alert('Invalid dialog page stage: "' + stage_m + '"');
				}


			} //

			function adjust_navig_bar(is_first, is_last)
			{
				if (is_first && !is_last) {
					firstNavigObj.style.display='none';
					secondNavigObj.innerText = "Next >";

				}
				else if (is_last && !is_first) {
					firstNavigObj.innerText = "< Previous";
					secondNavigObj.innerText = "Finish >";
				}
				else if (!is_first && !is_last){
					firstNavigObj.innerText = "< Previous";
					secondNavigObj.innerText = "Next >";
				}
				else if (is_first && is_last) {
					// finished
					firstNavigObj.style.display='none';
					secondNavigObj.style.display='none';
				}
			}


			function switch_on_view_table(tbl)
			{
				tbl.style.display='inline';
			}



			function prev_page()
			{

				var url = "/htExportImportWizard.jsp?operation="+prev_oper_m;
				url += '&page_uid=<%=SC.getUniquePageId()%>';

				mForm.action = url;
				mForm.submit();
			}

			function next_page()
			{

				var url = "/htExportImportWizard.jsp?operation="+next_oper_m + prepare_parameters() +
					"&set_param=true&page_uid=<%=SC.getUniquePageId()%>";

				mForm.prov_symb_list.value="";
				
				if (stage_m=='step_25_done') {
					// export symbols
					// if we are here prepare selected symbols list
					mForm.prov_symb_list.value = getXGridSelectedRows(xGrid_symb_to_export_m, 0, new Array(1,2) );
				}

				mForm.action = url;
				mForm.submit();

			}

			function prepare_parameters()
			{
				var params="";
				if (stage_m=='start_done') {
					params += "&history_provider="+encodeURIComponent(providersObj.options(providersObj.selectedIndex).innerText);
					params += "&history_provider_idx="+providersObj.selectedIndex;
				}
				else if (stage_m=='step_1_done') {

					if (expImp[0].checked) {
						params+="&operation_type=export";

					}
					else if (expImp[1].checked) {
						params+="&operation_type=import";
					}

				}
				else if (stage_m=='step_2_done') {

					if (operationType_m=='import') {

						if ((impProp[0].checked))
							params+="&imp_behav=overwrite";
						else if (impProp[1].checked)
							params+="&imp_behav=skip";
						else if (impProp[2].checked)
							params+="&imp_behav=exception";
						else if (impProp[3].checked)
							params+="&imp_behav=bulk_silent";

						// rt provider only for import
						params += "&rt_provider=" +encodeURIComponent(rtproviderObj.options(rtproviderObj.selectedIndex).innerText);
						params += "&rt_provider_idx=" +rtproviderObj.selectedIndex;

					}

					//



					params += "&data_profile=" + encodeURIComponent(profileObj.options(profileObj.selectedIndex).innerText);
					params += "&data_profile_idx=" + profileObj.selectedIndex;



					if (isLogObj.checked)
						params += "&oper_log=true";
					else
						params += "&oper_log=false";

				}
				
				else if (stage_m=='step_25_done') {
					// export symbols
					// if we are here prepare selected symbols list

					//params += "&prov_symb_list=";
					//params += getXGridSelectedRows(xGrid_symb_to_export_m, 0, new Array(1,2) );

					// just skip

				}
				else if (stage_m=='step_3_done') {
					// date time parameters

					var val = periodSelectionObj.options(periodSelectionObj.selectedIndex).value;
					if (val=='s1') {

						date_beg_m = f_date_b.value;
					}

					if (val=='s2')
					{
						date_end_m = f_date_e.value;
					}

					if (val=='s12')
					{
						date_beg_m = f_date_b.value;
						date_end_m = f_date_e.value;
					}

					params+="&oper_dbeg="+encodeURIComponent(date_beg_m);
					params+="&oper_dend="+encodeURIComponent(date_end_m);
					params+="&date_selector_idx="+periodSelectionObj.selectedIndex;


				}
				else if (stage_m=='step_4_done') {
					params+="&operation_str_id=" + encodeURIComponent(OperIdObj.value);
				}
				else if (stage_m=='finish_done') {

				}
				else {
					alert('Invalid dialog page stage: "' + stage_m + '"');
					return "";
				}

				return params;
			}

			function adjust_period_selection()
			{
				var val = periodSelectionObj.options(periodSelectionObj.selectedIndex).value;
				dateTimeObj.style.display='none';

				f_date_e.disabled=false;
				f_date_b.disabled=false;

				f_trigger_e.disabled=false;
				f_trigger_b.disabled=false;

				date_beg_m = "";
				date_end_m = "";

				var now = new Date();


				if (val=='1d') {
					// subtrcat 1 day from now
					var mbeg = now.getMilliseconds() - 1*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);

				}
				else if (val=='3d') {
					var mbeg = now.getMilliseconds() - 3*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1w') {
					// substruct one 7 days
					var mbeg = now.getMilliseconds() - 7*24*60*60*1000;
					var mbeg_date = new Date();
					mbeg_date.setMilliseconds(mbeg);


					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1m') {
					//subtruct one month
					var mbeg_date = new Date();
					var month = mbeg_date.getMonth();

					month--;
					if (month < 0) {
						var year = mbeg_date.getYear();
						year--;
						mbeg_date.setYear(year);
						mbeg_date.setMonth(0);
					}
					else {
						mbeg_date.setMonth(month);
					}

					date_beg_m = dateToString(mbeg_date);

				}
				else if (val=='6m') {
					var mbeg_date = new Date();
					var month = mbeg_date.getMonth();

					month = month - 6;
					if (month < 0) {
						var year = mbeg_date.getYear();
						year--;

						mbeg_date.setYear(year);
						mbeg_date.setMonth(11+month);
					}
					else {
						mbeg_date.setMonth(month);
					}

					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='1y') {
					var mbeg_date = new Date();
					var year = mbeg_date.getYear();
					year--;
					mbeg_date.setYear(year);

					date_beg_m = dateToString(mbeg_date);
				}
				else if (val=='whole') {
					// implicitely it is assumed - just pass empty strings
				}
				else if (val=='s1') {
					dateTimeObj.style.display='inline';

					f_date_e.disabled=true;
					f_trigger_e.disabled=true;


				}
				else if (val=='s2') {
					dateTimeObj.style.display='inline';

					f_date_b.disabled=true;
					f_trigger_b.disabled=true;


				}
				else if (val=='s12') {
					dateTimeObj.style.display='inline';

				}

				//alert(date_beg_m + ' - ' + date_end_m);


			}

		</script>
	</head>

	<body onload="on_load();">

		<table cellspacing="3" cellpadding="3"  width=100% height=100% class=x8>
			<tr>
				<td height=350px>
					<!-- View for each step-->
					<table style="display:none;" id="start_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Select History Provider
							</td>
						</tr>

						<tr>
							<td align=left height=20px>
								<select id="providersObj" style="width:100%">
									<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "provider_list") %>

								</select>
							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>
					</table>

					<table style="display:none;" id="step_1_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Select export or import operation
							</td>
						</tr>

						<tr>
							<td align=left height=20px>
								<input type=radio name="expImp" CHECKED>Export</input>
							</td>
						</tr>
						<tr>
							<td align=left height=20px>
								<input type=radio name="expImp">Import</input>
							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>

						<script type="text/javascript">
							if (operationType_m=='import') {
								expImp[1].checked = true;
							}
							else {
								expImp[0].checked = true;
							}
						</script>
					</table>

					<table style="display:none;" id="step_2_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Select export/import properties
							</td>
						</tr>

						<tr>
							<td height=20px align=left>
								<input id="isLogObj" type=checkbox>Enable detailed logging</input>
								<script type="text/javascript">

									isLogObj.checked = (operLog_m == 'true' ? true : false);
								</script>
							</td>
						</tr>

						<tr>
							<td height=20px>
							</td>
						</tr>



						<tr>
							<td align=left height=20px>
								Select Data profile
							</td>
						</tr>

						<tr>
							<td height=20px align=left>
								<select id="profileObj">
									<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "profile_list") %>
								</select>

							</td>
						</tr>

						<tr>
							<td height=20px>
							</td>
						</tr>


						<tr>
							<td height=20px>
								<table id="importPropObj" width=100% class=x8>
									<tr>
										<td align=left height=20px>
											Import properties
										</td>
									</tr>
									<tr>
										<td align=left height=20px>
											<input type=radio name="impProp" CHECKED>Overwrite existing data</input>
										</td>
									</tr>
									<tr>
										<td align=left height=20px>
											<input type=radio name="impProp">Leave intact existing data</input>
										</td>
									</tr>
									<tr>
										<td align=left height=20px>
											<input type=radio name="impProp" >Raise an exception on data collision</input>
										</td>
									</tr>
									<tr>
										<td align=left height=20px>
											<input type=radio name="impProp" >Bulk silently</input>
										</td>
									</tr>

									<tr>
										<td align=left height=20px>
										</td>
									</tr>

									<tr>
										<td align=left height=20px>
											Select RT provider as import target
										</td>
									</tr>

									<tr>
										<td height=20px align=left>
											<select id="rtproviderObj">
												<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "rt_provider_list") %>
											</select>
										</td>
									</tr>

								</table>
								<script>
									if (importBehavior_m=='overwrite') {
										impProp[0].checked = true;
									}
									else if (importBehavior_m=='skip') {
										impProp[1].checked = true;
									}
									else if (importBehavior_m=='exception') {
										impProp[2].checked = true;
									}
									else if (importBehavior_m=='bulk_silent') {
										impProp[3].checked = true;
									}
									else
										impProp[0].checked = true;
								</script>

							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>
					</table>


					<table style="display:none;" id="step_25_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<td align=left height=20px>
							Export Symbols selection
						</td>

						<tr>
							<td height=20px>
								<table width=100% class=x8>

									<td align=left height=130px>
										<div id="provider_symbol_list_to_export" style="width:100%;height:150px;border-style:solid; border-width:1px;"></div>
									</td>


								</table>
							</td>
						</tr>

						<tr>
							<td height="20px" align="left">
								<a href="#" onclick="xgrid_table_provider_symbol_list_to_export_select_all(true)">Check All</a>
							</td>
						</tr>

						<tr>
							<td height="20px" align="left">
								<a href="#" onclick="xgrid_table_provider_symbol_list_to_export_select_all(false)">Uncheck All</a>
							</td>
						</tr>

						<tr>
							<td height="20px" align="left">
								<a href="#" onclick="xgrid_table_provider_symbol_list_to_export_select_single_provider(true)">Check selected provider</a>
							</td>
						</tr>

						<tr>
							<td height="20px" align="left">
								<a href="#" onclick="xgrid_table_provider_symbol_list_to_export_select_single_provider(false)">Uncheck selected provider</a>
							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>

					</table>

					<table style="display:none;" id="step_3_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Export/import time periods
							</td>
						</tr>

						<tr>
							<td height=20px>
							</td>
						</tr>
						
						<tr>
							<td height=20px>
								<table width=100% class=x8 cellpadding="2" cellspacing="2" id="importSymbolsPeriodObj" class=x8>
									<tr>
										<td height=20px align=left>
											History Provider "<%= SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_history_provider") %>" may provide the following data on import:
										</td>
									</tr>

									<tr>
										<td align=left height=90px>
											<div id="import_provider_datetimes" style="width:100%;height:150px;border-style:solid; border-width:1px;"></div>
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
								Select History Provider period to be processed
							</td>
						</tr>

						<tr>
							<td height=20px align=left>
								<select id="periodSelectionObj" onchange="adjust_period_selection();">
									<option value="1d">1 day before</option>
									<option value="3d">3 days before</option>
									<option value="1w">1 week before</option>
									<option value="1m">1 month before</option>
									<option value="6m">6 months before</option>
									<option value="1y">1 year before</option>
									<option value="whole">Whole History Provider period</option>
									<option value="s1">Select first date</option>
									<option value="s2">Select last date</option>
									<option value="s12">Select both dates</option>
								</select>
								<script type="text/javascript">
									periodSelectionObj.selectedIndex = date_selector_idx_m;
								</script>
							</td>
						</tr>

						<tr>
							<td height=20px align=left>
								<table style="display:none" id="dateTimeObj" width=100% class =x8>
									<tr>
										<td align=left width=80px>
											Start Date (GMT)
										</td>

										<td>
											<table  width=100% cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
												<tr>
													<td width=120px>
														<input  style="width: 100%" class=x8 type="text" name="date" id="f_date_b" />
													</td>

													<td>
														<img  src="imgs/img.gif" id="f_trigger_b" style="cursor: pointer; border: 1px solid red;" title="Date selector"
																 onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
													</td>
													<script>
														f_date_b.value = oper_dbeg_m;
													</script>
												</tr>
											</table>


											<script type="text/javascript">
												Calendar.setup({
													inputField     :    "f_date_b",     // id of the input field
													ifFormat       :    "%d-%m-%Y %H:%M",      // format of the input field
													button         :    "f_trigger_b",  // trigger for the calendar (button ID)
													align          :    "Tl",           // alignment (defaults to "Bl")
													singleClick    :    true,
													timeFormat     :    "24",
													showsTime      :    true
												});
											</script>
										</td>

										<td>
										</td>
									</tr>

									<tr>
										<td align=left width=80px>
											End Date (GMT)
										</td>

										<td>

											<table  width=100% cellspacing="0" cellpadding="0" style="border-collapse: collapse;">
												<tr>
													<td width=120px>
														<input  style="width: 100%" class=x8 type="text" name="date" id="f_date_e" />
													</td>

													<td>
														<img  src="imgs/img.gif" id="f_trigger_e" style="cursor: pointer; border: 1px solid red;" title="Date selector"
																	onmouseover="this.style.background='red';" onmouseout="this.style.background=''" />
													</td>
													<script>
														f_date_e.value = oper_dend_m;
													</script>
												</tr>
											</table>

											<script type="text/javascript">
												Calendar.setup({
													inputField     :    "f_date_e",     // id of the input field
													ifFormat       :    "%d-%m-%Y %H:%M",      // format of the input field
													button         :    "f_trigger_e",  // trigger for the calendar (button ID)
													align          :    "Tl",           // alignment (defaults to "Bl")
													singleClick    :    true,
													timeFormat     :    "24",
													showsTime      :    true
												});
											</script>

										</td>

										<td>
										</td>
									</tr>
								</table>
							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>
					</table>

					<table style="display:none;" id="step_4_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Review the parameters and confirm the operation
							</td>
						</tr>

						<tr>
							<td height=20px>
							</td>
						</tr>

						<tr>
							<td height=20px>
								<table class=x8 cellpadding="3" cellspacing="3" width=100%>

									<tr>
										<td align=left class=x2 width=160px>
											Operation ID
										</td>

										<td align=left>
											<input align=left id="OperIdObj" type="text"></input>
											<script>
												var str_date = dateToString( new Date());
												OperIdObj.value = "OP_" + str_date;
											</script>
										</td>

										<td>

										</td>
									</tr>

									<tr>

										<%
										StringBuffer val0 = new StringBuffer();
										if (SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_operation_type").equals("import")) {
											val0.append("<td align=left class=x2 width=160px>RT Provider</td>");
											val0.append("<td align=left>");
											val0.append( SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_rt_provider"));
											val0.append("</td>");
										}
										%>

										<%= val0.toString() %>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px>
											Data Profile
										</td>

										<td align=left>
											<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_profile")%>
										</td>

										<td>

										</td>
									</tr>


									<tr>
										<td align=left class=x2 width=160px>
											History Provider Name
										</td>

										<td align=left>
											<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_history_provider")%>
										</td>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px>
											Operation type
										</td>

										<td align=left>
											<%=  SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_operation_type")%>
										</td>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px >
											Detail log enabled
										</td>

										<td align=left>
											<%=SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_log")%>
										</td>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px>
											Date From
										</td>

										<td align=left>

											<%=
											SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dbeg").length() > 0 ? SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dbeg") : "Default"
											%>
										</td>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px>
											Date To
										</td>

										<td align=left>
											<%=
											SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dend").length() > 0 ? SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_oper_dend") : "Default"
											%>
										</td>

										<td>

										</td>
									</tr>

									<tr>

										<%= SC.prepareProviderSymbolListHtml(request) %>

										<td>

										</td>
									</tr>

									<tr>
										<td align=left class=x2 width=160px>
											<%
											String val = "";
											if (SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_operation_type").equals("import"))
													val = "Import handle data";
											%>

											<%= val %>
										</td>

										<td align=left>
											<%
											String val2 = "";
											if (SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_operation_type").equals("import")) {
													String behav = SC.getStringSessionValue(SC.getUniquePageId(),request, "wiz_import_behav");
													if (behav.equals("overwrite")) {
														val2 = "Overwrite existing data";
													} else if (behav.equals("skip")) {
														val2 = "Leave intact existing data";
													} else if (behav.equals("exception")) {
														val2 = "Raise an exception on data collision";
													} else if (behav.equals("bulk_silent")) {
														val2 = "Bulk insert silently ignoring all";
													}

											}
											%>

											<%= val2 %>
										</td>

										<td>

										</td>

									</tr>




								</table>
							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>
					</table>

					<table style="display:none;" id="finish_done_view" width=100% height=100%  cellspacing="3" cellpadding="3" class=x8>
						<tr>
							<td align=left height=20px>
								Export/Import operation pending... Check logs/background jobs for finish status
							</td>
						</tr>

						<tr>
							<td>

							</td>
						</tr>

						<tr>
							<td>
							</td>
						</tr>
					</table>
				</td>
			</tr>

			<tr>
				<td heigh=20px>

				</td>
			</tr>

			<tr>
				<td>
					<table class=x8>
						<tr>
							<td  width=80px align=left>
								<a id="firstNavigObj" href="#" onclick="prev_page();"></a>
							</td>

							<td>

							</td>

							<td  width=80px align=right>
								<a id="secondNavigObj" href="#" onclick="next_page();"></a>
							</td>
						</tr>
					</table>
				</td>
			</tr>
		</table>

		<form method="post" action="" id="mForm" style="display:none">
			<input type=hidden name="prov_symb_list" value="0"></input>
		</form>

	</body>
</html>
