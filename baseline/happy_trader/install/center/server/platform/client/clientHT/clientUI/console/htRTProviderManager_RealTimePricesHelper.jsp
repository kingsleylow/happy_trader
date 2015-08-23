<%@ page contentType="text/html;charset=windows-1251"%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<title id='tltObj'>Real-Time Prices</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- OLD DHTML grid -->

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
		
		<script src="scriptaculous-js-1.9.0/lib/prototype.js" type="text/javascript"></script>
		<script src="scriptaculous-js-1.9.0/src/scriptaculous.js" type="text/javascript"></script>




		<script>
			// color const
			var COLOR_BID_DEAL = 1;
			var COLOR_ASK_DEAL = 100;
			var COLOR_NEUTRAL_DEAL = 50;
			var COLOR_NA_DEAL = 0;

			var url_m = null;
			var timerIntervalMsec_m = 5000;
			var priceGrid_m = null;
			var histGrid_m = null;
			var priceGridLevel1_m = null;
			var priceGridLevel2_m = null;
			var doNotReload_m= false;
			
			// level 2
			var IS_BID = -1;
			var IS_ASK = 1;
			var IS_DUMMY = 0;
			
			var window_level2_m = null;
			
			


			function create_hist_grid()
			{
				histGrid_m = new dhtmlXGridObject("histGridObj");

				//histGrid_m.setImagePath("/imgs/grid/");
				histGrid_m.setImagePath("js/grid3/codebase/imgs/");
				histGrid_m.setEditable(false);
				histGrid_m.setHeader("RT Provider, Provider Time, Symbol, open, high, low, close, open2, high2, low2, close2, volume");
				histGrid_m.setInitWidths("130,130,130,80,80,80,80,80,80,80,80,80");
				histGrid_m.setColTypes("ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro,ro");
				histGrid_m.setColAlign("left,left,left,left,left,left,left,left,left,left,left,left");
				histGrid_m.setColSorting("str,na,str,na,na,na,na,na,na,na,na,na");
				histGrid_m.enableResizing("true,true,true,true,true,true,true,true,true,true,true,true");
				histGrid_m.enableTooltips("false,false,false,false,false,false,false,false,false,false,false,false");

				histGrid_m.init();
			}
			

			function create_price_grid()
			{
				priceGrid_m = new dhtmlXGridObject("rtGridObj");

				//priceGrid_m.setImagePath("/imgs/grid/");
				priceGrid_m.setImagePath("js/grid3/codebase/imgs/");
				priceGrid_m.setEditable(false);
				priceGrid_m.setHeader("RT Provider, Provider Time, Symbol, Bid, Ask, Volume");
				priceGrid_m.setInitWidths("130,130,130,80,80,80");
				priceGrid_m.setColTypes("ro,ro,ro,ro,ro,ro");
				priceGrid_m.setColAlign("left,left,left,left,left,left");
				priceGrid_m.setColSorting("str,na,str,na,na,na");
				priceGrid_m.enableResizing("true,true,true,true,true,true");
				priceGrid_m.enableTooltips("false,false,false,false,false,false");

				priceGrid_m.init();
			}

			function create_level1_grid()
			{
				priceGridLevel1_m = new dhtmlXGridObject("rtGridLevel1Obj");

				//priceGridLevel1_m.setImagePath("/imgs/grid/");
				priceGridLevel1_m.setImagePath("js/grid3/codebase/imgs/");
				priceGridLevel1_m.setEditable(false);
				priceGridLevel1_m.setHeader("RT Provider, Provider Time, Symbol, Best Bid Price, Best Bid Volume, Best Ask Price, Best Ask Volume, Open Interest");
				priceGridLevel1_m.setInitWidths("130,130, 130,100,100,100,100,100");
				priceGridLevel1_m.setColTypes("ro,ro,ro,ro,ro,ro,ro,ro");
				priceGridLevel1_m.setColAlign("left,left,left,left,left,left,left,left");
				priceGridLevel1_m.setColSorting("str,na,str,na,na,na,na,na");
				priceGridLevel1_m.enableResizing("true,true,true,true,true,true,true,true");
				priceGridLevel1_m.enableTooltips("false,false,false,false,false,false,false,false");

				priceGridLevel1_m.init();

			}
			
			function create_level2_grid()
			{
				priceGridLevel2_m = new dhtmlXGridObject("rtGridLeve2Obj");

				//priceGridLevel2_m.setImagePath("/imgs/grid/");
				priceGridLevel2_m.setImagePath("js/grid3/codebase/imgs/");
				priceGridLevel2_m.setEditable(false);
				priceGridLevel2_m.setHeader("RT Provider, Provider Time, Symbol, Launch L2");
				priceGridLevel2_m.setInitWidths("130,130, 130,100");
				priceGridLevel2_m.setColTypes("ro,ro,ro,ro");
				priceGridLevel2_m.setColAlign("left,left,left,left");
				priceGridLevel2_m.setColSorting("str,na,str,na");
				priceGridLevel2_m.enableResizing("true,true,true,true");
				priceGridLevel2_m.enableTooltips("false,false,false,false");

				priceGridLevel2_m.init();
			}
			
			function on_load()
			{
							
				doNotReload_m = false;
				create_price_grid();
				create_level1_grid();
				create_hist_grid();
				create_level2_grid();
				
				var iframe_elem = document.getElementById("frmContentObj");

				url_m = '/htRTProviderManager_RealTimePrices.jsp?providers='+encodeURIComponent('<%= request.getParameter("providers") %>');
				iframe_elem.onload = disconnected;
				iframe_elem.src = url_m;

				if (iframe_elem.readyState)
					setTimeout("MS_checkiframe()", timerIntervalMsec_m);

			}

			function disconnected()
			{
				
				if (!doNotReload_m) {
					var iframe_elem = document.getElementById("frmContentObj");
					iframe_elem.src = url_m;

					if (iframe_elem.readyState)
						setTimeout("MS_checkiframe()", timerIntervalMsec_m);
				}
				
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


			
			function insert_hist_data(rtprovider, rtsymbol,  ptime, open, high, low, close, open2, high2, low2, close2, volume)
			{

			
				var row_id = rtprovider+rtsymbol;
								
				if (histGrid_m.doesRowExist(row_id)) {
					histGrid_m.cells(row_id, 1 ).setValue(ptime);
					histGrid_m.cells(row_id, 3 ).setValue(open);
					histGrid_m.cells(row_id, 4 ).setValue(high);
					histGrid_m.cells(row_id, 5 ).setValue(low);
					histGrid_m.cells(row_id, 6 ).setValue(close);
					histGrid_m.cells(row_id, 7 ).setValue(open2);
					histGrid_m.cells(row_id, 8 ).setValue(high2);
					histGrid_m.cells(row_id, 9 ).setValue(low2);
					histGrid_m.cells(row_id, 10 ).setValue(close2);
					histGrid_m.cells(row_id, 11 ).setValue(volume);

				}
				else {
					var entry = new Array( rtprovider, ptime, rtsymbol, open, high, low, close, open2, high2, low2, close2, volume );
					histGrid_m.addRow(row_id, entry);
				}
				

				
			}
			
			
			function insert_rt_data(rtprovider, rtsymbol,  ptime, bid, ask, volume, color)
			{
				var row_id = rtprovider+rtsymbol;
				
				if (priceGrid_m.doesRowExist(row_id)) {
					priceGrid_m.cells(row_id, 1 ).setValue(ptime);
					priceGrid_m.cells(row_id, 3 ).setValue(bid);
					priceGrid_m.cells(row_id, 4 ).setValue(ask);
					priceGrid_m.cells(row_id, 5 ).setValue(volume);

				}
				else {
					var entry = new Array( rtprovider, ptime, rtsymbol, bid, ask, volume );
					priceGrid_m.addRow(row_id, entry);
				}

				if (color== COLOR_BID_DEAL) {
					priceGrid_m.setRowColor(row_id,"red");
				}
				else if (color == COLOR_ASK_DEAL) {
					priceGrid_m.setRowColor(row_id,"green");
				}
				else {
					priceGrid_m.setRowColor(row_id,"window");
					
				}
				
			}

			function insert_level1_data(rtprovider, rtsymbol,  ptime, bid_price, bid_volume, ask_price ,ask_volume, open_interest)
			{
				var row_id = rtprovider+rtsymbol;
				if (priceGridLevel1_m.doesRowExist(row_id)) {
					priceGridLevel1_m.cells(row_id, 1 ).setValue(ptime);

					priceGridLevel1_m.cells(row_id, 3 ).setValue(bid_price);
					priceGridLevel1_m.cells(row_id, 4 ).setValue(bid_volume);
					priceGridLevel1_m.cells(row_id, 5 ).setValue(ask_price);
					priceGridLevel1_m.cells(row_id, 6 ).setValue(ask_volume);
					priceGridLevel1_m.cells(row_id, 7 ).setValue(open_interest);
				}
				else {
					var entry = new Array( rtprovider, ptime, rtsymbol, bid_price, bid_volume, ask_price, ask_volume, open_interest );
					priceGridLevel1_m.addRow(row_id, entry);
				}
			}
			
			function insert_level2_data(rtprovider, rtsymbol, ptime, data)
			{
				//
				var symbol = rtprovider + ' [' + rtsymbol + ']'
				
				//var obj_name = symbol+'_page';
				
			
				
				if (priceGridLevel2_m.doesRowExist(symbol)) {
					priceGridLevel2_m.cells(symbol, 1 ).setValue(ptime);
				}
				else { 
					var click_txt = "<a href='#' onclick='launch_l2_box(\""+rtprovider+"\", \""+rtsymbol+"\")'>Click</a>";
					var entry = new Array( rtprovider, ptime, rtsymbol, click_txt );
					priceGridLevel2_m.addRow(symbol, entry);
				}
				
				var to_eval = 'u=' + data;
				var data_obj = eval(to_eval);

				var cur_count = 0;
				for(var id in data_obj){
					cur_count++;
				}
					
				
					
				if (window_level2_m != null) {
					
					if (headerObj.innerText == symbol ) {
						resize_level2_table(cur_count);
						populate_level2_data(data_obj);
					}
					
					
				} 
				
			}
			
			function populate_level2_data(data_obj)
			{
				var i = 1;
				for(var id in data_obj){
					var data_arr = data_obj[id];   
					
					var price = new Number(data_arr[0]).toFixed(4);
					var type = data_arr[1];
					var volume = data_arr[2];
					var is_self = parseInt(data_arr[3]);
					var is_best_bid = parseInt(data_arr[4]);
					var is_best_ask = parseInt(data_arr[5]);
					
					// from the first
				
					var row_i = level2table_obj.rows(i);
					
					row_i.cells(0).innerHTML = '';
					row_i.cells(1).innerHTML = '';
					row_i.cells(2).innerHTML = '';
					row_i.cells(3).innerHTML = '';
					
					row_i.cells(0).bgColor = 'Beige'; // Buy
					row_i.cells(2).bgColor = 'Bisque'; // Sell
					
					row_i.cells(3).bgColor = 'Yellow';  // ID
					
					
					row_i.cells(0).style.border = '';
					row_i.cells(2).style.border = '';
					
					// ID
					row_i.cells(3).innerHTML = i;
					
					// price
					row_i.cells(1).innerHTML = price;
						
					// delegate
					if (type == IS_BID) {
						// BUY
						row_i.cells(0).innerHTML = volume;
						if (is_self != 0)
							row_i.cells(0).bgColor = 'red';
						if (is_best_bid != 0)
							row_i.cells(0).style.border='1px solid black';
					}
					else if (type== IS_ASK) {
						// SELL
						row_i.cells(2).innerHTML = volume;
						if (is_self != 0)
							row_i.cells(2).bgColor = 'lime';
						if (is_best_ask != 0)
							row_i.cells(2).style.border='1px solid black';
					}
						
					
					
					i++;
					
				}
			}
			
			function resize_level2_table(newsize)
			{
				var rowCount = level2table_obj.rows.length; 
				
				// the first row is header
				rowCount--;
				if (rowCount > newsize) {
					var to_remove = rowCount -newsize;
					for(i = 0; i < to_remove; i++) {
						
						var nr = level2table_obj.rows.length; 
						level2table_obj.deleteRow(nr-1);

					}
				}
				else if (rowCount < newsize) {
					var to_insert = newsize -rowCount;
				
					
					for(i = 0; i < to_insert; i++) {
						var rowCount = level2table_obj.rows.length; 
						var row = level2table_obj.insertRow(rowCount);
						var cell_1 = row.insertCell(0); 
						var cell_2 = row.insertCell(1); 
						var cell_3 = row.insertCell(2); 
						var cell_4 = row.insertCell(3); 
					}
					
				}
			}
			
			function launch_l2_box(rtprovider, symbol)
			{
				
				//Event.observe(window, 'load', init, false);
				
				if (window_level2_m == null) {
					window_level2_m = new Draggable('formsettings');
					$('formsettings').style.display = 'inline';
					headerObj.innerText = rtprovider + " [" + symbol + "]";
				}
			}

			function insertErrorData(data)
			{
				doNotReload_m = true;
				ErrorDataObj.innerHTML += (data + "\n");
			}

			function finishInsertingErrorData()
			{

				alert(ErrorDataObj.innerHTML);
			}

			function deinit_level2_window()
			{
				if (window_level2_m != null) {
					$('formsettings').style.display = 'none';
					window_level2_m.destroy();
					window_level2_m = null;
				}
			}
			
			




		</script>
		<style type="text/css">
			.window {
				background: #d8e7fe;
				position: absolute;
				top: 200px;
				left: 200px;
				width: 200px;
				padding: 1px;
				height: 400px;
				-moz-border-radius: 7px;
				border: 2px solid #21416d;
			}
			.window_div {
				-moz-border-radius-topleft: 5px;
				-moz-border-radius-topright: 5px;
				padding: 6px;
				display: block;
				background-color: #9cc6ff;
				font-size: 13px;
				margin: 0px;
			}



		</style>
	</head>
	<body id='bodyObj' onload="on_load();">

		<div id="formsettings" class="window" style="display:none">
			<div id="formsettings_header" class="window_div">
				<div id="headerObj"></div>
				<img src="imgs/etc/close.jpg" onclick="deinit_level2_window();" />
			</div>

			<div class="windowinner" STYLE="overflow: scroll;width:200px;height:410px">
				<table id="level2table_obj" STYLE="font-size:smaller">
					
					<tr bgcolor="buttonface" class="header">
						<td>
							Buy
						</td>
						<td>
							Price
						</td>
						<td>
							Sell
						</td>
						<td>
							ID
						</td>
					</tr>	
				</table>	

			</div>
		</div>




		<div id="ErrorDataObj"width="100%" height="100%" class="x8" style="display:none" ></div>
		<table id="dataObj_Tbl" width="100%" height="100%" class="x8">

			<tr>
				<td class="x3"width="100%" height="20px" bgcolor="buttonface">
					<div align="left">History Quotes</div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="25%" style="border:1px black solid">
					<div  id="histGridObj" style='height:100%; width:100%; '></div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="20px">
				</td>
			</tr>

			<tr>
				<td class="x3"width="100%" height="20px" bgcolor="buttonface">
					<div align="left">Quotes</div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="25%" style="border:1px black solid">
					<div  id="rtGridObj" style='height:100%; width:100%; '></div>
				</td>
			</tr>



			<tr>
				<td width="100%" height="20px">
				</td>
			</tr>

			<tr>
				<td class="x3"width="100%" height="20px" bgcolor="buttonface">
					<div align="left">Level 1 Data</div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="25%" style="border:1px black solid">
					<div  id="rtGridLevel1Obj" style='height:100%; width:100%; '></div>
				</td>
			</tr>

			<tr>
				<td class="x3"width="100%" height="20px" bgcolor="buttonface">
					<div align="left">Level 2 Data</div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="25%" style="border:1px black solid">
					<div  id="rtGridLeve2Obj" style='height:100%; width:100%; '>

					</div>
				</td>
			</tr>

			<tr>
				<td width="100%" height="20px" bgcolor="buttonface">

				</td>
			</tr>

		</table>

		<iframe id="frmContentObj" width=100px height=100px style="display:none"></iframe>


	</body>
</html>