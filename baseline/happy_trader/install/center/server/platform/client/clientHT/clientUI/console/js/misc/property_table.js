// ---------------------------------
function ExportImportXGrid(xGrid, pageUid)
{
	//-----------------

	this._checkDependen = function()
	{
		// helper.js
		try {
			helper_is_included();
		}
		catch(e)
		{
			alert('helper.js must be included before!');
			return false;
		}

		return true;
	}

	this._initExportForm = function()
	{


		// init export form
		var exportForm = document.createElement("form");
		exportForm.id="frm953CC25E-F373-471f-BED4-7E2D15F3FB3F";
		exportForm.style.display='none';

		exportForm.method='post';
		exportForm.innerHTML = '<input type=hidden name="table_xml_data" value=""></input>';

		
		document.body.appendChild(exportForm);

		return exportForm;
		
	}
	
	this._getXGridAllRowsAllColumns_XMLParameter = function()
	{
		return this._getXGridSelectedRowsAllColumns_XMLParameter(true);
	}

	this._getXGridSelectedRowsAllColumns_XMLParameter = function(process_all_rows)
	{
		var result='<?xml version="1.0" encoding="ISO-8859-1"?>';
		result += '<ent name="xrid_serialization">';

		var rowNum = this.xGrid_m.getRowsNum();
		var colNum = this.xGrid_m.getColumnsNum();

		var selectedRows = new String(this.xGrid_m.getSelectedRowId());
		var selectedRowArr = selectedRows.split(",");


		// columns
		result += '<par name="columns" type="4104">';

		for(i=0; i < colNum; i++) {
			result += '<val>'+this.xGrid_m.getColumnLabel(i)+'</val>';
		}
		result += '</par>';

		result += '<xmlpar name="data">';
		result += '<ent name="data">';

		result += '<par name="rows_num" type="1">';
		result += '<val>'+selectedRowArr.length+'</val>';
		result += '</par>';

		result += '<xmlparlist name="rows">';


		for(i = 0; i < rowNum; i++ ) {


			var rowId = this.xGrid_m.getRowId(i);

			// if it is selected
			var found = false;
			
			if (process_all_rows == null) {
				// search selected
				for(r = 0; r < selectedRowArr.length; r++) {
					if (selectedRowArr[r]==rowId) {
						found = true;
						break;
					}
				}
			}
			else {
				found = true;
			}

			// selected
			if (found) {

				result += '<ent name="">';
				for(k = 0; k < colNum;k++) {

					var col_label = this.xGrid_m.getColumnLabel(k);
					var value = this.xGrid_m.cells(rowId, k).getValue();

					result += '<xmlpar name="'+col_label+'">';
					result += '<ent name="">';
					result += '<par name="value" type="8">'; // type of parameter - so far VT_String
					result += '<val><![CDATA['+value+']]></val>';
					result += '</par>';
					result += '</ent>';
					result += '</xmlpar>';

				}

				result += '</ent>';
			}



		}; // row loop

		result += '</xmlparlist>';
		result += '</ent>';
		result += '</xmlpar>';
		result += "</ent>";


		return result;
	}

	this._checkErrorSessionVar = function()
	{
		// check for errors
		var error_info_url = '/HtReturnSessionVar_v2.jsp?response_type=xml&session_var=error_info&cookie='+ this.page_uid_m;
		this.xmlErrorhttp_m.open('GET', error_info_url, false);
		this.xmlErrorhttp_m.send(null);


		if (this.xmlErrorhttp_m.responseXML!=null && this.xmlErrorhttp_m.responseText != null && this.xmlErrorhttp_m.responseText.length>0)	{
			document.body.innerHTML = extractErrorMessage(this.xmlErrorhttp_m.responseXML);
			return true;
		}

		return false;
	}

	// public
	// import
	//
	this.exportSelectedRows = function(prop_id)
	{
		if (this.exportForm_m==null) {
			this.exportForm_m = this._initExportForm();
		}

		var url = "/HtPropertyTable_v2.jsp?page_uid="+encodeURIComponent(this.page_uid_m)+
		"&type=EXPORT_CSV"+	"&prop_id="+encodeURIComponent(prop_id);

		var table_data = this._getXGridSelectedRowsAllColumns_XMLParameter();

		this.exportForm_m.action = url;
		this.exportForm_m.table_xml_data.value = table_data;

		//this.exportForm_m.target=prop_id+(new Date()).getMilliseconds();

		this.exportForm_m.submit();

	}
	
	this.serializeAllRowsAsXmlParameter = function()
	{
		var table_data = this._getXGridAllRowsAllColumns_XMLParameter();
		return table_data;
	}
	
	// export all rows
	this.exportAllRows = function(prop_id)
	{
		if (this.exportForm_m==null) {
			this.exportForm_m = this._initExportForm();
		}
		
		var table_data = this._getXGridAllRowsAllColumns_XMLParameter();
		
		var url = "/HtPropertyTable_v2.jsp?page_uid="+encodeURIComponent(this.page_uid_m)+
		"&type=EXPORT_CSV"+	"&prop_id="+encodeURIComponent(prop_id);
	
	
		
		this.exportForm_m.action = url;
		this.exportForm_m.table_xml_data.value = table_data;

		//this.exportForm_m.target=prop_id+(new Date()).getMilliseconds();

		this.exportForm_m.submit();
		
	}

	this.importRows = function()
	{
		if (this.xmlhttp_m==null) {
			this.xmlhttp_m = getXmlHttp();
			this.xmlErrorhttp_m = getXmlHttp();
		}
	
		// first of all show dialog
		var importData = window.showModalDialog(
			'htTextInput.jsp',
			//'htTextInput_iFrame.jsp',
			'',
			'resizable:1;dialogHeight=550px;dialogWidth=600px;help:0;status:0;scroll:0'
			);

		if (importData==null) {
			return;
		}

		var url = "/HtPropertyTable_v2.jsp?page_uid="+encodeURIComponent(this.page_uid_m)+"&type=IMPORT_CSV";

		var data = "table_csv_data="+importData;

	
		this.xmlhttp_m.open('POST', url, false);

		this.xmlhttp_m.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		if (data != null) {
			this.xmlhttp_m.setRequestHeader("Content-length", data.length);
		}

		this.xmlhttp_m.setRequestHeader('Accept',	'image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*');
		this.xmlhttp_m.send(data);

		var xrid_data = this.xmlhttp_m.responseText;
		var xgrid_xml = this.xmlhttp_m.responseXML;

		// in case of error xml is not valid
	
	

		if(this._checkErrorSessionVar()) {
			return;
		}
	
	

		if (xrid_data != null && xrid_data.length > 0) {
	
			// ready to load
			// first of all will check column names
			var result = xgrid_xml.selectNodes("/rows/head/column");
			if (result==null) {
				alert('Internal error in ExportImportXGrid: cannot find columns');
				return;
			}

			var colNum = this.xGrid_m.getColumnsNum();
			var colNumEqual = 0;
			for(i=0;i<result.length;i++){
				//alert(result[i].text);

		
				for(k = 0; k < colNum; k++) {
					if (this.xGrid_m.getColumnLabel(k) == result[i].text) {
						colNumEqual++;
						break;
					}
				}

			}
			//
			if (colNumEqual!=colNum) {
				// non-matched columns
				alert('Non-matched columns in the imported data!');
				return;
			}

			// make real import
			this.xGrid_m.clearAll(false);
			//lert(xgrid_xml.text);
			//this.xGrid_m.parseXML(xgrid_xml);

		
			var rows = xgrid_xml.selectNodes("/rows/row");
			if (rows==null) {
				alert('Internal error in ExportImportXGrid: cannot find rows');
				return;
			}
			var rowsImportedNum = rows.length;
			var cols = xgrid_xml.selectNodes("/rows/row/cell");

			var cellsImportedNum = cols.length;

			var cols_per_row = cellsImportedNum / rowsImportedNum;
		
			var colidx = 1;
			var rowidx = 1;

	
			for(i=0;i<cellsImportedNum;i++){

				if (colidx==1) {
					this.xGrid_m.addRow(rowidx, "");
			
				}

				this.xGrid_m.cells(rowidx, colidx-1).setValue(cols[i].text);

				if (colidx >= cols_per_row) {
					colidx = 1;
					rowidx++;
				}
				else {
					colidx++;
				}
			
			
			}
		

		};

	
	
	};

	//
	//
	this.dependency_included_m = this._checkDependen();
	
	///////////////////////////
	// members
	this.xGrid_m = xGrid;
	this.page_uid_m = pageUid;
	this.exportForm_m = null;
	this.xmlhttp_m = null;
	this.xmlErrorhttp_m = null;
	
}; // end of class



