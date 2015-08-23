/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.form;

import java.util.Date;
import java.util.List;
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


/**
 *
 * @author Administrator
 */
@Entity
@Table(name = "SERVERS", uniqueConstraints = {
	@UniqueConstraint(columnNames = {"SERVER_NAME", "COMPANIES_ID"})})
public class Server {

	@Id
	@Column(name = "ID")
	@GeneratedValue
	private Integer id;

	@Column(name = "SERVER_NAME")
	private String server_name;


	@Column(name = "IS_DEMO", columnDefinition = "TINYINT")
	@Type(type = "org.hibernate.type.NumericBooleanType")
	private boolean demo;

	@ManyToOne
	@JoinColumn(name = "COMPANIES_ID")
	private Company company;

	@OneToMany(mappedBy = "server")
	private List<Account> accounts;

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

	
	public String getServer_name() {
		return server_name;
	}

	public void setServer_name(String server_name) {
		this.server_name = server_name;
	}

	
	public boolean getDemo() {
		return demo;
	}

	
	public void setDemo(boolean demo) {
		this.demo = demo;
	}

	
	public Company getCompany() {
		return company;
	}


	public void setCompany(Company company) {
		this.company = company;
	}

	public void setAccounts(List<Account> accounts) {
		this.accounts = accounts;
	}

	public List<Account> getAccounts() {
		return accounts;
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


	public int getEnabled() {
		return enabled;
	}

	
	public void setEnabled(int enabled) {
		this.enabled = enabled;
	}
}
