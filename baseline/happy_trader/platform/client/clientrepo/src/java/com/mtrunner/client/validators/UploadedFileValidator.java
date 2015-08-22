/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.validators;


import java.io.File;
import org.springframework.stereotype.Controller;
import org.springframework.stereotype.Service;
import org.springframework.validation.Errors;
import org.springframework.validation.Validator;

/**
 *
 * @author Administrator
 */
@Service
public class UploadedFileValidator implements Validator {
	public boolean supports(Class<?> paramClass) {
		//return UploadedFile.class.equals(paramClass);
		return true;
	}

	public void validate(Object obj, Errors errors) {
		/*
		UploadedFile file = (UploadedFile) obj;
		  if (file.getFile().getSize() == 0) {
		   errors.rejectValue("file", "valid.file");
		  }
		*/
	}
}
