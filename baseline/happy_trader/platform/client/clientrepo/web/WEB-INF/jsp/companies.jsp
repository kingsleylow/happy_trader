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

        <title>Companies</title>
      
         <!-- JQUERY -->
        <ct:jqincl base="company" up="true"></ct:jqincl>
        
        <!-- OTHER-->
        <script type="text/javascript" src="<ct:basepath base="company" />../resources/etc/css/default.css"></script>
        
        
        <style type="text/css">.hidden {display: none;}</style>
        <script type="text/javascript">
            $(function() {



            });
        </script>

    </head>
    <body>
        <base href="<ct:basepath base="company" />" />
      
      
        <form:form method="post" action="add_company.html" commandName="company">
            <c:if test="${!empty companyObj}">
                <input type="hidden" name="id" value="${companyObj.getId()}" />
                <input type="hidden" name="created" value="${companyObj.getCreated().getTime()}" />
            </c:if>
            
            <table>
                <tr>
                    <td><form:label path="CompanyName">Company Name</form:label></td>
                    <td><form:input path="CompanyName" value="${companyObj.getCompanyName()}"/></td>

                </tr>

                <tr>
                    <td colspan="2">
                        <input type="submit" value="Add new company"name="add" />
                        <input type="submit" value="Edit company" name="update"/>
                    </td>
                </tr>

            </table>  
        </form:form>

        <h3>Companies</h3>

        <c:if  test="${!empty companies_list}">
            <table class="data">
                <tr>
                    <th>Company name</th>
                    <th>Enabled</th>
                    <th>&nbsp;</th>
                    <th>&nbsp;</th>
                </tr>
                <c:forEach items="${companies_list}" var="company_i">
                    <tr>
                        <td>${company_i.getCompanyName()}</td>
                        <td>${company_i.isEnabled()}</td>
                        <td><fmt:formatDate value="${company_i.getCreated()}"  pattern="dd/MM/yyyy H:m" /></td>
                        <td><fmt:formatDate value="${company_i.getLastUpdated()}"  pattern="dd/MM/yyyy H:m" /></td>
                        <td>
                            <c:choose>
                                <c:when test="${company_i.isEnabled() == 1}">
                                    Yes
                                </c:when>
                                <c:otherwise>
                                    No
                                </c:otherwise>
                            </c:choose>
                        </td>
                        <td>
                            <c:choose>
                                <c:when test="${company_i.isEnabled() == 1}">
                                    <a href="disable_company/${company_i.getId()}/do.html">disable</a>
                                </c:when>
                                <c:otherwise>
                                     <a href="restore_company/${company_i.getId()}/do.html">enable</a>
                                </c:otherwise>   
                            </c:choose>
                        </td>
                        <td>
                            <c:if  test="${company_i.isEnabled() == 1}">
                                <a href="edit_company/${company_i.getId()}/do.html">Edit</a>
                            </c:if>
                        </td>    
                    </tr>
                </c:forEach>
            </table>
        </c:if>

        <div>
            &nbsp;
        </div>    
        <a href="../administration/administration.html">Navigate to main page</a>
    </body>
</html>