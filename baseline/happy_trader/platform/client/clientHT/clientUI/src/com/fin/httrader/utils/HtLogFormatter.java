/*
 * HtLogFormatter.java
 *
 * Created on December 17, 2006, 7:59 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package com.fin.httrader.utils;

import java.util.GregorianCalendar;
import java.util.logging.LogRecord;

/**
 *
 * @author Administrator
 */
public class HtLogFormatter extends java.util.logging.Formatter{

  private GregorianCalendar tm_m = new GregorianCalendar();

  /** Creates a new instance of HtLogFormatter */
  public HtLogFormatter() {
    super();
  }

  public String format(LogRecord record) {
    StringBuilder out = new StringBuilder();

    tm_m.setTimeInMillis(record.getMillis());

    out.append(record.getLevel());
    out.append(" - ");
    out.append(String.format("%1$td-%1$tm-%1$tYT%1$tH:%1$tM:%1$tS.%1$tL",tm_m));

    out.append(" - ");
    out.append(record.getThreadID());
    out.append(" - ");
    out.append(record.getMessage());
    out.append(" @ ");
    out.append(record.getSourceClassName());
    out.append(" - ");
    out.append(record.getSourceMethodName());

    out.append("\r\n");



    return out.toString();

  }

}
