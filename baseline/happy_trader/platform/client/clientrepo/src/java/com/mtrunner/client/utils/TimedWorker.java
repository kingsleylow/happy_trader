/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.mtrunner.client.utils;

import java.text.SimpleDateFormat;
import java.util.Date;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;


/**
 *
 * @author Administrator
 */
@EnableScheduling
public class TimedWorker  {

	private static final SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");

    @Scheduled(fixedRate = 5000)
    public void reportCurrentTime() {
         //System.out.println("The time is now " + dateFormat.format(new Date()));
    }
	
}
