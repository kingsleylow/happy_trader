
var XgridHelperObject = {
    CHECKBOX_UNCHEKED: "<input type=checkbox>",
    CHECKBOX_CHEKED: "<input type=checkbox CHECKED>",


		
		get_checkbox_status2: function (checkbox_str_val) {
			// this on the input has string like <input type=checkbox>
			// and as teh output returns true or false

			 var s = checkbox_str_val.toUpperCase();

			 var idx = s.indexOf('CHECKED');
			 return (idx >=0);

    }
}