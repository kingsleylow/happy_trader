/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller.bind;

import com.mtrunner.client.userlist.form.Company;
import java.beans.PropertyEditorSupport;
import org.springframework.stereotype.Component;


@Component
public class GeneralBooleanEditorSupport extends PropertyEditorSupport{
	@Override
	public void setAsText(String text) throws IllegalArgumentException {
		setValue(false);

		boolean bool_s = Boolean.parseBoolean(text);
		setValue(bool_s);

	}
}
