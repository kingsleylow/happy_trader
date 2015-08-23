function helper_is_included()
{
	return 1;
}

function getTimeZoneOffsetMsec()
{
	return (new Date()).getTimezoneOffset() * 60 * 1000;
}

function parseDateTimeToGmt(dtStr)
{
	var parsedDate = Date.parseExact(dtStr, 'd-M-yyyy HH:mm');
	if (parsedDate==null) {
		//alert('The date must be in the format: "d-M-yyyy HH:mm", whereas passed is: '+ dtStr);
		return -1;
	}


	var shift_millisec = (new Date()).getTimezoneOffset() * 60 * 1000;
	var gmt_time = parsedDate.getTime() + shift_millisec;

	return gmt_time;

}

function millisecDateToString(mdate)
{
	return dateToString(new Date(mdate));
}

function dateToString(mdate)
{
	// "%d-%m-%Y %H:%M"
	var month = new String(mdate.getMonth() + 1);
	if (month.length < 2)
		month = "0"+month;
	    
	var date =  new String(mdate.getDate());
	if (date.length < 2)
		date = "0"+date;
	
	var hours =  new String(mdate.getHours());
	if (hours.length < 2)
		hours = "0"+hours;
	    
	var minutes =  new String(mdate.getMinutes());
	if (minutes.length < 2)
		minutes = "0"+minutes;
	    
	var result = date + "-" + month + "-" +  mdate.getFullYear() + " " + hours + ":"+ minutes;
	    
	return result;
}

function dateToString2(mdate)
{
	// "%d-%m-%Y %H:%M"
	var month = new String(mdate.getMonth() + 1);
	if (month.length < 2)
		month = "0"+month;
	    
	var date =  new String(mdate.getDate());
	if (date.length < 2)
		date = "0"+date;
	
	var hours =  new String(mdate.getHours());
	if (hours.length < 2)
		hours = "0"+hours;
	    
	var minutes =  new String(mdate.getMinutes());
	if (minutes.length < 2)
		minutes = "0"+minutes;
	
	var seconds =  new String(mdate.getSeconds());
	if (seconds.length < 2)
		seconds = "0"+seconds;
	
	    
	var result = date + "-" + month + "-" +  mdate.getFullYear() + " " + hours + ":"+ minutes + ":"+seconds;
	    
	return result;
}

// this allows select path on local drives
function selectLocalPath()
{
	
	var newFormElement = document.createElement('form');
	newFormElement.method='post';
	newFormElement.innerHTML = '<input type="file" name="dummy_file_param">';
    
    
	newFormElement.dummy_file_param.click();
	var result = newFormElement.dummy_file_param.value;
    
	return result;
}

// this function allows
// xGrid - xgrid refe
// cbId -  indidec of check box column
// colArr = array of columns we want to reap the results
function getXGridSelectedRows(xGrid, cbId, colArr )
{
	var result="";
	var rowNum = xGrid.getRowsNum();
    
	for(i = 0; i < rowNum; i++ ) {
		var rowId = xGrid.getRowId(i);
		if (xGrid.getColType(cbId)=='ch') {
			cb_cell = xGrid.cells(rowId, cbId );
            
			var cb_cell_value = cb_cell.getValue();
			
            
			if (cb_cell_value==1) {
                
				// checked
				for(k = 0; k < colArr.length;k++) {
					var value = xGrid.cells(rowId, colArr[k]).getValue();
					result += value + ",";
				}
			}
		}
		else {
			alert('The column is not of "ch" type');
			return undefined;
		}
	}
    
	return result;
}

function getCommaSeparatedList(dataarr)
{
	var result = "";
	if (dataarr != null)
	{
		for(i = 0; i < dataarr.length; i++)
		{
			result += dataarr[i] + ",";
			
		}
	}

	return result;
}

function getXGridAllRows(xGrid, colArr )
{
	var result="";
	var rowNum = xGrid.getRowsNum();
    
	for(i = 0; i < rowNum; i++ ) {
		var rowId = xGrid.getRowId(i);

	           
       
		for(k = 0; k < colArr.length;k++) {
			var value = xGrid.cells(rowId, colArr[k]).getValue();
			result += value + ",";
		}
        
	}
    
	return result;
}

function getXGridSelectedRowsForPropertyTable(xGrid, colArr )
{
	var result="";
	var rowNum = xGrid.getRowsNum();
    
	var selectedRows = new String(xGrid.getSelectedRowId());
	var selectedRowArr = selectedRows.split(",");
    
       
	for(i = 0; i < rowNum; i++ ) {
		var rowId = xGrid.getRowId(i);
        
		var found = false;
		for(r = 0; r < selectedRowArr.length; r++) {
			if (selectedRowArr[r]==rowId) {
				found = true;
			}
		}
        
		if (found) {
			for(k = 0; k < colArr.length;k++) {
				var value = xGrid.cells(rowId, colArr[k]).getValue();
				result += value + ",";
			}
		}
        
	}
    
	return result;
}

// this passes selected columns as url parameter
function serializeXGridToParameter(xGrid, colArr)
{
	var result="";
	var rowNum = xGrid.getRowsNum();
	var colsNum = xGrid.getColumnsNum();
    
	result += (rowNum+","+colsNum+",");
    
	for(i = 0; i < rowNum; i++ ) {
		var rowId = xGrid.getRowId(i);
            
       
		for(k = 0; k < colArr.length;k++) {
			var value = xGrid.cells(rowId, colArr[k]).getValue();
			result += (xGrid.getColumnLabel(colArr[k]) + "," + value + ",");
		}
        
	}
    
	return result;
    
}

function initTableWithDummyWhitspaces(table_id)
{
	
	var tbl = document.getElementById(table_id);

	if (tbl != undefined) {
		for (i=0; i < tbl.rows.length; i++) {
			for (j=0; j < tbl.rows(i).cells.length; j++) {
				//alert(i + ' - ' + j + ' -- (' + tbl.cells(j).innerText +')');

				if (tbl.rows(i).cells(j).innerHTML.length <=0) {
					// just add
					tbl.rows(i).cells(j).innerText = ' ';
				}

			}
		}

	}
	else {
		alert("Internal error: table \""+table_id+"\" cannot be undefined");
	}

	
}


//
function setPageCaption(txtCapt) {
  
	var pTop = top;
	if (pTop==null)
		return;
    
	var tFrame = pTop.treeframe;
	if (tFrame != null) {
		tFrame.printHint(txtCapt);
	}
    
}

function browserType() {
	var b=(function x(){})[-5]=='x'?'FF3':(function x(){})[-6]=='x'?'FF2':/a/[-1]=='a'?'FF':'\v'=='v'?'IE':/a/.__proto__=='//'?'Saf':/s/.test(/a/.toString)?'Chr':/^function \(/.test([].sort)?'Op':'Unknown';
	return b;
}

/*
// creates xml http request
function getXmlHttp(){
	var xmlhttp;
	try {
		xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
	
	} catch (e) {
		try {
			xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
		
		} catch (E) {
			xmlhttp = false;
		}
	}
	if (!xmlhttp && typeof XMLHttpRequest!='undefined') {
		xmlhttp = new XMLHttpRequest();
		
	}
	return xmlhttp;
}
*/

function getXmlHttp()
{
	var xmlhttp = null;
	try {
		xmlhttp = new window.XMLHttpRequest;

	} catch (E) {
			alert('Your IE dows not suppport window.XMLHttpRequest');
			
	}

	return xmlhttp;
}


function makeAsynchXmlHttpRequest(toutsecs, method, url, data, respHandler, errHandler)
{
	var xmlhttp = getXmlHttp();
	xmlhttp.open(method, url, true);


	xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	if (data != null) {
		xmlhttp.setRequestHeader("Content-length", data.length);
	}
	xmlhttp.setRequestHeader('Accept',	'image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*');
	

	var timeout = -1;

	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState != 4) 
			return;

		if (timeout > 0)
			clearTimeout(timeout);


		if (xmlhttp.status == 200) {
			// 
			if (respHandler != null) {
				respHandler(xmlhttp);
			}

		} else {
			if (errHandler != null) {
				errHandler(xmlhttp.statusText)
			}
		}
	};



	// do
	
	xmlhttp.send(data);
	if(toutsecs > 0) {
		timeout = setTimeout( function(){ 
			xmlhttp.abort(); 
			handleError("Time over");
		},
		toutsecs*1000);
	}

}

function makeAsynchXmlHttpRequestExternHandler(xmlhttp, toutsecs, method, url, data, respHandler, errHandler)
{
	xmlhttp.open(method, url, true);
	xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	if (data != null) {
		xmlhttp.setRequestHeader("Content-length", data.length);
	}
	
	xmlhttp.setRequestHeader('Accept',	'image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*');


	var timeout = -1;

	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState != 4)
			return;

		if (timeout > 0)
			clearTimeout(timeout);


		if (xmlhttp.status == 200) {
			// 
			if (respHandler != null) {
				respHandler(xmlhttp);
			}

		} else {
			if (errHandler != null) {
				errHandler(xmlhttp.statusText)
			}
		}
	};



	// do
	xmlhttp.send(data);
	if(toutsecs > 0) {
		timeout = setTimeout( function(){
			xmlhttp.abort();
			handleError("Time over");
		},
		toutsecs*1000);
	}

}

function extractErrorMessage(respxml)
{
	var strresult = "";
	if (respxml==null || respxml==undefined)
		return "";

	var result = respxml.selectNodes("/errorinfo/entry");
	if (result==null)
		return "";

	for(var i=0;i<result.length;i++){
		strresult += result[i].text;
	}

	return strresult;
	
}


function getElementsByClassName(cl) {
	var retnode = [];
	var myclass = new RegExp('\\b'+cl+'\\b');
	var elem = document.getElementsByTagName('*');
	for (var i = 0; i < elem.length; i++) {
		var classes = elem[i].className;
		if (myclass.test(classes)) {
			retnode.push(elem[i]);
		}
	}
	return retnode;
}

function deselectXGridAllRows(xGrid) {
	xGrid.clearSelection();
}

function selectXGridAllRows(xGrid) {
	xGrid.selectAll();
}

function deleteSelectedItems(xGrid) {
	xGrid.deleteSelectedItem();
}

function addNewRowToXGrid(xGrid) {
	var num = xGrid.getRowsNum();
	var maxId = 0;

	if (num==null || num < 0) {
		num=0;
		maxId = 0;
	}
	else {
		maxId = getMaxRowId(xGrid);
		maxId++;
	}

		

	var colsNum = xGrid.getColumnsNum();
	var vals = "";
	if (colsNum >1)
		vals = new Array(colsNum);

	var curRow = getXGridCurrentlySelectedRowIndex(xGrid);
	if (curRow >= 0) {
		xGrid.addRow(maxId, vals,curRow );
	}
	else {
		xGrid.addRow(maxId, vals);
	}
	
	// then we need to iterate ove this row and set zero to all checkboxes
	for(i=0; i < colsNum;i++) {
		cb_cell = xGrid.cells(maxId, i );

		if (xGrid.getColType(i)=='ch') {

			xGrid.cells(maxId, i).setValue(0);
		}
	}

	maxId = getMaxRowId(xGrid);
		
}

// resolve maximum row ID assuming they are comparable
function getMaxRowId(xGrid) {
	var rowNum = xGrid.getRowsNum();
	var maxRowId = -1;
	for(i = 0; i < rowNum; i++ ) {
		var rowId = parseInt( xGrid.getRowId(i) );

		if (i==0) {
			maxRowId=rowId;
			continue;
		}

		if (rowId > maxRowId)
			maxRowId = rowId;
	}
	//

		
	return maxRowId;
}

// return currently selected row index or -1
function getXGridCurrentlySelectedRowIndex(xGrid)
{
	var selRowsArray_str = (new String(xGrid.getSelectedRowId())).split(',');
	
	if (selRowsArray_str.length > 0)
		return xGrid.getRowIndex( selRowsArray_str[selRowsArray_str.length-1] );

	return -1;
}

// This will parse a delimited string into an array of
// arrays. The default delimiter is the comma, but this
// can be overriden in the second argument.
function CSVToArray( strData, strDelimiter ){
	// Check to see if the delimiter is defined. If not,
	// then default to comma.
	strDelimiter = (strDelimiter || ",");

	// Create a regular expression to parse the CSV values.
	var objPattern = new RegExp(
		(
			// Delimiters.
			"(\\" + strDelimiter + "|\\r?\\n|\\r|^)" +

			// Quoted fields.
			"(?:\"([^\"]*(?:\"\"[^\"]*)*)\"|" +

			// Standard fields.
			"([^\"\\" + strDelimiter + "\\r\\n]*))"
			),
		"gi"
		);

	// Create an array to hold our data. Give the array
	// a default empty first row.
	var arrData = [[]];

	// Create an array to hold our individual pattern
	// matching groups.
	var arrMatches = null;

	// Keep looping over the regular expression matches
	// until we can no longer find a match.
	while (arrMatches = objPattern.exec( strData )){

		// Get the delimiter that was found.
		var strMatchedDelimiter = arrMatches[ 1 ];


		// Check to see if the given delimiter has a length
		// (is not the start of string) and if it matches
		// field delimiter. If id does not, then we know
		// that this delimiter is a row delimiter.
		if (
			strMatchedDelimiter.length &&
			(strMatchedDelimiter != strDelimiter)
			){

			// Since we have reached a new row of data,
			// add an empty row to our data array.
			arrData.push( [] );

		}

		// Now that we have our delimiter out of the way,
		// let's check to see which kind of value we
		// captured (quoted or unquoted).
		if (arrMatches[ 2 ]){


			// We found a quoted value. When we capture
			// this value, unescape any double quotes.
			var strMatchedValue = arrMatches[ 2 ].replace(
				new RegExp( "\"\"", "g" ),
				"\""
				);


		} else {

			// We found a non-quoted value.
			var strMatchedValue = arrMatches[ 3 ];

		}

		// Now that we have our value string, let's add
		// it to the data array.
		arrData[ arrData.length - 1 ].push( strMatchedValue );
	}


	// Return the parsed data.
	return( arrData );
}

function wrapErrorToHTML(sMsg)
{
		var errorHTML = "";
		errorHTML += "<link href='css/console.css' rel='stylesheet' type='text/css'/></head> ";


		errorHTML += " <table cellspacing='0' cellpadding='0' border='0' width='80%'>  ";
		errorHTML += " <tr> ";
		errorHTML += " <td width='5%' bgcolor='#cccc99'>&nbsp;</td> ";
		errorHTML += " <td width='90%' bgcolor='#cccc99'> ";
		errorHTML += "         <table cellspacing='0' cellpadding='0' border='0' width='100%'> ";
		errorHTML += "           <tr> ";
		errorHTML += "             <td>&nbsp;</td> ";
		errorHTML += "            </tr> ";
		errorHTML += "            <tr> ";
		errorHTML += "               <td class='OraErrorHeader' ><img src='imgs/errorl.gif'/>Error ";
		errorHTML += "                 <hr noshade size='1'/> ";
		errorHTML += "               </td> ";
		errorHTML += "            </tr> ";
		errorHTML += "            <tr> ";
		errorHTML += "               <td class='OraErrorText'> ";
		errorHTML += "                 &nbsp; ";
		errorHTML += "               </td> ";
		errorHTML += "            </tr> ";
		errorHTML += "            <tr> ";
		errorHTML += "               <td class='OraErrorText'> ";
		errorHTML += sMsg;
		errorHTML += "               </td> ";
		errorHTML += "            </tr> ";
		errorHTML += "            <tr> ";
		errorHTML += "               <td>&nbsp;</td> ";
		errorHTML += "            </tr>  ";
		errorHTML += "         </table>  ";
		errorHTML += "     </td>  ";
		errorHTML += "     <td width='5%' bgcolor='#cccc99'>&nbsp;</td> ";
		errorHTML += "   </tr>  ";
		errorHTML += "   </table> ";

		return  errorHTML;
}


function pathToFile(str) {   
  var nOffset = Math.max(0, Math.max(str.lastIndexOf('\\'), str.lastIndexOf('/')));  
  var eOffset = str.lastIndexOf('.');    
  if(eOffset < 0) {
	  eOffset = str.length;
  } 
  
  return {
	  isDirectory: eOffset == str.length,  
	  path: str.substring(0, nOffset),  
	  name: str.substring(nOffset > 0 ? nOffset + 1 : nOffset, eOffset),  
	  extension: str.substring(eOffset > 0 ? eOffset + 1 : eOffset, str.length)
  }; 
} 

