function replaceSubstring(inSource, inToReplace, inReplaceWith) {

  var outString = inSource;
  while (true) {
    var idx = outString.indexOf(inToReplace);
    if (idx === -1) {
      break;
    }
    outString = outString.substring(0, idx) + inReplaceWith +
      outString.substring(idx + inToReplace.length);
  }
  return outString;

}


function prepareJsonResponse(txt)
{
    if (!isVariableValued(txt))
        return "";
    
    var new_txt= txt.replace(/[\n\r\t\0\x0B]/g,'');
    // \u -> u
    new_txt= new_txt.replace(/\\u/g,'u');
    
   
    
    return new_txt;  
}

// function to dump object
function prepareHTMLtext(text) {
    var result = "<p>" + text + "</p>";
    result = result.replace(/\r\n\r\n/g, "</p><p>").replace(/\n\n/g, "</p><p>");
    result = result.replace(/\r\n/g, "<br />").replace(/\n/g, "<br />");

    return result;
}

function getFilterData(baseElement)
{
   
    var result = null;

    // look for int-checkbox-div
    // and extract filter_type and val with checked
    $('#'+baseElement).find('div.int-checkbox-div').find("input[type='checkbox']").each(function( index ) {

        if (isVariableValued($(this).attr("checked")) ) {
            // checked
            
                
            var _int_id = $(this).attr('_int_id');
            var _filter_type = $(this).attr("_filter_type");

            if (result == null)
                result = {};

            if (!checkProperty(result, null, _filter_type)) {
                result[ _filter_type ] = {};
            }
         
            result[ _filter_type ][_int_id] = $(this).val();

                
        };
    });

    return result;
}

function createCheckBoxListFromArray(arraydata, null_value, attribute_value)
{
    var r = "";

    if (arraydata == null)
        return r;

    
    for(var i in arraydata) {
        var val = arraydata[i];
        
        if (!isVariableValued(val)) {
            
            if (isVariableValid(null_value)) {
                val = null_value;

            }
        }
        
       
        r += "<div class='int-checkbox-div'><input _filter_type='"+attribute_value+"' type='checkbox' value='" + arraydata[i] + "' _int_id='"+i+"'>" + val + "</input></div>";
       
    }

    
    return r;
    
}

function createSelectOptionsFromArray(arraydata)
{
    var r = "";

    if (arraydata == null)
        return r;

   
    for(var i = 0; i < result.length; i++) {
         r += "<option _int_id='"+i+"'>" + result[i] + "</option>";
    }

    return r;
}

function regexpGetAttribute(strval, tagval) {
    //var html_text = ui.item.html();
    //var reg = /\s(\w+?)="(.+?)"/g;
    //var select_id = -1;
    var rexp_e = new RegExp('\\s(\\w+?)="(.+?)"', 'g');
    while (true) {
        var res = rexp_e.exec(strval);
        if (res !== null) {
            if (tagval === res[1]) {
                return res[2];
            }
        }
        else 
            return null;
    }
}

function dump(arr, level) {
    var dumped_text = "";
    if (!level)
        level = 0;

    //The padding given at the beginning of the line.
    var level_padding = "";
    for (var j = 0;j < level + 1;j++)
        level_padding += "    ";

    if (typeof (arr) === 'object') {
        //Array/Hashes/Objects 
        for (var item in arr) {
            var value = arr[item];

            if (typeof (value) === 'object') {
                //If it is an array,
                dumped_text += level_padding + "'" + item + "' ...\n";
                dumped_text += dump(value, level + 1);
            }
            else {
                dumped_text += level_padding + "'" + item + "' => \"" + value + "\"\n";
            }
        }
    }
    else {
        //Stings/Chars/Numbers etc.
        dumped_text = "===>" + arr + "<===(" + typeof (arr) + ")";
    }
    return dumped_text;
}

var Base64 = {
    _keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=", 
  
    encode : function (input) {
        var output = "";
        var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
        var i = 0
        input = Base64._utf8_encode(input);
        while (i < input.length) {
            chr1 = input.charCodeAt(i++);
            chr2 = input.charCodeAt(i++);
            chr3 = input.charCodeAt(i++);
            enc1 = chr1 >> 2;
            enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
            enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
            enc4 = chr3 & 63;
            if (isNaN(chr2)) {
                enc3 = enc4 = 64;
            }
            else if (isNaN(chr3)) {
                enc4 = 64;
            }
            output = output + this._keyStr.charAt(enc1) + this._keyStr.charAt(enc2) + this._keyStr.charAt(enc3) + this._keyStr.charAt(enc4);
        }
        return output;
    },

  
    decode : function (input) {
        var output = "";
        var chr1, chr2, chr3;
        var enc1, enc2, enc3, enc4;
        var i = 0;
        input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
        while (i < input.length) {
            enc1 = this._keyStr.indexOf(input.charAt(i++));
            enc2 = this._keyStr.indexOf(input.charAt(i++));
            enc3 = this._keyStr.indexOf(input.charAt(i++));
            enc4 = this._keyStr.indexOf(input.charAt(i++));
            chr1 = (enc1 << 2) | (enc2 >> 4);
            chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
            chr3 = ((enc3 & 3) << 6) | enc4;
            output = output + String.fromCharCode(chr1);
            if (enc3 != 64) {
                output = output + String.fromCharCode(chr2);
            }
            if (enc4 != 64) {
                output = output + String.fromCharCode(chr3);
            }
        }
        output = Base64._utf8_decode(output);
        return output;
    },
    //   
    _utf8_encode : function (string) {
        string = string.replace(/\r\n/g, "\n");
        var utftext = "";
        for (var n = 0;n < string.length;n++) {
            var c = string.charCodeAt(n);
            if (c < 128) {
                utftext += String.fromCharCode(c);
            }
            else if ((c > 127) && (c < 2048)) {
                utftext += String.fromCharCode((c >> 6) | 192);
                utftext += String.fromCharCode((c & 63) | 128);
            }
            else {
                utftext += String.fromCharCode((c >> 12) | 224);
                utftext += String.fromCharCode(((c >> 6) & 63) | 128);
                utftext += String.fromCharCode((c & 63) | 128);
            }
        }
        return utftext;

    },

    //����� ��� ������������ �� urf8  
    _utf8_decode : function (utftext) {
        var string = "";
        var i = 0;
        var c = c1 = c2 = 0;
        while (i < utftext.length) {
            c = utftext.charCodeAt(i);
            if (c < 128) {
                string += String.fromCharCode(c);
                i++;
            }
            else if ((c > 191) && (c < 224)) {
                c2 = utftext.charCodeAt(i + 1);
                string += String.fromCharCode(((c & 31) << 6) | (c2 & 63));
                i += 2;
            }
            else {
                c2 = utftext.charCodeAt(i + 1);
                c3 = utftext.charCodeAt(i + 2);
                string += String.fromCharCode(((c & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
                i += 3;
            }
        }
        return string;
    }
}

function insertTextInto(source, index, string) {
  if (index > 0)
    return source.substring(0, index) + string + source.substring(index, this.length);
  else
    return string + source;
};

function isVariableValid(vrbl) {
    return (typeof (vrbl) !== "undefined" && vrbl !== null);
}

function isVariableValued(vrbl) {
    if ((typeof (vrbl) !== "undefined" && vrbl !== null)) {
        if (vrbl.length > 0)
            return true;
    }

    return false;
}

function isObjectEmpty(objdata)
{
    for(var key in objdata) {
        if (objdata.hasOwnProperty(key)) {
            return false;
        }
    }
           
    return true;
}

// check if object valid and has sioome properties
function isObjectValid(objdata)
{
    if (objdata !== null) {
        if (typeof (objdata) === "object") {
           return !isObjectEmpty(objdata);
         }
    }
    
    return false;
}

function parseIntHelper(val)
{
                if (!isVariableValid(val))
                    return  -1;
                
                var val_i = parseInt(val);
                if (isNaN(val_i))
                    return -1;
                
                return val_i;
            
}
            

function parseIntAsValidUnixTime(value)
{
    var val_i = parseInt(value);
    if (isNaN(val_i))
        return -1;
    
    if (val_i < 0)
        return -1;
    
    return val_i;
}

function date_and_time(unix_msec) {
    var date = new Date(unix_msec);
    //zero-pad a single zero if needed
    var zp = function(val) {
        return (val <= 9 ? '0' + val : '' + val);
    }

    //zero-pad up to two zeroes if needed
    var zp2 = function(val) {
        return val <= 99 ? '0' + val : (val <= 9 ? '00' + val : '' + val);
    }

    var d = date.getDate();
    var m = date.getMonth() + 1;
    var y = date.getFullYear();
    var h = date.getHours();
    var min = date.getMinutes();
    var s = date.getSeconds();
    var ms = date.getMilliseconds();
    return '' + y + '-' + zp(m) + '-' + zp(d) + ' ' + zp(h) + ':' + zp(min) + ':' + zp(s) + '.' + zp2(ms);
}

function stringToBoolean(string){
        var string_s = new String(string); 
	switch(string_s.toLowerCase()){
		case "true": case "yes": case "1": return true;
		case "false": case "no": case "0": case null: return false;
		default: return Boolean(string_s);
	}
}

function resolveBooleanValue(val)
{
        if (!isVariableValid(val)) {
            return false;
        }
        
        var val_i = parseInt(val);
        if (!isNaN(val_i)) {
            
            if (val_i === 0)
                return false;
            else
                return true;
        };
        
        // try string 
        return stringToBoolean(val);
}

function uniqid() {
    var newDate = new Date;
    var partOne = newDate.getTime();
    var partTwo = 1 + Math.floor((Math.random() * 32767));
    var partThree = 1 + Math.floor((Math.random() * 32767));
    var id = partOne + '-' + partTwo + '-' + partThree;
    return id;
}

function genrandom(min/* max */, max) {
    return Math.floor(arguments.length > 1 ? (max - min + 1) * Math.random() + min : (min + 1) * Math.random());
}

var FunWrapper = {
    isValidFun : function (fun) {
        return (fun != null && typeof (fun) == 'function');
    },
    safeFunCall_0 : function (fun) {
        if (this.isValidFun(fun)) {
            return fun();
        }
        return null;
    },
    safeFunCall_1 : function (fun, arg1) {
        if (this.isValidFun(fun)) {
            return fun(arg1);
        }
        return null;
    },
    safeFunCall_2 : function (fun, arg1, arg2) {
        if (this.isValidFun(fun)) {
            return fun(arg1, arg2);
        }
        return null;
    },
    safeFunCall_3 : function (fun, arg1, arg2, arg3) {
        if (this.isValidFun(fun)) {
            return fun(arg1, arg2, arg3);
        }
        return null;
    },
    safeFunCall_4 : function (fun, arg1, arg2, arg3, arg4) {
        if (this.isValidFun(fun)) {
            return fun(arg1, arg2, arg3, arg4);
        }
        return null;
    }
}

function getSimpleProperty(obj, property) {
    if ((typeof (obj) != "undefined" && obj != null)) {
        if ((typeof (property) != "undefined" && property != null)) {

            if (typeof (property) == 'string' || property instanceof String) {
                var v = obj[property];
                if (typeof (v) !== 'undefined' && v != null) {
                    return v;
                }

            }
        }
    }

    return null;
}

function checkProperty(obj, callback, property) {
    if ((typeof (obj) != "undefined" && obj != null)) {

        if ((typeof (property) != "undefined" && property != null)) {

            if (typeof (property) == 'string' || property instanceof String) {
                var v = obj[property];
                if (typeof (v) !== 'undefined' && v != null) {
                    return true;
                }
                else {
                    FunWrapper.safeFunCall_1(callback, 'object property: "' + property + '" is undefined');
                    return false;
                }

            }
            else {
                // assume array
                var isOk = true;
                for (i = 0;i < property.length;i++) {

                    var v2 = obj[property[i]];
                    if (typeof (v2) === 'undefined' || v2 == null) {
                        FunWrapper.safeFunCall_1(callback, 'object property: "' + property[i] + '" is undefined');
                        isOk = false;
                    }
                }

                return isOk;

            }

        }
        else {
            // no property  - just checked object
            return true;
        }

    }
    else {
        FunWrapper.safeFunCall_1(callback, 'object is undefined');

        return false;
    }
}




function createValidHtmlFromXml(xmldata)
{
    if (isVariableValid(xmldata)) {
        var r = xmldata.replace(new RegExp(['<'],"g"), "&lt;");
        r = r.replace(new RegExp(['>'],"g"), "&gt;");
        r = r.replace(/\r\n/g, "<br/>");
        r = r.replace(/\n/g, "<br/>");

        return r;
    }

    return "";
}

function createValidXmlFromHtml(htmldata)
{
        if (!isVariableValued(htmldata))
            return "";

        r = htmldata.replace(new RegExp(["&lt;"],"g"), "<");
        r = r.replace(new RegExp(["&gt;"],"g"), ">");
        r = r.replace(/<br\/>/g, "\r\n");
       

        return r;
}

function clearDropDownList(buListObj)
{
    while(buListObj.get('length') > 0) {
        buListObj.remove(0);
    }
}

function clearSelectList(selectListId)
{
    if (isVariableValid(selectListId)) {
         $('#'+selectListId +' option').each(function(index, option) {
                $(option).remove();
         });
    }
}

function parseDate_YYYYMMDD(str)
{
    // parse YYYYMMDD
    if (!isVariableValued(str))
        return null;
    
    if (str.length !== 8)
        return null;

    var y = str.substring(0,4);
    var m = str.substring(4,6);
    var d = str.substring(6,8);

    return [y,m,d];
}
