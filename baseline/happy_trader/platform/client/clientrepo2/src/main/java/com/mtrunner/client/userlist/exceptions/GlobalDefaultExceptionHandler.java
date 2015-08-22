/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.userlist.exceptions;

import com.mtrunner.client.userlist.controller.ServerControler;
import com.mtrunner.client.utils.GeneralUtils;
import javax.servlet.http.HttpServletRequest;
import org.apache.log4j.Logger;
import org.springframework.core.annotation.AnnotationUtils;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.servlet.config.annotation.EnableWebMvc;
import org.springframework.web.servlet.ModelAndView;  

/**
 *
 * @author Administrator
 */
@EnableWebMvc
@ControllerAdvice
public class GlobalDefaultExceptionHandler {

		static Logger log_m = Logger.getLogger(GlobalDefaultExceptionHandler.class.getName());

		
    @ExceptionHandler(Exception.class)
    public ModelAndView defaultErrorHandler(HttpServletRequest req, Exception e) throws Exception {
			
				log_m.info("-------------------------------------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
				log_m.error("Request: " + req.getRequestURL() + " raised " + e.toString());

			
        // If the exception is annotated with @ResponseStatus rethrow it and let
        // the framework handle it - like the OrderNotFoundException example
        // at the start of this post.
        // AnnotationUtils is a Spring Framework utility class.
        if (AnnotationUtils.findAnnotation(e.getClass(), ResponseStatus.class) != null)
            throw e;

        // Otherwise setup and send the user to a default error-view.
        ModelAndView mav = new ModelAndView();
        //mav.addObject("exception", e);
				mav.addObject("error_string", GeneralUtils.prepareValidHtml(GeneralUtils.getFullErrorString(e)));
								
        mav.addObject("url", req.getRequestURL());
        mav.setViewName("error");
        return mav;
    }
}
