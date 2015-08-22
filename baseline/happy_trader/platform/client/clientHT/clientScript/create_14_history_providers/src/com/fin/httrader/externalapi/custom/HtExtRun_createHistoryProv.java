/*
 * HtExtRun_createHistoryProv.java
 *
 * Created on 20 Сентябрь 2008 г., 12:25
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.externalapi.custom;

import com.fin.httrader.configprops.HtHistoryProviderProp;
import com.fin.httrader.externalapi.*;
import com.fin.httrader.model.HtDatabaseProxy;
import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.HtUtils;
import java.util.Calendar;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExtRun_createHistoryProv implements HtExtRunner{ 
   
    
    // ----------------------------
    public void HtExtRun_createHistoryProv()
    {
    }
    
    // ----------------------------
    public void run(HtExtLogger logger) throws Exception {
	logger.addStringEntry("Will create 14 history providers");
	
	HtExtConfiguration configurator = new HtExtConfiguration();
	
	// need to perform a series of exports
	HtHistoryProviderProp prop_GBPUSD = new HtHistoryProviderProp();
	prop_GBPUSD.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_GBPUSD.initParams_m.put("ask_prices", "false");
	prop_GBPUSD.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\GBPUSD_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_GBPUSD", prop_GBPUSD ));
	
	HtHistoryProviderProp prop_EURGBP = new HtHistoryProviderProp();
	prop_EURGBP.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_EURGBP.initParams_m.put("ask_prices", "false");
	prop_EURGBP.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\EURGBP_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_EURGBP", prop_EURGBP ));
	
	HtHistoryProviderProp prop_GBPCHF = new HtHistoryProviderProp();
	prop_GBPCHF.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_GBPCHF.initParams_m.put("ask_prices", "false");
	prop_GBPCHF.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\GBPCHF_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_GBPCHF", prop_GBPCHF ));
	
	
	HtHistoryProviderProp prop_CHFJPY = new HtHistoryProviderProp();
	prop_CHFJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_CHFJPY.initParams_m.put("ask_prices", "false");
	prop_CHFJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\CHFJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_CHFJPY", prop_CHFJPY ));
	
	
	HtHistoryProviderProp prop_AUDJPY = new HtHistoryProviderProp();
	prop_AUDJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_AUDJPY.initParams_m.put("ask_prices", "false");
	prop_AUDJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\AUDJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_AUDJPY", prop_AUDJPY ));
	
	HtHistoryProviderProp prop_EURJPY = new HtHistoryProviderProp();
	prop_EURJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_EURJPY.initParams_m.put("ask_prices", "false");
	prop_EURJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\EURJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_EURJPY", prop_EURJPY ));
	

	HtHistoryProviderProp prop_USDCHF = new HtHistoryProviderProp();
	prop_USDCHF.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_USDCHF.initParams_m.put("ask_prices", "false");
	prop_USDCHF.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\USDCHF_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_USDCHF", prop_USDCHF ));

	HtHistoryProviderProp prop_CADJPY = new HtHistoryProviderProp();
	prop_CADJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_CADJPY.initParams_m.put("ask_prices", "false");
	prop_CADJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\CADJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_CADJPY", prop_CADJPY ));

	HtHistoryProviderProp prop_AUDUSD = new HtHistoryProviderProp();
	prop_AUDUSD.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_AUDUSD.initParams_m.put("ask_prices", "false");
	prop_AUDUSD.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\AUDUSD_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_AUDUSD", prop_AUDUSD ));

	HtHistoryProviderProp prop_USDJPY = new HtHistoryProviderProp();
	prop_USDJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_USDJPY.initParams_m.put("ask_prices", "false");
	prop_USDJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\USDJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_USDJPY", prop_USDJPY ));

	HtHistoryProviderProp prop_EURUSD = new HtHistoryProviderProp();
	prop_EURUSD.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_EURUSD.initParams_m.put("ask_prices", "false");
	prop_EURUSD.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\EURUSD_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_EURUSD", prop_EURUSD ));

	HtHistoryProviderProp prop_EURCHF = new HtHistoryProviderProp();
	prop_EURCHF.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_EURCHF.initParams_m.put("ask_prices", "false");
	prop_EURCHF.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\EURCHF_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_EURCHF", prop_EURCHF ));

	HtHistoryProviderProp prop_GBPJPY = new HtHistoryProviderProp();
	prop_GBPJPY.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_GBPJPY.initParams_m.put("ask_prices", "false");
	prop_GBPJPY.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\GBPJPY_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_GBPJPY", prop_GBPJPY ));

	HtHistoryProviderProp prop_USDCAD = new HtHistoryProviderProp();
	prop_USDCAD.providerClass_m.append("com.fin.httrader.hstproviders.HtMetatraderCustomProvider");
	prop_USDCAD.initParams_m.put("ask_prices", "false");
	prop_USDCAD.initParams_m.put("file_to_import", "C:\\work\\MetaTrader_broko_2\\experts\\files\\USDCAD_export.dat");
	HtExtServerCommand.commandRunWrapper(logger, configurator.setHistoryProviderProperty("MT_Broko_Hist_USDCAD", prop_USDCAD ));

	logger.addStringEntry("Finished creating 14 providers");
	
    }
    
    public String getRunName() {
	return "run_1";
    }
    
    
    
    
    
}
