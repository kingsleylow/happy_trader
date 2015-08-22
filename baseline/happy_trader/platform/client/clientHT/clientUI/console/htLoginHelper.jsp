<%@ page contentType="text/html;charset=windows-1251"%>
<html>
    <head>
	<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
	<meta http-equiv="Cache-Control" content="No-cache">
	<link href="css/console.css" rel="stylesheet" type="text/css"/>
	<title id='tltObj'>Login</title>
	<script>
	function do_load()
	{
		
	<% 
	   String operation = request.getParameter("operation");
	   if (operation != null && operation.length() > 0) {
	%>
	      window.open('/htLogin.jsp?operation=<%= operation %>', '_top');
	<%
	   } else {
	%>
	      window.open('/htLogin.jsp', '_top');
	<%
	    };
	%>
	}
	

	</script>
    </head>
    <body onload='do_load()'>
    </body>
</html>