/*
 * This code is reposndible for creating navigation grid
 */

function NavigationBar(
  caption,
	barDivObjName,
	pageSize,
	numberOfPages,
	usrFun_generUrl,
	usrFun_Navigate,
	usrFun_SwitchOnLoadInProgress,
	usrFun_SwitchOffLoadInProgress,
	namePrfx
	)
{
	this.namePrfx_m = (namePrfx != undefined ? namePrfx:"");
	this.caption_m = caption;
	this.history_pageIndex_m = 0;
	this.historyBaseUrl_m = null;
	this.pageSize_m = pageSize;
	this.numberOfPages_m = numberOfPages;

	this.selectPageObj_m = null;
	this.pageNumObj_m = null;
	this.navigBarObj_m = null;
	this.nextNavObj_m = null;

	// user function on creation URL
	this.usrFun_generUrl_mf = usrFun_generUrl; // usrFun_generUrl_mf(history_pageIndex_m, pageSize_m)
	this.usrFun_Navigate_mf = usrFun_Navigate; // usrFun_Navigate_mf(historyBaseUrl_m)
	this.usrFun_SwitchOnLoadInProgress_mf = usrFun_SwitchOnLoadInProgress; // usrFun_SwitchOnLoadInProgress() -- optional
	this.usrFun_SwitchOffLoadInProgress_mf = usrFun_SwitchOffLoadInProgress; //usrFun_SwitchOffLoadInProgress() -- optional

	


	// ------------------
	// methods
	this.enable = function() {
		this.navigBarObj_m.disabled=false;
		this.selectPageObj_m.disabled=false;
	}

	this.disable = function() {
		

		this.navigBarObj_m.disabled=true;
		this.selectPageObj_m.disabled=true;

	}

	this.getPageIndex = function()
	{
		return this.history_pageIndex_m;
	}

	// this function is called when navigation is finished
	this.callback_NavigationFinished=function(rowsNumLoaded, isSuccess)
	{
		if (this.usrFun_SwitchOffLoadInProgress_mf != null)
			this.usrFun_SwitchOffLoadInProgress_mf();

		

		if (isSuccess)	{
			if (rowsNumLoaded > 0) {

				var lastRowIdx = 1*this.history_pageIndex_m + rowsNumLoaded;
				var firstRowIdx = 1*this.history_pageIndex_m + 1;

				this.pageNumObj_m.innerText = 'Rows from: ' + firstRowIdx  + ' to: '+ lastRowIdx;


				if (rowsNumLoaded < this.pageSize_m) {
					// no ned in prev navigation
					this.disable_last_navigation();
				}
				else {
					this.enable_last_navigation();
				}
			}
			else {
				// dataset is empty
				this.pageNumObj_m.innerText = 'No rows available';

				this.disable_last_navigation();
			}
		}
	}

	// -----------------
	// 

	

	

	// private
	//////////////////////////////////////////////////////
	// initializer
	this.init = function(barDivObjName_p)
	{
	
		var divobj = document.getElementById(barDivObjName_p);
		if (divobj==undefined || divobj==null) {
			alert('Invalid DIV: ' + barDivObjName_p);
			return;
		}

		if (this.usrFun_generUrl_mf==undefined || this.usrFun_generUrl_mf==null) {
			alert('The callback "usrFun_generUrl" must be defined');
			return;
		}

		if (this.usrFun_Navigate_mf==undefined || this.usrFun_Navigate_mf==null) {
			alert('The callback "usrFun_Navigate" must be defined');
			return;
		}

	
		var htmls = '<table id="'+this.namePrfx_m+'navigBarObj" width="100%" height="100%" class="x8">';
		htmls+=	 	'<tr>';
		htmls+=							'<td bgcolor="buttonface" >';
		htmls+=							'</td>';
		htmls+=						'<td id="'+this.namePrfx_m+'firstNavObj" width="18px "align=left bgcolor="buttonface">';
		htmls+=							'<a id="'+this.namePrfx_m+'clickObj_firstPageHistory" href="#" onclick="">';
		htmls+=								'<img src="imgs/srmvall.gif" border="0" align="absmiddle"/>';
		htmls+=							'</a>';
		htmls+=						'</td>';
		htmls+=						'<td id="prevNavObj" width="18px "align=left bgcolor="buttonface">';
		htmls+=							'<a id="'+this.namePrfx_m+'clickObj_prevPageHistory" href="#" onclick="">';
		htmls+=								'<img src="imgs/srmv.gif" border="0" align="absmiddle"/>';
		htmls+=							'</a>';
		htmls+=						'</td>';
		htmls+=						'<td width="200px" class="x3" bgcolor="buttonface">';
		htmls+=							'<div align="center">'+this.caption_m+'</div>';
		htmls+=						'</td>';
		htmls+=						'<td id="'+this.namePrfx_m+'nextNavObj" width="18px" align=right bgcolor="buttonface" >';
		htmls+=							'<a id="'+this.namePrfx_m+'clickObj_nextPageHistory" href="#" onclick="">';
		htmls+=								'<img src="imgs/smv.gif" border="0" align="absmiddle"/>';
		htmls+=							'</a>';
		htmls+=						'</td>';
		htmls+=						'<td bgcolor="buttonface">';
		htmls+=						'</td>';
		htmls+=						'<td width="80px"  align=right bgcolor="buttonface">';
		htmls+=						'Navigate to:';
		htmls+=						'</td>';
		htmls+=						'<td width="150px" align=left bgcolor="buttonface">';
		htmls+=							'<select onchange="" style="width:100%" id="'+this.namePrfx_m+'selectPageObj" >';
		htmls+=							'</select>';
		htmls+=						'</td>';
		htmls+=						'<td width="150px" id="'+this.namePrfx_m+'pageNumObj" align=right bgcolor="buttonface">';
		htmls+=						'</td>';
		htmls+=					'</tr>';
		htmls+=			'</table>';

		// init that
		divobj.innerHTML = htmls;
	
				
		// set up
		this.selectPageObj_m = document.getElementById(this.namePrfx_m+"selectPageObj");
		this.pageNumObj_m = document.getElementById(this.namePrfx_m+"pageNumObj");
		this.navigBarObj_m = document.getElementById(this.namePrfx_m+"navigBarObj");
		this.nextNavObj_m = document.getElementById(this.namePrfx_m+"nextNavObj");

		if (this.selectPageObj_m==null || this.pageNumObj_m==null || this.navigBarObj_m==null|| this.nextNavObj_m==null ||
			this.selectPageObj_m==undefined || this.pageNumObj_m==undefined || this.navigBarObj_m==undefined|| this.nextNavObj_m==undefined)
		{
			alert('Some of internal objects cannot be found');
			return;
		}

		// set up page selector
		this.setUpPageSelector();
		
		// assign some events handlers
		var clickObj_firstPageHistory_o = document.getElementById(this.namePrfx_m+"clickObj_firstPageHistory");
		var clickObj_prevPageHistory_o = document.getElementById(this.namePrfx_m+"clickObj_prevPageHistory");
		var clickObj_nextPageHistory_o = document.getElementById(this.namePrfx_m+"clickObj_nextPageHistory");

		
		var self = this;
		this.selectPageObj_m.onchange = function() {
			self.navigate_through_page_selector();
		};

		clickObj_firstPageHistory_o.onclick = function() { 
			self.first_page_history();
		};

		clickObj_prevPageHistory_o.onclick = function() {
			self.prev_page_history();
		};

		clickObj_nextPageHistory_o.onclick  = function() {
			self.next_page_history();
		};
		
		
	}

	this.setUpPageSelector = function()
	{

		this.selectPageObj_m.last_page_index = (this.numberOfPages_m  -1) * this.pageSize_m;

		for(var i = 0; i < this.numberOfPages_m;i++) {
			var oOption = document.createElement("OPTION");
			var pageIndex = i*this.pageSize_m;

			oOption.text='Rows from: ' + (pageIndex+1);
			oOption.value=pageIndex;

			this.selectPageObj_m.add(oOption);

		}

		var oOption = document.createElement("OPTION");
		oOption.text='Extend...';
		oOption.value = -9999;
		this.selectPageObj_m.add(oOption);
	}

	this.extendPageSelector= function()
	{
	

		if (this.selectPageObj_m==undefined || this.selectPageObj_m==null) {
			alert('Invalid object "selectPageObj"');
			return;
		}

		// remove the last
		var oldLastIdx = this.selectPageObj_m.options.length-1;
		this.selectPageObj_m.remove(oldLastIdx);

		// set up new number of pages to add

		var numberOfPages_add = (this.selectPageObj_m.last_page_index/this.pageSize_m + 1);


		for(var i = oldLastIdx; i < numberOfPages_add + oldLastIdx;i++) {
			var oOption = document.createElement("OPTION");
			var pageIndex = i*this.pageSize_m;

			oOption.text='Rows from: ' + ( pageIndex +1);
			oOption.value=pageIndex;

			this.selectPageObj_m.add(oOption);

		}

		var oOption = document.createElement("OPTION");
		oOption.text='Extend...';
		oOption.value = -9999;
		this.selectPageObj_m.add(oOption);

		// new

		this.selectPageObj_m.last_page_index = (numberOfPages_add*2 -1) * this.pageSize_m;


		// set up selected index
		this.selectPageObj_m.options(oldLastIdx).selected=true;


	}

	this.disable_last_navigation = function()
	{

	

		this.nextNavObj_m.disabled =true;
		this.nextNavObj_m.bgColor ='orange';
	}

	this.enable_last_navigation	= function()
	{

		

		this.nextNavObj_m.disabled =false;
		this.nextNavObj_m.bgColor='buttonface';
	}

	
	this.adjustSelectObjectToHistoryIndex= function()
	{

		

		this.pageNumObj_m.innerText = '';
		if (this.selectPageObj_m.last_page_index < this.history_pageIndex_m) {
			// extending that
			this.extendPageSelector();
		}

		// try to find
		var found = false;
		for(var i = 0; i < this.selectPageObj_m.options.length; i++) {
			if (this.selectPageObj_m.options(i).value >= 0 && this.selectPageObj_m.options(i).value==this.history_pageIndex_m) {
				this.selectPageObj_m.options(i).selected=true;
				found = true;
				break;
			}
		}

		if (!found)	{
			alert('Internal exception - cannot find the selected index: ' + this.history_pageIndex_m);
		}
	}

	
	// button click handlers
	this.next_page_history = function()
	{

		this.history_pageIndex_m = this.history_pageIndex_m*1 +this. pageSize_m;
		this.adjustSelectObjectToHistoryIndex();

		//
		this.do_navigate();
		

	}

	this.prev_page_history = function()
	{
		var history_pageIndex_new = this.history_pageIndex_m - this.pageSize_m;
		if (history_pageIndex_new <=0 ) {
			this.history_pageIndex_m = 0;
		}
		else {
			this.history_pageIndex_m = history_pageIndex_new;
		}

		
		this.adjustSelectObjectToHistoryIndex();

		//
		this.do_navigate();
	}

	this.first_page_history = function()
	{
	
		this.history_pageIndex_m = 0;
		this.adjustSelectObjectToHistoryIndex();

		//
		this.do_navigate();
		
	}

	this.navigate_through_page_selector = function()
	{

		

		this.pageNumObj_m.innerText = '';
		var pageIndex = this.selectPageObj_m.options(this.selectPageObj_m.selectedIndex).value;


		// extend
		if (pageIndex==-9999) {
			// add
			this.extendPageSelector();

			// try on more time
			// assuming we have the correctly selected index
			this.navigate_through_page_selector();

		}
		else {
			// normal navigation
			this.history_pageIndex_m = pageIndex;

			this.do_navigate();
		}
	}

	this.do_navigate = function()
	{
		this.historyBaseUrl_m = this.usrFun_generUrl_mf(this.history_pageIndex_m, this.pageSize_m);
		if (this.usrFun_SwitchOnLoadInProgress_mf != null)
			this.usrFun_SwitchOnLoadInProgress_mf();
		// pass additional parameters
		this.usrFun_Navigate_mf(this.historyBaseUrl_m, this.history_pageIndex_m, this.pageSize_m);

	}


	/////////////////////////////////////////
	// call init
	this.init(barDivObjName);
	
}

