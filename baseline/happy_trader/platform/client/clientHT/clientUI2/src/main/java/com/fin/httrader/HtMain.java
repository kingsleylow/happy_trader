
package com.fin.httrader;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.RunningParameters;
import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.model.HtCommandProcessor;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.fin.httrader.utils.win32.HtSysUtils;
import com.fin.httrader.utils.xmlparamserial.HtXmlParameterSerialization;
import java.util.Set;




/**
 *
 * @author Administrator
 */

public class HtMain {

	// some constants
	
	public static class Indicator
	{
	};
	
	public static void main(String[] args) throws Exception   {
		
			
			
		HtCommandProcessor.instance().get_HtBootProxy().remote_startServer(args);
		HtCommandProcessor.instance().get_HtConsoleProxy().remote_consoleLoop();
		HtCommandProcessor.instance().get_HtBootProxy().remote_shutdown();
		 
		
	}

}
