// ----------------------
// base request class

function HtMtRequestHelper(user, password, command_name)
{
     this.init_ = function()
     {
         var r = {};
         r.HUSR = $.md5(this.user_m);
         r.HPW = $.md5(this.password_m);
         r.COMMAND_NAME = command_name;
         
         return r;
     };
     
     this.getBaseObject = function()
     {
         return this.base_object_m;
     };
     
    this.getRequestDataHelper = function(data)
    {
        var r = {
           request_data: JSON.stringify(data)
        };
        
        return r;
    }
     
     this.user_m = user;
     this.password_m = password;
     this.base_object_m = this.init_();
     
}

// ----------------------------

function HtStartInstanceReq(user, password, debug, force_create, path_to_settings)
{
    
    this.COMMAND = "START_INSTANCE_REQ";
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        this.current_obj_m.DEBUG=false;
       
        this.current_obj_m.DEBUG = false;
        if (resolveBooleanValue(debug)) {
            this.current_obj_m.DEBUG = true;
        }
        
        
        this.current_obj_m.FORCE_CREATE = false;
        if (resolveBooleanValue(force_create)) {
            this.current_obj_m.FORCE_CREATE = true;
        }
        
        this.current_obj_m.SETTINGS_FILE = "";
        if (isVariableValid(path_to_settings)) {
            this.current_obj_m.SETTINGS_FILE = path_to_settings;
        }
       
        
        // command name
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
         
    
   
    this.current_obj_m = null;
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
    
};

// ----------------------------

function HtStopInstanceReq(user, password)
{
    this.COMMAND = "STOP_INSTANCE_REQ";   
    this.init_ = function()
    {
        
        return true;
    };
    
    this.getData  = function()
    {
        return this.base_m.getBaseObject();
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------
// get status
function HtGetStatusReq(user, password)
{
    this.COMMAND = "GET_STATUS_REQ";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        
        return true;
    };
    
    this.getData  = function()
    {
        return this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------

function HtRestartMtReq(user, password, mt_instance)
{
    /* 
     * user_list_arr = [ {}, {}, {}]
     * {NAME:..., PASSWORD:..., SERVER: ... , LIFETIME:... }
     * 
     */
    this.COMMAND = "RESTART_MT";   
    
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
         
        if (isVariableValid(mt_instance)) {
            this.current_obj_m.MT_INSTANCE = mt_instance;
        } else {
            this.current_obj_m.MT_INSTANCE = "";
        }
         
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
    
    this.current_obj_m = null;
    
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
}

// ----------------------------

function HtStartPollUsersReq(user, password, mt_instance, user_list_arr, company_name)
{
    /* 
     * user_list_arr = [ {}, {}, {}]
     * {NAME:..., PASSWORD:..., SERVER: ... , LIFETIME:... }
     * 
     */
    this.COMMAND = "POLL_USERS_REQ";   
    
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
         
        if (isVariableValid(mt_instance)) {
            this.current_obj_m.MT_INSTANCE = mt_instance;
        } else {
            this.current_obj_m.MT_INSTANCE = "";
        }
        
        if (isVariableValid(company_name)) {
            this.current_obj_m.COMPANY_NAME = company_name;
        } else {
            this.current_obj_m.COMPANY_NAME = "";
        }
        
        
        this.current_obj_m.DATA = {};
        this.current_obj_m.DATA.USER_LIST = [];
        
        // init user list
        if (isVariableValid(user_list_arr)) {
             for (var user_row in user_list_arr) {
                 
                  var new_user = {
                     NAME: user_list_arr[user_row].NAME,
                     PASSWORD: user_list_arr[user_row].PASSWORD,
                     LIFETIME: parseInt(user_list_arr[user_row].LIFETIME),
                     SERVER: user_list_arr[user_row].SERVER
                 };
                 
                 this.current_obj_m.DATA.USER_LIST.push(new_user);
             }
        }
         
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
    
    this.current_obj_m = null;
    
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
}

// ----------------------------

function HtClearUsersReq(user, password, mt_instance)
{
    this.COMMAND = "CLEAR_ALL_USERS";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        if (isVariableValid(mt_instance)) {
            this.current_obj_m.MT_INSTANCE = mt_instance;
        } else {
            this.current_obj_m.MT_INSTANCE = "";
        }
        
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------

function HtGetPositionHistoryReq(
      user, 
      password, 
      company_name, 
      account_name, 
      server_name,
      openPosFilterTime, // open pos filetr time
      closePosFilterTime, // close pos filter time
      isIncludeChangeDetails, // if we want to include chnages
      typeOfPosToSelect // if we want to include history or open or both
)
{
   	
        
    this.COMMAND = "GET_POSITION_HISTORY_REQ";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        if (isVariableValid(company_name)) {
            this.current_obj_m.COMPANY_NAME = company_name;
        } else {
            this.current_obj_m.COMPANY_NAME = "";
        }
        
        if (isVariableValid(server_name)) {
            this.current_obj_m.SERVER_NAME = server_name;
        } else {
            this.current_obj_m.SERVER_NAME = "";
        }
        
        if (isVariableValid(account_name)) {
            this.current_obj_m.ACCOUNT_NAME = account_name;
        } else {
            this.current_obj_m.ACCOUNT_NAME = "";
        }
        
        this.current_obj_m.OPEN_POS_FILTER_TIME = parseIntAsValidUnixTime(openPosFilterTime);
        this.current_obj_m.CLOSE_POS_FILTER_TIME = parseIntAsValidUnixTime(closePosFilterTime);
        
        this.current_obj_m.INCLUDE_CHANGE_DETAILS = false;
        if (isVariableValid(isIncludeChangeDetails)) {
            
            if (isIncludeChangeDetails) {
                this.current_obj_m.INCLUDE_CHANGE_DETAILS = true;
            }
        }
        
        if (isVariableValid(typeOfPosToSelect)) {
            var val = parseInt(typeOfPosToSelect);
            
            if (val == HtMtBrdidgeConsts.QUERY_POSITION_ONLY_HISTORY) {
                this.current_obj_m.TYPE_OF_POS_TO_SELECT = HtMtBrdidgeConsts.QUERY_POSITION_ONLY_HISTORY;
            }
            else if (val == HtMtBrdidgeConsts.QUERY_POSITION_ONLY_OPEN) {
                this.current_obj_m.TYPE_OF_POS_TO_SELECT = HtMtBrdidgeConsts.QUERY_POSITION_ONLY_OPEN;
            }
            else if (val == HtMtBrdidgeConsts.QUERY_POSITION_ONLY_BOTH) {
                this.current_obj_m.TYPE_OF_POS_TO_SELECT = HtMtBrdidgeConsts.QUERY_POSITION_ONLY_BOTH;
            }
            else
                // default
                this.current_obj_m.TYPE_OF_POS_TO_SELECT = HtMtBrdidgeConsts.QUERY_POSITION_ONLY_BOTH;
        }
       
        
        
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------

function HtGetAccountHistoryReq(
      user, 
      password, 
      company_name, 
      account_name
)
{
   	
        
    this.COMMAND = "GET_ACCOUNT_HISTORY_REQ";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        if (isVariableValid(company_name)) {
            this.current_obj_m.COMPANY_NAME = company_name;
        } else {
            this.current_obj_m.COMPANY_NAME = "";
        }
        
        if (isVariableValid(account_name)) {
            this.current_obj_m.ACCOUNT_NAME = account_name;
        } else {
            this.current_obj_m.ACCOUNT_NAME = "";
        }
        
       
        
        
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------

function HtSubscribeUserReq(
      user, 
      password, 
      company_name, 
      account_name,
      account_server,
      account_password,
      mt_instance,
      acc_history_req,
      pos_history_req,
      prev_cookie_id
)
{
   	
        
    this.COMMAND = "SUBSCRIBE_USER_REQ";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        if (isVariableValid(mt_instance)) {
            this.current_obj_m.MT_INSTANCE = mt_instance;
        } else {
            this.current_obj_m.MT_INSTANCE = "";
        }
        
        if (isVariableValid(account_password)) {
            this.current_obj_m.ACCOUNT_PASSWORD = account_password;
        } else {
            this.current_obj_m.ACCOUNT_PASSWORD = "";
        }
        
        // must override in acc_history_req and pos_history_req
        if (isVariableValid(company_name)) {
            this.current_obj_m.COMPANY_NAME = company_name;
        } else {
            this.current_obj_m.COMPANY_NAME = "";
        }
        
         if (isVariableValid(account_server)) {
            this.current_obj_m.SERVER = account_server;
        } else {
            this.current_obj_m.SERVER = "";
        }
        
        if (isVariableValid(account_name)) {
            this.current_obj_m.ACCOUNT_NAME = account_name;
        } else {
            this.current_obj_m.ACCOUNT_NAME = "";
        }
        
        this.current_obj_m.HISTORY_ACCOUNT_REQUEST = null;
        if (isVariableValid(acc_history_req)) {
          acc_history_req.current_obj_m.ACCOUNT_NAME =  this.current_obj_m.ACCOUNT_NAME;
          acc_history_req.current_obj_m.COMPANY_NAME =  this.current_obj_m.COMPANY_NAME;
          
          this.current_obj_m.HISTORY_ACCOUNT_REQUEST = acc_history_req.getData();
        }
        
        this.current_obj_m.HISTORY_POSITION_REQUEST = null;
        if (isVariableValid(pos_history_req)) {
          pos_history_req.current_obj_m.ACCOUNT_NAME =  this.current_obj_m.ACCOUNT_NAME;
          pos_history_req.current_obj_m.COMPANY_NAME =  this.current_obj_m.COMPANY_NAME;
          this.current_obj_m.HISTORY_POSITION_REQUEST = pos_history_req.getData();
        }
        
        this.current_obj_m.PREV_COOKIE = -1;
        if (isVariableValid(prev_cookie_id)) {
            var prev_cookie_int = parseInt(prev_cookie_id);
            if (!isNaN(prev_cookie_int))
                this.current_obj_m.PREV_COOKIE = prev_cookie_int;
        }
        
                
        return true;
    };
    
    this.getData  = function()
    {
        return  this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------
// wraps Req_G_QUERY_AUTOLOAD_USERS
function HtGetAutoloadUsers(user, password)
{
    this.COMMAND = "GET_AUTOLOAD_USERS_REQ";   
    this.init_ = function()
    {
        this.current_obj_m = this.base_m.getBaseObject();
        
        
        return true;
    };
    
    this.getData  = function()
    {
        return this.current_obj_m;
    };
    
    this.getRequestData = function()
    {
        return this.base_m.getRequestDataHelper(this.getData());
    };
   
    this.current_obj_m = null;
    
    this.base_m = new HtMtRequestHelper(user, password, this.COMMAND);
    
    // must be the last
    this.initialized_m = this.init_();
   
};

// ----------------------------