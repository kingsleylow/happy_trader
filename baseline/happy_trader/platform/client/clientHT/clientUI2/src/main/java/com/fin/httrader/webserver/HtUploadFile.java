/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.hlpstruct.ExternalScriptStructure;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.Uid;
import static com.fin.httrader.webserver.HtWebUtils.getContext;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author Victor_Zoubok
 * Common code to upload the file
 */
public class HtUploadFile extends HtServletsBase {

	

	@Override
	public String getContext() {
		return this.getClass().getSimpleName();
	}
        
        @Override
	public void doGet(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
            res.setContentType("text/html; charset=windows-1251");
            forceNoCache(res);
            
            res.getOutputStream().print("NO GET ALLOWED");
            res.getOutputStream().flush();
        }

	@Override
	public void doPost(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		StringBuilder fileName = new StringBuilder();
		try {

			String operation = getStringParameter(req, "operation", false);

			if (operation.equalsIgnoreCase("upload_file")) 		{
					// -----------------------
					
					DiskFileItemFactory factory = new DiskFileItemFactory();
					factory.setSizeThreshold(1024 * 1024);
					ServletFileUpload upload = new ServletFileUpload(factory);


					List /* FileItem */ items = upload.parseRequest(req);
					Iterator iter = items.iterator();


					
					
					String tmp = HtSystem.getSystemTempPath();

					while (iter.hasNext()) {
								FileItem item = (FileItem) iter.next();

								String name = item.getFieldName();
								String fileN = item.getName();
								String contentType = item.getContentType();
								boolean isInMemory = item.isInMemory();
							 long sizeInBytes = item.getSize();
								
								
								
								File tmpF = new File(tmp + "/" + fileN);
								item.write(tmpF);

					}
					
					
					// -----------------------
			}	
			else if (operation.equalsIgnoreCase("upload_template_package"))		{
				// this uploads to temp directory
				// ...\extjars\templates\


				String new_file_name = getStringParameter(req, "new_file_name", true);
				
				HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalTemplatesScriptsDirectory();

				

				HtWebUtils.uploadToFile(
								req,
								res,
								HtCommandProcessor.instance().get_HtConfigurationProxy().remote_getExternalTemplatesScriptsDirectory(),
								new_file_name,
								fileName
				);
				
				returnOutput(res, "Uploaded OK: "+fileName);

			} else {
				throw new HtException(getContext(), "doPost", "Invalid operation: " + operation);
			}


		} catch (Throwable e) {
				returnOutput(res, "Error during upload of the file: " + fileName +"["+ e.getMessage() + "]");
		}

		res.flushBuffer();
		res.getOutputStream().close();

		
	}

	

	public void returnOutput(HttpServletResponse res, String data) throws IOException
	{

						res.getOutputStream().print("<html>");
						res.getOutputStream().print("<head>");

						res.getOutputStream().print("<script type=\"text/javascript\">");
						res.getOutputStream().print("function init() {");
						res.getOutputStream().print("if (parent.on_frame_load)  parent.on_frame_load(); else alert('Cannot find on_frame_load() function!'); }; ");
						res.getOutputStream().print("window.onload=init");
						res.getOutputStream().print("</script>");

						res.getOutputStream().print("</head>");
						res.getOutputStream().print("<body><textarea>");
						res.getOutputStream().print(data);
					
						res.getOutputStream().print("</textarea></body>");
						res.getOutputStream().print("</html>");
					
	}
}
