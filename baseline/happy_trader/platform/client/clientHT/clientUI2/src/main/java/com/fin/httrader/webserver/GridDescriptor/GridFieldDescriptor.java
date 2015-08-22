/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.GridDescriptor;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author DanilinS
 */
public class GridFieldDescriptor {
	
	public GridFieldDescriptor(String fieldName, String fieldType, String sort, String align)
	{
		fieldName_m.append(fieldName);
		fieldType_m.append(fieldType);
		sort_m.append( sort );
		align_m.append( align );

	}

	public GridFieldDescriptor(String fieldName)
	{
		// default field type
		fieldName_m.append(fieldName);
		fieldType_m.append("ro");
		sort_m.append("str");
		align_m.append("left");


	}
	
	public GridFieldDescriptor(String fieldName, String fieldType)
	{
		// default field type
		fieldName_m.append(fieldName);
		fieldType_m.append(fieldType);
		sort_m.append("str");
		align_m.append("left");
}
	
	public String getFieldName()
	{
		return fieldName_m.toString();
	}

	public String getFieldType()
	{
		return fieldType_m.toString();
	}

	public String getFieldSort()
	{
		return sort_m.toString();
	}

	public String getFieldAlign()
	{
		return align_m.toString();
	}

	private StringBuilder fieldName_m = new StringBuilder();

	private StringBuilder fieldType_m = new StringBuilder();

	private StringBuilder align_m = new StringBuilder();

	private StringBuilder sort_m = new StringBuilder();

}
