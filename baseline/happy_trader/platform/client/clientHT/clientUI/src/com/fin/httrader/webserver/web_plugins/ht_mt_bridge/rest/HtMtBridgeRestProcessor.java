/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver.web_plugins.ht_mt_bridge.rest;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

/**
 *
 * @author Administrator
 */
@Path("resthtmt")
public class HtMtBridgeRestProcessor {
		@GET
		@Path("test/{param}")
    @Produces(MediaType.TEXT_PLAIN)
    public Response  test(@PathParam("param") String msg) {
       String output = "Jersey say : " + msg;
 
			return Response.status(200).entity(output).build();
    }
}
