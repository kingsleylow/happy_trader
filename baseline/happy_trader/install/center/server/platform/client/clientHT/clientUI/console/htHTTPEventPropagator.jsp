<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtHTTPEventPropagator" scope="request"/>
<jsp:setProperty name="SC" property="*"/>

<%
// this is going to propagate all alert data and other XML events to all external clients
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
    SC.initialize_Get(request,response);
}
%>