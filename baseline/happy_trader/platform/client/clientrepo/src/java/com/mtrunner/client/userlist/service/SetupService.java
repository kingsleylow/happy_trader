/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.service;

import au.com.bytecode.opencsv.CSVWriter;
import com.fin.httrader.eventplugins.ht_mt_bridge.helper.HtMtJsonUtils;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtUtils;
import com.mtrunner.client.jaxb.ht_settings.ObjectFactory;
import com.mtrunner.client.jaxb.ht_settings.Settings;

import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.userlist.controller.ServerControler;
import com.mtrunner.client.userlist.dao.AccountDAO;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.form.Server;
import com.mtrunner.client.utils.GeneralUtils;
import com.mtrunner.client.utils.SetupEntity;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.OutputStream;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.Unmarshaller;
import org.apache.commons.io.FileUtils;
import org.apache.log4j.Logger;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 *
 * @author Administrator
 */
@Service
public class SetupService {

	static Logger log_m = Logger.getLogger(SetupService.class.getName());

	@Autowired
	private AccountService accService_m;
	
	public void clearSetup()
	{
		GeneralUtils.deleteDirectoryQuietly( new File( SettingsManager.getInstance().getMtTragetBaseDir() ));
		GeneralUtils.deleteDirectoryQuietly( new File( SettingsManager.getInstance().getMtDataBaseDir()));
	}

	public SetupEntity parseSetupXml() throws Exception {
		SetupEntity setupEntity = new SetupEntity();
		
		File f = new File( SettingsManager.getInstance().getMtSetupXml()  );
		
		if (f.exists()) {
				JAXBContext jc = JAXBContext.newInstance(Settings.class);

        Unmarshaller unmarshaller = jc.createUnmarshaller();
        Settings settings = (Settings) unmarshaller.unmarshal(f);
				
				setupEntity.setDebugconsole(Boolean.parseBoolean( settings.getGeneralparams().getHtserver().getDebugconsole() ));
				setupEntity.setHeartbeatjavahtsec(settings.getHeartbeatjavahtsec().intValue());
				setupEntity.setHtserverinternaleventlevel(settings.getGeneralparams().getHtserver().getEventlevel().intValue());
				setupEntity.setHtserverinternalloglevel(settings.getGeneralparams().getHtserver().getInternalloglevel().intValue());
				setupEntity.setHtserverport(settings.getGeneralparams().getHtserver().getPort().intValue());
				
				List<Settings.Generalparams.Eventpluginparams.Entry> entries = settings.getGeneralparams().getEventpluginparams().getEntry();
				for(Settings.Generalparams.Eventpluginparams.Entry e: entries) {
					if (e.getName().equalsIgnoreCase("MAX_DELAY_POLL_QUEUE_ALIVE_SEC"))
						setupEntity.setMAX_DELAY_POLL_QUEUE_ALIVE_SEC(Integer.valueOf( e.getValue() ));
					
					if (e.getName().equalsIgnoreCase("PROCESS_ACCOUNT_UPDATE"))
						setupEntity.setPROCESS_ACCOUNT_UPDATE(Boolean.valueOf( e.getValue() ));
					
					if (e.getName().equalsIgnoreCase("PROCESS_BALANCE_UPDATE"))
						setupEntity.setPROCESS_BALANCE_UPDATE(Boolean.valueOf( e.getValue() ));
					
				}
				
				
				
				setupEntity.setMt4datareadintrvalsec(settings.getMt4Datareadintrvalsec().intValue());
				setupEntity.setMt4loglevel(settings.getMt4Loglevel().intValue());
				setupEntity.setOtherData(null);
				setupEntity.setZombiemt4killersec(settings.getZombiemt4Killer().intValue());
				
				// which companies are deployed
				
				HashMap<String, Integer> terminal_entries = new HashMap<String, Integer>();
				HashMap<String, String> paths = new HashMap<String, String>();
				
				List<Settings.Mtinstances.Instance> mts = settings.getMtinstances().getInstance();
				for(Settings.Mtinstances.Instance mts_i: mts) {
					Integer val = terminal_entries.get(mts_i.getCompanyid());
					
					if (val != null) {
						
						val++;
						terminal_entries.put(mts_i.getCompanyid(), val);
						
					} else {
						terminal_entries.put(mts_i.getCompanyid(), 1);
					}
					
					//File distr_f = new File(mts_i.getPath());
					//if (!distr_f.exists())
					//	throw new Exception("MT distribituve file not found: " + distr_f.getAbsolutePath());
					
					paths.put(mts_i.getCompanyid(), mts_i.getMtDistrib());
			
				}
				
				JSONArray arrays_json  = new JSONArray();
				for(Iterator<String> it = terminal_entries.keySet().iterator(); it.hasNext(); ) {
					JSONObject obj = new JSONObject();
					
					String company_name = it.next();
					int instance_parts = terminal_entries.get(company_name);
					Company company = accService_m.resolveCompanyByName(company_name);
					
					if (company == null)
						throw new Exception("Cannot resolve company by name: " + company_name);
					
					obj.put("instance_name", company_name);
					obj.put("id", company.getId());
					obj.put("instance_parts", instance_parts);
					obj.put("zip_path", paths.get(company_name));
					
					
					arrays_json.add( obj );
										
				}
				
				JSONObject upper_json  =new JSONObject();
				upper_json.put("DATA", arrays_json);
				
				setupEntity.setOtherData(upper_json.toString());
				
		}
		
		return setupEntity;
	};
		
		
	public void createSetupXml(SetupEntity setupentity) throws Exception {
		
		ObjectFactory factory = new ObjectFactory();
		Settings settings = factory.createSettings();
		
		settings.setHeartbeatjavahtsec(BigInteger.valueOf(setupentity.getHeartbeatjavahtsec()));
		settings.setMt4Datareadintrvalsec(BigInteger.valueOf(setupentity.getMt4datareadintrvalsec()));
		settings.setMt4Loglevel(BigInteger.valueOf(setupentity.getMt4loglevel()));
		settings.setZombiemt4Killer(BigInteger.valueOf(setupentity.getZombiemt4killersec()));
		
		// general params
		settings.setGeneralparams(factory.createSettingsGeneralparams());
		settings.getGeneralparams().setEventpluginparams(factory.createSettingsGeneralparamsEventpluginparams());
		settings.getGeneralparams().setHtserver(factory.createSettingsGeneralparamsHtserver());
		
		settings.getGeneralparams().getHtserver().setDebugconsole( Boolean.valueOf( setupentity.isDebugconsole()).toString() );
		settings.getGeneralparams().getHtserver().setEventlevel(BigInteger.valueOf(setupentity.getHtserverinternaleventlevel()));
		settings.getGeneralparams().getHtserver().setInternalloglevel(BigInteger.valueOf(setupentity.getHtserverinternalloglevel()));
		settings.getGeneralparams().getHtserver().setPort(BigInteger.valueOf(setupentity.getHtserverport()));
		
		//
		Settings.Generalparams.Eventpluginparams.Entry e1 = factory.createSettingsGeneralparamsEventpluginparamsEntry();
		e1.setName("PROCESS_ACCOUNT_UPDATE");
		e1.setValue(Boolean.toString( setupentity.isPROCESS_ACCOUNT_UPDATE() ));
		
		Settings.Generalparams.Eventpluginparams.Entry e2 = factory.createSettingsGeneralparamsEventpluginparamsEntry();
		e2.setName("PROCESS_BALANCE_UPDATE");
		e2.setValue(Boolean.toString( setupentity.isPROCESS_BALANCE_UPDATE()));
		
		Settings.Generalparams.Eventpluginparams.Entry e3 = factory.createSettingsGeneralparamsEventpluginparamsEntry();
		e3.setName("MAX_DELAY_POLL_QUEUE_ALIVE_SEC");
		e3.setValue(String.valueOf( setupentity.getMAX_DELAY_POLL_QUEUE_ALIVE_SEC() ));
		
		settings.getGeneralparams().getEventpluginparams().getEntry().add(e1);
		settings.getGeneralparams().getEventpluginparams().getEntry().add(e2);
		settings.getGeneralparams().getEventpluginparams().getEntry().add(e3);
		
						
		// remove base path
		GeneralUtils.deleteDirectoryQuietly( new File( SettingsManager.getInstance().getMtTragetBaseDir() ));
		GeneralUtils.deleteDirectoryQuietly( new File( SettingsManager.getInstance().getMtDataBaseDir()));
	
		settings.setMtinstances( factory.createSettingsMtinstances() );
		
		int global_instance_id = 1;
		String data = setupentity.getOtherData();
		if (!HtUtils.nvl(data)) {
			JSONObject obj = (JSONObject) JSONValue.parseWithException(data);
			JSONArray objs = (JSONArray)obj.get("DATA");
			
			
			for(int i=0; i < objs.size(); i++) {
				JSONObject obj_i = (JSONObject)objs.get(i);
				
				
				String company_name = HtMtJsonUtils.<String>getJsonField(obj_i, "instance_name");
				int company_id = HtMtJsonUtils.getIntJsonField(obj_i, "id");
				
				String zip_path = HtMtJsonUtils.<String>getJsonField(obj_i, "zip_path");
				if (HtUtils.nvl(zip_path))
					throw new Exception("Invalid zip file");
				
				int instance_parts = HtMtJsonUtils.getIntJsonField(obj_i, "instance_parts");
				//int total_accounts = HtMtJsonUtils.getIntJsonField(obj_i, "total_accounts");
				
				
				if (HtUtils.nvl(company_name))
					throw new Exception("Invalid instance name");
				
				//if (instance_id <0)
				//	throw new Exception("Invalid instance ID");
				
				Company company_i = this.accService_m.resolveCompanyFetchEnabledAccounts(company_id);
				if (company_i == null)
					throw new Exception("Company cannot be resolved via id: " + company_id);
				
				// hack!!!!
				List<Account> filtered_accounts = new ArrayList();
				for(Account acc_i: company_i.getAccounts()) {
					if (acc_i.isEnabled() != 0)
						filtered_accounts.add(acc_i);
				}
				
				int total_accounts = filtered_accounts.size();
				int total_accounts_idx = 0;
				
				if (total_accounts <=0)
					throw new Exception("Invalid total accounst number");
				
				if (instance_parts <1 || instance_parts > total_accounts )
					throw new Exception("Invalid parts number per each MT instnace");
				
				int accounts_per_page = total_accounts / instance_parts;
				
				// loop for all instances 
				for(int j = 0; j < instance_parts; j++) {
					Settings.Mtinstances.Instance instance = factory.createSettingsMtinstancesInstance();

					String global_instance_id_s = "mt_" + global_instance_id++;

					File zip_path_d = new File(SettingsManager.getInstance().getMtDistrDir() + File.separatorChar + zip_path);
					File target = installZip(zip_path_d, global_instance_id_s);

					// need to add some entries
					
					instance.setAutoinstall("true");
					instance.setAutoqueuerestore("false");
					instance.setName(global_instance_id_s);
					instance.setCompanyid(company_name);
					instance.setPath(target.getAbsolutePath());
					
					List<Account> part = new ArrayList<Account>();
					
					for(int m = 0; m < accounts_per_page; m++) {
						
						
						if (total_accounts_idx >= total_accounts)
							break;
						
						part.add(filtered_accounts.get( total_accounts_idx++ ));
						
					}
					
					if (j == (instance_parts-1)) {
						// last iteration
						while(total_accounts_idx < total_accounts) {
							part.add(filtered_accounts.get( total_accounts_idx++ ));
						}
					}
					
					
					// files with symbol
					File data_file = installData( global_instance_id_s, part );
					instance.setAutoload(data_file.getAbsolutePath());
					
					instance.setMtDistrib( zip_path );

					settings.getMtinstances().getInstance().add(instance);
				}
			}
		
			
		}
			

		javax.xml.bind.JAXBContext jaxbCtx = javax.xml.bind.JAXBContext.newInstance(settings.getClass().getPackage().getName());
		javax.xml.bind.Marshaller marshaller = jaxbCtx.createMarshaller();
		marshaller.setProperty(javax.xml.bind.Marshaller.JAXB_ENCODING, "UTF-8"); //NOI18N
		marshaller.setProperty(javax.xml.bind.Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);
		
		String outputFilePath = SettingsManager.getInstance().getMtSetupXml();
		File outf = new File(outputFilePath);
	
		
		// rename
		File renamedFile = new File(outf.getAbsolutePath() + "." + (long)HtDateTimeUtils.getCurGmtTime());
		FileUtils.copyFile(outf, renamedFile);
		FileUtils.deleteQuietly(outf);
		
		OutputStream os = new FileOutputStream( outf );
		
		marshaller.marshal(settings, os);

	}
	
	
	private File installData( String instance_internal_id, List<Account> part) throws Exception
	{
		String targetDir = SettingsManager.getInstance().getMtDataBaseDir() + File.separatorChar + instance_internal_id;
		File targetDir_f = new File(targetDir);
		targetDir_f.mkdirs();
		
		File targetFile_f = new File(targetDir + File.separatorChar + "accounts.csv");
		
		
		try (CSVWriter writer = new CSVWriter(new FileWriter(targetFile_f), '\t')) {
			for(Account p:  part) {
				
				
				int server_id = p.getServer().getId();
				Server srv = accService_m.resolveServer( server_id );
				if (srv == null)
					throw new Exception("Invalid server resolved by ID: " + server_id);
				
				String [] data = new String[]{p.getAccount_id(), p.getInvest_password(),  srv.getServer_name(), "-1"};
				writer.writeNext(data);
				
			}
		}
		
		// data files
		return targetFile_f;
	}
	
	private File installZip(File zip_path_d, String instance_internal_id) throws Exception
	{
			
		
		
		byte[] buffer = new byte[2048];
		
		
		if (!zip_path_d.exists())
					throw new Exception("Invalid zip path: " + zip_path_d.getAbsolutePath());
		
		// 
		String targetDir = SettingsManager.getInstance().getMtTragetBaseDir() + File.separatorChar + instance_internal_id;
		File targetDir_f = new File(targetDir);
		
		//targetDir_f.mkdirs();
		
		// read zip
		
    ZipInputStream stream = new ZipInputStream(new FileInputStream(zip_path_d));

		String basePath = null;
		try {
			ZipEntry entry;
      while((entry = stream.getNextEntry())!=null)
      {
				String name = entry.getName();
				if (!entry.isDirectory() && entry.getName().indexOf("terminal.exe") != -1) {
					// our base
					basePath = name;
					break;
				}
			}
		}
		finally 
		{
			stream.close();
		}
		
		// base found
		if (basePath==null)
			throw new Exception("Invalid zip content, no treminal.exe");
		
		// open zip again and extract ecerything
		String basePathParent = new File(basePath).getParent();
		if (basePathParent == null) {
			basePathParent = ""; 
		}
		
		stream = new ZipInputStream(new FileInputStream(zip_path_d));
	
		try {
			ZipEntry entry;
      while((entry = stream.getNextEntry())!=null)
      {
				String name = HtFileUtils.enforceSeparatorInPath( entry.getName() );
				
				
				String relativePath = HtFileUtils.getRelativePath(name, basePathParent, File.separator);
				if (HtUtils.nvl(relativePath))
					continue;
				
				String pathToStore = targetDir + File.separator + relativePath;
				File relativePath_f = new File(pathToStore);
				
				if (entry.isDirectory()) {
					// create dir and continue
					relativePath_f.mkdirs();
					continue;
				}
				
				
				// else
				// create parent
				relativePath_f.getParentFile().mkdirs();
				
				FileOutputStream output = null;
				try {
					output = new FileOutputStream(relativePath_f.getAbsolutePath());
					int len = 0;
					while ((len = stream.read(buffer)) > 0) {
						output.write(buffer, 0, len);
					}
				} finally {
					// we must always close the output file
					HtUtils.silentlyCloseOutputStream(output);
				}
				
			}
		}
		finally 
		{
			stream.close();
		}
		
	
		return targetDir_f;
		
	}

}
