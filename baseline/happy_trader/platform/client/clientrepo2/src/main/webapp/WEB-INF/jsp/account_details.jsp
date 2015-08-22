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
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/htmtbridge/ajax_helper.js" ></script>

        <!-- W2UI -->
        <link rel="stylesheet" type="text/css" href="<ct:basepath base="accdt" />../resources/lib/w2ui-1.4.2/w2ui-1.4.2.css" />
        <!--
        <link rel="stylesheet" type="text/css" href="<ct:basepath base="accdt" />../resources/lib/w2ui-1.4.2/w2ui-1.4.2-ext.css" />
        -->
        <script type="text/javascript" src="<ct:basepath base="accdt" />../resources/lib/w2ui-1.4.2/w2ui-1.4.2.min.js" ></script>
        
        <!-- HIGH CHARTS -->
        <script src="<ct:basepath base="accdt" />../resources/lib/highcharts-4.0.4/js/highcharts.js"></script>
        <script src="<ct:basepath base="accdt" />../resources/lib/highcharts-4.0.4/js/modules/exporting.js"></script>

        <style>
            .base-divs-this-page {
               height: 200px; 
               border: 1px solid #ddd; 
               border-top: 0px;
            } 
            
            .nav-divs-this-page {
               height: 36px;
               padding: 4px; 
               border: 1px solid silver; 
               border-radius: 3px;
            } 
            .input-page-num-this-page
            {
                width:100%;
            }
            
            .input-page-cont-div-this-page
            {
                width:30px;
                height:25px;
                display: block;
                overflow: hidden;
                border: 1px solid silver; 
                text-align: center;
                vertical-align:middle;
            };
            
            .chart-div-this-page
            {
                min-width: 100%;
                width: 100%;
                height: 200px; 
                margin: 0 auto;
            };
            
            .header-this-page
            {
                font-size: larger;
            }
            
            
        </style>
        
        <script>
            
            var HT_GATE_URL = '<ct:basepath base="accdt" />../htgate/happyTraderGate';
            var DIRECTION_PREV = 1;
            var DIRECTION_NEXT  =-1;
            
            //
            var PAGE_SIZE=9;
            //
            
            var pos_grid_column_descriptor_m  =
                 [
                     {NAME: "POSITION_UID", FORMATTER: null}, 
                     {NAME: "TICKET", FORMATTER: null}, 
                     {NAME:"SYMBOL", FORMATTER: null}, 
                     {NAME:"OPEN_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue}, 
                     {NAME:"OPEN_PRICE", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}, 
                     {NAME:"CLOSE_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue}, 
                     {NAME:"CLOSE_PRICE", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}, 
                     {NAME:"SL", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}, 
                     {NAME:"TP", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}, 
                     {NAME:"PROFIT", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}, 
                     {NAME:"COMMENT", FORMATTER: null}, 
                     {NAME:"EXPIRATION_TIME", FORMATTER: HtMtBrdidgeFormatters.formatDateValue}, 
                     {NAME:"DIRECTION", FORMATTER: HtMtBrdidgeFormatters.formatDirection}, 
                     {NAME:"STATE", FORMATTER: HtMtBrdidgeFormatters.formatPosState},
                     {NAME:"LOTS", FORMATTER: HtMtBrdidgeFormatters.formatPriceValue}
                 ];
                 
            
            var openGridData_m = 
            {
                grid_table: null,
                num_page_div: null,
                cur_page_m : 1,
                no_data_detected_m: true
            };
            
            var historyGridData_m = 
            {
                grid_table: null,
                num_page_div: null,
                cur_page_m : 1,
                no_data_detected_m: true
            };
            
            
            
            
            function start_load()
            {
                
            }
            
            function stop_load()
            {
                
            }
            
            function navigation_total_perf()
            {
                start_load();
                
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
                            //initPositions_HistRequest(AllHistGridData, custom_data_p, DIRECTION_NEXT);
                            stop_load();
                            init_perf_char(custom_data_p);
                        },
                         function(reason) {
                             stop_load();
                             CommonDialogHelper.show("Internal error", reason);
                         },
                         function(code, reason, custom_data)
                         {
                            stop_load();
                            HtMtBrdidgeCommon.DefaultCustomErrorHandler(code, reason, custom_data);
                         }
                );
            }
            
            function navigation(griddata, is_hist, direction)
            {
                
                griddata.no_data_detected_m = true;
                var pageNum = griddata.cur_page_m;
                
                start_load();
                
                
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
                            stop_load();
                            init_data_table(griddata, custom_data_p, direction);
                            
                        },
                        function(reason)
                        {
                            stop_load();
                            CommonDialogHelper.show("Internal error", reason);
                        },
                        function(code, reason, custom_data)
                        {
                            stop_load();
                            HtMtBrdidgeCommon.DefaultCustomErrorHandler(code, reason, custom_data);
                        }
                );
              
            }
            
            
            
            function pre_init()
            {
                $(".base-divs-this-page").css('display', 'none');
                $("#perf_tab").css('display', 'inline');
                
                
            }
            
            function post_init()
            {
                openGridData_m.grid_table = w2ui['grid_open_orders_name'];
                historyGridData_m.grid_table = w2ui['grid_close_orders_name'];
                
                openGridData_m.num_page_div  = $('#page_num_inp_open_orders_div');
                historyGridData_m.num_page_div = $('#page_num_inp_close_orders_div');
                
                openGridData_m.grid_table.clear(false);
                historyGridData_m.grid_table.clear(false);
                
                // init def tab
                navigation_total_perf();
                
                $("#accBalanceObj").html( 'Balance: ' + HtMtBrdidgeFormatters.formatPriceValue(${lastBalance}) + ' ${currency}');
               
                
            }

            function next_btn_click(griddata,is_hist)
            {
                if (!griddata.no_data_detected_m) {
                        griddata.cur_page_m++;
                }
                    
                navigation(griddata,  is_hist, DIRECTION_NEXT);
            }
            
            function prev_btn_click(griddata,is_hist)
            {
                if (griddata.no_data_detected_m) {
                    if (griddata.cur_page_m > 1)
                        griddata.cur_page_m--;
                }
                    
                griddata.cur_page_m = griddata.cur_page_m-1;
                if (griddata.cur_page_m < 1)
                    griddata.cur_page_m = 1;
                    
                navigation(griddata, is_hist, DIRECTION_PREV);
            }
            
            function init_perf_char(custom_data_p)
            {
                // here we can calculate drop down
                
                if (!isVariableValid(custom_data_p))
                    return;
               
                var data_obj = custom_data_p.DATA;
                if (!isVariableValid(data_obj))
                    return;
                 
                //var is_changes_included = data_obj.CHANGES_INCLUDED;
                //alert('Changes included: ' +is_changes_included);
                var position_list = data_obj.POSITION_LIST;
                
                if (!isVariableValid(position_list))
                    return;
                
                var data = [];
                var profit_arr = [];
                
                
                if (position_list.length > 0) {
                    for (var i = 0; i < position_list.length; i++) {
                        var row = position_list[i];
                        
                        var profit = row['PROFIT'];
                        var close_time = row['CLOSE_TIME'];
                        
                        if (isVariableValid(profit) && isVariableValid(close_time)) {
                            
                            
                            var profit_i  = parseInt(profit);
                            var close_time_i = parseInt(close_time);
                            if (!isNaN(profit_i) && !isNaN(close_time_i)) {
                                
                                profit_arr.push([close_time_i, profit_i]);
                            }
        
                        }
                        
                    }
                    
                    

                    profit_arr.sort(function(a,b) { return parseInt(a[0]) - parseInt(b[0]) } );
                    //alert(dump(profit_arr));

                    
                    // create out data
                    var cummulative_profit = 0;
                   
                    
                    for(var i = 0; i < profit_arr.length; i++) {
                        cummulative_profit += profit_arr[i][1];
                        data.push([parseInt(profit_arr[i][0]), parseInt(cummulative_profit)]);
                        //data.push([parseInt(profit_arr[i][0]), parseInt(HtMtBrdidgeFormatters.formatPriceValue(cummulative_profit) )]);
                        //data.push([parseInt(i), parseInt(cummulative_profit)]);
                    }
                }
                
               
                var dropdown = calculateDropDown(data);
                //alert(dump(dropdown));
                //alert(dump(data));
                //
                var perf_chart = $('#perf-chart-container').highcharts();
                series = perf_chart.series[0];
                series.setData(data);
                
                // put drawdown
                var relative_drawdown = ((dropdown[0] / dropdown[1]) * 100).toFixed(2);
                //alert(relative_drawdown);
                var chart = $('#perf-chart-container').highcharts();
                
                chart.setTitle({text: "Performance: " + relative_drawdown + "% drawdown"});
                chart.redraw();
                
            }
            
            function calculateDropDown(data) {
                var abs_dropdown = -1;
                if (data === null || data.length <=0)
                    return -1;
                
                var prevmaxi = 0;
                var prevmini = 0;
                var maxi = 0;
                
                var abs_min = -1;
                var abs_max = -1;
  
                for(var i = 0; i < data.length; i++) {
                    
                  
                    
                    if (i === 0) {
                        abs_min = abs_max = data[i][1];
                    }
                    
                    if (abs_min > data[i][1])
                        abs_min = data[i][1];
                    
                    if (abs_max < data[i][1])
                        abs_max = data[i][1];
                    
                    
                    if (data[i][1] > data[maxi][1]) {
                        maxi = i;
                    } else {
                        if (( data[maxi][1] - data[i][1] ) > (data[prevmaxi][1] - data[prevmini][1])) {
                            prevmaxi = maxi;
                            prevmini = i;
                        }
                    }
                }
                
                abs_dropdown = data[prevmaxi][1] - data[prevmini][1];
                abs_range = abs_max - abs_min;
                
                
                return [abs_range,abs_dropdown ];
            }
            
            function init_data_table(griddata, custom_data_p)
            {
                
                if (!isVariableValid(custom_data_p))
                    return;
               
                var data_obj = custom_data_p.DATA;
                if (!isVariableValid(data_obj))
                    return;
                 
                //var is_changes_included = data_obj.CHANGES_INCLUDED;
                //alert('Changes included: ' +is_changes_included);
                var position_list = data_obj.POSITION_LIST;
                
                if (!isVariableValid(position_list))
                    return;
                
                // init
                var idx = 0;
                if (position_list.length > 0) {
                    
                    // clear
                    griddata.grid_table.clear(false);
                
                    
                    for (var i = 0; i < position_list.length; i++) {
                        var rec={recid: ++idx};
                        var row = position_list[i];
                        for (var descriptor_elem in pos_grid_column_descriptor_m) {
                            var descriptor_entry = pos_grid_column_descriptor_m[descriptor_elem];
                            
                            var format_function = descriptor_entry.FORMATTER;
                            var name = descriptor_entry.NAME;
                            var onCalcFunc = descriptor_entry.ON_CALC_FUN;
                            
                            
                            if (!isVariableValid(name))
                                continue;
                          
                            var raw_value = null;
                            if (!isVariableValid(onCalcFunc)) {
                               raw_value = row[ name ];

                            }
                            else {
                               raw_value = onCalcFunc(pos_grid_column_descriptor_m, row);
                            }
                            
                            rec[name] = ( isVariableValid(format_function) ? format_function(raw_value) : raw_value);
                    
                        }
                        //alert('adding: ' + dump(rec));
                    
                        griddata.grid_table.add(rec);
                        
                    }
                    
                    
                    griddata.no_data_detected_m = false;
                    
                    // now can update page num
                    griddata.num_page_div.html(griddata.cur_page_m);
                }
            }
            
            
            
            $(function () {
                pre_init();
                
                $('#main_tab').w2tabs({
                        name: 'main_tab_name',
                        active: 'perf',
                        tabs: [
                                { id: 'perf', caption: 'Performance',closable: false },
                                { id: 'open_orders', caption: 'Open orders', closable: false },
                                { id: 'close_orders', caption: 'History orders', closable: false }
                        ],
                        onClick: function (event) {
                            //alert(event.target);
                            //$('#tab-content').html('Tab: ' + event.target);
                            var tab_id = event.target + "_tab";
                            
                            $(".base-divs-this-page").css('display', 'none');
                            $('#'+tab_id).css('display', 'inline');
                            
                            
                            if (tab_id === 'perf_tab') {
                                // performance activation
                                navigation_total_perf();
                            }
                            else if (tab_id === 'open_orders_tab') {
                                navigation(openGridData_m, false, DIRECTION_NEXT);
                            }
                            else if (tab_id === 'close_orders_tab') {
                                navigation(historyGridData_m, true, DIRECTION_NEXT);
                            }
                        }
                });

                $('#grid_close_orders').w2grid({
                    name: 'grid_close_orders_name',
                    header: 'History orders',
                    columns: [
                        {field: 'OPEN_TIME', caption: 'Open Time', size: '160px'},
                        {field: 'TICKET', caption: 'Order', size: '90px'},
                        {field: 'DIRECTION', caption: 'Type', size: '60px'},
                        {field: 'LOTS', caption: 'Size', size: '60px'},
                        {field: 'SYMBOL', caption: 'Item', size: '80px'},
                        {field: 'OPEN_PRICE', caption: 'Open', size: '90px'},
                        {field: 'SL', caption: 'S/L', size: '90px'},
                        {field: 'TP', caption: 'T/P', size: '90px'},
                        {field: 'CLOSE_TIME', caption: 'Close Time', size: '160px'},
                        {field: 'CLOSE_PRICE', caption: 'Close', size: '90px'},
                        {field: 'PROFIT', caption: 'Profit', size: '90px'}
                    ],
                    records: [
                        
                    ]
                });
                
                $('#grid_open_orders').w2grid({
                    name: 'grid_open_orders_name',
                    header: 'Open orders',
                    columns: [
                        {field: 'OPEN_TIME', caption: 'Open Time', size: '160px'},
                        {field: 'TICKET', caption: 'Order', size: '90px'},
                        {field: 'DIRECTION', caption: 'Type', size: '60px'},
                        {field: 'LOTS', caption: 'Size', size: '60px'},
                        {field: 'SYMBOL', caption: 'Item', size: '80px'},
                        {field: 'OPEN_PRICE', caption: 'Open', size: '90px'},
                        {field: 'SL', caption: 'S/L', size: '90px'},
                        {field: 'TP', caption: 'T/P', size: '90px'},
                        {field: 'PROFIT', caption: 'Profit', size: '90px'}
                    ],
                    records: [
                        
                    ]
                });
                
                
                $('#grid_open_orders_toolbar').w2toolbar({
                    name: 'grid_open_orders_toolbar_name',
                    items: [
                        { type: 'html', id: 'page_num_inp', html: '<div id="page_num_inp_open_orders_div" class="input-page-cont-div-this-page">1</div>' },
                        { type: 'button',  id: 'prev_grid_open_orders_toolbar_btn',  img: 'icon-left', hint: 'Next' },
                        { type: 'button',  id: 'next_grid_open_orders_toolbar_btn',  img: 'icon-right', hint: 'Previous' }
                    ],
                    onClick: function (event) {
                        //console.log('Target: '+ event.target, event);
                        //alert(event.target + ', '+event);
                        if (event.target === 'next_grid_open_orders_toolbar_btn') {
                            next_btn_click(openGridData_m,false);
                        }
                        else if (event.target === 'prev_grid_open_orders_toolbar_btn')  {
                            prev_btn_click(openGridData_m,false);
                        }
                    }
                });
                
                $('#grid_close_orders_toolbar').w2toolbar({
                    name: 'grid_close_orders_toolbar_name',
                    items: [
                        { type: 'html', id: 'page_num_inp', html: '<div id="page_num_inp_close_orders_div" class="input-page-cont-div-this-page">1</div>' },
                        { type: 'button',  id: 'prev_grid_close_orders_toolbar_btn',  img: 'icon-left', hint: 'Next' },
                        { type: 'button',  id: 'next_grid_close_orders_toolbar_btn',   img: 'icon-right', hint: 'Previous' }
                    ],
                    onClick: function (event) {
                        //console.log('Target: '+ event.target, event);
                        //alert(event.target + ', '+event);
                        if (event.target === 'next_grid_close_orders_toolbar_btn') {
                            next_btn_click(historyGridData_m,true);
                        }
                        else if (event.target === 'prev_grid_close_orders_toolbar_btn')  {
                            prev_btn_click(historyGridData_m,true);
                        }
                    }
                });
                
               
                
                // chart
                $('#perf-chart-container').highcharts({
                    chart: {
                        zoomType: 'x'
                    },
                     tooltip: {
                        formatter: function() {
                             return 'Profit: ' + HtMtBrdidgeFormatters.formatPriceValue(this.y) + ' ${currency}';
                        }
                    },
                    title: {
                        text: 'Performance',
                        x: -20 //center
                    },
                    xAxis: {
                        type: 'datetime',
                        /*
                        dateTimeLabelFormats: { // don't display the dummy year
                            month: '%e. %b',
                            year: '%b'
                        },
                        */
                       labels: {
                            formatter: function() {
                                return Highcharts.dateFormat('%a %d %b %H:%M:%S', this.value);
                            }
                        },
                        
                        title: {
                            text: 'Date'
                        }
                    },
                    yAxis: {
                        title: {
                            text: 'Profit'
                        },
                        labels: {
                            formatter: function() {
                                return HtMtBrdidgeFormatters.formatPriceValue(this.value) + ' ${currency}';
                            }
                        }
                    },
                    series: [
                        {
                            name: "Absolute profit",
                            data: []
                        }
                    ]
                        
                    
                });
                
               
               $('#top-toolbar').w2toolbar({
                    name: 'top_toolbar_name',
                    items: [
                        { 
                            type: 'html',  
                            id: 'html_item1',  
                            html: '<table class="header-this-page"><tr><td>Account: ${accountObj.getAccount_id()} [ ${holderName} ]</td></tr>'+
                                   '<tr><td>&nbsp;</td></tr>'+
                                   '<tr><td><div id="accBalanceObj"></div></td></tr>'+
                                   '<tr><td>&nbsp;</td></tr>'+
                                   '</table>'
                        }
                        
                       
                    ],
                    onClick: function (event) {
                        console.log('Target: '+ event.target, event);
                    }
                });
                
                $('#bottom-toolbar').w2toolbar({
                    name: 'bottom_toolbar_name',
                    items: [
                        { type: 'html',  id: 'html_item1',  
                            html: ''
                                    
                        }
                       
                    ],
                    onClick: function (event) {
                        console.log('Target: '+ event.target, event);
                    }
                });
          
                post_init();
            });
        </script>

    </head>
    <body>
        <base href='<ct:basepath base="accdt" />' />
       
        <div id="top-toolbar" style="padding: 4px; border: 1px solid silver; border-radius: 3px">
           
        </div>
        
        <div id="main_tab"></div>
        
        
        <div id="perf_tab" class="base-divs-this-page">
            <div id="perf-chart-container" class="chart-div-this-page">
            </div>    
        </div>
        
        <div id="open_orders_tab" class="base-divs-this-page">
            
            <div id="grid_open_orders" style="width: 100%; height: 250px;">
            </div>
            
            <div id="grid_open_orders_toolbar" class="nav-divs-this-page" >
            </div>
         
        </div>
        
        <div id="close_orders_tab"  class="base-divs-this-page">
            <div id="grid_close_orders" style="width: 100%; height: 250px;">
            </div>
            
            <div id="grid_close_orders_toolbar" class="nav-divs-this-page">
            </div>
        </div>
        
         <div>
         </div>   
        
        <div id="bottom-toolbar" style="padding: 4px; border: 1px solid silver; border-radius: 3px">
    </body>    
    </html>    