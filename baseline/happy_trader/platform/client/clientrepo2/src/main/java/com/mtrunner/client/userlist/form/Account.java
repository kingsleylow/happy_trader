/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.form;

import java.util.Date;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.persistence.UniqueConstraint;
import org.hibernate.annotations.Type;

@Entity
@Table(name = "ACCOUNTS", uniqueConstraints = {
	@UniqueConstraint(columnNames = {"ACCOUNT_ID", "COMPANIES_ID", "USERS_ID", "SERVERS_ID"})})
public class Account {

	@Id
	@Column(name = "ID")
	@GeneratedValue
	private Integer id;

	@Column(name = "ACCOUNT_ID")
	private String account_id;

	@Column(name = "PASSWORD")
	private String password;

	@Column(name = "INVEST_PASSWORD")
	private String invest_password;

	@Column(name = "ENABLED")
	@Type(type = "integer")
	private int enabled;

	@ManyToOne
	@JoinColumn(name = "COMPANIES_ID")
	private Company company;

	@ManyToOne
	@JoinColumn(name = "USERS_ID")
	private User user;

	@ManyToOne
	@JoinColumn(name = "SERVERS_ID")
	private Server server;

	@Column(name = "CREATED")
	@Type(type = "double")
	private double created;

	@Column(name = "LAST_UPDATED")
	@Type(type = "double")
	private double last_upadated;

	
	public Integer getId() {
		return id;
	}

	
	public void setId(Integer id) {
		this.id = id;
	}

	public String getAccount_id() {
		return account_id;
	}


	public void setAccount_id(String account_id) {
		this.account_id = account_id;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}


	public String getInvest_password() {
		return invest_password;
	}

	
	public void setInvest_password(String invest_password) {
		this.invest_password = invest_password;
	}

	public Company getCompany() {
		return company;
	}

	public void setCompany(Company company) {
		this.company = company;
	}

	public Server getServer() {
		return server;
	}

	public void setServer(Server server) {
		this.server = server;
	}

	
	public User getUser() {
		return user;
	}

	
	public void setUser(User user) {
		this.user = user;
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

	
	public int isEnabled() {
		return enabled;
	}

	
	public void setEnabled(int enabled) {
		this.enabled = enabled;
	}

};
