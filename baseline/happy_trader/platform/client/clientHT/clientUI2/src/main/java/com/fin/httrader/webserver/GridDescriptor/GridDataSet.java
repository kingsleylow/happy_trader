/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.GridDescriptor;

import com.fin.httrader.utils.HtException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author DanilinS
 */
public class GridDataSet {

	private String getContext() {
		return getClass().getSimpleName();
	}

	public GridDataSet()
	{
	}

	public GridDataSet(GridFieldDescriptor[] descriptors)
	{
		if (descriptors != null) {
			for(int i = 0; i < descriptors.length; i++) {
				addColumn(descriptors[i]);
			}
		}
	}

	public void addColumn(GridFieldDescriptor descriptor)
	{
		int id = descriptors_m.size();
		descriptors_m.put(id, descriptor);
	}

	public void addRow(String[] row)
	{
		rows_m.add(row);
	}

	public void addRow()
	{
		rows_m.add(new String[descriptors_m.size()]);
	}

	public String[] getRow(int idx) throws Exception
	{
		if (idx >=0 && idx < rows_m.size())
			return rows_m.get(idx);
		else
			throw new HtException(getContext(), "getRow", "Index: " + idx + " is not valid");
	}

	public String getRowValue(int rowidx, int colidx) throws Exception
	{
		String[] row = getRow(rowidx);

		if (colidx >=0 && colidx < descriptors_m.size())
			return row[colidx];
		else
			throw new HtException(getContext(), "getRowValue", "Column index: " + colidx + " is not valid");

	}

	public void setRowValue(int rowidx, int colidx, String value) throws Exception
	{
		String[] row = getRow(rowidx);

		if (colidx >=0 && colidx < descriptors_m.size())
			row[colidx] = value;
		else
			throw new HtException(getContext(), "setRowValue", "Column index: " + colidx + " is not valid");

	}
	

	public GridFieldDescriptor getDescriptor(int idx) throws Exception
	{
		if (!descriptors_m.containsKey(idx))
			throw new HtException(getContext(), "getDescriptor", "Index: " + idx + " is not valid");

		return descriptors_m.get(idx);
	}

	public int getColumnsNumber()
	{
		return descriptors_m.size();
	}

	public int getRowsNumber()
	{
		return rows_m.size();
	}

	public void setHeaderWrapped(boolean wrapped)
	{
		headerWrapped_m = wrapped;
	}

	public boolean isHeaderWrapped()
	{
		return headerWrapped_m;
	}




// -------------

	
	private Map<Integer, GridFieldDescriptor> descriptors_m = new HashMap<Integer, GridFieldDescriptor>();

	private List<String[] > rows_m = new ArrayList<String[] >();

	private boolean headerWrapped_m = false;
}
