<%@ page contentType="text/html;charset=windows-1251"%>
<%
response.setHeader("Pragma", "No-cache");
response.setHeader("Cache-Control", "No-cache");
response.setDateHeader("Expires", 1);
%>

<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
		<meta http-equiv="Cache-Control" content="No-cache">
		<link href="css/console.css" rel="stylesheet" type="text/css"/>


		<!--
<script type="text/javascript" src="js/misc/helper.js"></script> 
--> 

		<script type="text/javascript">
		

			function show_separate_window(url)
			{
				window.open(url, '_blank');
			}

			function show_main_frame(url)
			{
				var frm = document.getElementById("basefrm");
				frm.contentWindow.navigate(url);
			}

			function on_base_load()
			{

				// get basefrm title
				var aTitles = basefrm.tltObj;



				var tName = aTitles.innerText;
				if (tName != null && tName.length > 0) {
					treeframe.printHint(tName);
				}
			}

		</script>

		<title>Happy Trader Management Console</title>
	</head>


	<!--	<frameset cols="*" id="fsTop"></frameset> -->
		<!--	<frameset rows="77,*" bordercolor="#f5fafa" framespacing="0" frameborder='1'> -->
	<frameset onload="on_base_load();" rows="77,*" bordercolor="#65a7cb" border="1" framespacing="2" frameborder='1'>
		<frame name="frmTop" src="htHeader.jsp"
				 target="rbottom" scrolling="no"
				 marginwidth="5" marginheight="0" noresize="noresize"
				 frameborder="0" />
		<frameset cols="*,80%" id="treeshower"
						border="1" bordercolor="#65a7cb" framespacing="2" frameborder="1">
			<frame name="treeframe" src="htCommands.jsp" marginwidth="0" marginheight="0" bordercolor="#65a7cb" frameborder="0" />
			<frame id="basefrm" name="basefrm" src="htWelcome.jsp" bordercolor="#65a7cb" marginwidth="7" marginheight="0" frameborder="0" scrolling="auto" />
		</frameset>
	</frameset>

	<body>
	</body>

</html>