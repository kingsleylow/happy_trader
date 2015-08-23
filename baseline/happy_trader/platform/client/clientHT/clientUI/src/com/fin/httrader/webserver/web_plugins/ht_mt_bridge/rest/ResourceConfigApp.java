/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.rest;

import java.util.HashSet;
import java.util.Set;
import javax.ws.rs.ApplicationPath;
import javax.ws.rs.core.Application;
import org.glassfish.jersey.filter.LoggingFilter;
import org.glassfish.jersey.server.ResourceConfig;
import org.glassfish.jersey.server.ServerProperties;

/**
 *
 * @author Administrator
 */
@ApplicationPath("/")
public class ResourceConfigApp extends ResourceConfig {

    public ResourceConfigApp() {
        // Register resources and providers using package-scanning.
        packages("com.fin.httrader.webserver.web_plugins.ht_mt_bridge.rest");

        // Register my custom provider - not needed if it's in my.package.
        //register(SecurityRequestFilter.class);
        // Register an instance of LoggingFilter.
        //register(new LoggingFilter(LOGGER, true));

        // Enable Tracing support.
        //property(ServerProperties.TRACING, "ALL");
    }
}
