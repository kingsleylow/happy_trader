/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.mtstruct_db;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;






@Entity
@Table(name="company_metainfo") 
public class HtMtCompanyMetainfo {
		
		private long companyMetanfoUid = -1; // PK
		private String companyName; 

		@Id
		@GeneratedValue(strategy=GenerationType.IDENTITY)
		@Column(name="id")
		public long getCompanyMetanfoUid() {
				return companyMetanfoUid;
		}

		
		public void setCompanyMetanfoUid(long companyMetanfoUid) {
				this.companyMetanfoUid = companyMetanfoUid;
		}

		
		@Column(name="company_name")
		public String getCompanyName() {
				return companyName;
		}

		public void setCompanyName(String companyName) {
				this.companyName = companyName;
		}
		
}
