/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.mtrunner.client.userlist.controller;

import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtSystem;
import com.fin.httrader.utils.JavaUid;
import com.fin.httrader.utils.Uid;
import com.mtrunner.client.jaxb.ht_settings.Settings;
import com.mtrunner.client.managers.SettingsManager;
import com.mtrunner.client.userlist.form.Account;
import com.mtrunner.client.userlist.form.Company;
import com.mtrunner.client.userlist.service.AccountService;
import com.mtrunner.client.userlist.service.SetupService;
import com.mtrunner.client.utils.SetupEntity;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;
import java.util.UUID;
import org.apache.log4j.Logger;
import org.hibernate.Hibernate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.ui.Model;
import org.springframework.validation.BindingResult;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.InitBinder;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

/**
 *
 * @author Administrator
 */
@Controller
@RequestMapping("/setup")
public class SetupController {

	static Logger log_m = Logger.getLogger(SetupController.class.getName());

	@Autowired
	private SetupService setupService_m;

	@Autowired
	private AccountService accountService_m;

	//@Autowired
	//UploadedFileValidator validator_m;
	/*
	 @InitBinder
	 private void initBinder(WebDataBinder binder) {
	 binder.setValidator(validator_m);
	 }
	 */
	@RequestMapping("/setup")
	public String initialData(Map<String, Object> map) throws Exception {

		readCurrentAccounts(map);

		return "setup";
	}
	
	@RequestMapping(value = "/clear_setup/do.html", method = RequestMethod.GET)
	public String clearSetup( Map<String, Object> map) throws Exception {

		
		setupService_m.clearSetup();
		readCurrentAccounts(map);
		
		return "setup";
	}

	@RequestMapping(value = "/generate_setup/do.html", method = RequestMethod.POST)
	public String createSetup(@ModelAttribute("setupentity") SetupEntity setupentity, BindingResult result, Map<String, Object> map) throws Exception {

		
		setupService_m.createSetupXml(setupentity);
		readCurrentAccounts(map);
		
		return "setup";
	}

	@RequestMapping(value = "/upload_main_xml/do.html", method = RequestMethod.POST)
	public String fileUploaded(@RequestParam("name") String name, @RequestParam("file") MultipartFile file) throws Exception {
		if (!file.isEmpty()) {
			try {
				byte[] bytes = file.getBytes();

				// Creating the directory to store file
				//String rootPath = System.getProperty("catalina.home");
				//File dir = new File(rootPath + File.separator + "tmpFiles");
				UUID uid = UUID.randomUUID();
				String tmp_path = HtSystem.getSystemTempPath() + File.separator + uid.toString() + name;

				// Create the file on server
				File serverFile = new File(tmp_path);
				BufferedOutputStream stream = new BufferedOutputStream(new FileOutputStream(serverFile));
				stream.write(bytes);
				stream.close();

				log_m.info("Server File Location=" + serverFile.getAbsolutePath());

				// then we need to parse that entry
				//SetupEntity setupEntry = setupService_m.createSetupXMLEntry(serverFile);

			} catch (Exception e) {
				throw new Exception("Upload file failure: " + e.getMessage());

			}
		} else {
			throw new Exception("Upload file failure- empty file!");
		}

		return "setup";
	}

	/**
	 * Helpers
	 */
	void readCurrentAccounts(Map<String, Object> map) throws Exception {
		List<Company> companies = accountService_m.listCompaniesFetchAccountsServers();

		map.put("companiesObj", companies);

		SetupEntity ent = setupService_m.parseSetupXml();
		map.put("setupEntityObj", ent);

		Set<File> mtDistrFiles = readDistribMtFiles();
		if (mtDistrFiles.size() <=0)
			throw new Exception("No valid zipped MT distributives");
		
		map.put("mtDistrFilesObj", mtDistrFiles);
		
		map.put("baseSetupFile", HtFileUtils.enforceSeparatorInPath( SettingsManager.getInstance().getMtSetupXml() ) );
	}
	
	
	

	private Set<File> readDistribMtFiles() throws Exception {

		String base = SettingsManager.getInstance().getMtDistrDir();
		Set<File> files = new TreeSet<File>();
		HtFileUtils.readAllfilesRecursively("zip", base, files);

		return files;
	}

	
	
}
