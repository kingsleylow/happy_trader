/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.authentities;

/**
 *
 * @author Administrator
 */
public class UserPrivilege {
	public String getName() {
        return name;
    }
 
    public void setName(String name) {
        this.name = name;
    }
 
    private String name;
 
    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("Privilege [name=");
        builder.append(name);
        builder.append("]");
        return builder.toString();
    }
}
