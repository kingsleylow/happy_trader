/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller.bind;

import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.userlist.service.AccountService;
import java.beans.PropertyEditorSupport;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author Administrator
 */
@Component
public class ServerEditorSupport extends PropertyEditorSupport{
	
	@Autowired
	private AccountService accountService_m;

	@Override
	public void setAsText(String text) throws IllegalArgumentException {
		setValue(null);

		int severId = Integer.valueOf(text);
		if (severId >= 0) {
			Server server = accountService_m.resolveServer(severId);
			if (server != null) {
				setValue(server);
			}

		}

	}
}
