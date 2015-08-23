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

        <title>User accounts</title>

      
          <!-- JQUERY -->
         <ct:jqincl base="account" up="true"></ct:jqincl>
        
        <style type="text/css">
                    .hidden {display: none;}
         </style>
         
        <script type="text/javascript">
            $(function() {

                $( "#company" ).change(function() {
                    // 
                    var id =  $(this).find('option:selected').attr('value');
                    //alert( "Handler for .change() called: " +id );
                    <c:choose>
                        <c:when test="${!empty accountObj}">
                            window.location.href = 'user_accounts/'+id+'/${accountObj.getId()}/do.html';

                        </c:when>
                        <c:otherwise>
                              window.location.href = 'user_accounts/'+id+'/do.html';
                        </c:otherwise>
                    </c:choose>
                    // need to 
                    
                    
                 });

            });
        </script>

    </head>
    <body>
       <base href="<ct:basepath base="account" />" />

    <!--
        // for company navigation we use that one only ${companyCurrentId}
    -->
    
    <form:form method="post" action="add_account.html" modelAttribute="account">
        <c:if test="${!empty accountObj}">
            <input type="hidden" name="id" value="${accountObj.getId()}" />
            <input type="hidden" name="created" value="${accountObj.getCreated().getTime()}" />
        </c:if>
            
        <table>
            <tr>

                <td><form:label path="account_id">Account ID</form:label></td>
                <td><form:input path="account_id" value="${accountObj.getAccount_id()}"/></td>

                <td><form:label path="password">Password</form:label></td>
                <td><form:input path="password" value="${accountObj.getPassword()}"/></td>

                <td><form:label path="invest_password">Investor password</form:label></td>
                <td><form:input path="invest_password" value="${accountObj.getInvest_password()}"/></td>

                <td><form:label path="user">Users</form:label></td>
                <td>
                    <form:select path="user">
                        <c:forEach items="${users_list}" var="user_i">
                            <c:if test="${user_i.isEnabled()}">
                                <c:choose>
                                    
                                        <c:when test="${!empty accountObj && user_i.getId() == accountObj.getUser().getId()}">
                                            <option value="${user_i.getId()}" selected="true">${user_i.getUserName()}</option>
                                        </c:when>
                                        <c:otherwise>
                                            <option value="${user_i.getId()}">${user_i.getUserName()}</option>
                                        </c:otherwise>
                                    
                                </c:choose>
                            </c:if>
                        </c:forEach>
                    </form:select>
                </td>
                
                
                
                <td><form:label path="server">Servers</form:label></td>
                <td>
                    <form:select path="server">
                        <c:forEach items="${servers_list}" var="server_i">
                            <c:choose>
                                <c:when test="${!empty accountObj && server_i.getId() == accountObj.getServer().getId()}">
                                    <option value="${server_i.getId()}" selected="true">${server_i.getServer_name()}</option>
                                </c:when>
                                <c:otherwise>
                                    <option value="${server_i.getId()}">${server_i.getServer_name()}</option>
                                </c:otherwise>
                            </c:choose> 
                        </c:forEach>
                    </form:select>
                </td>
                

        <td><form:label path="company">Companies</form:label></td>
        <td>
            <form:select path="company">
                <c:forEach items="${companies_list}" var="company_i">
                    <c:choose>
                        <c:when test="${!empty companies_list && !empty companyCurrentId && company_i.getId() == companyCurrentId}">
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
        <input type="submit" value="Add new Account" name="add"/>
        <input type="submit" value="Edit account" name="update"/>
        <input type="submit" value="Remove disabled accounts" name="remove"/>
    </td>
</tr>

</table>  
</form:form>

<h3>Accounts</h3>

<c:if  test="${!empty accountsList}">
    <table class="data">
        <tr>
            <th>First Name</th>
            <th>Second Name</th>
            <th>Phone</th>
            <th>Email</th>
            <th>Company name</th>
            <th>Account ID</th>
            <th>Trading Server</th>
            <th>Created</th>
            <th>Updated</th>
            <th>Enabled</th>
            <th>Go to details</th>
            <th>&nbsp;</th>
            <th>&nbsp;</th>
        </tr>
        <c:forEach items="${accountsList}" var="acc_i">
            <c:set var="entryEnabled" value="${acc_i.getUser().isEnabled() && acc_i.getCompany().isEnabled()==1 && acc_i.getServer().getEnabled()==1}"></c:set>
            <tr>
                <td>${acc_i.getUser().getFirstName()} </td>
                <td>${acc_i.getUser().getLastName()}</td>
                <td>${acc_i.getUser().getTelephone()}</td>
                <td>${acc_i.getUser().getEmail()}</td>
                <td>${acc_i.getCompany().getCompanyName()}</td>
                <td>${acc_i.getAccount_id()}</td>
                <td>${acc_i.getServer().getServer_name()}</td>
                <td><fmt:formatDate value="${acc_i.getCreated()}"  pattern="dd/MM/yyyy H:m" /></td>
                <td><fmt:formatDate value="${acc_i.getLastUpdated()}"  pattern="dd/MM/yyyy H:m" /></td>
                <td>
                    <c:choose>
                        <c:when test="${acc_i.isEnabled() == 1}">
                            Yes
                        </c:when>
                        <c:otherwise>
                            No
                        </c:otherwise>
                    </c:choose>
                </td>
                <c:choose>
                    <c:when test="${entryEnabled}">
                        <td>
                            <c:choose>
                                <c:when test="${acc_i.isEnabled() == 1}">
                                    <a href="disable_user_account/${acc_i.getId()}/do.html">disable</a>
                                </c:when>
                                <c:otherwise>
                                    <a href="restore_user_account/${acc_i.getId()}/do.html">enable</a>
                                </c:otherwise>   
                            </c:choose>
                        </td>
                        
                        <c:if  test="${acc_i.isEnabled() == 1}">
                            <td>
                                <a href="edit_account/${acc_i.getId()}/do.html">Edit</a>
                            </td>  
                            <td>
                                <a href="../accdt/account_details/${acc_i.getUser().getUserName()}/${acc_i.getId()}/do.html">Go to details</a>
                            </td> 
                        </c:if>
                        
                    </c:when>
                    <c:otherwise>
                        <td>
                            user or company disabled or server disabled
                        </td>
                        <td>

                        </td>    
                    </c:otherwise>
                </c:choose>

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