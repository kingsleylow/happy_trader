/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.auth.provider;


import com.mtrunner.client.auth.UserService;
import com.mtrunner.client.auth.filters.AdminUsernamePasswordAuthenticationFilter;
import com.mtrunner.client.userlist.authentities.UserRole;
import com.mtrunner.client.userlist.authentities.UserToAuthenticate;
import java.security.Principal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import org.apache.log4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationProvider;
import org.springframework.security.authentication.AuthenticationServiceException;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.authentication.dao.AbstractUserDetailsAuthenticationProvider;
import org.springframework.security.authentication.dao.DaoAuthenticationProvider;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Component;

/**
 *
 * @author Administrator
 */
@Component("customAuthenticationProvider")
public class CustomAuthenticationProvider implements AuthenticationProvider {

	static Logger log_m = Logger.getLogger(CustomAuthenticationProvider.class.getName());
	
	@Autowired
	private UserService userService_m;

	@Override
	public Authentication authenticate(Authentication authentication) throws AuthenticationException {
		
		
		String username = authentication.getName();
		log_m.debug("Tryinng to check if Joomla DB has a user: " + username);
		
		// password is ignored
		String password = (String) authentication.getCredentials();
		UserDetails user = userService_m.loadUserByUsername(username);
		
		log_m.debug("User is resolved: " + ( user != null ? user.getUsername() : "N/A"));
	
		
		if (user == null) {
			throw new BadCredentialsException("Username is invalid");
		}

		Collection<? extends GrantedAuthority> authorities = user.getAuthorities();

		return new UsernamePasswordAuthenticationToken(user, password, authorities);
	}

	@Override
	public boolean supports(Class<?> arg0) {
		return true;
	}

}

