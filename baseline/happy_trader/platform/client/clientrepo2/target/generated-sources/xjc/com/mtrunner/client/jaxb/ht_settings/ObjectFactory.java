//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v2.2.11 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2015.05.23 at 06:26:20 PM BST 
//


package com.mtrunner.client.jaxb.ht_settings;

import javax.xml.bind.annotation.XmlRegistry;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the com.mtrunner.client.jaxb.ht_settings package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {


    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: com.mtrunner.client.jaxb.ht_settings
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link Settings }
     * 
     */
    public Settings createSettings() {
        return new Settings();
    }

    /**
     * Create an instance of {@link Settings.Generalparams }
     * 
     */
    public Settings.Generalparams createSettingsGeneralparams() {
        return new Settings.Generalparams();
    }

    /**
     * Create an instance of {@link Settings.Generalparams.Eventpluginparams }
     * 
     */
    public Settings.Generalparams.Eventpluginparams createSettingsGeneralparamsEventpluginparams() {
        return new Settings.Generalparams.Eventpluginparams();
    }

    /**
     * Create an instance of {@link Settings.Mtinstances }
     * 
     */
    public Settings.Mtinstances createSettingsMtinstances() {
        return new Settings.Mtinstances();
    }

    /**
     * Create an instance of {@link Settings.Generalparams.Htserver }
     * 
     */
    public Settings.Generalparams.Htserver createSettingsGeneralparamsHtserver() {
        return new Settings.Generalparams.Htserver();
    }

    /**
     * Create an instance of {@link Settings.Generalparams.Eventpluginparams.Entry }
     * 
     */
    public Settings.Generalparams.Eventpluginparams.Entry createSettingsGeneralparamsEventpluginparamsEntry() {
        return new Settings.Generalparams.Eventpluginparams.Entry();
    }

    /**
     * Create an instance of {@link Settings.Mtinstances.Instance }
     * 
     */
    public Settings.Mtinstances.Instance createSettingsMtinstancesInstance() {
        return new Settings.Mtinstances.Instance();
    }

}