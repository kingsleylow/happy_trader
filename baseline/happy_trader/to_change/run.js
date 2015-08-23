// Helper function:
function print(str) {
	WScript.Echo(str);
}

function writeToFileHelper(file_name, content) {
	if (content == null)
		content = "";
	// write this file
	var file_f = fso.OpenTextFile(file_name, 2, true, 0);
	if (file_f != null) {
		file_f.WriteLine(content);
		file_f.Close();
	}
	else
		throw new Error(-1, "Cannot write to: " + file_name);
}



////////////////////////////

print("Setup Happy Trader started");



var objArgs = WScript.Arguments;
if (objArgs.length != 1) {
	throw new Error(-1, "Invalid arguments passed");
}
var SETUP_FILE = objArgs(0);
if (SETUP_FILE.length <= 0) {
	throw new Error(-1, "Arguments must be <setup file>");
}


print("Setup file: "+SETUP_FILE);

var doc = new ActiveXObject("msxml2.DOMDocument.6.0");
doc.async = false;
doc.resolveExternals = false;
doc.validateOnParse = false;

// Load an XML file into the DOM instance.
doc.load(SETUP_FILE);


var happyTraderUrl_v = doc.getElementsByTagName("happyTraderUrl")[0].childNodes[0].nodeValue;
print("Happy Trader core URL:" + happyTraderUrl_v);

var happyTraderUser_v = doc.getElementsByTagName("happyTraderUser")[0].childNodes[0].nodeValue;
print("Happy Trader User:" + happyTraderUser_v);

var happyTraderPassword_v = doc.getElementsByTagName("happyTraderPassword")[0].childNodes[0].nodeValue;
print("Happy Trader Pasword:" + happyTraderPassword_v);

var historyPositionsPageSize_v = doc.getElementsByTagName("historyPositionsPageSize")[0].childNodes[0].nodeValue;
print("History position size:" + historyPositionsPageSize_v);

var mysqluser_v = doc.getElementsByTagName("mysqluser")[0].childNodes[0].nodeValue;
print("MySQL HT user:" + mysqluser_v);

var mysqlpassword_v = doc.getElementsByTagName("mysqlpassword")[0].childNodes[0].nodeValue;
print("MySQL HT pasword:" + mysqlpassword_v);

var mysqlport_v = doc.getElementsByTagName("mysqlport")[0].childNodes[0].nodeValue;
print("MySQL HT port:" + mysqlport_v);

var mysqlhost_v = doc.getElementsByTagName("mysqlhost")[0].childNodes[0].nodeValue;
print("MySQL HT host:" + mysqlhost_v);

var htraderjavamemory_v = doc.getElementsByTagName("htraderjavamemory")[0].childNodes[0].nodeValue;
print("Happy Trader core URL:" + htraderjavamemory_v);

var javahome_v = doc.getElementsByTagName("javahome")[0].childNodes[0].nodeValue;
print("JAVA_HOME:" + javahome_v);

var htraderjavamemory_v = doc.getElementsByTagName("htraderjavamemory")[0].childNodes[0].nodeValue;
print("Happy Trader Java memory size:" + htraderjavamemory_v);


//

var mysqluserec_v = doc.getElementsByTagName("mysqluserec")[0].childNodes[0].nodeValue;
print("MySQL end client user:" + mysqluserec_v);

var mysqlpasswordec_v = doc.getElementsByTagName("mysqlpasswordec")[0].childNodes[0].nodeValue;
print("MySQL end client pasword:" + mysqlpasswordec_v);

var mysqlportec_v = doc.getElementsByTagName("mysqlportec")[0].childNodes[0].nodeValue;
print("MySQL end client port:" + mysqlportec_v);

var mysqlhostec_v = doc.getElementsByTagName("mysqlhostec")[0].childNodes[0].nodeValue;
print("MySQL end client host:" + mysqlhostec_v);


//



var htdebuglevel_v = doc.getElementsByTagName("htdebuglevel")[0].childNodes[0].nodeValue;
print("Happy Trader core debug level:" + htdebuglevel_v);


var htclientdebuglevel_v = doc.getElementsByTagName("htclientdebuglevel")[0].childNodes[0].nodeValue;
print("Happy Trader HT client debug level:" + htclientdebuglevel_v);


var winstonedebuglevel_v = doc.getElementsByTagName("winstonedebuglevel")[0].childNodes[0].nodeValue;
print("Happy Trader winstone debug level:" + winstonedebuglevel_v);

// end client



// open zip file
var shell = WScript.CreateObject("WScript.Shell");
var execRes = shell.Run("cmd /C rmdir tmp /S /Q", 1, true);
execRes = shell.Run("cmd /C xcopy /q /r /e /y ..\\install tmp\\install\\", 1, true);
execRes = shell.Run("cmd /C unzip -q -o tmp\\install\\center\\server\\clientapp\\clientrepo.war -d tmp\\tmp_clientrepo", 1, true);

// edit files
var fso = new ActiveXObject("Scripting.FileSystemObject");
var first_file = "tmp\\install\\center\\server\\supp\\user_data.cmd";
var file_content = "";


var user_data_cmd_f = fso.OpenTextFile(first_file, 1, false, 0);
if (user_data_cmd_f != null) {

	while (!user_data_cmd_f.AtEndOfStream) {
		
		var text_line = user_data_cmd_f.ReadLine();
		// substutute content
		text_line = text_line.replace(/HT_DEBUG_LEVEL\=[\d]+$/, "HT_DEBUG_LEVEL=" + htdebuglevel_v);
		text_line = text_line.replace(/HT_CLIENT_DEBUG_LEVEL\=\w+$/, "HT_CLIENT_DEBUG_LEVEL=" + htclientdebuglevel_v);
		text_line = text_line.replace(/WINSTONE_DEBUG_LEVEL\=\w+$/, "WINSTONE_DEBUG_LEVEL=" + winstonedebuglevel_v);

		// java home
		text_line = text_line.replace(/JAVA_HOME\=[\s\(\)\w\.\_\\\/\:]+$/, "JAVA_HOME=" + javahome_v);

		var connectUrlString = "jdbc:mysql://" + mysqlhost_v + ":" + mysqlport_v + "/htrader?user=" + mysqluser_v + "&password=" + mysqlpassword_v + "&characterEncoding=UTF-8";
		text_line = text_line.replace(/MYSQL_CONNECTION_URL\=[\w\:\/\?\&\=\-\"]+$/, "MYSQL_CONNECTION_URL=" + "\""+connectUrlString+"\"");

		text_line = text_line.replace(/JAVA_MEMORY_PARAM\=[\"\w\-]+$/, "JAVA_MEMORY_PARAM=" + "\"" + htraderjavamemory_v + "\"");

		file_content += text_line + "\r\n";


	}

	file_content += ":: UPDATED BY HELPER SETUP SCRIPT ::" + "\r\n";

	
	user_data_cmd_f.Close();
}
else
	throw new Error(-1, "Cannot read from: " + first_file);



writeToFileHelper(first_file, file_content);
print("Updated the file OK: " + first_file);

// need to update war


//print(file_content);
file_content = "";
// change common.properties
var file_common_prop = "tmp\\tmp_clientrepo\\WEB-INF\\common.properties";
var file_common_prop_f = fso.OpenTextFile(file_common_prop, 1, false, 0);
if (file_common_prop_f != null) {

	while (!file_common_prop_f.AtEndOfStream) {

		var text_line = file_common_prop_f.ReadLine();
		text_line = text_line.replace(/common.happyTraderUrl\=[\w\:\/\?\&\=\-\"]+$/, "common.happyTraderUrl=" + happyTraderUrl_v);
		text_line = text_line.replace(/common.happyTraderUser\=\w+$/, "common.happyTraderUser=" + happyTraderUser_v);
		text_line = text_line.replace(/common.happyTraderPassword\=\w+$/, "common.happyTraderPassword=" + happyTraderPassword_v);
		text_line = text_line.replace(/common.historyPositionsPageSize\=\d+$/, "common.historyPositionsPageSize=" + historyPositionsPageSize_v);

		file_content += text_line + "\r\n";


	}

	file_content += "# UPDATED BY HELPER SETUP SCRIPT" + "\r\n";

	file_common_prop_f.Close();
}
else
	throw new Error(-1, "Cannot read from: " + file_common_prop);

// write this file
writeToFileHelper(file_common_prop, file_content);

print("Updated the file OK: " + file_common_prop);


// jdbc properties
file_content = "";
var file_jdbc_prop = "tmp\\tmp_clientrepo\\WEB-INF\\jdbc.properties";
var file_jdbc_prop_f = fso.OpenTextFile(file_jdbc_prop, 1, false, 0);
if (file_jdbc_prop_f != null) {

	while (!file_jdbc_prop_f.AtEndOfStream) {

		
		var text_line = file_jdbc_prop_f.ReadLine();
		var connectUrlString = "jdbc:mysql://" + mysqlhostec_v + ":" + mysqlportec_v + "/client_store?characterEncoding=UTF-8";
		text_line = text_line.replace(/jdbc.databaseurl\=[\w\:\/\?\&\=\-\"]+$/, "jdbc.databaseurl=" + connectUrlString);
		text_line = text_line.replace(/jdbc.username\=[\w\W]+$/, "jdbc.username=" + mysqluserec_v);
		text_line = text_line.replace(/jdbc.password\=[\w\W]+$/, "jdbc.password=" + mysqlpasswordec_v);
	

		file_content += text_line + "\r\n";


	}

	file_content += "# UPDATED BY HELPER SETUP SCRIPT" + "\r\n";

	file_jdbc_prop_f.Close();
}
else
	throw new Error(-1, "Cannot read from: " + file_common_prop);

// write

// write this file
writeToFileHelper(file_jdbc_prop, file_content);

print("Updated the file OK: " + file_jdbc_prop);

// need to pack war file
print("Zipping clientrepo_updated.war");
var old_dir = shell.CurrentDirectory;
var new_dir = old_dir + "\\tmp\\tmp_clientrepo";

shell.CurrentDirectory = new_dir;
execRes = shell.Run("cmd /C DEL ..\\install\\center\\server\\clientapp\\*.* /S /Q", 1, true);
execRes = shell.Run("cmd /C ..\\..\\zip -q -r ..\\install\\center\\server\\clientapp\\clientrepo.war *", 1, true);


// create final zip
new_dir = old_dir + "\\tmp";
shell.CurrentDirectory = new_dir;

var nowdate = new Date();
var target_name = "fixed_install_" +
	nowdate.getFullYear() + nowdate.getMonth() + nowdate.getDay() + nowdate.getHours() + "_" + nowdate.getMinutes() + "_" + nowdate.getSeconds() + ".zip";

execRes = shell.Run("cmd /C ..\\zip -q -r -5 ..\\..\\" + target_name + " install", 1, true);

shell.CurrentDirectory = old_dir;

print("Removing temp data");
execRes = shell.Run("cmd /C rmdir tmp /S /Q", 1, true);

print("Finished");



