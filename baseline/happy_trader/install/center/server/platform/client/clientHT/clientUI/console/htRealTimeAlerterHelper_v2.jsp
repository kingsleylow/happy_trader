<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtRealTimeAlerterHelper" scope="request"/>
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
		
		<title id='tltObj'>Real-Time Alerts</title>
		<bgsound id='soundObj' src="" loop="-1"/>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- OLD DHTML grid -->

		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>




		<!-- GRID -->

		<script type="text/javascript" src="js/misc/helper.js"></script>

		<!-- JQUERY -->
		<script src="js/jtip/jquery.js" type="text/javascript"></script>
		<script src="js/jtip/jquery.hoverIntent.js" type="text/javascript"></script>
		<script src="js/jtip/jquery.cluetip.js" type="text/javascript"></script>
		<script type="text/javascript">
			$(document).ready(function(){


			});

		</script>
		<link rel="stylesheet" href="css/jtip/jquery.cluetip.css" type="text/css" />


		<script>

			// constants priorities
			var AP_LOW =			3;

			var AP_MEDIUM =		2;

			var AP_HIGH =			1;

			var AP_CRITICAL =	0;

			// ----------------------------

			var timerIntervalMsec_m = 500;

			// initially set to null
			var url_m = null;

			var historyBaseUrl_m = null;

			var numberUnreadMessages_m = -1;

			var fatalError_m = false;

			var cutoffTime_m = -1;

			var beginTime_m = -1;

			var historyGrid_m = null;

			var rtGrid_m = null;

			// the fisrt index
			var history_pageIndex_m = 0;



			// ------------------------------

			function initHistoryGridObject()
			{
				historyGrid_m = new dhtmlXGridObject("dataTblObj");


				historyGrid_m.setImagePath("/imgs/grid/");
				historyGrid_m.setEditable(false);
				historyGrid_m.setHeader("Alert Time, Priority, Run Name, Source, ID, Small Data");
				historyGrid_m.setInitWidths("150,80,240,240,100,370");
				historyGrid_m.setColTypes("ro,ro,ro,ro,ro,ed");
				historyGrid_m.setColAlign("left,left,left,left,left,left");
				historyGrid_m.setColSorting("na,na,na,na,na,na");
				historyGrid_m.enableResizing("true,true,true,true,true,true");
				historyGrid_m.enableTooltips("false,false,false,false,false,false");
				historyGrid_m.setOnRowSelectHandler(on_history_grid_row_selected);


				historyGrid_m.init();

			}

			function initRtGridObject()
			{
				rtGrid_m = new dhtmlXGridObject("rtTblObj");

				rtGrid_m.setImagePath("/imgs/grid/");
				rtGrid_m.setEditable(false);
				rtGrid_m.setHeader("Alert Time, Priority, Run Name, Source, ID, Small Data");
				rtGrid_m.setInitWidths("150,80,240,240,100,370");
				rtGrid_m.setColTypes("ro,ro,ro,ro,ro,ed");
				rtGrid_m.setColAlign("left,left,left,left,left,left");
				rtGrid_m.setColSorting("na,na,na,na,na,na");
				rtGrid_m.enableResizing("true,true,true,true,true,true");
				rtGrid_m.enableTooltips("false,false,false,false,false,false");

				rtGrid_m.init();
			}



			

			// call back on show toolbox popup
			function on_cluetip_show(ct, ci)
			{


				var id_attr = ci.find('div').attr('id');
				if (id_attr != null) {

					decreaseNumberNonReadRows(id_attr);
					checkIfDoSound();
				}

			}


			function on_load()
			{

				url_m = '/htRealTimeAlerter_v2.jsp?content_filter='+
					encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "content_filter")%>') +
					'&run_name_filter='+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "run_name_filter")%>') +
					'&session_filter='+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "session_filter")%>') +
					'&bdate_l='+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "bdate_l")%>') +
					'&priority_filter='+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "priority_filter")%>')+
					'&page_uid=<%= SC.getUniquePageId() %>';



				// start
				var iframe_elem = document.getElementById("frmContentObj");
				iframe_elem.onload = disconnected;

				iframe_elem.src = url_m;
				if (iframe_elem.readyState)
					setTimeout("MS_checkiframe()", timerIntervalMsec_m);

				update_cluetip();
			

			}

			function disconnected()
			{
				// if fatal error - return
				if (fatalError_m)
					return;

				var iframe_elem = document.getElementById("frmContentObj");
				iframe_elem.src = url_m;

				if (iframe_elem.readyState)
					setTimeout("MS_checkiframe()", timerIntervalMsec_m);
			}

			function MS_checkiframe() {
				var iframe_elem = document.getElementById("frmContentObj");
				if (iframe_elem != undefined) {

					if (iframe_elem.readyState=="complete") {
						disconnected();
					} else {
						setTimeout("MS_checkiframe()", timerIntervalMsec_m);
					}
				}
			}


			// ----------------------------

			function ct(cutoffTime, beginTimeStamp)
			{
				// set up cutoff time - will be used later
				try {
					setUpCutoffTime(cutoffTime, beginTimeStamp);
				}
				catch(e)
				{
					alert(e.message);
				}
			}

			function em(dateStr, errorMsg)
			{
				try {

				}
				catch(e)
				{
					alert(e.message);
				}
			}

			function hb(dateStr)
			{
				try {
					insertHeartBeat(dateStr);
				}
				catch(e)
				{
					alert(e.message);
				}
			}


			function re(dateStr, priorityStr, priority, runName, session, id, alertdata)
			{
				try {
					insertRTEntry(dateStr, priorityStr, priority, runName, session, id, alertdata);
				}
				catch(e)
				{
					alert(e.message);
				}
			}


			// -----------------------------

			function createHistoryURL(orderTimeValue, finalOrderTimeValue)
			{
				historyBaseUrl_m = '/HtRealTimeAlerterHelper_HistoryProvider.jsp?'+
					"&order_time_value="+orderTimeValue+
					"&final_order_time_value="+finalOrderTimeValue+
					"&cutoff_timestamp="+cutoffTime_m+
					"&begin_timestamp="+beginTime_m+
					"&content_filter="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "content_filter")%>')+
					"&run_name_filter="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "run_name_filter")%>') +
					"&priority_filter="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "priority_filter")%>')+
					"&session_filter="+encodeURIComponent('<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "sessions_filter")%>')+
					"&read_page_num="+history_pageIndex_m+
					"&read_rows_per_page=" + <%= SC.getPageSize() %>+
					"&page_uid=<%= SC.getUniquePageId() %>";
			}

			// ------------------------------------
			// Navigation

			function reinitGridAndNavigate()
			{
				switchOnLoadInProgress();

				createHistoryURL("desc", "asc");

				if (historyGrid_m != null) {
					historyGrid_m.destructor();
					historyGrid_m = null;
				}

				initHistoryGridObject();
				historyGrid_m.loadXML(historyBaseUrl_m, check_error_after_history_load);
			}

			function prev_page_history()
			{

				// if currently zero roes do nothing
				if (historyGrid_m.getRowsNum() <=0) {
					alert("No available rows");
					return;
				}


				history_pageIndex_m = history_pageIndex_m*1 + <%= SC.getPageSize() %>;
				adjustSelectObjectToHistoryIndex();

				reinitGridAndNavigate();

			}

			function next_page_history()
			{
				var history_pageIndex_new = history_pageIndex_m - <%= SC.getPageSize() %>;
				if (history_pageIndex_new <=0 ) {
					history_pageIndex_m = 0;
				}
				else {
					history_pageIndex_m = history_pageIndex_new;
				}



				// load XML and check error
				adjustSelectObjectToHistoryIndex();

				reinitGridAndNavigate();
			}

			function first_page_history()
			{
				history_pageIndex_m = 0;

				adjustSelectObjectToHistoryIndex();

				reinitGridAndNavigate();
			}

			function navigate_through_page_selector()
			{

				pageNumObj.innerText = '';
				var pageIndex = selectPageObj.options(selectPageObj.selectedIndex).value;


				// extend
				if (pageIndex==-9999) {
					// add
					extendPageSelector();

					// try on more time
					// assuming we have the correctly selected index
					navigate_through_page_selector();

				}
				else {
					// normal navigation
					history_pageIndex_m = pageIndex;

					reinitGridAndNavigate();


				}
			}

			// sets up page selector
			// toExtend means extending pages to more
			function setUpPageSelector()
			{
				selectPageObj.last_page_index = (<%= SC.getNumberPagesToShow() %> -1) * <%= SC.getPageSize()%>;

				var numberOfPages = <%= SC.getNumberPagesToShow() %>;

				for(var i = 0; i < numberOfPages;i++) {
					var oOption = document.createElement("OPTION");
					var pageIndex = i*<%= SC.getPageSize() %>;

					oOption.text='Rows from: ' + (pageIndex+1);
					oOption.value=pageIndex;

					selectPageObj.add(oOption);

				}

				var oOption = document.createElement("OPTION");
				oOption.text='Extend...';
				oOption.value = -9999;
				selectPageObj.add(oOption);


			}



			// extends page selector 2 times
			function extendPageSelector()
			{
				// remove the last
				var oldLastIdx = selectPageObj.options.length-1;
				selectPageObj.remove(oldLastIdx);

				// set up new number of pages to add

				var numberOfPages_add = (selectPageObj.last_page_index/<%= SC.getPageSize()%> + 1);


				for(var i = oldLastIdx; i < numberOfPages_add + oldLastIdx;i++) {
					var oOption = document.createElement("OPTION");
					var pageIndex = i*<%= SC.getPageSize() %>;

					oOption.text='Rows from: ' + ( pageIndex +1);
					oOption.value=pageIndex;

					selectPageObj.add(oOption);

				}

				var oOption = document.createElement("OPTION");
				oOption.text='Extend...';
				oOption.value = -9999;
				selectPageObj.add(oOption);

				// new

				selectPageObj.last_page_index = (numberOfPages_add*2 -1) * <%= SC.getPageSize()%>;


				// set up selected index
				selectPageObj.options(oldLastIdx).selected=true;


			}

			function switchOnLoadInProgress()
			{
				bodyObj.disabled=true;
			}

			function switchOffLoadInProgress()
			{
				bodyObj.disabled=false;
			}

			// -------------------------------------------------


			// this is generally initializes all
			function setUpCutoffTime(cutoffTime, beginTimeStamp)
			{
				// reinit that grids

				if (rtGrid_m != null) {
					rtGrid_m.destructor();
					rtGrid_m = null;
				}

				initRtGridObject();

				// reset unread messages
				numberUnreadMessages_m = 0;
				history_pageIndex_m = 0;
				cutoffTime_m = cutoffTime;
				beginTime_m = beginTimeStamp;


				setUpPageSelector();


				// load XML and check error

				adjustSelectObjectToHistoryIndex();
				reinitGridAndNavigate();

			}

			// checks if any error happend after history load
			function check_error_after_history_load()
			{

				$.ajax({
					async:true,
					type: "GET",
					url: "/HtReturnSessionVar_v2.jsp",
					data: "response_type=xml&session_var=error_info&cookie=<%=SC.getUniquePageId() %>",
					complete: function(loader,status){
						check_error_after_history_load_c(loader);
					}
				});


			}

			function check_error_after_history_load_c(loader)
			{


				switchOffLoadInProgress();

				if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
					// error
					fatalError_m = true;

					var iframe_elem = document.getElementById("frmContentObj");
					if (iframe_elem != undefined) {
						iframe_elem.src='';
					}

					// parse XML
					document.body.innerHTML = "";
					var result = loader.responseXML.selectNodes("/errorinfo/entry");
					for(var i=0;i<result.length;i++){
						document.body.innerHTML += result[i].text;
					}
				}
				else {
					// all os ok - update
					historyPageLoaded_c();

				}

			}

			// claaa back when all is good with history greed
			function historyPageLoaded_c()
			{


				// synchronize page selector
				// if there are row in the grid - we are cool with our selection
				if (historyGrid_m.getRowsNum() > 0) {

					var lastRowIdx = 1*history_pageIndex_m + historyGrid_m.getRowsNum();
					var firstRowIdx = 1*history_pageIndex_m + 1;

					pageNumObj.innerText = 'Rows from: ' + firstRowIdx  + ' to: '+ lastRowIdx;
					update_cluetip();



					if (historyGrid_m.getRowsNum()< <%= SC.getPageSize() %>) {
						// no ned in prev navigation
						disable_prev_navigation();
					}
					else {
						enable_prev_navigation();
					}
				}
				else {
					// dataset is empty
					pageNumObj.innerText = 'No rows available';

					disable_prev_navigation();
				}
			}

			function adjustSelectObjectToHistoryIndex()
			{

				pageNumObj.innerText = '';

				if (selectPageObj.last_page_index < history_pageIndex_m) {
					// extending that
					extendPageSelector();
				}

				// try to find
				var found = false;
				for(var i = 0; i < selectPageObj.options.length; i++) {
					if (selectPageObj.options(i).value >= 0 && selectPageObj.options(i).value==history_pageIndex_m) {
						selectPageObj.options(i).selected=true;
						found = true;
						break;
					}
				}

				if (!found)	{
					alert('Internal exception - cannot find the selected index: ' + history_pageIndex_m);
				}
			}

			function disable_prev_navigation()
			{

				prevNavObj.disabled =true;
				prevNavObj.bgColor ='orange';
			}

			function enable_prev_navigation()
			{

				prevNavObj.disabled =false;
				prevNavObj.bgColor='buttonface';
			}


			function insertErrorMessage(dateStr, errorMsg)
			{

			}




			function insertRTEntry(dateStr, priorityStr, priority, runName, session, id, alertdata)
			{

				var tlt = dateStr + " - " + priorityStr;
				var entry = new Array(dateStr,priorityStr,runName,session,getHRefCell(id, tlt),alertdata);
				rtGrid_m.addRow(id, entry);

				// update cluetip
				update_last_cluetip(id);

				// scroll
				if (scrollEndObj.checked==true) {
					//rtGrid_m.selectRowById(id, null, true, null);
					rtGrid_m.selectRow(rtGrid_m.getRowIndex(id),false,false,true);

				}

				// set up current row as unread
				setupCurrentRowAsUnread(id);

				// sound
				checkIfDoSound();

			}


			function insertHeartBeat(dateStr)
			{
				// update heart beat

				heartBeatValueObj.innerText = dateStr;

			}




			// -----------------------------

			function getHRefCell(id, tlt)
			{
				//return "<a class='basic' href='javascript:void(0)' id='aid_"+id+"' title='"+tlt+"' rel='/HtRealTimeAlerterHelper_AlertDataPopup.jsp?id="+ id +"' onclick=\"d_rt('"+id+"')\" >"+id+"</a>";
				return "<div class='basic' style='cursor:hand' id='aid_"+id+"' title='"+tlt+"' rel='/HtRealTimeAlerterHelper_AlertDataPopup.jsp?id="+ id +"'><b>"+id+"</b></a>";
			}

			function d_rt(id)
			{
				decreaseNumberNonReadRows(id);
				checkIfDoSound();

				var url = "/htShowDetailedAlert.jsp?ids="+id;
				window.open(url);
			}

			function d(id)
			{
				var url = "/htShowDetailedAlert.jsp?ids="+id;
				window.open(url);
			}

			function mark_all_read()
			{

				allNumberRead();
				checkIfDoSound();

			}
			//////////////////////////////
			// sound functions
			function switchSoundOn()
			{
				soundObj.src = "/sounds/alert.wav";
			}

			function switchSoundOff()
			{
				soundObj.src = "";
			}

			function changeRowSelection(rowid, colorVal, textColor)
			{
				for(var i = 0; i < rtGrid_m.getColumnsNum(); i++ ) {
					var cell_i = rtGrid_m.cellById(rowid, i);
					cell_i.setBgColor(colorVal);
					cell_i.setTextColor(textColor);
				}
			}

			// set up one rows as unread and adjust counter
			//  and visually set up that
			function decreaseNumberNonReadRows(rowid)
			{
				var isNonread = rtGrid_m.getUserData(rowid, "non_read");

				if (isNonread) {
					rtGrid_m.setUserData(rowid, "non_read", false);
					numberUnreadMessages_m--;

					//
					changeRowSelection(rowid, 'window', 'buttontext' );

				}

			}

			function allNumberRead()
			{
				rtGrid_m.forEachRow(
				function(rowid){
					// here id - row ID
					var isNonread = rtGrid_m.getUserData(rowid, "non_read");
					if (isNonread) {
						rtGrid_m.setUserData(rowid, "non_read", false);
						numberUnreadMessages_m--;

						changeRowSelection(rowid, 'window', 'buttontext' );

						//
					}
				}
			)
			}

			// this set up new current row as unread
			function setupCurrentRowAsUnread(rowid)
			{
				rtGrid_m.setUserData(rowid, "non_read", true);
				numberUnreadMessages_m++;

				changeRowSelection(rowid, 'chartreuse', 'buttontext' );

			}

			// callback every time sound status may be chnaged
			function checkIfDoSound()
			{
				if (enableSoundObj.checked==true) {
					if (numberUnreadMessages_m > 0) {
						switchSoundOn();
					}
					else {
						switchSoundOff();
					}
				}
				else {
					switchSoundOff();
				}
			}

			/////////////////////////
			function update_cluetip()
			{
				$('div.basic').cluetip(

				{
					height: '700px',
					width:  '600px',
					ajaxCache: true,
					positionBy: 'auto',
					dropShadow:  false,
					arrows:      true,
					sticky: true,
					mouseOutClose: true,
					closePosition: 'title',
					closeText: '<img src="/imgs/jtip/cross.png" alt="close" />',
					onShow: on_cluetip_show
			
				}
			);

			}

			function update_last_cluetip(id)
			{

				var id_str = "#aid_"+id;
				$(id_str).cluetip(
				{
					height: '700px',
					width:  '600px',
					ajaxCache: true,
					positionBy: 'auto',
					dropShadow:  false,
					arrows:      true,
					sticky: true,
					mouseOutClose: true,
					closePosition: 'title',
					closeText: '<img src="/imgs/jtip/cross.png" alt="close" />',
					onShow: on_cluetip_show
				
				}
			);
			}


			

			function on_history_grid_row_selected(id)
			{
				if (enableHintOnTop.checked) {
					var url='/HtRealTimeAlerterHelper_AlertDataPopup.jsp?id='+id;

					mForm_dialog.action = url;
					mForm_dialog.target='dialog_hint_<%= SC.getUniquePageId()%>';
					//mForm_dialog.id.value = id;
					
					mForm_dialog.submit();
					
					
				}
			}

		</script>
	</head>

	<body id="bodyObj" onload="on_load();">

		<table width=100% height=100% class=x8>
			<tr height=20px >
				<td align=left width=400px>
					<table width=100% height=100% class=x8 >


						<tr height=20px>
							<td colspan=3 style="font:larger" class="x3">
								<div align=left>
									Real-Time Alerts
								</div>
							</td>
						</tr>

						<tr height=20px>
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								From Filter (GMT)
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "begin_date_s")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px >
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Content Filter
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "content_filter")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Run Name Filter
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "run_name_filter")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Session Filter
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "session_filter")%>
							</td>
							<td></td>
						</tr>

						<tr height=20px>
							<td align=right class=x3 bgcolor='buttonface' width=160px>
								Priority Filter
							</td>
							<td align=left>
								<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "priority_filter_s")%>
							</td>
							<td></td>
						</tr>


					</table>
				</td>

				<td>
				</td>

				<td width=200px>
					<table width=100% height=100% class=x8>

						<tr height=20px>
							<td align=left>
								<input id='scrollEndObj' checked=true type=checkbox >Scroll View to the Bottom</input>
							</td>
							<td>
							</td>
						</tr>


						<tr height=20px>
							<td align=left>
								<input id='enableSoundObj' checked=true type=checkbox onclick="checkIfDoSound();">Enable sound</input>
							</td>
							<td>
							</td>
						</tr>

						<tr height=20px>
							<td align=left>
								<input id='enableHintOnTop'  type=checkbox>Keep alert open in the separate window</input>
							</td>
							<td>
							</td>
						</tr>

						<tr height=20px>
							<td align=left>
								<a href="#" onclick="mark_all_read();">Mark all read</a>
							</td>
							<td align="left">
							</td>
						</tr>

						<tr>
							<td>
							</td>
							<td>
							</td>
						</tr>

					</table>
				</td>

			</tr>


			<tr colspan=3 height=20px >
				<td ></td>
			</tr>




			<tr class=x3 height=20px>
				<td colspan="3">

					<table width="100%" height="100%" class="x8">

						<tr>

							<td  bgcolor='buttonface' >
							</td>

							<td id="prevNavObj" width="18px "align=left bgcolor='buttonface'>
								<a href="#" onclick="prev_page_history();">
									<img src="imgs/srmv.gif" border="0" align="absmiddle"/>
								</a>
							</td>

							<td width="200px" class="x3" bgcolor='buttonface' >
								<div align="center"> History Alerts </div>
							</td>

							<td width="18px" align=right bgcolor='buttonface' >
								<a href="#" onclick="next_page_history();">
									<img src="imgs/smv.gif" border="0" align="absmiddle"/>
								</a>
							</td>

							<td width="18px" align=right bgcolor='buttonface' >
								<a href="#" onclick="first_page_history();">
									<img src="imgs/smvall.gif" border="0" align="absmiddle"/>
								</a>
							</td>

							<td bgcolor='buttonface'>
							</td>


							<td width="80px"  align=right bgcolor='buttonface'>
								Navigate to:
							</td>

							<td width="150px" align=left bgcolor='buttonface'>
								<select onchange='navigate_through_page_selector()' style="width:100%" id="selectPageObj">
								</select>
							</td>

							<td width="150px" id="pageNumObj" align=right bgcolor='buttonface'>
							</td>

						</tr>

					</table>


				</td>
			</tr>

			<!-- HISTORY GRID OBJECT -->
			<tr width=100%>
				<td align=left colspan=3>
					<div style='height:100%; width:100%; border: 1px solid black'>
						<table  class=x8  width=100% height=100%>
							<tr>
								<td align="center">
									<div id="dataTblObj" style='height:100%; width:100%; '></div>
								</td>
							</tr>
						</table>

					</div>

				</td>
			</tr>

			<!-- HISTORY GRID OBJECT -->

			<tr class=x3 height=20px>
				<td colspan=3 align=center bgcolor='buttonface' >
					Real-Time Alerts
				</td>
			</tr>

			<!-- RT GRID OBJECT -->
			<tr width=100%>
				<td align=left colspan=3>

					<div  style='height:100%; width:100%; border: 1px solid black'>
						<table  class=x8  width=100% height=100%>
							<tr>
								<td align="center">
									<div  id="rtTblObj" style='height:100%; width:100%; '></div>
								</td>
							</tr>
						</table>

					</div>

				</td>
			</tr>

			<!-- RT GRID OBJECT -->

			<tr height=20px >
				<td colspan=3>
					<table class=x8 width=100% height=100%>
						<tr>
							<td class=x3 bgcolor='buttonface' align=right width=160px>Last heart-beat (GMT)</td>
							<td id='heartBeatValueObj' bgcolor='buttonface' align=left></td>
							<td width="100px" id='rowCountObj' bgcolor='buttonface' align=right></td>
						</tr>
					</table>
				</td>

			</tr>
		</table>


		<iframe id="frmContentObj" width=1px height=1px style="display:none" ></iframe>
		
		<form method="POST" action="" id="mForm_dialog" width=1px height=1px style="display:none">
		</form>


	</body>
</html>