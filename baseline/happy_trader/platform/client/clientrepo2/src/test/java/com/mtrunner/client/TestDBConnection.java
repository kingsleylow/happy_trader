/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client;

import com.mtrunner.client.utils.GeneralUtils;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import static org.junit.Assert.assertNotNull;

import java.io.InputStream;
import java.util.Properties;

import org.jboss.arquillian.container.test.api.Deployment;
import org.jboss.arquillian.container.test.api.OperateOnDeployment;
import org.jboss.arquillian.container.test.api.TargetsContainer;
import org.jboss.arquillian.junit.Arquillian;
import org.jboss.shrinkwrap.api.ShrinkWrap;
import org.jboss.shrinkwrap.api.asset.StringAsset;
import org.jboss.shrinkwrap.api.spec.WebArchive;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 *
 * @author Administrator
 */
@RunWith(Arquillian.class)
public class TestDBConnection {

	private static final String WEBINF_SRC = "src/main/webapp/WEB-INF";
	final Properties jdbc_prop = new Properties();

	@Deployment(name = "deployment_default")
	public static WebArchive createDeployment() throws IOException {
		WebArchive archive = ShrinkWrap.create(WebArchive.class)
						.addClass(GeneralUtils.class);
		return archive;
	}

	@Before
	public void initialize() throws IOException 
	{
		File[] files = new File(WEBINF_SRC).listFiles();
		for (File f : files) {
			if (f.getAbsolutePath().indexOf("jdbc.properties") != -1) {
				jdbc_prop.load(new FileReader(f));
			}
		}
		
		// read property file
		System.out.println("jdbc.properties: ---------------------------------");

		System.out.println(jdbc_prop.toString());
		assertTrue("jdbc.properties cannot be empty", jdbc_prop.size() > 0);
	}

	@Test
	@OperateOnDeployment("deployment_default")
	public void testConnectivity() throws Exception {

		
		System.out.println("Connectivity test -----------------------------------");
		
		checkDBConnect(
						jdbc_prop.getProperty("jdbc.databaseurl"),
						jdbc_prop.getProperty("jdbc.username"),
						jdbc_prop.getProperty("jdbc.password")
		);

		checkDBConnect(
						jdbc_prop.getProperty("jdbc.databaseurljoomla"),
						jdbc_prop.getProperty("jdbc.usernamejoomla"),
						jdbc_prop.getProperty("jdbc.passwordjoomla")
		);
	}

	@Test
	@OperateOnDeployment("deployment_default")
	public void testHtDbUsers() throws Exception {
					// test clientrepo users
					System.out.println("Users HT DB test-----------------------------------");
					
				
					
					String sql = "SELECT user_name, firstname, lastname FROM client_store.users where enabled=1";
					
					PrintColumns.showSQL(sql, 
									jdbc_prop.getProperty("jdbc.databaseurljoomla"), 
									jdbc_prop.getProperty("jdbc.usernamejoomla"), 
									jdbc_prop.getProperty("jdbc.passwordjoomla")
					);

	}
	
	@Test
	@OperateOnDeployment("deployment_default")
	public void testJoomlaUsers() throws Exception {
					// test clientrepo users
					System.out.println("Logged Joomla users-----------------------------------");
					
					String tprfx = jdbc_prop.getProperty("jdbc.databasejoomla.table.prefix");
					System.out.println("Joomla DB tables prefix: " + tprfx);
					
					String sql = "SELECT u.username, u.name, u.email, u.id, UNIX_TIMESTAMP()-s.time \"expiration\" FROM "+tprfx+"_users u, " +
							tprfx + "_session s " + 
							"where s.userid=u.id and u.block=0 and s.guest=0";
					
					PrintColumns.showSQL(sql, 
									jdbc_prop.getProperty("jdbc.databaseurljoomla"), 
									jdbc_prop.getProperty("jdbc.usernamejoomla"), 
									jdbc_prop.getProperty("jdbc.passwordjoomla")
					);


	}

	private void checkDBConnect(String url, String user, String password) throws Exception {
		try {
			System.out.println("Checking DB: " + url + " - " + user);

			System.out.println("Available schemas:");
			String sql = "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA";
			PrintColumns.showSQL(sql, url, user, password);

			System.out.println("-------------------------------------------");
		} catch (Exception e) {
			throw new RuntimeException(e);
		} finally {

		}
	}

}
