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
        <title>Account details</title>
       
        
         <!-- JQUERY -->
        <ct:jqincl base="accdt" up="true"></ct:jqincl>
        
        
        <!-- HT BRIDGE -->
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/htmtbridge/htmtbridge_common.js"></script>
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/htmtbridge/htmtbridge_engine.js"></script>
        
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/htmtbridge/libs.js"></script>
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/htmtbridge/ajax_helper.js"></script>
      
        <!-- DHTML XGRID -->
        <link rel="stylesheet" type="text/css" href="<ct:basepath base="accdt" />../resources/lib/codebase/dhtmlxgrid.css">
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/codebase/dhtmlxcommon.js"></script>
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/codebase/dhtmlxgrid.js"></script>
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/codebase/dhtmlxgridcell.js"></script>
        
        <!-- OTHER-->
        <link rel="stylesheet" type="text/css" href="<ct:basepath base="accdt" />../resources/etc/css/default.css"></link>
        
        
        <script type="text/javascript">
            
            var GridData = 
            {
                mtPosGrid_m : null,
                cur_page_m : 1,
                no_data_detected_m : true
            };
            
            var OpenGridData = 
            {
                mtPosGrid_m : null,
                cur_page_m : 1,
                no_data_detected_m : true
            };
            
            var AllHistGridData = 
            {
                mtPosGrid_m : null,
                cur_page_m : 1,
                no_data_detected_m : true
            };
            
            var HT_GATE_URL = '<ct:basepath base="accdt" />../htgate/happyTraderGate';
            var DIRECTION_PREV = 1;
            var DIRECTION_NEXT  =-1;
            var PAGE_SIZE=3;
           
           
            //var mtPosGrid_m = null;
            //var cur_page_m = 1;
            //var no_data_detected_m = true;
            
            var pos_grid_column_descriptor_m  =
                 [
                     {NAME: "POSITION_UID", FORMATTER: null, CAPTION: "ID", SIZE: 50}, 
                     {NAME: "TICKET", FORMATTER: null, CAPTION: "Ticket", SIZE: 70}, 
                     {NAME:"SYMBOL", FORMATTER: null, CAPTION: "Symbol", SIZE: 70}, 
                     {NAME:"OPEN_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue, CAPTION: "Open Time", SIZE: 130}, 
                     {NAME:"OPEN_RPICE", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue, CAPTION: "Open Price", SIZE: 90}, 
                     {NAME:"CLOSE_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue, CAPTION: "Close Time", SIZE:130}, 
                     {NAME:"CLOSE_PRICE", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue, CAPTION: "Close Price", SIZE:90}, 
                     {NAME:"SL", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue, CAPTION: "Stop Loss", SIZE:100}, 
                     {NAME:"TP", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue, CAPTION: "take Profit", SIZE:100}, 
                     {NAME:"PROFIT", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue, CAPTION: "Profit", SIZE:100}, 
                     {NAME:"COMMENT", FORMATTER: null, CAPTION: "Comment", SIZE:200}, 
                     {NAME:"EXPIRATION_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue, CAPTION: "Expiration", SIZE:130}, 
                     {NAME:"DIRECTION", FORMATTER: HtMtBrdidgeFormatters.formatDirection, CAPTION: "Direction", SIZE:70}, 
                     {NAME:"STATE", FORMATTER: HtMtBrdidgeFormatters.formatPosState, CAPTION: "State", SIZE:70}
                 ];
            
            function initPositions_HistRequest(griddata, custom_data_p, direction)
            {
                
                if (!isVariableValid(custom_data_p))
                    return;
               
                var data_obj = custom_data_p.DATA;
                if (!isVariableValid(data_obj))
                    return;
                 
                //var is_changes_included = data_obj.CHANGES_INCLUDED;
                //alert('Changes included: ' +is_changes_included);
                var position_list = data_obj.POSITION_LIST;
                
                // position list helper
                initPositionsHelper(griddata, position_list, direction);
            }
            
            function initPositionsHelper(griddata, position_list, direction)
            {
                if (!isObjectValid(position_list)) {
                    return;
                }
               
                if (position_list.length > 0) {
                    griddata.mtPosGrid_m.clearAll();
                
               
                    for (var i = 0; i < position_list.length; i++) {
                        var row = HtMtBrdidgeCommon.createXgridRowFromArrayFormatters(position_list[i], pos_grid_column_descriptor_m  );

                        // ticket will be row ID as unique in this context
                        griddata.mtPosGrid_m.addRow(position_list[i].TICKET, row);
                    }
                    
                    griddata.no_data_detected_m = false;
                }
                
                
            }
            
            function histNavigation(griddata, is_hist, direction)
            {
                griddata.no_data_detected_m = true;
                var pageNum = griddata.cur_page_m;
                
                // get history orders
                AjaxRequestHelper.postRequestHelperNoUI(
                        HT_GATE_URL,
                        {
                            account_id:"${accountObj.getAccount_id()}",
                            company_name:"${accountObj.getCompany().getCompanyName()}",
                            server_name:"${accountObj.getServer().getServer_name()}",
                            num_page:pageNum,
                            page_size: PAGE_SIZE,
                            is_history: is_hist
                        },
                        // final function
                        function(code, reason, custom_data)
                        {
                            var custom_data_p = jQuery.parseJSON(custom_data);
                            //alert(custom_data);
                            initPositions_HistRequest(griddata, custom_data_p, direction);
                            
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
               
                // init xgrid
                GridData.mtPosGrid_m = new dhtmlXGridObject('mtPositions_grid_obj');
                GridData.mtPosGrid_m.setImagePath("<ct:basepath base="accdt" />../resources/lib/codebase/imgs/");
                GridData.mtPosGrid_m.setHeader(HtMtBrdidgeCommon.getXgridHeader(GridData.mtPosGrid_m, pos_grid_column_descriptor_m));
             
                GridData.mtPosGrid_m.setInitWidths(HtMtBrdidgeCommon.getXgridWidth(GridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                
                GridData.mtPosGrid_m.setColAlign(HtMtBrdidgeCommon.getXgridAlign(GridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                GridData.mtPosGrid_m.setColTypes(HtMtBrdidgeCommon.getXgridTypes(GridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                GridData.mtPosGrid_m.setSkin("glassy_blue");
                
                GridData.mtPosGrid_m.init();
                
                // 
                OpenGridData.mtPosGrid_m = new dhtmlXGridObject('mtOpenPositions_grid_obj');
                OpenGridData.mtPosGrid_m.setImagePath("<ct:basepath base="accdt" />../resources/lib/codebase/imgs/");
                OpenGridData.mtPosGrid_m.setHeader(HtMtBrdidgeCommon.getXgridHeader(OpenGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
             
                OpenGridData.mtPosGrid_m.setInitWidths(HtMtBrdidgeCommon.getXgridWidth(OpenGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                
                OpenGridData.mtPosGrid_m.setColAlign(HtMtBrdidgeCommon.getXgridAlign(OpenGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                OpenGridData.mtPosGrid_m.setColTypes(HtMtBrdidgeCommon.getXgridTypes(OpenGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                OpenGridData.mtPosGrid_m.setSkin("glassy_blue");
                
                OpenGridData.mtPosGrid_m.init();
                
                //
                // all history
                
                AllHistGridData.mtPosGrid_m = new dhtmlXGridObject('mtPositions_grid_all_obj');
                AllHistGridData.mtPosGrid_m.setImagePath("<ct:basepath base="accdt" />../resources/lib/codebase/imgs/");
                AllHistGridData.mtPosGrid_m.setHeader(HtMtBrdidgeCommon.getXgridHeader(AllHistGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
             
                AllHistGridData.mtPosGrid_m.setInitWidths(HtMtBrdidgeCommon.getXgridWidth(AllHistGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                
                AllHistGridData.mtPosGrid_m.setColAlign(HtMtBrdidgeCommon.getXgridAlign(AllHistGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                AllHistGridData.mtPosGrid_m.setColTypes(HtMtBrdidgeCommon.getXgridTypes(AllHistGridData.mtPosGrid_m, pos_grid_column_descriptor_m));
                AllHistGridData.mtPosGrid_m.setSkin("glassy_blue");
                
                AllHistGridData.mtPosGrid_m.init();
                //
               
                
                $( "#nextBtn" ).button().click(function( event ) {
                    
                    if (!GridData.no_data_detected_m) {
                        GridData.cur_page_m++;
                    }
                    
                    histNavigation(GridData,  true, DIRECTION_NEXT);
                });
                
                $( "#prevBtn" ).button().click(function( event ) {
                    
                    if (GridData.no_data_detected_m) {
                        if (GridData.cur_page_m > 1)
                            GridData.cur_page_m--;
                    }
                    
                    GridData.cur_page_m = GridData.cur_page_m-1;
                    if (GridData.cur_page_m < 1)
                        GridData.cur_page_m = 1;
                    
                    histNavigation(GridData, true, DIRECTION_PREV);
                });
                
                //
                
                $( "#nextBtnOpen" ).button().click(function( event ) {
                    
                    if (!OpenGridData.no_data_detected_m) {
                        OpenGridData.cur_page_m++;
                    }
                    
                    histNavigation(OpenGridData,  false, DIRECTION_NEXT);
                });
                
                $( "#prevBtnOpen" ).button().click(function( event ) {
                    
                    if (OpenGridData.no_data_detected_m) {
                        if (OpenGridData.cur_page_m > 1)
                            OpenGridData.cur_page_m--;
                    }
                    
                    OpenGridData.cur_page_m = OpenGridData.cur_page_m-1;
                    if (OpenGridData.cur_page_m < 1)
                        OpenGridData.cur_page_m = 1;
                    
                    histNavigation(OpenGridData, false, DIRECTION_PREV);
                });
                
            } // end if init
            
            function readAllClosedHistoryData()
            {
                // this is just to get all positions to create chart
                
               
                // get history orders
                AjaxRequestHelper.postRequestHelperNoUI(
                        HT_GATE_URL,
                        {
                            account_id:"${accountObj.getAccount_id()}",
                            company_name:"${accountObj.getCompany().getCompanyName()}",
                            server_name:"${accountObj.getServer().getServer_name()}",
                            page_size: PAGE_SIZE,
                            num_page:-1,
                            is_history: true
                        },
                        // final function
                        function(code, reason, custom_data)
                        {
                            var custom_data_p = jQuery.parseJSON(custom_data);
                            //alert("All hist pos:" + dump(custom_data));
                            initPositions_HistRequest(AllHistGridData, custom_data_p, DIRECTION_NEXT);
                            
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
            
            // --------------------
            // 
            
            $(function() {
                     init();
                     histNavigation( GridData, true, DIRECTION_NEXT );
                     histNavigation( OpenGridData, false, DIRECTION_NEXT );
                     readAllClosedHistoryData();
            }); // end
            
            
            
            
        </script>
    </head>
    <body>
        
         <base href='<ct:basepath base="accdt" />' />
        
         
         <table>
             <tr>
                 <td>Account ID</td>
                 <td>User name</td>
                 <td>Company name</td>
                 <td>Trading server</td>
                 <td>Your balance</td>
                 
                 <td></td>
             </tr>
             <tr>
                 <td>${accountObj.getAccount_id()}</td>
                 <td>${userObj.getUserName()}</td>
                 <td>${accountObj.getCompany().getCompanyName()}</td>
                 <td>${accountObj.getServer().getServer_name()}</td>
                 <td>${lastBalance}</td>
                 <td></td>
             </tr>    
         </table>    
         
        <div>&nbsp;</div>
        <button id="prevBtn">&lt;&lt;</button>
        <button id="nextBtn">&gt;&gt;</button>
        <div>&nbsp;</div>            
        <div id="mtPositions_grid_obj" style="width:1200px;height:200px;border: 1px black solid"></div>
        
        <div>&nbsp;</div>            
        
        <div>&nbsp;</div>
        <button id="prevBtnOpen">&lt;&lt;</button>
        <button id="nextBtnOpen">&gt;&gt;</button>
        <div>&nbsp;</div>  
        
        <div id="mtOpenPositions_grid_obj" style="width:1200px;height:200px;border: 1px black solid"></div>
        
        <div>&nbsp;</div>  
        <div>All history positions</div>
        <div id="mtPositions_grid_all_obj" style="width:1200px;height:200px;border: 1px black solid"></div>

        <a href="../accdt/account_details/${accountObj.getId()}/do.html">Refresh account data</a>
        
                 
    </body>    
</html>    