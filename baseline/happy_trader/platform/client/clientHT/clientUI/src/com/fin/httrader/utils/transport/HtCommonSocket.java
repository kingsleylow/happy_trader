/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.transport;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

/**
 *
 * @author Victor_Zoubok
 * This is base class for encapsulating socket functionality
 * to allow in future make DDE or other intraprocess communications
 */
public abstract class HtCommonSocket {
	
	public abstract InputStream getInputStream() throws IOException;
	
	public abstract OutputStream getOutputStream() throws IOException;
	
	public abstract void close() throws IOException;
					
}
