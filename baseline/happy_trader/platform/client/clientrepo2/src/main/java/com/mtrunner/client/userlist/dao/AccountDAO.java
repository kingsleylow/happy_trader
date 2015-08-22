/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.dao;

import com.mtrunner.client.userlist.form.Account;
import java.util.List;
import org.hibernate.SessionFactory;

/**
 *
 * @author Administrator
 */
public interface AccountDAO {
		public SessionFactory getSessionFactory();
		
}
