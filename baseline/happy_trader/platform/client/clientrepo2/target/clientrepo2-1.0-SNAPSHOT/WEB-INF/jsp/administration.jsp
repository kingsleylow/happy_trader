<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core"%>
<%@ taglib prefix="form" uri="http://www.springframework.org/tags/form"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions"%>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ taglib prefix="fmt" uri="http://java.sun.com/jsp/jstl/fmt" %>
<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ taglib uri="/WEB-INF/customTag.tld" prefix="ct" %>

<html>
    <head>

       <title>Administration</title>
        
       <base href="<ct:basepath base="" />" />
         

         <!-- JQUERY -->
        <ct:jqincl base="" up="false"></ct:jqincl>
        
        <!-- OTHER-->
        <script type="text/javascript" src="<ct:basepath base="" />resources/etc/css/default.css"></script>
        
        <style type="text/css">.hidden {display: none;}</style>
        <script type="text/javascript">
            
            $(function() {



            });
        </script>

    </head>
    <body>
        
        
      
        <h3>Administration</h3>

        <!--
        <c:if test="${pageContext.request.userPrincipal.name != null}">
	   <h2>Welcome : ${pageContext.request.userPrincipal.name}</h2> 
           <br>
           <a href="<ct:basepath base="" />j_spring_security_logout" >Logout</a> 
           <br>
	</c:if>
	    -->
        
        <a href="company/companies.html">Companies</a>
        <br>
        <a href="user/users.html">Users</a>
        <br>
        <a href="account/user_accounts.html">Accounts</a>
        <br>
        <a href="server/servers.html">Servers</a>
        <br>
        <a href="setup/setup.html">Setup</a>
        <br>
        <br>
        
       

    </body>
</html>