

var HtMtBrdidgeConsts = {
    INVALID_UID: "00000000000000000000000000000000",
    
    QUERY_POSITION_ONLY_HISTORY: 1,
    QUERY_POSITION_ONLY_OPEN: 2,
    QUERY_POSITION_ONLY_BOTH: 3,
    
    QUERY_ACCOUNT_DEAFULT : 0x0000,
    QUERY_ACCOUNT_INCLUDE_BALANCE_HISTORY : 0x0001,
    QUERY_ACCOUNT_INCLUDE_ACCOUNT_HISTORY : 0x0002,
                
    // this is all price figures multiplier 
    PRICE_GENERAL_MULTIPLIER: 10000
};

var HtMtBrdidgeFormatters = {
    formatUIDValue: function(uidval)
    {
        if (!isVariableValid(uidval))
            return "Not Valid";
        
        if (HtMtBrdidgeConsts.INVALID_UID === uidval)
            return "Not Valid";
        
        
        return uidval;
        
    },
    formatPriceValue: function(num)
    {
        if (!isVariableValid(num))
            return "";

        if (num_s <= 0)
            return "";

        var num_s = num / HtMtBrdidgeConsts.PRICE_GENERAL_MULTIPLIER;
        return num_s.toFixed(2);
    },
    formatDateValue: function(unix_msec)
    {
        if (!isVariableValid(unix_msec))
            return "";

        var unix_msec_i = parseInt(unix_msec);
        if (isNaN(unix_msec_i))
            return "";
            
        if (unix_msec_i <= 0)
            return "";


        return date_and_time(unix_msec_i);
    },
    formatPosState: function(val) {
        return HtMtBridgePosState.toString(val);
    },
    formatDirection: function(val) {
        return HtMtBridgeTradeDirection.toString(val);
    },
    formatIntValue: function (val) {
        var val_i = parseInt(val);
        if (isNaN(val_i))
            return "";
        
        return val_i;
    },
    formatBoolValue: function ( val) {
        var r  = resolveBooleanValue(val);
        if (r)
            return "Yes";
        else
            return "No";
    }
    
    

};

var HtMtBrdidgeCommon = {
    parseBoolean: function(val)
    {
        var val_i = parseInt(val);
        if (!isNaN(val_i)) {
            
            if (val_i === 0)
                return false;
            else
                return true;
        };
        
        // try string 
        return stringToBoolean(val);
    },
    
    createXgridRowFromArray: function(arraydata, columnarr)
    {

        var result = "";
        for (i = 0; i < columnarr.length; i++) {
            result += arraydata[columnarr[i]] + ",";
        }
        result = result.substring(0, result.length - 1);

        return result;
    },
    
    getXgridTypes: function(xgrid, columnarr) {
        if (!isVariableValid(columnarr))
            return null;

        if (!isVariableValid(xgrid))
            return null;

        var result = "";

        for (var descriptor_elem in columnarr) {

            var type = columnarr[descriptor_elem].TYPE;
            if (!isVariableValid(type))
                type = "ro";

            result += type + ",";
        }

        result = result.substring(0, result.length - 1);

        return result;
    },
    
    getXgridWidth: function(xgrid, columnarr) {
        if (!isVariableValid(columnarr))
            return null;

        if (!isVariableValid(xgrid))
            return null;

        var result = "";

        for (var descriptor_elem in columnarr) {

            var size = parseInt(columnarr[descriptor_elem].SIZE);
            if (isVariableValid(size) && !isNaN(size) && size > 0)
                result += size + ",";
            else
                result += "100" + ",";
        }

        result = result.substring(0, result.length - 1);

        return result;
    },
    
    getXgridAlign: function(xgrid, columnarr) {
        if (!isVariableValid(columnarr))
            return null;

        if (!isVariableValid(xgrid))
            return null;

        var result = "";

        for (var descriptor_elem in columnarr) {

            var align = columnarr[descriptor_elem].ALIGN;
            if (!isVariableValid(align))
                align = "left";

            result += align + ",";
        }

        result = result.substring(0, result.length - 1);

        return result;
    },
    
    getXgridHeader: function(xgrid, columnarr) {
        if (!isVariableValid(columnarr))
            return null;

        if (!isVariableValid(xgrid))
            return null;

        var result = "";

        for (var descriptor_elem in columnarr) {

            var caption = columnarr[descriptor_elem].CAPTION;
            if (!isVariableValid(caption))
                continue;

            result += caption + ",";
        }

        result = result.substring(0, result.length - 1);

        return result;

    },
    
    createXgridRowFromArrayFormatters: function(arraydata, columnarr)
    {
        var result = "";
        if (!isVariableValid(columnarr))
            return null;

        for (var descriptor_elem in columnarr) {

            var format_function = columnarr[descriptor_elem].FORMATTER;
            var name = columnarr[descriptor_elem].NAME;

            if (!isVariableValid(name))
                continue;

            var raw_value = arraydata[ name ];
            var value = "";

            if (isVariableValid(format_function)) {
                value = format_function(raw_value);
            }
            else {
                value = raw_value;
            }

            result += value + ",";
        }
        result = result.substring(0, result.length - 1);

        return result;
    },
    
    // this is error handler ->  error:function (jqXHr, textStatus, errorThrown) {...
    DefaultErrorHandler: function(reason)
    {
        CommonDialogHelper.show("Internal error", reason);
    },
    DefaultCustomErrorHandler: function(code, reason, custom_data) {
        CommonDialogHelper.show("Error", "Reason: [" + reason + "] - [" + custom_data + " ] - ( " + code + " )");
    }
}



var HtMtBridgePosState = {
    STATE_DUMMY: 0,
    STATE_OPEN: 2,
    STATE_HISTORY: 3,
    toString: function(val) {

        if (!isVariableValid(val))
            return "";

        var val_i = parseInt(val);
        if (isNaN(val_i))
            return "";

        if (val_i == HtMtBridgePosState.STATE_DUMMY)
            return "DUMMY";
        if (val_i == HtMtBridgePosState.STATE_OPEN)
            return "OPEN";
        if (val_i == HtMtBridgePosState.STATE_HISTORY)
            return "HISTORY";

        return "";
    }


};


var HtMtBridgePosType = {
    POS_DUMMY: 0,
    POS_BUY: 1, // alive
    POS_SELL: 2, // alive
    POS_LIMIT_BUY: 3, // pending
    POS_LIMIT_SELL: 4, // pending
    POS_STOP_BUY: 5, // pending
    POS_STOP_SELL: 6, // pending
    POS_STOP_LIMIT_BUY: 7,
    POS_STOP_LIMIT_SELL: 8,
    // close
    POS_CLOSE_LONG: 9,
    POS_CLOSE_SHORT: 10,
    POS_CLOSE_LIMIT_LONG: 11,
    POS_CLOSE_LIMIT_SHORT: 12,
    POS_CLOSE_STOP_LONG: 13,
    POS_CLOSE_STOP_SHORT: 14,
    POS_CLOSE_STOP_LIMIT_LONG: 15,
    POS_CLOSE_STOP_LIMIT_SHORT: 16,
    POS_CANCELLED: 17
};


var HtMtBridgeTradeDirection = {
    TD_NONE: 0,
    TD_LONG: 1,
    TD_SHORT: 2,
    TD_BOTH: 3,
    toString: function(val) {

        if (!isVariableValid(val))
            return "";


        var val_i = parseInt(val);
        if (isNaN(val_i))
            return "";



        if (val_i == HtMtBridgeTradeDirection.TD_NONE)
            return "NONE";
        if (val_i == HtMtBridgeTradeDirection.TD_LONG)
            return "LONG";
        if (val_i == HtMtBridgeTradeDirection.TD_SHORT)
            return "SHORT";
        if (val_i == HtMtBridgeTradeDirection.TD_BOTH)
            return "BOTH";

        return "";
    }
};

var HtMtEventWrapperBaseTypes= {
		TYPE_ACCOUNT_MINFO_UPDATE : 1,
		TYPE_BALANCE_UPDATE : 2,
		TYPE_POSITION_UPDATE : 3,
		TYPE_HEARTBEAT : 4,
		TYPE_ACCOUNT_UPDATE : 5,
		TYPE_POSITION_HISTORY : 6,
		TYPE_ACCOUNT_HISTORY : 7
                
};