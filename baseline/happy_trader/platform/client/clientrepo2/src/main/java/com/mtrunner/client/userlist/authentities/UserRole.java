/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.authentities;

import java.util.List;
import org.springframework.security.core.GrantedAuthority;
/**
 *
 * @author Administrator
 */
public class UserRole implements GrantedAuthority {
	private static final long serialVersionUID = 1L;
    private String name;
 
    private List<UserPrivilege> privileges;
 
    public String getName() {
        return name;
    }
 
    public void setName(String name) {
        this.name = name;
    }
 
    @Override
    public String getAuthority() {
        return this.name;
    }
 
    public List<UserPrivilege> getPrivileges() {
        return privileges;
    }
 
    public void setPrivileges(List<UserPrivilege> privileges) {
        this.privileges = privileges;
    }
 
    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("Role [name=");
        builder.append(name);
        builder.append(", privileges=");
        builder.append(privileges);
        builder.append("]");
        return builder.toString();
    }
}
