

var AjaxRequestHelper = {
    blockUI: function()
    {
        $('html').block( {
            message : '<table><tr><td><img src="lib/images/busy.gif"/></td></tr></table>', centerX : true, centerY : true, fadeIn : 0, fadeOut : 0, timeout : 0, css :  {
                top : '5%', height : 'auto', cursor : 'null', border : 'none', textalign : 'center', backgroundColor : 'auto'
            },
            overlayCSS :  {
                backgroundColor : '#666666', opacity : 0.5
            }
        });
    },
    unblockUI: function()
    {
        $('html').unblock();
    },
    
    // this is no block/unblock UI, so all is in bckground 
    postRequestHelperNoUI : function (url_s, input_data, onOkFun, onErrorFun, onCustomErrorFun) {
        $.ajax( {
            url : url_s, 
            type : "POST", 
            data : input_data, 
            dataType : 'xml', 
            contentType : 'application/x-www-form-urlencoded; charset=UTF-8',
            headers: {
                'Connection':'close',
                'Cache-Control': 'Public'
            },
            success: function(xml) {        
                 // on complete 
              
                AjaxRequestHelper.parseAjaxResponse(xml, onOkFun, onErrorFun, onCustomErrorFun);
            },
            error:function (jqXHr, textStatus, errorThrown) {
                // on error
                FunWrapper.safeFunCall_1(onErrorFun, "Request error status: [" + textStatus + "], message: [" + errorThrown + "]");
            }
        }).always(function () {
           
        });
    },

    postRequestHelper : function (url_s, input_data, onOkFun, onErrorFun, onCustomErrorFun) {
  
       $('html').block( {
            message : '<table><tr><td><img src="lib/images/busy.gif"/></td></tr></table>', centerX : true, centerY : true, fadeIn : 0, fadeOut : 0, timeout : 0, css :  {
                top : '5%', height : 'auto', cursor : 'null', border : 'none', textalign : 'center', backgroundColor : 'auto'
            },
            overlayCSS :  {
                backgroundColor : '#666666', opacity : 0.5
            }
        });

    $.ajax( {
            url : url_s, 
            type : "POST", 
            data : input_data, 
            dataType : 'xml', 
            contentType : 'application/x-www-form-urlencoded; charset=UTF-8',
            headers: null,
            /*headers: {
                
                'Connection':'close',
                'Cache-Control': 'Public' 
            } , */
            success: function(xml) {        
                 // on complete 
                //alert('success');
                $('html').unblock();
                AjaxRequestHelper.parseAjaxResponse(xml, onOkFun, onErrorFun, onCustomErrorFun);
            },
            error:function (jqXHr, textStatus, errorThrown) {
                // on error
                FunWrapper.safeFunCall_1(onErrorFun, "Request error status: [" + textStatus + "], message: [" + errorThrown + "]");
            }
        }).always(function () {
            $('html').unblock();
        });
    },
    
    parseAjaxResponse : function (xml, onOkFun, onErrorFun, onCustomErrorFun) {
        // alert( data );
        if (!isVariableValid(xml)) {
            //alert("Valid response not returned");
            FunWrapper.safeFunCall_1(onErrorFun, "Valid response not returned");
            return;
        }

        //var trimmed_data = data.replace(/^\s+|\s+$/g, "");
        //if (trimmed_data.length <=0)
        //    return;
        var resp_object = null;
        try {
            
            var code =  null;
            var reason = null;
            var custom_data = null;
            
            $(xml).find('ajax_call').each(function(){
                $(xml).find('status_code').each(function(){
                    code = parseInt($(this).text());
                    if (isNaN(code))
                        code = -1;
                });
    
                $(xml).find('status_string').each(function(){
                    reason = $(this).text();
                });
    
                $(xml).find('data_string').each(function(){
                    custom_data = $(this).text();
                });

            });

           

            if (!isVariableValid(code)) {
                FunWrapper.safeFunCall_1(onErrorFun, "Valid response code not returned");
                return;
            }

            if (!isVariableValid(reason)) {
                FunWrapper.safeFunCall_1(onErrorFun, "Valid response reason not returned");
                return;
            }

            if (!isVariableValid(custom_data)) {
                FunWrapper.safeFunCall_1(onErrorFun, "Valid response data not returned");
                return;
            }
            
            //alert('step1');

            // must base 64 decode here
            var custom_data_decoded = Base64.decode(custom_data);
            var reason_decoded =  Base64.decode(reason);

            //alert('returning: ' + custom_data_decoded);
            
         
            try {
                // have everuthing
                if (code === 0) {
                    FunWrapper.safeFunCall_3(onOkFun, code, reason_decoded, custom_data_decoded);
                }
                else {
                    FunWrapper.safeFunCall_3(onCustomErrorFun, code, reason_decoded, custom_data_decoded);
                }
            }
            catch (e2) {
                FunWrapper.safeFunCall_1(onErrorFun, "Untrapped error in user request handlers: [" + e2.message + "]");
            }

        }
        catch (e) {
            FunWrapper.safeFunCall_1(onErrorFun, "Exception happened on parsing server request: [" + e.message + "]");
        }
    }

}


// need some stuff to wrap error responses
//

// just a helper to show dialog
// html can be used as msg_p
CommonDialogHelper = {
    showAlert: function(msg_p, width_p, height_p)
    {
        CommonDialogHelper.show('Alert', msg_p, width_p, height_p);
    },
    show: function(title_p, msg_p, width_p, height_p)
    {
        var dlg_id = uniqid();
        var dlg_html = '<div id="'+dlg_id+'_CommonDialogHelper_DlgObj'+'" title="'+title_p+'">'+
                                '<div id="'+dlg_id+'_int_div'+'">'+msg_p+'</div>' + '</div>';

        
         var s_width = width_p;
         var s_height = height_p;

         if (!isVariableValid(width_p))
               s_width = 600;

            if (!isVariableValid(height_p))
               s_height = 400;

        var dlg = $(dlg_html).dialog( 
                                {
                                    resizable: false,
                                    autoOpen : false, 
                                    height : s_height, 
                                    width : s_width, 
                                    modal : true, 
                                    buttons :  
                                    {
                                       Close : function () {
                                          $(this).dialog("close");
                                       }
                                    },
                                    open: function(event,ui) {
                                       
                                    },
                                    close : function(ev, ui) { 
                                        $(this).remove(); 
                                    }
        });

        dlg.dialog( "open" );     
    }

};

// upper level function
// wraps any error messages
// intended to call parser handler directly
AjaxRequestUpperLevel = {
    postRequestHelperUpper: function(url_s, input_data, onCustomOkFun, width, height)
    {
            var s_width = width;
            var s_height = height;

            if (!isVariableValid(width))
               s_width = 600;

            if (!isVariableValid(height))
               s_height = 400;


            AjaxRequestHelper.postRequestHelper(
              url_s,
              input_data,
              // on ok
              function (code, reason, custom_data) {
                 FunWrapper.safeFunCall_3(onCustomOkFun, code, reason, custom_data );
              },
              // on error
              function (reason) {
                  // error
                  CommonDialogHelper.show("Error", reason, s_width, s_height);
              },

             // on custom error
              function (code, reason, custom_data) {
                  // custom error
                  CommonDialogHelper.show("Error on processing", "Error occured, code: '" + code+ "' Reason: '" + reason + "'", s_width, s_height);
              });
    }

};

function DownloadThroughForm()
{
    this._init = function()
    {   
        this.formId_m = uniqid() + '_download_form_obj';

        var form = $('<form></form>').
            attr("id",this.formId_m ).
            attr("name", this.formId_m).
            css('display', 'none').
            attr('method', 'post'); 
      
            
        $("<input type='hidden' name='tmp_uid' value='' ></input>").appendTo(form);
        $("<input type='hidden' name='file_name' value='' ></input>").appendTo(form);
        $("<input type='hidden' name='remove_file' value='true' ></input>").appendTo(form);
        $("<input type='hidden' name='content' value='' ></input>").appendTo(form);

        form.appendTo('body');
                       
        return true;
    };
        
    this.downloadFile = function(action, uid, file_name, to_remove)    
    {
        if (!this.initialized_m)
        {
            this._init();
        }

       
        var form = $('#'+this.formId_m);
                       
        form.attr('action', action);
        form.children('input[name="tmp_uid"]').val(uid);
        form.children('input[name="file_name"]').val(file_name);
        form.children('input[name="remove_file"]').val(to_remove);
                
       
                
        form.submit();
    };

    this.pushContent = function(action, file_name, content)    
    {
        if (!this.initialized_m)
        {
            this._init();
        }

       
        var form = $('#'+this.formId_m);
                       
        form.attr('action', action);
        
        form.children('input[name="file_name"]').val(file_name);
        form.children('input[name="content"]').val(content);
                
       
                
        form.submit();
    };
        
        
    //
    this.formId_m = null;
    this.initialized_m = false;
}


