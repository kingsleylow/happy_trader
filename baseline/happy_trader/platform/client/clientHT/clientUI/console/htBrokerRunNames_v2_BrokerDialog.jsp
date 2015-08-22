<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtBrokerRunNames_v2_BrokerDialog" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
}
else if (methodName.equalsIgnoreCase("POST")) {

		SC.initialize_Post(request,response);
}
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
			<meta http-equiv="Cache-Control" content="No-cache">

				<title id='tltObj' >Broker Dialog</title>

				<link href="css/console.css" rel="stylesheet" type="text/css"/>

				<link rel="stylesheet" type="text/css" href="js/tree_100722_pro/codebase/dhtmlxtree.css">
					<script src="js/tree_100722_pro/codebase/dhtmlxcommon.js"></script>
					<script src="js/tree_100722_pro/codebase/dhtmlxtree.js"></script> 
					<script src="js/tree_100722_pro/codebase/ext/dhtmlxtree_start.js"></script> 


					<script>
			
						
				
						var tree_m = null;
						function page_load()
						{
							tree_m = new dhtmlXTreeObject("treeBoxObj","100%", "100%", 0);
							tree_m.setImagePath("js/tree_100722_pro/codebase/imgs/");
				
							var data = document.getElementById("xmlDataObj").innerText;
							tree_m.loadXMLString(data);
							
							
							// reset
							document.getElementById("xmlDataObj").innerText = '';
							
						}
						
						function do_expand()
						{
							if (tree_m != null) {
								tree_m.openAllItems(0);
							}
						}
						
						function do_collapse()
						{
							if (tree_m != null) {
								tree_m.closeAllItems(0);
							}
						}
						
						
			
					</script>	
					<style type="text/css">


						html,body{
							height:100%;
							margin:0px;
							padding:0px
						}
					</style>
					</head>
					<body  id="bodyObj" onload="page_load();">


						<textarea id="xmlDataObj" style="display:none">
							<%= SC.getStringSessionValue(SC.getUniquePageId(), request, "report_data") %>
						</textarea>	

						<table width=100% height="100%">
							<tr class=x2>
								<td width=100px height="20px">
									<a href='#' onclick='do_expand()'>Expand Nodes</a>
								</td>
								<td width=100px height="20px">
									<a href='#' onclick='do_collapse()'>Collapse Nodes</a>
								</td>

								<td>
								</td>	

							</tr>

							<tr>
								<td colspan=3 align="center" style='position:relative'>
									<div align=left id="treeBoxObj" style='border: 1px solid black;width:99%;height:99%;position:absolute;top:0px'>
									</div>
								</td>
							</tr>	

						</table>


					</body>
					</html>
