/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package htrangebreaker.utils;

import com.fin.httrader.utils.HtFileUtils;
import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.tuple.TupleBinding;
import com.sleepycat.bind.tuple.TupleInput;
import com.sleepycat.bind.tuple.TupleOutput;
import com.sleepycat.je.Cursor;
import com.sleepycat.je.Database;
import com.sleepycat.je.DatabaseConfig;
import com.sleepycat.je.DatabaseEntry;
import com.sleepycat.je.DatabaseException;
import com.sleepycat.je.Environment;
import com.sleepycat.je.EnvironmentConfig;
import com.sleepycat.je.LockConflictException;
import com.sleepycat.je.LockMode;
import com.sleepycat.je.OperationStatus;
import com.sleepycat.je.SecondaryConfig;
import com.sleepycat.je.SecondaryDatabase;
import com.sleepycat.je.SecondaryKeyCreator;
import com.sleepycat.je.Transaction;
import com.sleepycat.je.TransactionConfig;
import htrangebreaker.utils.gui.DiskCachedTableModel;
import java.io.File;
import java.io.IOException;
import java.util.regex.Pattern;

/**
 *
 * @author DanilinS
 * based on Berlkley DB
 */
public class EventBerkleyDbBuffer implements DiskCachedTableModel.DiskCachedTableModelInterface {

	public static interface RowExportReceiver {

		public long getExportedRowsCount();

		public void onExportBegin();

		public void onExportFinish();

		public boolean onMatchingRowArrived(Object[] row);
	}

	public static class SymbolKeyCreator implements SecondaryKeyCreator {

		private TupleBinding theBinding_m;

		public SymbolKeyCreator(TupleBinding theBinding1) {
			theBinding_m = theBinding1;
		}

		public boolean createSecondaryKey(SecondaryDatabase secDb,
						DatabaseEntry keyEntry,
						DatabaseEntry dataEntry,
						DatabaseEntry resultEntry) {

			try {
				String symbol = (String) theBinding_m.entryToObject(dataEntry);
				resultEntry.setData(symbol.getBytes("UTF-8"));

			} catch (IOException willNeverOccur) {
			}
			return true;
		}
	}

	public static class ObjectArrayTupleBinding extends TupleBinding {

		// Write a MyData2 object to a TupleOutput
		public void objectToEntry(Object object, TupleOutput to) {

			Object[] data = (Object[]) object;
			if (data != null) {
				int length = data.length;
				to.writeInt(length);

				for (int i = 0; i < length; i++) {
					String val = data[i].toString();

					to.writeString(val);
				}
			} else {
				to.writeInt(0);
			}

		}

		// Convert a TupleInput to a MyData2 object
		public Object entryToObject(TupleInput ti) {

			// Data must be read in the same order that it was
			// originally written.

			int length = ti.readInt();

			if (length > 0) {
				Object[] result = new Object[length];
				for (int i = 0; i < length; i++) {
					result[i] = ti.readString();
				}

				return result;
			}

			return null;

		}
	};

	// ---------------------------------
	private static final int COMMIT_RECORDS_NUMBER = 1000;
	private Database db_m = null;
	private Database symbolKeydb_m = null;
	private static Environment env_m = null;
	private File tmpDir_m = null;
	private long counter_m = 0;
	private Object counterMtx_m = new Object();
	private EntryBinding keyBind_m = TupleBinding.getPrimitiveBinding(Long.class);
	private ObjectArrayTupleBinding dataBind_m = new ObjectArrayTupleBinding();
	private Transaction writeTransactionHandle_m = null;

	// ------------------------------
	public void open() throws DatabaseException, IOException {


		synchronized (counterMtx_m) {
			counter_m = 0;
		}

		// Set up the environment.
		EnvironmentConfig myEnvConfig = new EnvironmentConfig();
		myEnvConfig.setAllowCreate(true);
		myEnvConfig.setTransactional(true);
		myEnvConfig.setTxnSerializableIsolation(false); // for fast performance



		/*
		SymbolKeyCreator fnkc = new SymbolKeyCreator(TupleBinding.getPrimitiveBinding(String.class));
		SecondaryConfig mySecConfig = new SecondaryConfig();
		mySecConfig.setAllowCreate(true);
		mySecConfig.setTransactional(true);
		mySecConfig.setSortedDuplicates(false);

		mySecConfig.setKeyCreator(fnkc);
		 */





		// Environment handles are free-threaded in JE,
		// so we do not have to do anything to cause the
		// environment handle to be free-threaded.

		// Set up the database
		DatabaseConfig myDbConfig = new DatabaseConfig();
		myDbConfig.setAllowCreate(true);
		myDbConfig.setTransactional(true);
		myDbConfig.setSortedDuplicates(false);


		// no DatabaseConfig.setThreaded() method available.
		// db handles in java are free-threaded so long as the
		// env is also free-threaded.

		// Open the environment
		tmpDir_m = HtFileUtils.createTempDirectory();

		env_m = new Environment(tmpDir_m, // Env home
						myEnvConfig);

		// Open the database. Do not provide a txn handle. This open
		// is autocommitted because DatabaseConfig.setTransactional()
		// is true.
		db_m = env_m.openDatabase(null, // txn handle
						"tmp_database", // Database file name
						myDbConfig);



		// secondary
		//symbolKeydb_m = env_m.openSecondaryDatabase(null, "tmp_database_symbol", db_m, mySecConfig);

		writeTransactionHandle_m = env_m.beginTransaction(null, null);


	}

	public void close() {

		abortTransactionWrapper(writeTransactionHandle_m);


		try {
			if (symbolKeydb_m != null) {
				symbolKeydb_m.close();
			}
		} catch (DatabaseException e) {
			HtFrameLogger.logError("Cannot close DB handle: " + e.getMessage());
		}



		try {
			if (db_m != null) {
				db_m.close();
			}
		} catch (DatabaseException e) {
			HtFrameLogger.logError("Cannot close DB handle: " + e.getMessage());
		}



		try {
			if (env_m != null) {
				env_m.close();
			}
		} catch (DatabaseException e) {
			HtFrameLogger.logError("Cannot close ENV handle: " + e.getMessage());
		}

		if (tmpDir_m != null) {
			try {
				HtFileUtils.deleteDirectory(tmpDir_m);
			} catch (Throwable e) {
			}
		}
	}

	public void addEntry(Object[] row) throws Exception {

		// resolve index
		long idx = -1;
		synchronized (counterMtx_m) {
			idx = Long.valueOf(counter_m);
		}


		try {

			//

			DatabaseEntry data_data = new DatabaseEntry();
			dataBind_m.objectToEntry(row, data_data);


			db_m.put(writeTransactionHandle_m, createKeyEntry(idx), data_data);

			if ((idx % COMMIT_RECORDS_NUMBER) == 0) {
				try {
					writeTransactionHandle_m.commit();
					writeTransactionHandle_m = env_m.beginTransaction(null, null);
				//HtFrameLogger.log("Commiting: " + idx+ " rows");

				} catch (DatabaseException e) {
					HtFrameLogger.logError("Error commiting transaction: " + e.getMessage());
				}
			}


		} catch (LockConflictException de) {
			throw new HtFrameException("Lock conflict: " + de.getMessage());
		} catch (DatabaseException e) {
			throw new HtFrameException("DB exception: " + e.getMessage());
		}

		//
		synchronized (counterMtx_m) {
			counter_m++;
		}
	}

	public Object[] getEntry(long rowidx) throws Exception {

		Transaction txn = null;
		Object[] row = null;

		try {

			DatabaseEntry data_data = new DatabaseEntry();


			txn = env_m.beginTransaction(null, null);

			// here is read uncommited for faster performance as we only add data
			if (db_m.get(txn, createKeyEntry(rowidx), data_data, LockMode.READ_UNCOMMITTED) == OperationStatus.SUCCESS) {
				row = (Object[]) dataBind_m.entryToObject(data_data);
			} else {
				throw new HtFrameException("Cannot find record with key: " + rowidx);
			}

			commitTransactionWrapper(txn);
			txn = null;

			


		} catch (LockConflictException de) {
			throw new HtFrameException("Lock conflict: " + de.getMessage());
		} catch (DatabaseException e) {
			throw new HtFrameException("DB exception: " + e.getMessage());
		} finally {
			abortTransactionWrapper(txn);
		}

		return row;
	}

	public long getCurCounter() {
		synchronized (counterMtx_m) {
			return counter_m;
		}
	}

	public long findNextRow(
					String filterSymbolValue,
					int symbolColumnId,
					long begin_row_idx,
					boolean include_first_index // if to include in the search begin_row_idx
	) throws Exception
	{
		Transaction txn = null;
		Cursor cursor = null;
		long result = -1;

		try {

			if (begin_row_idx <0)
				begin_row_idx = 0;

			txn = env_m.beginTransaction(null, null);

			DatabaseEntry foundKey = new DatabaseEntry();
			DatabaseEntry foundData = new DatabaseEntry();

			cursor = db_m.openCursor(null, null);

			// search initial key
			OperationStatus retVal =cursor.getSearchKey(createKeyEntry(begin_row_idx), foundData, LockMode.READ_UNCOMMITTED);

			if (retVal == OperationStatus.SUCCESS) {
				if (!include_first_index) {
					// advance
					retVal = cursor.getNext(foundKey, foundData, LockMode.READ_UNCOMMITTED);
				}
			}


			while (retVal == OperationStatus.SUCCESS) {
				// getData() on the DatabaseEntry objects returns the byte array
				// held by that object. We use this to get a String value. If the
				// DatabaseEntry held a byte array representation of some other
				// data type (such as a complex object) then this operation would
				// look considerably different.
				Object[] row = (Object[]) dataBind_m.entryToObject(foundData);
				String svalue_i = (String) row[symbolColumnId];

				if (filterSymbolValue != null) {
					if (svalue_i.equalsIgnoreCase(filterSymbolValue)) {
						result = getValueFromKey(foundKey);
						break;
					}
				}


				retVal = cursor.getNext(foundKey, foundData, LockMode.READ_UNCOMMITTED);

			}


			commitTransactionWrapper(txn);
			txn = null;


		} catch (LockConflictException de) {
			throw new HtFrameException("Lock conflict: " + de.getMessage());
		} catch (DatabaseException e) {
			throw new HtFrameException("DB exception: " + e.getMessage());
		} finally {
			if (cursor != null) {

				try {
					cursor.close();
				} catch (Exception e) {
					HtFrameLogger.logError("Error closing search: " + e.getMessage());
				}
			}

			abortTransactionWrapper(txn);
		}

		return result;

	}

	// need to search opposite direction
	public long findPrevRow(
					String filterSymbolValue,
					int symbolColumnId,
					long begin_row_idx,
					boolean include_first_index
	) throws Exception
	{
		Transaction txn = null;
		Cursor cursor = null;
		long result = -1;

		try {

			
			txn = env_m.beginTransaction(null, null);

			DatabaseEntry foundKey = new DatabaseEntry();
			DatabaseEntry foundData = new DatabaseEntry();

			cursor = db_m.openCursor(null, null);
			OperationStatus retVal = OperationStatus.NOTFOUND;

			// search initial key
			if (begin_row_idx < 0)
				retVal =cursor.getLast(foundKey, foundData, LockMode.READ_UNCOMMITTED);
			else
				retVal =cursor.getSearchKey(createKeyEntry(begin_row_idx), foundData, LockMode.READ_UNCOMMITTED);

			if (retVal == OperationStatus.SUCCESS) {
				if (!include_first_index) {
					// advance
					retVal = cursor.getPrev(foundKey, foundData, LockMode.READ_UNCOMMITTED);
				}
			}


			while (retVal == OperationStatus.SUCCESS) {
				// getData() on the DatabaseEntry objects returns the byte array
				// held by that object. We use this to get a String value. If the
				// DatabaseEntry held a byte array representation of some other
				// data type (such as a complex object) then this operation would
				// look considerably different.
				Object[] row = (Object[]) dataBind_m.entryToObject(foundData);
				String svalue_i = (String) row[symbolColumnId];

				if (filterSymbolValue != null) {
					if (svalue_i.equalsIgnoreCase(filterSymbolValue)) {
						result = getValueFromKey(foundKey);
						break;
					}
				}


				retVal = cursor.getPrev(foundKey, foundData, LockMode.READ_UNCOMMITTED);

			}


			commitTransactionWrapper(txn);
			txn = null;

		} catch (LockConflictException de) {
			throw new HtFrameException("Lock conflict: " + de.getMessage());
		} catch (DatabaseException e) {
			throw new HtFrameException("DB exception: " + e.getMessage());
		} finally {
			if (cursor != null) {

				try {
					cursor.close();
				} catch (Exception e) {
					HtFrameLogger.logError("Error closing search: " + e.getMessage());
				}
			}

			abortTransactionWrapper(txn);
		}

		return result;
	}

	public void exportUtility(String filterSymbolValue, int symbolColumnId, RowExportReceiver exportInterface) throws Exception {
		Transaction txn = null;
		Cursor cursor = null;

		exportInterface.onExportBegin();

		try {



			txn = env_m.beginTransaction(null, null);

			DatabaseEntry foundKey = new DatabaseEntry();
			DatabaseEntry foundData = new DatabaseEntry();

			cursor = db_m.openCursor(null, null);

			while (cursor.getNext(foundKey, foundData, LockMode.READ_UNCOMMITTED) == OperationStatus.SUCCESS) {
				// getData() on the DatabaseEntry objects returns the byte array
				// held by that object. We use this to get a String value. If the
				// DatabaseEntry held a byte array representation of some other
				// data type (such as a complex object) then this operation would
				// look considerably different.
				Object[] row = (Object[]) dataBind_m.entryToObject(foundData);
				String svalue_i = (String) row[symbolColumnId];

				boolean stop = false;
				if (filterSymbolValue != null) {
					if (svalue_i.equalsIgnoreCase(filterSymbolValue)) {
						stop = exportInterface.onMatchingRowArrived(row);
					}
				} else {
					stop = exportInterface.onMatchingRowArrived(row);
				}

				if (stop) {
					break;
				}

			}


			commitTransactionWrapper(txn);
			txn = null;
		



		} catch (LockConflictException de) {
			throw new HtFrameException("Lock conflict: " + de.getMessage());
		} catch (DatabaseException e) {
			throw new HtFrameException("DB exception: " + e.getMessage());
		} finally {
			if (cursor != null) {

				try {
					cursor.close();
				} catch (Exception e) {
					HtFrameLogger.logError("Error closing search: " + e.getMessage());
				}
			}

			abortTransactionWrapper(txn);
		}

		exportInterface.onExportFinish();


	}

	/**
	 * Helpers
	 * /
	 */
	private DatabaseEntry createKeyEntry(long key) {
		DatabaseEntry key_data = new DatabaseEntry();
		keyBind_m.objectToEntry(Long.valueOf(key), key_data);

		return key_data;
	}

	private void abortTransactionWrapper(Transaction txn) {
		if (txn != null) {

			try {
				txn.abort();
			} catch (Exception e) {
				HtFrameLogger.logError("Error aborting transaction: " + e.getMessage());
			}

		}
	}

	private void commitTransactionWrapper(Transaction txn) {
		if (txn != null) {

			try {
				txn.commit();
			} catch (Exception e) {
				HtFrameLogger.logError("Error comitting transaction: " + e.getMessage());
			}

		}
	}

	private long getValueFromKey(DatabaseEntry db_key) {
		Long obj = (Long) keyBind_m.entryToObject(db_key);
		if (obj != null) {
			return obj.longValue();
		} else {
			return -1;
		}
	}
}
