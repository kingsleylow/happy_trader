		<%@ page contentType="text/html;charset=windows-1251"%>
<jsp:useBean id="SC" class="com.fin.httrader.webserver.HtWL4RTProviderService" scope="request"/>
<jsp:setProperty name="SC" property="*"/>


<%
String methodName = request.getMethod();
if (methodName.equalsIgnoreCase("GET")) {
    SC.initialize_Get(request,response);
}
%>