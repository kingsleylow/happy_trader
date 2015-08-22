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

        <title>Users</title>

          <!-- JQUERY -->
         <ct:jqincl base="user" up="true"></ct:jqincl>

        <style type="text/css">.hidden {display: none;}</style>
        <script type="text/javascript">
            $(function() {



            });
        </script>

    </head>
    <body>
           <base href="<ct:basepath base="user" />" />
        
        <!--
        -->
      
        <form:form method="post" action="add_user.html" commandName="user">
            <c:if test="${!empty userObj}">
                <input type="hidden" name="id" value="${userObj.getId()}" />
                <input type="hidden" name="created" value="${userObj.getCreated().getTime()}" />
            </c:if>
            
            <input type="hidden" name="id" value="${userObj.getId()}" />
            <table>
                <tr>
                    <td><form:label path="UserName">User ID</form:label></td>
                    <td><form:input path="UserName" value="${userObj.getUserName()}"/></td>

                    <td><form:label path="FirstName">First name</form:label></td>
                    <td><form:input path="FirstName" value="${userObj.getFirstName()}"/></td>

                    <td><form:label path="LastName">Second name</form:label></td>
                    <td><form:input path="LastName" value="${userObj.getLastName()}"/></td>  

                    <td><form:label path="telephone">Phone</form:label></td>
                    <td><form:input path="telephone" value="${userObj.getTelephone()}"/></td> 

                    <td><form:label path="email">Email</form:label></td>
                    <td><form:input path="email" value="${userObj.getEmail()}"/></td> 

                   
                </tr>

                <tr>
                    <td colspan="2">
                        <input type="submit" value="Add new User" name="add" />
                        <input type="submit" value="Edit user" name="update"/>
                    </td>
                </tr>

            </table>  
        </form:form>

        <h3>Users</h3>

        <c:if  test="${!empty users_list}">
            <table class="data">
                <tr>
                    <th>User name</th>
                    <th>First Name</th>
                    <th>Second Name</th>
                    <th>Phone</th>
                    <th>Email</th>
                    <th>Enabled</th>
                    <th>Creation Date</th>
                    <th>Last Update Date</th>
                    <th>&nbsp;</th>
                    <th>&nbsp;</th>
                </tr>
                <c:forEach items="${users_list}" var="user_i">
                    <tr>
                        <td>${user_i.getUserName()}</td>
                        <td>${user_i.getFirstName()}</td>
                        <td>${user_i.getLastName()}</td>
                        <td>${user_i.getTelephone()}</td>
                        <td>${user_i.getEmail()}</td>
                        
                        <td>
                            <c:choose>
                                <c:when test="${user_i.isEnabled()}">
                                    Yes
                                </c:when>
                                <c:otherwise>
                                    No
                                </c:otherwise>
                            </c:choose>
                        </td>
                        
                        <td><fmt:formatDate value="${user_i.getCreated()}"  pattern="dd/MM/yyyy H:m" /></td>
                        <td><fmt:formatDate value="${user_i.getLastUpdated()}"  pattern="dd/MM/yyyy H:m" /></td>
                        
                        <td>
                            <c:choose>
                                <c:when test="${user_i.isEnabled()}">
                                    <a href="disable_user/${user_i.getId()}/do.html">disable</a>
                                </c:when>
                                <c:otherwise>
                                     <a href="restore_user/${user_i.getId()}/do.html">enable</a>
                                </c:otherwise>   
                            </c:choose>
                        </td>
                        
                        <td>
                            <c:if  test="${user_i.isEnabled()}">
                                <a href="edit_user/${user_i.getId()}/do.html">Edit</a>
                                <a href="../accdt/bridge/${user_i.getUserName()}/do.html">Go to details</a>
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