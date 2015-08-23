/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils.gui;

import com.fin.httrader.utils.HtDateTimeUtils;
import htrangebreaker.utils.EventsFileBuffer;
import htrangebreaker.utils.HtFrameLogger;
import java.util.Calendar;
import java.util.LinkedHashMap;
import java.util.Map;
import javax.swing.table.AbstractTableModel;

/**
 *
 * @author DanilinS
 */
public class DiskCachedTableModel extends AbstractTableModel {

	public static interface DiskCachedTableModelInterface
	{
		public Object[] getEntry(long rowidx) throws Exception;
		
	}


	private static class RecordCache extends LinkedHashMap<Integer,Object[] > {
   
		private int cacheSize_m;

    public RecordCache (int size) {
			super(size, 1.1f, false);
      cacheSize_m = size;
    }

		@Override
    protected boolean removeEldestEntry(Map.Entry<Integer, Object[] > eldest) {
      if (size() > cacheSize_m) {
        return true;
      }
			 
      return false;
    }
  }

	// -------------------------------
	private  static final int MAX_CACHE_SIZE = 1000;
	private int rowCount_m = 0;
	private int columnCount_m = 0;
  private RecordCache cache_m = null;
	private DiskCachedTableModelInterface buffer_m = null;
	private Integer[] editableCells_m = null;


	public DiskCachedTableModel(DiskCachedTableModelInterface filebuffer, Integer[] editableCells)
	{
		cache_m = new RecordCache(MAX_CACHE_SIZE);
		buffer_m = filebuffer;
		editableCells_m = editableCells;
	}


	public int getRowCount() {
		return rowCount_m;
	}

	public int getColumnCount()
	{
		return columnCount_m;
	}

	public Object getValueAt(int rowIndex, int columnIndex)
	{
		
		
		Object[] row = cache_m.get(rowIndex);
		if (row == null) {
			try {

				row = buffer_m.getEntry(rowIndex);
				cache_m.put(rowIndex, row);

				// System.out.println(HtDateTimeUtils.time2SimpleString_Gmt(HtDateTimeUtils.getCurGmtTime()) + " - cache row asked: "+ rowIndex);

			}
			catch(Throwable e)
			{
				HtFrameLogger.logError(e, "Exception on retreiving data from file buffer: " + e.toString() + " row idx="+rowIndex+" col idx="+columnIndex);
				return null;
			}

		}
		

		return row[columnIndex];

	}

	@Override
	public boolean isCellEditable(int rowIndex, int mColIndex)
		{
		if (editableCells_m != null)
		{
			for(int i = 0; i < editableCells_m.length; i++) {
				if (editableCells_m[i] == mColIndex)
					return true;
			}
		
		}

		return false;
		}

	// ----------- new functions ------------



	
	public void incrementRowCount()
	{
		rowCount_m++;
	}

	public void appendNewEntry(Object[] row)
	{
		cache_m.put(rowCount_m, row);
		rowCount_m++;
	}

	public void clearRowCount()
	{

		rowCount_m = 0;
		cache_m.clear();
	}



	public void setColumnCount(int columnCount)
	{
		columnCount_m = columnCount;
	}

}
