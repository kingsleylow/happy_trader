/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;

import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtTimeCalculator;
import com.fin.httrader.utils.HtUtils;
import htrangebreaker.utils.gui.DiskCachedTableModel;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.RandomAccessFile;
import java.io.Serializable;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 * @author DanilinS
 */
public class EventsFileBuffer implements DiskCachedTableModel.DiskCachedTableModelInterface{

	public static class FileStructure implements Serializable {

		FileStructure(Object[] data) {
			data_m = data;
		}
		public Object[] data_m = null;
	}
	private RandomAccessFile writer_m = null;
	private RandomAccessFile writerIdx_m = null;
	private RandomAccessFile reader_m = null;
	private RandomAccessFile readerIdx_m = null;
	private long curPtr_m = 0;
	private long curCounter_m = 0;
	private File tmpfile_m = null;
	private File tmpfile_idx_m = null;
	private Object mtx_m = new Object();
	//private List<Long> indexList_m = new ArrayList<Long>();

	public EventsFileBuffer() {
	}

	public long getCurCounter() {
		synchronized (mtx_m) {
			return curCounter_m;
		}
	}

	public void open() throws Exception {

		curPtr_m = 0;
		curCounter_m = 0;

		tmpfile_m = File.createTempFile("efb_", ".dat");
		tmpfile_idx_m = File.createTempFile("efb_idx_", ".dat");

		writer_m = new RandomAccessFile(tmpfile_m, "rw");
		writerIdx_m = new RandomAccessFile(tmpfile_idx_m, "rw");

		reader_m = new RandomAccessFile(tmpfile_m, "r");
		readerIdx_m = new RandomAccessFile(tmpfile_idx_m, "r");
	}

	// note that all objects in the array must be serializable
	public void addEntry(Object[] row) throws Exception {
		if (row != null) {


			byte[] data = serializeToBytes(row);

			if (data == null) {
				throw new HtFrameException("Cannot serialize data to cache");
			}

			synchronized (mtx_m) {
				writer_m.write(HtMathUtils.convertLongToByte(data.length));
				writer_m.write(data);


				// write index
				writerIdx_m.write(HtMathUtils.convertLongToByte(curPtr_m));

				// advance current pointer
				curPtr_m += data.length + HtUtils.SOCKET_LENGTH_HEADER_BYTES;
				curCounter_m++;
			}

			//synchronized(indexList_m) {
			//	indexList_m.add(curPtr_m);
			//}


		}


	}

	public Object[] getEntry(long rowidx) throws Exception {

		long lastIdx = flushAll();
		if (lastIdx <= rowidx)
			throw new HtFrameException("Last index detected: " + lastIdx + " but requested is more: " + rowidx );

		return readEntryHelper(rowidx);

	}

	public void close() {
		try {
			synchronized (mtx_m) {

				curPtr_m = 0;
				curCounter_m = 0;

				if (writerIdx_m != null) {
					writerIdx_m.close();
					writerIdx_m = null;
				}


				if (writer_m != null) {
					writer_m.close();
					writer_m = null;
				}


				if (readerIdx_m != null) {
					readerIdx_m.close();
					readerIdx_m = null;
				}


				if (reader_m != null) {
					reader_m.close();
					reader_m = null;
				}

				if (tmpfile_m != null) {
					tmpfile_m.delete();
					tmpfile_m = null;
				}


				if (tmpfile_idx_m != null) {
					tmpfile_idx_m.delete();
					tmpfile_idx_m = null;
				}


			}
		} catch (Throwable e) {
				HtFrameLogger.logError(e, "Exception on closing file cache: " + e.toString());
		}

	}

	public long findNextRow(
					int columnId,
					int contentIdx,
					Pattern symbolPattern,
					Pattern contentPattern,
					long begin_row_idx,
					boolean isAndPatern,
					int stop_search
	) throws Exception
	{

		long lastIdx = flushAll();
		if (stop_search >= 0) {
			if (stop_search <= lastIdx) {
				lastIdx = stop_search;
			}
		}
		

		if (lastIdx <= begin_row_idx)
			throw new HtFrameException("Last index detected: " + lastIdx + " but requested is more: " + begin_row_idx );

		long bind = -1;
		if (begin_row_idx == -1)
			bind = 0;
		else
			bind = begin_row_idx;

		boolean found = false;
		while(bind < lastIdx) {

		
			if (isValueMatches(bind, columnId, contentIdx, symbolPattern, contentPattern, isAndPatern)) {
				found = true;
				break;
			}
			

			bind++;
		}

		if (found)
			return bind;
		else
			return -1;
		
	}

	public long findPrevRow(
					int columnId,
					int contentIdx,
					Pattern symbolPattern,
					Pattern contentPattern,
					long end_row_idx,
					boolean isAndPatern
		) throws Exception
	{
		long lastIdx = flushAll();

		if (lastIdx <= end_row_idx)
			throw new HtFrameException("Last index detected: " + lastIdx + " but requested is more: " + end_row_idx );

		long bind = -1;
		

		if (end_row_idx == -1)
			bind = lastIdx - 1;
		else
			bind = end_row_idx;

		boolean found = false;
		while(bind >=0) {

		
			if (isValueMatches(bind, columnId, contentIdx, symbolPattern, contentPattern,isAndPatern )) {
				// match found
				found = true;
				break;
			}


			bind--;
		}

		if (found)
			return bind;
		else
			return -1;
	}



	/**
	 * Helpers
	 */

	private boolean isValueMatches(long rowidx, int columnIdx, int contentIdx, Pattern symbolPattern, Pattern contentPattern, boolean isAndPatern) throws Exception
	{
			Object[] rowdata = readEntryHelper(rowidx);
			String symbolValue = (String)rowdata[columnIdx];
			String contentValue = (String)rowdata[contentIdx];

			Matcher mtch_symbol = null, mtch_content = null;

			if (symbolPattern != null) {
				mtch_symbol = symbolPattern.matcher(symbolValue);
			}

			if (contentPattern != null) {
				mtch_content = contentPattern.matcher(contentValue);
			}


			// OR
			if (isAndPatern)
				return ( (mtch_symbol != null && mtch_symbol.find()) && ( mtch_content != null && mtch_content.find() ) );
			else
				return ( (mtch_symbol != null && mtch_symbol.find()) || ( mtch_content != null && mtch_content.find() ) );
	}


	private Object[] readEntryHelper(long rowidx) throws Exception
	{
		long indxOffset = rowidx * HtUtils.SOCKET_LENGTH_HEADER_BYTES;
		readerIdx_m.seek(indxOffset);

		byte[] buffer_ptr = new byte[HtUtils.SOCKET_LENGTH_HEADER_BYTES];
		int read = readerIdx_m.read(buffer_ptr);

		if (read != HtUtils.SOCKET_LENGTH_HEADER_BYTES) {
			throw new HtFrameException("Cannot read from cache index file: " + tmpfile_idx_m.getName() + " row idx=" + rowidx);
		}

		long fileOffset = HtMathUtils.convertByteToLong(buffer_ptr);


		reader_m.seek(fileOffset);

		byte[] buffer_length_ptr = new byte[HtUtils.SOCKET_LENGTH_HEADER_BYTES];
		read = reader_m.read(buffer_length_ptr);
		if (read != HtUtils.SOCKET_LENGTH_HEADER_BYTES) {
			throw new HtFrameException("Cannot read byte array length from cache file: " + tmpfile_m.getName() + " row idx=" + rowidx);
		}

		long chunk_size = HtMathUtils.convertByteToLong(buffer_length_ptr);
		byte[] buffer_data = new byte[(int) chunk_size];

		read = reader_m.read(buffer_data);
		if (read != chunk_size) {
			throw new HtFrameException("Cannot read byte array from cache file: " + tmpfile_m.getName() + " row idx=" + rowidx);
		}

		return deserializeFromBytes(buffer_data);
	}
	
	private long flushAll() throws Exception
	{
		synchronized (mtx_m) {
			writer_m.getFD().sync();
			writerIdx_m.getFD().sync();

			return curCounter_m;
		}
	}



	Object[] deserializeFromBytes(byte[] data) throws Exception {
		ByteArrayInputStream bis = new ByteArrayInputStream(data);
		ObjectInputStream oin = new ObjectInputStream(bis);

		FileStructure fs = (FileStructure) oin.readObject();
		return fs.data_m;
	}

	byte[] serializeToBytes(Object[] row) throws Exception {
		FileStructure fs = new FileStructure(row);
		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		ObjectOutputStream oos = new ObjectOutputStream(bos);

		oos.writeObject(fs);
		oos.flush();
		oos.close();

		// write data - length and then data itself
		return bos.toByteArray();

	}
}
