<!-- This jsp provides selection of one or more items via checkboxes -->
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<!-- DHTML grid -->
		<link rel="STYLESHEET" type="text/css" href="css/dhtmlxgrid.css">

		<script  src="js/grid/dhtmlxcommon.js"></script>
		<script  src="js/grid/dhtmlxgrid.js"></script>
		<script  src="js/grid/dhtmlxgridcell.js"></script>
		<script  src="js/grid/dhtmlxgrid_start.js"></script>

		<title></title>
		<script type="text/javascript">
			var grid_m = null;
			function on_load()
			{
				var args = window.dialogArguments;
				if (args != null) {
					textObj.innerText = args['comment_text'];
					var itemListArr = args['items'];
					var selectedItems = args['selected_items'];
					

					grid_m = new dhtmlXGridObject("gridObj");

					grid_m.setImagePath("imgs/grid/");
					grid_m.setEditable(true);
					grid_m.setHeader("Item, Select");
					grid_m.setInitWidths("*,40");
					grid_m.setColTypes("ro,ch");
					grid_m.setColAlign("left,center");
					grid_m.setColSorting("str,na");
					grid_m.enableResizing("true,true");
					grid_m.enableTooltips("false,false");

					grid_m.init();

					// add items

					if (itemListArr != null) {
						for (var i in itemListArr) {
							var entry = new Array( itemListArr[i], "" );
							grid_m.addRow(i, entry);

							if (selectedItems != null) {

																
								if (selectedItems[i] == 1 ) {
									grid_m.cells(i, 1).setValue(1);
								} else {
									grid_m.cells(i, 1).setValue(0);
								}
								
							}
							
							
						}

												
					}

				} // end args != null
			}


			function on_close()
			{


				var result = new Array();

				// returns only selected items
				for(i = 0; i < grid_m.getRowsNum(); i++ ) {
					var rowId = grid_m.getRowId(i);

					var item_i = grid_m.cells(rowId, 0).getValue();
					var selected_i = grid_m.cells(rowId, 1).getValue();

					if (selected_i==1) {
						result[rowId] = item_i;
						//alert(item_i + ' - ' + selected_i);
					}

					
				}
				

				window.returnValue = result;
				window.close();
			}

			function on_cancel()
			{
				window.returnValue = null;
				window.close();
			}

			function check_all()
			{
				for(i = 0; i < grid_m.getRowsNum(); i++ ) {
					var rowId = grid_m.getRowId(i);
					grid_m.cells(rowId, 1).setValue(1);
				}
			}
			
		</script>

	</head>
	<body onload="on_load();">
		<table cellpadding="4" cellspacing="4" width="100%" height="100%" border="0" class=x8>
			<tr>
				<td align=left id="textObj" class=x63 height="20px">
				</td>
			</tr>

			
			<tr>
				<td  align=left>
					<div id="gridObj" style="width:100%;height:100%;border-style:solid; border-width:1px;">
					</div>
				</td>
			</tr>

			<tr>
				<td height="20px">
				</td>
			</tr>

			<tr>
				<td height="20px">
					<table width=100% class=x8>
						<tr>
							<td>
							</td>

							<td align=right width=110px>
								<a href='#' onclick="check_all();">Check All</a>
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
			</tr>


		</table>
	</body>
</html>