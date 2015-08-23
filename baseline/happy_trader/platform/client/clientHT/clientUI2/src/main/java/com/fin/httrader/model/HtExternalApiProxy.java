/*
 * HtExternalApiProxy.java
 *

 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package com.fin.httrader.model;

import com.fin.httrader.externalapi.HtExtScriptLogger;
import com.fin.httrader.hlpstruct.ExternalScriptStructure;

import com.fin.httrader.hlpstruct.externalrunstruct.RunClassEntry;
import com.fin.httrader.hlpstruct.externalrunstruct.RunPackageDescriptor;
import com.fin.httrader.managers.RtExternalApiRunManager;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.io.File;
import java.io.OutputStream;
import java.util.List;
import java.util.Set;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public class HtExternalApiProxy extends HtProxyBase {

	private String getContext() {
		return this.getClass().getSimpleName();

	}

	
	@Override
	public String getProxyName() {
		return "external_api_proxy";
	}

	/** Creates a new instance of HtExternalApiProxy */
	public HtExternalApiProxy() {
	}

	// --------------------------------------------------


	public Set<File> remote_getListOfTemplateJars() throws Exception
	{
		return RtExternalApiRunManager.instance().getListOfTemplateJars();
	}

	public void remote_removeTemplateJar(String fileName) throws Exception
	{
		RtExternalApiRunManager.instance().removeTemplateJar(fileName);
	}

	public File remote_resolveTemplateJarFilePath(String fileName) throws Exception
	{
		return RtExternalApiRunManager.instance().resolveTemplateJarFilePath(fileName);
	}

	/*
	public String remote_getXmlDescriptorFromJar(String packageLoadPath) throws Exception
	{
		return RtExternalApiRunManager.instance().getXmlDescriptorFromJar(packageLoadPath);
	}
	*
	*/ 

	public void remote_updateXmlDescriptorFromJar(String packageLoadPath, String newXml) throws Exception
	{
		RtExternalApiRunManager.instance().updateXmlDescriptorFromJar(packageLoadPath, newXml);
	}

	public void remote_executeWholeJarInCallingThread(HtExtScriptLogger logger, Uid runUid) throws Exception
	{
		RtExternalApiRunManager.instance().executeWholeJarInCallingThread(logger, runUid);
	}

	public RunPackageDescriptor remote_resolveDescriptorFromJar(Uid runUid) throws Exception
	{
		return RtExternalApiRunManager.instance().resolveDescriptorFromJar(runUid);
	}
	
	public RunPackageDescriptor remote_resolveDescriptorFromJar(String packageLoadPath) throws Exception
	{
		return RtExternalApiRunManager.instance().resolveDescriptorFromJar(packageLoadPath);
	}

	/*
	public void remote_removeTempFile(String packageLoadPath) throws Exception
	{
		RtExternalApiRunManager.instance().removeTempFile(packageLoadPath);
	}
	 *
	 */

	public Uid remote_copyTemplateToWorkingDirectory(Uid newUid, String templateName) throws Exception
	{
		return RtExternalApiRunManager.instance().copyTemplateToWorkingDirectory(newUid, templateName);
	}

	
	public void remote_insertNewEntry(ExternalScriptStructure estruct) throws Exception
	{
		RtExternalApiRunManager.instance().insertNewEntry(estruct);
	}

	public void remote_updateEntry(ExternalScriptStructure estruct) throws Exception
	{
		RtExternalApiRunManager.instance().updateEntry(estruct);
	}

	public List<ExternalScriptStructure> remote_queryScriptRuns() throws Exception
	{
		return RtExternalApiRunManager.instance().queryScriptRuns();
	}

	public ExternalScriptStructure remote_queryScriptRun(Uid runUid) throws Exception
	{
		return RtExternalApiRunManager.instance().queryScriptRun(runUid);
	}

	public void remote_removeEntry(Uid uid) throws Exception
	{
		RtExternalApiRunManager.instance().removeEntry(uid);
	}

	public void remote_copyJarFileToStream(OutputStream out, Uid runUid) throws Exception
	{
		RtExternalApiRunManager.instance().copyJarFileToStream(out,runUid);
	}

	public void remote_copyLogFileToStream(OutputStream out, Uid runUid) throws Exception
	{
		RtExternalApiRunManager.instance().copyLogFileToStream(out,runUid);
	}

	public File remote_resolveHtmlLogFilePath(Uid uid, boolean mustexist) throws Exception
	{
		return RtExternalApiRunManager.instance().resolveHtmlLogFilePath(uid, mustexist);
	}

	public File remote_resolveJarFilePath(Uid uid) throws Exception
	{
		return RtExternalApiRunManager.instance().resolveJarFilePath(uid);
	}

}
