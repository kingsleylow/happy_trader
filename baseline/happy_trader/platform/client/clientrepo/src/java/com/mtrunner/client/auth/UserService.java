/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth;

import com.mtrunner.client.userlist.authentities.UserRole;
import com.mtrunner.client.userlist.authentities.UserToAuthenticate;
import java.util.ArrayList;
import java.util.List;
import org.springframework.beans.factory.access.BeanFactoryLocator;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.access.ContextSingletonBeanFactoryLocator;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Component;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
 

@Component("userService")
public class UserService implements UserDetailsService {
 
    
    @Override
    public UserDetails loadUserByUsername(final String username) throws UsernameNotFoundException {
			
			//BeanFactoryLocator ctxl = ContextSingletonBeanFactoryLocator.getInstance();
			try {
				UserToAuthenticate user= JoomlaDBUtil.getInstance().resolveJoomlaUser(username);
				return user;
			}
			catch(Exception e)
			{
				throw new UsernameNotFoundException(e.getMessage());
			}
			
			
    }
}