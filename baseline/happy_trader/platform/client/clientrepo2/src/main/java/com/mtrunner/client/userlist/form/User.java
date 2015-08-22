/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.form;

import java.sql.Timestamp;
import java.util.Date;
import java.util.List;
import java.util.Set;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.UniqueConstraint;
import org.hibernate.annotations.Type;

@Entity
@Table(name = "USERS", uniqueConstraints = {
	@UniqueConstraint(columnNames = {"USER_NAME"})}
)
public class User {

	@Id
	@Column(name = "ID")
	@GeneratedValue
	private Integer id;

	@Column(name = "USER_NAME")
	private String UserName;

	@Column(name = "FIRSTNAME")
	private String FirstName;

	@Column(name = "LASTNAME")
	private String LastName;

	@Column(name = "TELEPHONE")
	private String telephone;

	@Column(name = "EMAIL")
	private String email;

	@OneToMany(mappedBy = "user")
	private List<Account> accounts;

	@Column(name = "CREATED")
	@Type(type = "double")
	private double created;

	@Column(name = "LAST_UPDATED")
	@Type(type = "double")
	private double last_upadated;

	@Column(name = "ENABLED", columnDefinition = "TINYINT")
	@Type(type = "org.hibernate.type.NumericBooleanType")
	private boolean enabled;

	
	public Integer getId() {
		return id;
	}

	
	public void setId(Integer id) {
		this.id = id;
	}

	
	public String getUserName() {
		return UserName;
	}


	public void setUserName(String user_name) {
		this.UserName = user_name;
	}


	public String getFirstName() {
		return FirstName;
	}

	
	public void setFirstName(String first_name) {
		this.FirstName = first_name;
	}


	public String getLastName() {
		return LastName;
	}


	public void setLastName(String last_name) {
		this.LastName = last_name;
	}

	public String getTelephone() {
		return telephone;
	}

	
	public void setTelephone(String telephone) {
		this.telephone = telephone;
	}

	
	public String getEmail() {
		return email;
	}

	
	public void setEmail(String email) {
		this.email = email;
	}

	
	public List<Account> getAccounts() {
		return accounts;
	}

	
	public void setAccounts(List<Account> accounts) {
		this.accounts = accounts;
	}

	public Date getCreated() {
		return new Date((long) created);
	}


	public void setCreated(Date created) {
		this.created = created.getTime();
	}

	
	public Date getLastUpdated() {
		return new Date((long) last_upadated);
	}

	
	public void setLastUpdated(Date last_upadated) {
		this.last_upadated = last_upadated.getTime();
	}

	
	public boolean isEnabled() {
		return enabled;
	}

	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}

}
