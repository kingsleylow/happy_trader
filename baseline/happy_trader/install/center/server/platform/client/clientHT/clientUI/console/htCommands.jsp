<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
	<head>
			<link rel="stylesheet" type="text/css" href="js/tree_100722_pro/codebase/dhtmlxtree.css">
			<script src="js/tree_100722_pro/codebase/dhtmlxcommon.js"></script>
			<script src="js/tree_100722_pro/codebase/dhtmlxtree.js"></script> 
			<script src="js/tree_100722_pro/codebase/ext/dhtmlxtree_start.js"></script> 

			<link href="css/base.css" rel="stylesheet" type="text/css"/>
			
			<script type="text/javascript">

				var sXML = 
					'<tree id="0">' +
							'<item text="Query" open="1" id="100">'+
								'<item text="Prices" id="101"><userdata name="url">/htShowHistoryData.jsp</userdata></item>'+
								'<item text="Drawable Objects" id="102"><userdata name="url">/htShowDrawableObjects.jsp</userdata></item>'+
								'<item text="Log" id="103"><userdata name="url">/htShowLogData.jsp</userdata></item>'+
								'<item text="Internal Log" id="104"><userdata name="ext_url">/HtReadFile.jsp?target_read=java_server</userdata></item>'+
								'<item text="Broker Dialog" id="105"><userdata name="url">/htBrokerDialog.jsp</userdata></item>'+
								'<item text="Broker Dialog 2" id="106"><userdata name="url">/htBrokerRunNames_v2.jsp</userdata></item>'+
								'<item text="Session Operations" id="107"><userdata name="url">/htBrokerDetailRunNames.jsp</userdata></item>'+
								'<item text="Alerts" id="108"><userdata name="url">/htShowAlertData.jsp</userdata></item>'+
								'<item text="Web Applications" id="109"><userdata name="url">/htDeployedWebApplications.jsp</userdata></item>'+
								'<item text="Background Jobs" id="110"><userdata name="url">/htShowBackgroundJobs.jsp</userdata></item>'+
							'</item>'+
							
							'<item text="Trading Server Management" open="1" id="200">'+
								'<item text="Start/Stop Trading Servers" id="201"><userdata name="url">/htStartStopServer.jsp</userdata></item>'+
								'<item text="Server Thread Management" id="202"><userdata name="url">/htServerAlgorithmLoad.jsp</userdata></item>'+
								'<item text="Run External Packages" id="203"><userdata name="url">/htExternalApiRun_v2.jsp</userdata></item>'+
							'</item>'+
							
							'<item text="Data Management" open="1" id="300">'+
								'<item text="Export/Import Data" id="301"><userdata name="url">/htExportImportWizard.jsp</userdata></item>'+
								'<item text="Start/Stop RT Data Providers" id="302"><userdata name="url">/htRTProviderManager.jsp</userdata></item>'+
								'<item text="Start/Stop Event Plugins" id="303"><userdata name="url">/htEventPluginsManager.jsp</userdata></item>'+
							'</item>'+
							
							'<item text="Administration" open="1" id="400">'+
								'<item text="Trading Servers" id="401"><userdata name="url">/htServers.jsp</userdata></item>'+
								'<item text="History Data Providers" id="402"><userdata name="url">/htHistoryProviders.jsp</userdata></item>'+
								'<item text="RT Data Providers" id="403"><userdata name="url">/htRTProviders.jsp</userdata></item>'+
								'<item text="Event Plugins" id="404"><userdata name="url">/htEventPlugins.jsp</userdata></item>'+
								'<item text="Data Profiles" id="405"><userdata name="url">/htDataProfile.jsp</userdata></item>'+
								'<item text="Algorithm/Broker Pairs" id="406"><userdata name="url">/htAlgBrkPair_v2.jsp</userdata></item>'+
								'<item text="Simulation Profiles" id="407"><userdata name="url">/htSimulationProfile.jsp</userdata></item>'+
								'<item text="Critical Errors" id="408"><userdata name="url">/htCriticalErrors.jsp</userdata></item>'+
							'</item>'+
							
					'</tree>';


				var tree_m = null;
				function printHint(str)
				{
					navPageHintObj.innerText = str;
					//alert(str);
				}
			
			 function doOnClick(id){
				var url = tree_m.getUserData(id,"url");
				if (url != undefined) {
					parent.show_main_frame(url);
				}
				
				var ext_url = tree_m.getUserData(id,"ext_url");
				if (ext_url != undefined) {
					parent.show_separate_window(ext_url);
				}
			}


				function on_load()
				{
					
					tree_m = new dhtmlXTreeObject("treeBoxObj","100%", "100%", 0);
					tree_m.setImagePath("js/tree_100722_pro/codebase/imgs/");
					tree_m.loadXMLString(sXML);
					
					tree_m.setOnClickHandler(doOnClick);
					sXML = '';

				
				}

			</script>

	</head>
	<body onload="on_load();">
	    <div id='navPageHintObj' style="width:100%;border-style:solid; border-width:1px;" ></div>
		<div align=left id="treeBoxObj" style="overflow:hidden"></div>
		


	</body>
</html>
