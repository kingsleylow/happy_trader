<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtTestAjax" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {

		SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
		SC.initialize_Post(request,response);
}
%>

<%
if (methodName.equalsIgnoreCase("GET")) {
%>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		<script type="text/javascript" src="js/misc/helper.js"></script>
		<script>
			function on_get_data()
			{
				
				var xmlHttp = getXmlHttp();
				var url = "/htTestAjax.jsp?page_uid=<%=SC.getUniquePageId() %>";

				url+= "&req_encoding="+ encodeURIComponent(req_enc_obj.options(req_enc_obj.selectedIndex).value);
				url+= "&xml_encoding="+ encodeURIComponent(xml_enc_obj.options(xml_enc_obj.selectedIndex).value);

				if (as_xml_obj.checked) {
					url+= "&as_xml=true";
				}
				else {
					url+= "&as_xml=false";
				}
				
				makeAsynchXmlHttpRequestExternHandler(
					xmlHttp,
						-1,
					'POST',
					url,
					'',
					function(loader)
					{
						if(loader.responseXML!=null && loader.responseText != null && loader.responseText.length>0) {
							var content_s = loader.responseText;
							var full_s = loader.getAllResponseHeaders() + "\n\n" + content_s;
							content.innerText = content_s;

							alert(full_s);

						}
						else {

							alert('Zero response arrived: '+errmsg);

						}
					},

					function(errmsg)
					{
						alert('Internal error on navigating: '+errmsg);
						
					}
				);
			}
			
		</script>
	</head>
	<body>
		<table class="x8">
		<tr>
			<td>
				Request encoding
				<select id="req_enc_obj">
					<option value="UTF-8">UTF-8</option>
					<option value="WINDOWS-1251">WINDOWS-1251</option>
				</select>
			</td>
		</tr>

		<tr>
			<td>
				XML encoding
				<select id="xml_enc_obj">
					<option value="UTF-8">UTF-8</option>
					<option value="WINDOWS-1251">WINDOWS-1251</option>
				</select>
			</td>
		</tr>



		<tr><td><input id="as_xml_obj" type="checkbox">As XML</input></td></tr>
		<tr><td><a href="#" onclick="on_get_data()">Request</a></td></tr>
		
		<tr><td><textarea id="content" style="width:400px;height=400px"></textarea></td></tr>
		</table>
	</body>
</html>

<%
}
%>