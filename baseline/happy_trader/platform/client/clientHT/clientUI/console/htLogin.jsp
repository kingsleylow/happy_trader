<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtLogin" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<% 
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
    SC.initialize_Get(request,response);
} else if (methodName.equalsIgnoreCase("POST")) {
    SC.initialize_Post(request,response);
}
%>

<html>
    <head>
	<meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
	<meta http-equiv="Cache-Control" content="No-cache">
	<link href="css/console.css" rel="stylesheet" type="text/css"/>
	<title id='tltObj'>Login</title>
	<script>
	function do_submit()
	{
	    mForm.action = "/htLogin.jsp?page_uid=<%=SC.getUniquePageId() %>";
	    mForm.user_id.value = userInpObj.value;
	    mForm.pass_id.value = passwordInpObj.value;
	    
	    mForm.submit();
	}
	

	</script>
    </head>
    <body>
	<table width="100%"  border="0" class=x8>
	    <tr>
		<td colspan="2" align=left class="ITRHomeWelcome">
		    <%= SC.getStringSessionValue(SC.getUniquePageId(), request, "hint") %>
		</td>
	    </tr>
	</table>
	
	<table width="100%"  border="0" class=x8>
	    <tr>
		<td height=20px>
		</td> 
		<td>
		</td>   
	    </tr>
	    
	    <tr>
		<td  align=left width=50px>
		    User
		</td>
		<td align=left>
		    <input type=text id="userInpObj"></input>
		</td>
	    </tr>
	    
	    <tr>
		<td  align=left width=50px>
		    Password
		</td>
		<td align=left>
		    <input type=password id="passwordInpObj"></input>
		</td>
	    </tr>
	    
	    <tr>
		<td height=20px>
		</td> 
		<td>
		</td>    
	    </tr>
	    
	    <tr>
		<td  align=left width=50px>
		    
		</td>
		<td align=left>
		    <a href='#' onclick="do_submit();">Login</a>
		</td>
	    </tr>
	</table>
	
	<form method="post" action="" id="mForm" style="display:none">
	    <input type=hidden name="user_id" value=""></input>
	    <input type=hidden name="pass_id" value=""></input>
	</form>
	
    </body>
</html>