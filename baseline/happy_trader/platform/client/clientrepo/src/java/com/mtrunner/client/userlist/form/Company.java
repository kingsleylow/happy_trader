/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.form;

import java.util.Date;
import java.util.List;
import java.util.Set;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.UniqueConstraint;
import org.hibernate.annotations.Type;

@Entity
@Table(name = "COMPANIES", 
	uniqueConstraints = {
	@UniqueConstraint(columnNames = {"COMPANY_NAME"})}
)
public class Company {

	@Id
	@Column(name = "ID")
	@GeneratedValue
	private Integer id;

	@OneToMany(mappedBy = "company")
	private List<Account> accounts;

	@OneToMany(mappedBy = "company")
	private List<Server> servers;

	@Column(name = "COMPANY_NAME")
	private String CompanyName;

	@Column(name = "CREATED")
	@Type(type = "double")
	private double created;

	@Column(name = "LAST_UPDATED")
	@Type(type = "double")
	private double last_upadated;

	@Column(name = "ENABLED")
	@Type(type = "integer")
	private int enabled;

	
	public Integer getId() {
		return id;
	}

	
	public void setId(Integer id) {
		this.id = id;
	}

	
	public List<Account> getAccounts() {
		return accounts;
	}

	public void setAccounts(List<Account> accounts) {
		this.accounts = accounts;
	}

	public List<Server> getServers() {
		return servers;
	}

	
	public void setServers(List<Server> servers) {
		this.servers = servers;
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

	
	public String getCompanyName() {
		return CompanyName;
	}

	
	public void setCompanyName(String CompanyName) {
		this.CompanyName = CompanyName;
	}

	
	public int isEnabled() {
		return enabled;
	}

	
	public void setEnabled(int enabled) {
		this.enabled = enabled;
	}

}
