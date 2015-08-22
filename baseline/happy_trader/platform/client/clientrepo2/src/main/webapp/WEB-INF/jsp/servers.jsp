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

        <title>Servers</title>
        
        <!-- JQUERY -->
        <ct:jqincl base="server" up="true"></ct:jqincl>
         
        
        <!-- OTHER-->
        <script type="text/javascript" src="<ct:basepath base="server" />../resources/etc/css/default.css"></script>

        <style type="text/css">.hidden {display: none;}</style>
        <script type="text/javascript">
            $(function() {



            });
        </script>

    </head>
    <body>
        <base href="<ct:basepath base="server" />" />

    <!--
    <c:if  test="${!empty serverObj}">
        "foo:" ${serverObj.getDemo()}
    </c:if>
    -->

    <form:form method="post" action="add_server.html" commandName="server">
        <c:if test="${!empty serverObj}">
                <input type="hidden" name="id" value="${serverObj.getId()}" />
                <input type="hidden" name="created" value="${serverObj.getCreated().getTime()}" />
        </c:if>
        
        <table>
            <tr>
                <td><form:label path="server_name">Server Name</form:label></td>
                <td><form:input path="server_name" value="${serverObj.getServer_name()}"/></td>
                
                <!--
                <td><form:label path="demo">Is Demo</form:label></td>
                <td><form:checkbox path="demo" value="${serverObj.getDemo()}" ></form:checkbox></td>
                -->
                
                <td><form:label path="demo">Is Demo</form:label></td>
                <td><form:select path="demo">
                    <c:choose>
                                <c:when test="${!empty serverObj && serverObj.getDemo()}">
                                    <option value="true" selected="true">Yes</option>
                                    <option value="false">No</option>
                                </c:when>
                                <c:otherwise>
                                    <option value="true">Yes</option>
                                    <option value="false" selected="true">No</option>
                                </c:otherwise>
                    </c:choose> 
                </form:select>
                </td>                    
                                    
                <td><form:label path="company">Companies</form:label></td>
                <td>
                <form:select path="company">
                    <c:forEach items="${companies_list}" var="company_i">
                        <c:choose>
                                <c:when test="${!empty companies_list && company_i.getId() == serverObj.getCompany().getId()}">
                                    <option value="${company_i.getId()}" selected="true">${company_i.getCompanyName()}</option>
                                </c:when>
                                <c:otherwise>
                                    <option value="${company_i.getId()}">${company_i.getCompanyName()}</option>
                                </c:otherwise>
                        </c:choose> 
                    </c:forEach>
                </form:select>
                </td>
                
            </tr>

            <tr>
                <td colspan="2">
                    <input type="submit" value="Add new server" name="add" />
                    <input type="submit" value="Edit server" name="update"/>
                </td>
            </tr>

        </table>  
    </form:form>

<h3>Servers</h3>

<c:if  test="${!empty servers_list}">
    <table class="data">
        <tr>
            <th>Server Name</th>
            <th>Is Demo</th>
            <th>Company Name</th>
            <th>Creation Date</th>
            <th>Update Date</th>
            <th>Enabled</th>
            <th>&nbsp;</th>
            <th>&nbsp;</th>
        </tr>
        <c:forEach items="${servers_list}" var="server_i">
            <tr>
                <td>${server_i.getServer_name()}</td>
                <td>
                    <c:choose>
                        <c:when test="${server_i.getDemo()}">
                            Yes
                        </c:when>
                        <c:otherwise>
                            No
                        </c:otherwise>
                    </c:choose>

                </td>
                <td>${server_i.getCompany().getCompanyName()}</td>

                <td><fmt:formatDate value="${server_i.getCreated()}"  pattern="dd/MM/yyyy H:m" /></td>
                <td><fmt:formatDate value="${server_i.getLastUpdated()}"  pattern="dd/MM/yyyy H:m" /></td>
                <td>
                    <c:choose>
                        <c:when test="${server_i.getEnabled() == 1}">
                            Yes
                        </c:when>
                        <c:otherwise>
                            No
                        </c:otherwise>
                    </c:choose>
                </td>
                <td>
                    <c:choose>
                        <c:when test="${server_i.getEnabled() == 1}">
                            <a href="disable_server/${server_i.getId()}/do.html">disable</a>
                        </c:when>
                        <c:otherwise>
                            <a href="restore_server/${server_i.getId()}/do.html">enable</a>
                        </c:otherwise>   
                    </c:choose>
                </td>
                <td>
                    <c:if  test="${server_i.getEnabled() == 1}">
                        <a href="edit_server/${server_i.getId()}/do.html">Edit</a>
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