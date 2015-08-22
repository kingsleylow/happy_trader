function ModalDialog()
{
	
	this.hide = function()
	{
			this._hideModal();
	}
	
	this.show  =function(	modalWidth,	modalHeight,	content)
	{
		if (this.mainDivId_m != null)	{
			this._revealModal( modalWidth,	modalHeight,	content);
		}
	}
	
	
	
	// -----------------
	// PRIVATE
	this._init = function()
	{
		/*
		<div id="modalPage">
    <div class="modalBackground">
    </div>
    <div class="modalContainer">
        <div class="modal">
            <div class="modalTop"><a href="javascript:hideModal('modalPage')">[X]</a></div>
            <div class="modalBody" ></div>
        </div>
    </div>
</div>
*/
		if (this.mainDivId_m != null)
			return;
		
		// create div
		var main_id = this._guidGenerator();
		var newdiv = document.createElement('div');
	
		newdiv.id= 'MODAL_DIV_' +main_id;
		newdiv.style.display = "none";
		
		var modal_top_href_id = "CLOSE_HREF_" + main_id;
		newdiv.innerHTML = 
		'<div class="modalBackground">' +
    '</div>'+
    '<div class="modalContainer">' +
        '<div class="modal">'+
            '<div class="modalTop"><a href="#" id="'+modal_top_href_id+'" onclick="">[X]</a></div>'+
            '<div class="modalBody" ></div>'+
        '</div>'+
    '</div>';
		
		document.body.appendChild(newdiv);
		this.mainDivId_m = newdiv;
		
		var href_obj = document.getElementById(modal_top_href_id);
		var me = this;
		href_obj.onclick = function()
		{
			me._hideModal();
		};
		
		
		
		
	
	}
	
	this._revealModal = function(modalwidth, modalhight, content)
	{
		window.onscroll = function () {
			this.mainDivId_m.style.top = document.body.scrollTop;
		};
		
		this.mainDivId_m.style.display = "block";
		this.mainDivId_m.style.top = document.body.scrollTop;
		
		
		var allel = this.mainDivId_m.getElementsByTagName('*');
	
		for(i = 0; i < allel.length; i++) {
			var elem_i = allel.item(i);
			if(elem_i.className.toUpperCase() == "modalBody".toUpperCase()) {
				elem_i.style.width = modalwidth;
				elem_i.style.height = modalhight;
				
				elem_i.innerHTML = content; 
				
				
				break;
			}
		}
		

	}
	
	this._hideModal = function()
	{
		if (this.mainDivId_m != null)
			this.mainDivId_m.style.display = "none";
	}
	
	this._guidGenerator = function() {
		var S4 = function() {       
			return (((1+Math.random())*0x10000)|0).toString(16).substring(1);     
		};     
		return (S4()+S4()+"_"+S4()+"_"+S4()+"_"+S4()+"_"+S4()+S4()+S4()); 
	} 


	
	// ------------------------
	// memebers
	
	
	this.mainDivId_m = null;
	
	// call init
	this._init();
	
}

ModalDialog.createDivContentFromList = function(data_array, onclick_fun, tag, td_style)
{
	var data = "";
	if (data_array != null) {
		data += "<div>";
		data += '<table cellpadding="0px" cellspacing="0px" class="pointer">';
		for(i = 0; i < data_array.length; i++) {
				
			var click_entry = onclick_fun(data_array[i], tag);
			if (td_style != null) {
				data += ("<tr class='x8' height=10px "+click_entry+" onMouseover='this.bgColor=\"#EEEEEE\"' onMouseout='this.bgColor=\"#FFFFFF\"'>" +
					"<td nowrap " +td_style+ " align=left>"+data_array[i] + '</td></tr>'); 
			
			}
			else {
				data += ("<tr class='x8' height=10px "+click_entry+" onMouseover='this.bgColor=\"#EEEEEE\"' onMouseout='this.bgColor=\"#FFFFFF\"'>" +
					"<td nowrap style='font-size:larger' align=left>"+data_array[i] + '</td></tr>'); 
			
			}
			
		}
								
		data += '</table>';
		data += "</div>";
	}
	
	return data;
	
}