/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller.bind;

import java.beans.PropertyEditorSupport;
import java.util.Date;
import org.springframework.stereotype.Component;

/**
 *
 * @author Administrator
 */
@Component
public class GeneralDateEditorSupport extends PropertyEditorSupport {
	@Override
	public void setAsText(String text) throws IllegalArgumentException {
		long date_l = Long.parseLong(text);
		if (date_l>=0)
			setValue(new Date(date_l));
		else
			setValue(new Date(0));
	
	}
}
