/*
 * HtServerAlgorithmLoad_AlgBrkParam.java
 *
 
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtException;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.TreeMap;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 *
 * @author Victor_Zoubok
 */
public class HtServerAlgorithmLoad_AlgBrkParam extends HtServletsBase {

	// --------------------------------
	public String getContext() {
		return this.getClass().getSimpleName();
	}

	/** Creates a new instance of HtServerAlgorithmLoad_AlgBrkParam */
	public HtServerAlgorithmLoad_AlgBrkParam() {
	}

	// -----------------------
	// here only get
	public boolean initialize_Get(HttpServletRequest req, HttpServletResponse res) throws ServletException, IOException {
		res.setContentType("text/html; charset=windows-1251");
		forceNoCache(res);

		try {
			// restore also page uid
			String page_uid = getStringParameter(req, "page_uid", true);
			if (page_uid.length() == 0) {
				// new UID
				this.generateUniquePageId();
			} else {
				setUniquePageId(page_uid);
			}

			// here only we are initializing alg brk parameters
			setStringSessionValue(getUniquePageId(), req, "alg_pair_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters", "");
			setStringSessionValue(getUniquePageId(), req, "thread_list", "");
			setStringSessionValue(getUniquePageId(), req, "list_source", "");

			// do whatever
			if (HtCommandProcessor.instance().get_HtServerProxy().isBusyWithLengthOperation()) {
				throw new HtException(getContext(), "initialize_Get", "Server is busy with lengthy operation: " +
								HtCommandProcessor.instance().get_HtServerProxy().getLengthyOperationDescription());
			}

			String cur_server_id = getStringParameter(req, "server_id", false);
			String alg_id = getStringParameter(req, "alg_pair", false);
			TreeMap<String, String> algParam = new TreeMap<String, String>();
			TreeMap<String, String> brkParam = new TreeMap<String, String>();

			List<Integer> threads = HtCommandProcessor.instance().get_HtServerProxy().remote_requestStartedThreads(cur_server_id);
			if (threads.size() <= 0) {
				throw new HtException(getContext(), "readAlgBrkParams", "No thread is started");
			}

			int tid = (int) getIntParameter(req, "thread_id", true);

			if (tid < 0) {

				// read from parameter storage
				HtCommandProcessor.instance().get_HtServerProxy().remote_getLoadedAlgorithmBrokerPairParameters(
								cur_server_id,
								alg_id,
								algParam,
								brkParam);

				setStringSessionValue(getUniquePageId(), req, "list_source", "This are parameters loaded by default from the pair: \"" + alg_id + "\"");

			} else {

				// read for a separate thread
				HtCommandProcessor.instance().get_HtServerProxy().remote_getLoadedAlgorithmBrokerPairParametersForTheThread(
								cur_server_id,
								alg_id,
								tid,
								algParam,
								brkParam);

				setStringSessionValue(getUniquePageId(), req, "list_source", "This are parameters for the thread: \"" + tid + "\" and the pair: \"" + alg_id + "\"");
			}



			setStringSessionValue(getUniquePageId(), req, "alg_pair_parameters", HtWebUtils.createXmlParametersTable(algParam));
			setStringSessionValue(getUniquePageId(), req, "brk_pair_parameters", HtWebUtils.createXmlParametersTable(brkParam));

			//
			
			setStringSessionValue(getUniquePageId(), req, "thread_list", HtWebUtils.createOptionList(Integer.valueOf(tid), threads, true));



		} catch (Throwable e) {
			writeHtmlErrorToOutput(res, e);
			return false;
		}
		
		return true;
	}
	
	
}
