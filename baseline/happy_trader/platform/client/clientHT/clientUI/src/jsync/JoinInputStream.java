//-< JoinInputStream.java >------------------------------------------*--------*
// JSYNC                      Version 1.04       (c) 1998  GARRET    *     ?  *
// (Java synchronization classes)                                    *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Jul-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Mutiplexing input from several threads.
//-------------------------------------------------------------------*--------*

package jsync;

import java.io.*;

/** Class multiplexing input from several streams. This class merge several 
 *  input streams into single input stream. Size of input buffer 
 *  determines maximal amount of data, which can be read from one stream
 *  without interleaving with data from other streams.
 */
public class JoinInputStream extends InputStream { 
    /** Create stream joining streams specified in the array.
     * 
     * @param streams array with input streams which should be joined
     * @param bufferSize specifies size of read buffer
     */
    public JoinInputStream(InputStream[] streams, int bufferSize) 
    { 
	openedStreams = nReaders = streams.length;
	reader = new Reader[nReaders];
	for (int i = 0; i < nReaders; i++) { 
	    reader[i] = new Reader(this, streams[i], bufferSize);
	    reader[i].start();
	}
	currentReader = 0;
    }

    /** Create stream joining two specified streams.
     * 
     * @param one first input stream to be merged
     * @param two second input stream to be merged
     * @param bufferSize specifies size of read buffer
     */
    public JoinInputStream(InputStream one, InputStream two, int bufferSize) { 
	this(new InputStream[] { one, two}, bufferSize);
    }

    /** Create stream joining two specified streams.
     * 
     * @param one first input stream to be merged
     * @param two second input stream to be merged
     */
    public JoinInputStream(InputStream one, InputStream two) {
	this(one, two, defaultBufferSize); 
    }
    
    /** Reads the next byte of data from one of input streams. The value 
     *  byte is returned as an <code>int</code> in the range 
     *  <code>0</code> to <code>255</code>. If no byte is available 
     *  because the end of the stream has been reached, the value 
     *  <code>-1</code> is returned. This method blocks until input data 
     *  is available, the end of the stream is detected or an exception 
     *  is catched.<p>
     * 
     * @return     the next byte of data, or <code>-1</code> if the end of the
     *             stream is reached.
     * @exception  IOException  if an I/O error occurs.
     */
    public synchronized int read() throws IOException { 
	while (openedStreams != 0) { 
	    for (int i = 0; i < nReaders; i++) { 
		Reader rd = reader[currentReader];
		if (rd.available > 0) { 
		    return rd.read();
		}
		currentReader += 1;
		if (currentReader == nReaders) { 
		    currentReader = 0;
		}
	    } 
	    try { 
		wait();
	    } catch(InterruptedException ex) { 
		break;
	    }
	}
	return -1;
    }

    /**
     * Reads up to <code>len</code> bytes of data from one of input streams 
     * into an array of bytes. This method blocks until some input is 
     * available. If the first argument is <code>null,</code> up to 
     * <code>len</code> bytes are read and discarded. 
     * <p>
     * The <code>read</code> method of <code>InputStream</code> reads a 
     * single byte at a time using the read method of zero arguments to 
     * fill in the array. Subclasses are encouraged to provide a more 
     * efficient implementation of this method. 
     *
     * @param      b     the buffer into which the data is read.
     * @param      off   the start offset of the data.
     * @param      len   the maximum number of bytes read.
     * @return     the total number of bytes read into the buffer, or
     *             <code>-1</code> if there is no more data because the end of
     *             the stream has been reached.
     * @exception  IOException  if an I/O error occurs.
     */
    public synchronized int read(byte b[], int off, int len) throws IOException
    {
	while (openedStreams != 0) { 
	    for (int i = 0; i < nReaders; i++) { 
		Reader rd = reader[currentReader];
		if (rd.available > 0) { 
		    return rd.read(b, off, len);
		}
		currentReader += 1;
		if (currentReader == nReaders) { 
		    currentReader = 0;
		}
	    } 	
	    try { 
		wait();
	    } catch(InterruptedException ex) { 
		break;
	    }
	}
	return -1;
    } 

    /** Close all attached input streams and stop their listener threads.
     */
    public synchronized void close() throws IOException { 
	for (int i = 0; i < nReaders; i++) { 
	    if (reader[i].available >= 0) { 
	        reader[i].close();
	    }
	}
    }
	
    /** Get index of thread from which data was retrieved in last 
     *  <code>read</code> operation. Indices are started from 0.
     * 
     * @return index of thread from which data was taken in last 
     *  <code>read</code> operation.
     */
    public final int getStreamIndex() { 
	return currentReader;
    }

    /** Default size of read buffer.
     */
    static public int defaultBufferSize = 4096;

    protected int      nReaders;
    protected Reader[] reader;
    protected int      currentReader;
    protected int      openedStreams;
}

class Reader extends Thread { 
    int             available;
    int             pos;
    byte[]          buffer; 
    InputStream     stream;
    IOException     exception;
    JoinInputStream monitor;

    Reader(JoinInputStream monitor, InputStream stream, int bufferSize) { 
	this.stream = stream;
	this.monitor = monitor;
	buffer = new byte[bufferSize];
	available = 0;
	pos = 0;
	exception = null;
    }
    
    public synchronized void run() { 
	while (true) { 
	    int len;
	    try { 
		len = stream.read(buffer);
	    } catch(IOException ex) { 
		exception = ex;
		len = -1;
	    }
	    synchronized (monitor) { 
		available = len;
		pos = 0;
		monitor.notify();
		if (len < 0) { 	
		    try { 
		        stream.close();
		    } catch(IOException ex) {}
		    monitor.openedStreams -= 1;
		    return;
		}
	    }
	    do { 
	        try { 
		    wait();
		} catch(InterruptedException ex) { 
 		    return;
		}
	    } while(available != 0); // preclude spurious wakeups 
	}
    }
    
    synchronized int read() throws IOException { 
	if (exception != null) { 
	    throw exception;
	}
	int ch = buffer[pos] & 0xFF;
	if (++pos == available) { 
	    available = 0;
	    notify();
	}
	return ch;
    }

    synchronized int read(byte[] b, int off, int len) throws IOException { 
	if (exception != null) { 
	    throw exception;
	}
	if (available - pos <= len) { 
	    len = available - pos;
	    available = 0;
	    notify();
	}
	System.arraycopy(buffer, pos, b, off, len);
	pos += len;
	return len;
    }

    void close() throws IOException {
	//stop();
	interrupt();
	stream.close();
    }
}

