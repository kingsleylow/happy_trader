/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.URLUTF8Encoder;
import java.io.File;
import java.io.IOException;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author DanilinS
 * This servlet just push back file content
 */
public class HtReturnFileContent extends HtServletsBase {

	public String getContext() {
		return this.getClass().getSimpleName();
	}

	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {

			boolean is_wrap_to_html = this.getBooleanParameter(req, "wrap_to_html", true);
			String init_fun = this.getStringParameter(req, "init_fun", true);


			DiskFileItemFactory factory = new DiskFileItemFactory();
			factory.setSizeThreshold(1024 * 1024);
			ServletFileUpload upload = new ServletFileUpload(factory);


			List /* FileItem */ items = upload.parseRequest(req);
			Iterator iter = items.iterator();



			while (iter.hasNext()) {
				FileItem item = (FileItem) iter.next();

				String name = item.getFieldName();
				//String contentType = item.getContentType();
				//boolean isInMemory = item.isInMemory();
				//long sizeInBytes = item.getSize();


				if (name.equalsIgnoreCase("package_data")) {

				
					if (is_wrap_to_html) {
						res.getOutputStream().print("<html>");
						res.getOutputStream().print("<head>");

						res.getOutputStream().print("<script type=\"text/javascript\">");
						res.getOutputStream().print("function init() {");
						res.getOutputStream().print("if (top.");
						res.getOutputStream().print(init_fun);
						res.getOutputStream().print( ") top.");
						res.getOutputStream().print( init_fun );
						res.getOutputStream().print( "(); }; ");
						res.getOutputStream().print("window.onload=init");
						res.getOutputStream().print("</script>");


						res.getOutputStream().print("</head>");
						res.getOutputStream().print("<body><textarea>");
					}

					// here we have package data - just copy the content
					HtFileUtils.copyInputStream(item.getInputStream(), res.getOutputStream());

					if (is_wrap_to_html) {
						res.getOutputStream().print("</textarea></body>");
						res.getOutputStream().print("</html>");
					}

					item.getInputStream().close();


				}
			/*
			else if (name.equalsIgnoreCase("package_data_file_path")) {
			String package_data_file_path = item.getString();
			File f = new File(package_data_file_path);

			//res.setHeader("Content-disposition", "attachment; filename=" + URLUTF8Encoder.encode(f.getName()));
			//res.setHeader("Content-Type", "application/octet-stream");

			}
			 */

			}

			// flush
			res.flushBuffer();
			res.getOutputStream().close();

		} catch (Throwable e) {
			HtLog.log(Level.WARNING, getContext(), "doPost", "Exception when returning back the content of the file: " + e.getMessage());
		}

	}
}
