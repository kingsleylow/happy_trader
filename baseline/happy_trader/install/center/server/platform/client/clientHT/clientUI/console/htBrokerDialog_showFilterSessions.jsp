<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">

		<title id='tltObj' >Show Sessions As Filtering Criteria</title>

		<link href="css/console.css" rel="stylesheet" type="text/css"/>

		

		<!-- XTREE -->
		
		<link rel="STYLESHEET" type="text/css" href="css/xtree/dhtmlxtree.css">

		<script  src="js/xtree/dhtmlxcommon.js"></script>

		<script  src="js/xtree/dhtmlxtree.js"></script>

		<script  src="js/xtree/dhtmlXTree_start.js"></script>

		<!-- misc functions -->
		<script type="text/javascript" src="js/misc/helper.js"></script>
	

		<script>
			function on_load()
			{

				var xmlhttp = getXmlHttp();
				xmlhttp.open('GET', '/HtReturnSessionVar_v2.jsp?cookie=<%=request.getParameter("cookie") %>&session_var=session_runid_ul&response_type=text', false);
				xmlhttp.send(null);
				if(xmlhttp.status == 200) {
						xtreeObj.innerHTML = xmlhttp.responseText;
				}
				

			
			}


		</script>
	</head>
	<body onload="on_load()">

		<table width=100% height="100%" class=x8>
			<tr>
				<td align="left">
					<div
						id="xtreeObj"
						style="width:100%;height:100%;border:1px solid black;overflow:hidden"
						class="dhtmlxTree"
						setImagePath="/imgs/xtree/"
					>
					
					</div>
				</td>
			</tr>

			
		</table>

	
	

	</body>
</html>