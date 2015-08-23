/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.utils.hlpstruct;

import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtMathUtils;
import com.fin.httrader.utils.HtUtils;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.util.List;
import java.util.logging.Level;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

/**
 *
 * @author DanilinS
 * This class is intende to exchange packets serializing/deserializing this
 */
public class HtXmlEventExchangePacket {

	public static final int PACKET_LIST_RTDATA =		6; // do not pass event list but pass rtdata list for saving resources
	public static final int PACKET_FINISH_HISTORY = 5;
	public static final int PACKET_LIST_EVENTS =		4;
	public static final int PACKET_SINGLE_EVENT =		3;
	public static final int PACKET_ERROR =					2;
	public static final int PACKET_HEARTBEAT =			1;
	public static final int PACKET_DUMMY =					0;

	// this represents standard packet
	// whcih can be passed through socket layer
	// for HTTP other type of encoding must be provided
	public static class Packet {

		public int packet_type_m = PACKET_DUMMY;
		public int packet_byte_length_m = -1;
		public byte[] data_m = null;

		public Packet() {
		}

		public Packet(int type) {
			packet_type_m = type;
			packet_byte_length_m = 1;
			data_m = new byte[1];

		}

		public void clear() {
			packet_type_m = PACKET_DUMMY;
			packet_byte_length_m = -1;
			data_m = null;
		}
	}

	public static interface LengthListener {

		public void onContentLength(int contentLength) throws Exception;

		public void onChunkFinish() throws Exception;
	};

	public static interface RtDataListener {

		public void onRtDataArrived(HtRtData rtdata) throws Exception;
	};

	public static interface ByteChunkListener {

		public void onByteChunkArrived(byte[] data) throws Exception;
	}

	public static class DataListSerializator {

		private Packet packet_m;
		private ZipOutputStream zout_m = null;
		private ByteArrayOutputStream bout_m = null;
		private long entryCount_m = 0;

		public DataListSerializator(Packet packet) {
			packet_m = packet;

		}

		public long getEntryCount() {
			return entryCount_m;
		}

		

		public void initizalize() throws Exception {
			if (zout_m == null) {
				bout_m = new ByteArrayOutputStream();
				zout_m = new ZipOutputStream(bout_m);
				zout_m.putNextEntry(new ZipEntry("data_entry"));
			}

		}

		public void addNewEntry(byte[] datachunk) throws Exception {

			byte[] datachunk_lengh_b = HtMathUtils.convertIntToByte4(datachunk.length);

			zout_m.write(datachunk_lengh_b);
			zout_m.write(datachunk);

			entryCount_m++;

		//HtLog.log(Level.INFO, "DataListSerializator", "addNewEntry", "Wrote chunk of length: " + (datachunk.length + datachunk_lengh_b.length));
		}

		public void finishSerialization(int packet_type) throws Exception {
			if (zout_m != null) {
					zout_m.close();

					packet_m.data_m = bout_m.toByteArray();
					packet_m.packet_byte_length_m = bout_m.size();
					packet_m.packet_type_m = packet_type;

					bout_m = null;
					zout_m = null;
					entryCount_m = 0;

			}
		
		}

		public void discardSerialization() throws Exception
		{
			if (zout_m != null) {
				zout_m.close();
				bout_m = null;
				zout_m = null;
				entryCount_m = 0;
			}
		}



		public Packet getPacket() {
			return packet_m;
		}

		public static void deserializeList(ByteChunkListener listener, Packet packet) throws Exception {

			ByteArrayInputStream bin = new ByteArrayInputStream(packet.data_m);


			ZipInputStream zis = new ZipInputStream(bin);
			ZipEntry entry = zis.getNextEntry();

			//int count;

			
			int datachunk_lengh = -1;

			if (entry != null) {


				while (true) {

					byte[] datachunk_lengh_b = forciblyReadFromStream(zis, 4);
					if (datachunk_lengh_b == null)
						break;
					datachunk_lengh = HtMathUtils.convertByteToInt4(datachunk_lengh_b);

					if (datachunk_lengh <= 0) {
						throw new HtException("DataListSerializator", "deserializeList", "Invaid chunk length");
					}

					byte[] buffer = forciblyReadFromStream(zis, datachunk_lengh);
					if (buffer == null)
						throw new HtException("DataListSerializator", "deserializeList", "Cannot read data from zipped stream of size: " + datachunk_lengh);
					
					listener.onByteChunkArrived(buffer);

				}
			}

			zis.close();
		}
	}

	// ----------------------------
	public static String getContext() {
		return HtXmlEventExchangePacket.class.getSimpleName();
	}

	// need this as sometimes we cannot read whole data from stream so need to read it byte by byte until -1 is retreived
	// returns null if cannot read
	public static byte[] forciblyReadFromStream(InputStream in, int datachunk_lengh) throws Exception
	{

		byte[] buffer = new byte[datachunk_lengh];
		int total = 0;
		byte[] tmp_buf = new byte[2048];
		int remaining = datachunk_lengh;

		while (total < datachunk_lengh) {
			int to_read = remaining;

			if (to_read > 2048) {
				to_read = 2048;
			}

			int count = in.read(tmp_buf, 0, to_read);

			if (count == -1) {
				return null;
			}

			System.arraycopy(tmp_buf, 0, buffer, total, count);

			total += count;
			remaining -= count;
		}

		return buffer;
	}

	public static void readPacketFromStream(Packet packet, InputStream in) throws Exception {
		// first type
		packet.clear();

		byte[] packet_type_buf = forciblyReadFromStream(in, HtUtils.SOCKET_LENGTH_HEADER_BYTES);
		if (packet_type_buf==null) {
			throw new HtException(getContext(), "readPacketFromStream", "Cannot read packet type field");
		}

		byte[] packet_byte_length_buf = forciblyReadFromStream(in, HtUtils.SOCKET_LENGTH_HEADER_BYTES);
		if (packet_byte_length_buf == null) {
			throw new HtException(getContext(), "readPacketFromStream", "Cannot read packet byte length field");
		}

		packet.packet_type_m = (int) HtMathUtils.convertByteToLong(packet_type_buf);
		packet.packet_byte_length_m = (int) HtMathUtils.convertByteToLong(packet_byte_length_buf);

		// read body
		packet.data_m = forciblyReadFromStream(in, packet.packet_byte_length_m);
		if (packet.data_m==null)
			throw new HtException(getContext(), "readPacketFromStream", "Cannot read packet data from stream, expected length: " + packet.packet_byte_length_m);


	}

	public static int writePacketToStream(Packet packet, OutputStream out, LengthListener clistener) throws Exception {
		if (packet.packet_byte_length_m <= 0 || packet.data_m == null) {
			throw new HtException(getContext(), "writePacketToStream", "Invalid packet for writing");
		}

		byte[] packet_byte_length_buf = HtMathUtils.convertLongToByte((long) packet.packet_byte_length_m);
		byte[] packet_type_buf = HtMathUtils.convertLongToByte((long) packet.packet_type_m);

		int totallength = packet_type_buf.length + packet_byte_length_buf.length + packet.data_m.length;


		if (clistener != null) {
			clistener.onContentLength(totallength);
		}

		out.write(packet_type_buf);
		out.write(packet_byte_length_buf);
		out.write(packet.data_m);

		if (clistener != null) {
			clistener.onChunkFinish();
		}

		return totallength;
	}

	public static void serializeSingleEvent(XmlHandler handler, XmlEvent event, Packet packet) throws Exception {
		XmlParameter xmlparameter = new XmlParameter();
		XmlEvent.convertEventToXmlParameter(event, xmlparameter);
		StringBuilder outs = new StringBuilder();

		handler.serializeParameter(xmlparameter, outs);
		byte[] data = outs.toString().getBytes("UTF-8");


		packet.data_m = data;
		packet.packet_byte_length_m = data.length;
		packet.packet_type_m = PACKET_SINGLE_EVENT;


	}

	// read the consequent portion from the stream
	public static XmlEvent deserializeSingleEvent(XmlHandler handler, Packet packet) throws Exception {

		if (packet.packet_type_m != PACKET_SINGLE_EVENT) {
			throw new HtException(getContext(), "deserialize", "Invalid packet type for chunk extraction");
		}


		String datas = new String(packet.data_m, "UTF-8");
		XmlParameter xmlparameter = new XmlParameter();
		handler.deserializeParameter(datas, xmlparameter);

		XmlEvent eventdata = new XmlEvent();
		XmlEvent.convertXmlParameterToEvent(eventdata, xmlparameter);

		return eventdata;
	}

	// ---------------
	public static void serializeAsErrorMessage(String errorMsg, Packet packet) throws Exception {
		packet.packet_type_m = PACKET_ERROR;
		packet.data_m = errorMsg.getBytes("UTF-8");
		packet.packet_byte_length_m = packet.data_m.length;
	}

	public static String deserializeAsErrorMessage(Packet packet) throws Exception {
		if (packet.packet_type_m != PACKET_ERROR) {
			throw new HtException(getContext(), "deserializeAsErrorMessage", "Invalid packet type");
		}

		return new String(packet.data_m, "UTF-8");
	}
	// --------------------
}
