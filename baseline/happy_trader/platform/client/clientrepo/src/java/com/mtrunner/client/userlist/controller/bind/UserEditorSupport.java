/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.controller.bind;

import com.mtrunner.client.userlist.form.User;
import com.mtrunner.client.userlist.service.AccountService;
import java.beans.PropertyEditorSupport;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author Administrator
 */
@Component
public final class UserEditorSupport extends PropertyEditorSupport {
	@Autowired
	private AccountService accountService_m;
	
    @Override
    public void setAsText(String text) throws IllegalArgumentException 
    {
				setValue(null);
			
				// reolve user
				int userId = Integer.valueOf(text);
				if (userId >=0) {
					User user = accountService_m.resolveUser(userId);
					if (user != null)
						setValue(user);
					
				}
				
    }
}
