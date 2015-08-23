<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtHTTPCommandServlet" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
// this provides communication with broker layer :)
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("POST")) {
    SC.initialize_Post(request,response);
}
else if (methodName.equalsIgnoreCase("GET")) {
    SC.initialize_Get(request,response);
}
%>