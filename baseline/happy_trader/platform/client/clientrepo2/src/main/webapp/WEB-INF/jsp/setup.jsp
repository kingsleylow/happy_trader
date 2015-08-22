<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core"%>
<%@ taglib prefix="form" uri="http://www.springframework.org/tags/form"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions"%>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ taglib prefix="fmt" uri="http://java.sun.com/jsp/jstl/fmt" %>
<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ taglib uri="/WEB-INF/customTag.tld" prefix="ct" %>

<html>
    <head>
        <title>Setup</title>

        <!-- JQUERY -->
        <ct:jqincl base="setup" up="true"></ct:jqincl>
        
        <!-- HT BRIDGE -->
        <script type="text/javascript" src="<ct:basepath base="setup" />../resources/lib/htmtbridge/htmtbridge_common.js"></script>
        <script type="text/javascript" src="<ct:basepath base="setup" />../resources/lib/htmtbridge/htmtbridge_engine.js"></script>
        
        <script type="text/javascript" src="<ct:basepath base="setup" />../resources/lib/htmtbridge/libs.js"></script>
        <script type="text/javascript" src="<ct:basepath base="setup" />../resources/lib/htmtbridge/ajax_helper.js"></script>
       
        <!-- OTHER-->
        <link rel="stylesheet" type="text/css" href="<ct:basepath base="setup" />../resources/etc/css/default.css"></link>

        <style type="text/css">
        </style>

        <script type="text/javascript">
            // globals
            var tabCounter_m = 2;
            var HT_PROXY_GATE_URL = '<ct:basepath base="accdt" />../htproxygate/happyTraderGate';
            var setup_data_m = ${setupEntityObj.getOtherData()};
            var CHANGE_CONSTANT="9F94C4610E4B4F13BAC477888B2D3744";
           
           
            $(function () {

                init();
            });
            
            function getSetupDataForCompany(companyName)
            {
                if (!isVariableValid(setup_data_m))
                    return null;
                
                if (!isVariableValid(setup_data_m.DATA))
                    return null;
               
                for(i = 0; i < setup_data_m.DATA.length; i++) {
                    var company_name_i = setup_data_m.DATA[i].instance_name;
                    //alert(company_name_i);
                    
                    if (isVariableValued(company_name_i)) {
                        if (company_name_i === companyName) {
                            return setup_data_m.DATA[i];
                        }
                    }
                }
                return null;
            }
            
            function startHt()
            {
                // get history orders
                var debug = $("#debug_lib").is(':checked');
                var request = new HtStartInstanceReq(CHANGE_CONSTANT, CHANGE_CONSTANT, debug, true, CHANGE_CONSTANT)
                // alert(dump(request));
             
                AjaxRequestHelper.postRequestHelperNoUI(
                        HT_PROXY_GATE_URL,
                        request.getRequestData(),
                        // final function
                        function(code, reason, custom_data)
                        {
                            var custom_data_p = jQuery.parseJSON(custom_data);
                            //alert(custom_data);
                            //alert(dump(custom_data_p));
                            
                             CommonDialogHelper.show("Message", "Happy trader start performed OK");
                            
                        },
                         function(reason) {
                            
                             CommonDialogHelper.show("Internal error", reason);
                         },
                         function(code, reason, custom_data)
                         {
                            HtMtBrdidgeCommon.DefaultCustomErrorHandler(code, reason, custom_data);
                         }
                );
            }
            
            
            function shutdownHt()
            {
                // get history orders
                var request = new HtStopInstanceReq(CHANGE_CONSTANT, CHANGE_CONSTANT)
                // alert(dump(request));
             
                AjaxRequestHelper.postRequestHelperNoUI(
                        HT_PROXY_GATE_URL,
                        request.getRequestData(),
                        // final function
                        function(code, reason, custom_data)
                        {
                            var custom_data_p = jQuery.parseJSON(custom_data);
                            //alert(custom_data);
                            //alert(dump(custom_data_p));
                            CommonDialogHelper.show("Message", "Happy trader shutdown performed OK");
                            
                        },
                         function(reason) {
                            
                             CommonDialogHelper.show("Internal error", reason);
                         },
                         function(code, reason, custom_data)
                         {
                            HtMtBrdidgeCommon.DefaultCustomErrorHandler(code, reason, custom_data);
                         }
                );
            }
            
            function restartHt()
            {
                // get history orders
                var debug = $("#debug_lib").is(':checked');
                var request = new HtRestartInstanceReq(CHANGE_CONSTANT, CHANGE_CONSTANT, debug, true, CHANGE_CONSTANT)
                // alert(dump(request));
             
                AjaxRequestHelper.postRequestHelperNoUI(
                        HT_PROXY_GATE_URL,
                        request.getRequestData(),
                        // final function
                        function(code, reason, custom_data)
                        {
                            var custom_data_p = jQuery.parseJSON(custom_data);
                            //alert(custom_data);
                            //alert(dump(custom_data_p));
                             CommonDialogHelper.show("Message", "Happy trader restart performed OK");
                            
                        },
                         function(reason) {
                            
                             CommonDialogHelper.show("Internal error", reason);
                         },
                         function(code, reason, custom_data)
                         {
                            HtMtBrdidgeCommon.DefaultCustomErrorHandler(code, reason, custom_data);
                         }
                );
            }

            function init()
            {
                $( "#mt4loglevel" ).selectmenu({width:100}).val(${setupEntityObj.getMt4loglevel()}).selectmenu('refresh', true);
                $( "#internalloglevel").selectmenu({width:100}).val(${setupEntityObj.getHtserverinternalloglevel()}).selectmenu('refresh', true);
                $("#eventlevel").selectmenu({width:100}).val(${setupEntityObj.getHtserverinternaleventlevel()}).selectmenu('refresh', true);
                $( "#heartbeatjavahtsec" ).spinner();
               //
 
                
                <c:forEach items="${companiesObj}" var="company_i">
                    <c:if  test="${company_i.getAccounts().size() > 0}">
                        <c:set var="cid" value="${company_i.getId()}"/>
                        $('#mtInstallZip_${cid}').selectmenu({width: 'auto'});    
                        $( "#spinner_${cid}" ).spinner(  {   min: 1, max:${company_i.getAccounts().size()} } ).spinner( "value", 1 );
                       
                        var obj = getSetupDataForCompany('${company_i.getCompanyName()}');
                                   
                        if (isVariableValid(obj)) {
                            //alert(dump(obj));
                            $( "#spinner_${cid}" ).spinner( "value", obj.instance_parts );
                            $("#cb_enable_${cid}").attr("checked","checked");
                            
                            $('#mtInstallZip_${cid}').val(obj.zip_path);
                            $('#mtInstallZip_${cid}').selectmenu('refresh', true);
                                      
                                      
                                        
                        }
                                 
                                 
                    </c:if>
                </c:forEach>

                
                $("#clearSetupBtn").button().click(function (event) {
                    var href = "clear_setup/do.html";
                    
                    //alert(href);
                    window.location.href  = href;
                    
                });
                
                $("#generateSetupBtn").button().click(function (event) {
                    
                    var data = parseInstanceData();
                    //alert(  $( "#heartbeatjavahtsec" ).val() );
                    //$("#setupMainForm input[name=heartbeatjavahtsec_s]").val( $( "#heartbeatjavahtsec" ).val() );
                    //$("#setupMainForm input[name=eventlevel_s]").val( $( "#eventlevel option:selected" ).text() );
                    var finaldata = {"DATA": data};
                   
                    $("#setupMainForm input[name=otherData]").val( JSON.stringify(finaldata) );
                    
                    $( "#setupMainForm" ).submit();
                    
                });
                
                
                $("#restartHtBtn").button().click(function (event) {
                    restartHt();
                });

                $("#shutdownHtBtn").button().click(function (event) {
                    shutdownHt();
                });
                
                $("#startHtBtn").button().click(function (event) {
                    startHt();
                });
               
               function parseInstanceData()
               {
                   var result = [];
                   $( ".comp-name-div" ).each(function( idx ) {
                       
                       
                       var id =  $(this).attr("_instance_id");
                       var instance_name = $("#instance_name_"+id).text();
                       //alert(instance_name);
                       
                       if ($("#cb_enable_"+id).is(':checked') ) {
                       
                            var mt_runner = {};
                            
                            mt_runner.instance_name = instance_name;
                            mt_runner.id = id;
                            mt_runner.zip_path= $('#mtInstallZip_'+id + ' option:selected').val();
                            mt_runner.instance_parts  = $('#spinner_'+id).val();
                            
                            
                            result.push(mt_runner);
                       }
                   });
                   
                   //alert(dump(result));
                   return result;
               }
    
    
            }
            
        </script>
    </head>

    <body>
        <base href="<ct:basepath base="setup" />" />

        <div>This is setup page</div>
        
       
        <table>
            <tr>
                <td>Base local setup path</td>
                <td>${baseSetupFile}</td>
                <td></td>
            </tr>   
        </table>    
                
      
                
        <form:form method="POST" id="setupMainForm" name="setupMainForm" action="generate_setup/do.html" commandName="setupEntityObj" >
            
       
                <table>
                    <tr>
                        <td><label for="mt4loglevel">MT4 log level</label></td>
                        <td>
                            <form:select path="mt4loglevel" name="mt4loglevel" id="mt4loglevel">
                                <option>0</option>
                                <option>1</option>
                                <option>2</option>
                                <option>3</option>
                                <option>4</option>
                                <option>5</option>
                            </form:select>
                        </td>
                        <td></td>
                    </tr>   
                    
                    <tr>
                        <td><label for="zombiemt4killersec">Kill zombie MT instances period</label></td>
                        <td>
                            <form:input path="zombiemt4killersec" type="text" id="zombiemt4killersec" name="zombiemt4killersec" value="${setupEntityObj.getZombiemt4killersec()}" readonly="readonly" />
     
                        </td>
                        <td></td>
                    </tr>   
                            
                    <tr>
                        <td><label for="heartbeatjavahtsec">Internal heartbeat interval sec</label></td>
                        <td>
                            <form:input path="heartbeatjavahtsec" type="text" id="heartbeatjavahtsec" name="heartbeatjavahtsec" value="${setupEntityObj.getHeartbeatjavahtsec()}" readonly="readonly" />
           
                        </td>
                        <td></td>
                    </tr>   
                            
                    <tr>
                        <td><label for="mt4datareadintrvalsec">Read MT data period sec</label></td>
                        <td>
                             <form:input path="mt4datareadintrvalsec" type="text" id="mt4datareadintrvalsec" name="mt4datareadintrvalsec"  value="${setupEntityObj.getMt4datareadintrvalsec()}" />
        
                        </td>
                        <td></td>
                    </tr>   
                             
                     <tr>
                        <td><label for="debugconsole">HT server debug console</label></td>
                        <td>
                             <form:checkbox path="debugconsole" id="debugconsole" name="debugconsole" value='${setupEntityObj.isDebugconsole() ? "true":"false"}' />
          
                        </td>
                        <td></td>
                    </tr>   
                             
                    <tr>
                        <td><label for="port">HT server port</label></td>
                        <td>
                              <form:input path="htserverport" type="text" id="port" name="port"  value="${setupEntityObj.getHtserverport()}" />
        
                        </td>
                        <td></td>
                    </tr>   
                    
                              
                    <tr>
                        <td><label for="MAX_DELAY_POLL_QUEUE_ALIVE_SEC">Maximum delay after HT poll queue is destroyed</label></td>
                        <td>
                              <form:input path="MAX_DELAY_POLL_QUEUE_ALIVE_SEC" type="text" id="MAX_DELAY_POLL_QUEUE_ALIVE_SEC" name="MAX_DELAY_POLL_QUEUE_ALIVE_SEC"  value="${setupEntityObj.getMAX_DELAY_POLL_QUEUE_ALIVE_SEC()}" />
        
                        </td>
                        <td></td>
                    </tr>   
                              
                    <tr>
                        <td><label for="internalloglevel">HT server internal log level</label></td>
                        <td>
                            <form:select path="htserverinternalloglevel" name="internalloglevel" id="internalloglevel">
                                <option>0</option>
                                <option>1</option>
                                <option>2</option>
                                <option>3</option>
                                <option>4</option>
                                <option>5</option>
                            </form:select>
                        </td>
                        <td></td>
                    </tr> 
                              
                    <tr>
                        <td><label for="eventlevel">HT server internal event level</label></td>
                        <td>
                            <form:select path="htserverinternaleventlevel" name="eventlevel" id="eventlevel">
                                <option>0</option>
                                <option>1</option>
                                <option>2</option>
                                <option>3</option>
                                <option>4</option>
                                <option>5</option>
                                <option>6</option>
                            </form:select>
                        </td>
                        <td></td>
                    </tr> 
                    
                </table>  
               
          
                
        
            <c:if  test="${!empty companiesObj}">
                <table>
                    <tr>
                        <td>Company Name</td>
                        <td>Accounts Number</td>
                        <td>Include into setup</td>
                        <td>Divide all accounts by</td>
                        <td>Select MT zip</td>
                        <td></td>
                    </tr>    
                <c:forEach items="${companiesObj}" var="company_i">
                    <c:if  test="${company_i.getAccounts().size() > 0}">
                    <c:set var="idPrfx" value="${company_i.getId()}"/>  
                    
                        <tr>
                            <td>
                                <div _instance_id="${idPrfx}" class="comp-name-div"></div>
                                <div id="instance_name_${idPrfx}">${company_i.getCompanyName()}</div>
                            </td>
                            <td>
                                ${company_i.getAccounts().size()}
                            </td>
                            <td>
                                <input type="checkbox" id="cb_enable_${idPrfx}" name="cb_enable_${idPrfx}" />
                            </td>
                            <td>
                                <div>
                                    <input id="spinner_${idPrfx}" name="spinner_$${idPrfx}" value="" />
                                </div>
                            </td>
                            <td>
                                <select name="mtInstallZip_${idPrfx}" id="mtInstallZip_${idPrfx}">
                                    <c:forEach items="${mtDistrFilesObj}" var="file_i">
                                        <option value="${file_i.getName()}">${file_i.getName()}</option>
                                    </c:forEach>
                                </select>
                            </td>
                            <td></td>
                        </tr>  
                    </c:if>           
                </c:forEach>
                </table>
            </c:if>
       
            
            <form:input path="otherData" type="hidden" id="otherData" name="otherData" value="foo" />
        </form:form>
        
        <div>        
            <label for="debug_lib">Use debug libraries</label>        
            <input type="checkbox" id="debug_lib" name="debug_lib" disabled="yes"/>
        </div>

        <!-- -->
        <button id="clearSetupBtn">Clear setup</button>
        <button id="generateSetupBtn">Generate setup</button>
        <button id="restartHtBtn">Restart Happy Trader</button>
        <button id="shutdownHtBtn">Shutdown Happy Trader</button>
        <button id="startHtBtn">Start Happy Trader</button>
        
        <div>
            &nbsp;
        </div>    
        <a href="../administration/administration.html">Navigate to main page</a>
        
    </body>    
</html>   
