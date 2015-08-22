/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.servlets;

import com.mtrunner.client.userlist.controller.ServerControler;
import java.io.IOException;
import java.io.PrintWriter;

import javax.naming.Context;
import javax.naming.InitialContext;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.log4j.Logger;


/**
 *
 * @author Administrator
 */
//@WebServlet(name = "SendMessageServlet", urlPatterns = "/sendMessage")
public class SendMessageServlet_old extends HttpServlet {

	static Logger log_m = Logger.getLogger(ServerControler.class.getName());

	@Override
	protected void doGet(HttpServletRequest httpServletRequest, HttpServletResponse httpServletResponse) throws ServletException, IOException {
		/*
		 logger.info("doGet() called");
		 String parameter = getTextParameter(httpServletRequest);
		 sendMessage(parameter);
		 writeResponse(httpServletResponse, parameter);
		 }
		 */
		/*
		httpServletResponse.setContentType("text/html;charset=UTF-8");

		log_m.info("doGet() called");
		String data = "data to send";

		try {
			sendMessage(data);

		} catch (Exception e) {
			log_m.error("error happened", e);
			data = e.toString();
		}

		try (PrintWriter out = httpServletResponse.getWriter()) {

			out.println("<!DOCTYPE html>");
			out.println("<html>");
			out.println("<head>");
			out.println("<title>Servlet SendMessageServlet2</title>");
			out.println("</head>");
			out.println("<body>");
			out.println("<h1>Servlet SendMessageServlet2 at " + httpServletRequest.getContextPath() + "</h1>");
			out.println("<div>" + data + "</div>");
			out.println("</body>");
			out.println("</html>");
		}
		*/
	}

	/*
	public void sendMessage(String data) throws Exception {

		InitialContext initCtx = new InitialContext();
    Context envContext = (Context) initCtx.lookup("java:comp/env");
    ConnectionFactory connectionFactory = (ConnectionFactory) envContext.lookup("jms/ConnectionFactory");
    Connection connection = connectionFactory.createConnection();
    Session session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
    Destination destination = session.createTopic("jms/topic/MyTopic");
    MessageProducer producer = session.createProducer(destination);
    TextMessage msg=session.createTextMessage();
    msg.setText(data);
    producer.send(msg);

	}
	*/
}
