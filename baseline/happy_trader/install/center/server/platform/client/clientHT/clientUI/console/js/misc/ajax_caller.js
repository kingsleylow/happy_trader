// this make the ajax call returning data in uniform format
function AjaxCaller(toutsecs)
{

	// main functions

		
	this.isResponseOk = function()
	{
		return (this.xml_http_status_m==200 && this.custom_code_m==0)
	}

	this.createErrorString  = function()
	{

		var sMsg = "";

		if (this.xml_http_status_m==200) {
			sMsg = "Operation resulted with code: " + this.custom_code_m + " returned: '" + this.custom_string_m + "'" ;
		}
		else {
			sMsg = "Invalid HTTP request: " + this.xml_http_status_m + " string description: '" +self.xml_http_status_string_m+ "'";
		}

		return  sMsg;
	}

		

	this.makeCall = function(method, url, postData, on_response_finished, on_start, on_finish)
	{
		if (Base64 == undefined) {
		  alert("Please include base64.js before");
		  return;
		}
		//
		var self = this;

		self.response_callback_m = null;
		self.start_callback_m = null;
		self.finish_callback_m = null;

		self.custom_code_m = -1;
		self.custom_string_m = "";
		self.xml_http_status_m = -1;
		self.xml_http_status_string_m = "";

		if (on_response_finished != undefined) {
			self.response_callback_m = on_response_finished;
		}

		if (on_start != undefined)
		{
			self.start_callback_m = on_start;
		}

		if (on_finish != undefined)
		{
			self.finish_callback_m = on_finish;
		}
		
		if (self.xmlhttp_m==null) {
			self.xmlhttp_m = self._get_xml_http();
			
		}

		self.timeout_m = -1;

		self.xmlhttp_m.open(method, url, true);
		self.xmlhttp_m.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		if (postData != null) {
			self.xmlhttp_m.setRequestHeader("Content-length", postData.length);
		}
		self.xmlhttp_m.setRequestHeader("Accept",	"image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, */*");

		
		self.xmlhttp_m.onreadystatechange = function()
		{
			if (self.xmlhttp_m.readyState != 4)
				return;

			if (self.timeout_m > 0)
				clearTimeout(self.timeout_m);

			self.xml_http_status_m = self.xmlhttp_m.status;
			self.xml_http_status_string_m = self.xmlhttp_m.statusText;
			if (self.xmlhttp_m.status == 200) {
				// 
				self._parse_200_response(self);

			} else {
				self._parse_non200_response(self);
			}
		}

		if (self.start_callback_m != null)
			self.start_callback_m();
		self.xmlhttp_m.send(postData);

		if(self.toutsecs_m > 0) {
			self.timeout_m = setTimeout( function() {
				self.xmlhttp_m.abort();
				self._handle_error_timeout(self);
			}
			, self.toutsecs_m*1000);
		}
		

	}
	
	///////////////////////////
	// helpers

	
	this._get_xml_dom = function()
	{
		var doc = null;
		var docStrings = [
		"Msxml2.DOMDocument.6.0",
		"Msxml2.DOMDocument.5.0",
		"Msxml2.DOMDocument.4.0",
		"MSXML2.DOMDocument.3.0",
		"MSXML2.DOMDocument",
		"MSXML.DOMDocument",
		"Microsoft.XMLDOM"
		];
		for (var d = 0; d < docStrings.length; d++) {
			if (doc === null) {
				try {
					doc = new ActiveXObject(docStrings[d]);
				}
				catch(e) {
					doc = null;
				}
			}
			else {
				break;
			}
		}
		return doc;
	};

	/*
	this._get_xml_http = function()
	{
		var xmlhttp = null;
		try {
			xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
				xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (E) {
				xmlhttp = null;
			}
		}
		if ((xmlhttp!=null) && (typeof XMLHttpRequest!="undefined")) {
			try {
				xmlhttp = new XMLHttpRequest();
			}
			catch(E3)
			{
				xmlhttp = null;
			}
		}
		
		return xmlhttp;
	}
	*/

	this._get_xml_http = function()
	{
		var xmlhttp = null;
		try {
			xmlhttp = new window.XMLHttpRequest;

		} catch (E) {
			alert('Your IE dows not suppport window.XMLHttpRequest');

		}

		return xmlhttp;
	}

	/////
	// user data functions

	this._handle_error_timeout = function(pself)
	{
		try {
			pself._on_response_finished(pself, true);
		}
		catch(e)
		{
			alert("ajax caller: timeout hander error: " + e.description);
		}

	}

	this._parse_200_response = function(pself)
	{
		
		try {

			// this is pure response text
			var pure_response = pself.xmlhttp_m.responseText.replace(/^\s\s*/, '').replace(/\s\s*$/, '');

			var parser = null;
			try {
				parser = this._get_xml_dom();
			}
			catch(e0)
			{
				alert("cannot create 'Microsoft.XMLDOM' object: " + e0.description );
				return;
			}

			parser.async = false;
			parser.loadXML(pure_response);

			//alert(pure_response);
			

			
			// parse here the content
			/*
			var statuscodes = pself.xmlhttp_m.responseXML.selectNodes("/ajax_call/status_code");
			var statusstrings = pself.xmlhttp_m.responseXML.selectNodes("/ajax_call/status_string");
			var datastringss = pself.xmlhttp_m.responseXML.selectNodes("/ajax_call/data_string");
			*/

			
			var statuscodes = parser.selectNodes("/ajax_call/status_code");
			var statusstrings = parser.selectNodes("/ajax_call/status_string");
			var datastringss = parser.selectNodes("/ajax_call/data_string");

			if (statuscodes.length!=1 || statusstrings.length !=1) {
				alert("ajax caller: invalid XML structure to be parsed");
				// finish response
				pself._on_response_finished(pself, false, pself.xmlhttp_m.status, pself.xmlhttp_m.statusText, null, null, null);
				return;
			}

			pself.custom_code_m = parseInt(statuscodes[0].text);
			pself.custom_string_m = Base64.decode(statusstrings[0].text);
			
			var content = datastringss[0].text;
			if (content != undefined && content != null) 
			  content = Base64.decode(content);
			else
			  content = "";
			
			

			//alert('Going to call handler');
			pself._on_response_finished(pself, false, pself.xmlhttp_m.status, pself.xmlhttp_m.statusText, pself.custom_code_m, pself.custom_string_m, content);
			

		}
		catch(e)
		{
			alert("ajax caller: response 200, handler error: " + e.description );
			
		}
	}

	this._parse_non200_response = function(pself)
	{
		try {
			pself._on_response_finished(pself, false, pself.xmlhttp_m.status, pself.xmlhttp_m.statusText, null, null, null);
		}
		catch(e)
		{
			alert("ajax caller: bad http handler error: " + e.description );
		}

	}

	// this is called at the end
	this._on_response_finished = function(pself, is_timeout, http_status, http_status_text, custom_code, custom_string, content)
	{
		
		if (pself.finish_callback_m != null)
			pself.finish_callback_m();

		//alert(is_timeout+'\n'+http_status+ '\n'+ http_status_text+'\n' +custom_code+'\n' + custom_string+'\n' + content);
		if (pself.response_callback_m != null)
		{
			pself.response_callback_m(is_timeout, http_status, http_status_text, custom_code, custom_string, content);
		}
	}

	
	///////////////////////////
	// members

		
	this.xmlhttp_m = null;
	this.toutsecs_m = toutsecs;
	this.timeout_m = -1;

	// callbacks
	this.response_callback_m = null;
	this.start_callback_m = null;
	this.finish_callback_m = null;


	this.custom_code_m = -1;
	this.custom_string_m = "";
	this.xml_http_status_m = -1;
	this.xml_http_status_string_m = "";
	
}