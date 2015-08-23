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
        <title>Select account</title>
        
        <!-- JQUERY -->
        <ct:jqincl base="accdt" up="true"></ct:jqincl>
    </head>
    <body>
        <base href='<ct:basepath base="accdt" />' />
        
        <div>
            Dear: ${userName}
        </div>    
        
        <c:if  test="${!empty accounts}">
            <table>
                <tr>
                        <td>Company</td>
                        <td>Account</td>
                        <td>Server</td>
                        <td>Go to details</a></td>
                </tr>
            <c:forEach items="${accounts}" var="acc_i">
                <c:set var="entryEnabled" value="${acc_i.getUser().isEnabled() && acc_i.getCompany().isEnabled()==1 && acc_i.getServer().getEnabled()==1}"></c:set>
                    <tr>
                        <td>${acc_i.getCompany().getCompanyName()}</td>
                        <td>${acc_i.getAccount_id()}</td>
                        <td>${acc_i.getServer().getServer_name()}</td>
                        <td><a href="../accdt/account_details/${userName}/${acc_i.getId()}/do.html">Click</a></td>
                    </tr>
            </c:forEach>
            </table>
        </c:if>
        
    </body>
    
    
</html>    