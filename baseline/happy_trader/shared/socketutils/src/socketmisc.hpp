#ifndef _SOCKETMISC_INCLUDED
#define _SOCKETMISC_INCLUDED

#define SOCKETLAYER "SOCKETLAYER"

// the length of array containg the length of the data packet
#define SOCKET_LENGTH_HEADER_BYTES 8


// helper structure to define the rt/level2 data packet to be received
struct HtInquotePacket
{
	
	static const CppUtils::Byte FLAG_RT_PACKET											= 1;
	
	static const CppUtils::Byte FLAG_LEVEL2_PACKET									= 2;

	static const CppUtils::Byte FLAG_LEVEL1_PACKET									= 3;

	static const CppUtils::Byte FLAG_XMLEVENT_PACKET								= 4;

	static const CppUtils::Byte FLAG_COMMON_SERVICE_REQUEST_HISTORY = 5;

	static const CppUtils::Byte FLAG_TRANSMITION_FINISH							= 6;

	static const CppUtils::Byte FLAG_COMMON_SERVICE_REQUEST_EXT_PACKAGE_RUN = 7;


	CppUtils::Byte flag_m;

	CppUtils::MemoryChunk data_m;

	CppUtils::String toString() const
	{
		CppUtils::String result;

		switch (flag_m) {
			case FLAG_RT_PACKET: 
				result+="RT"; 
				break;
			case FLAG_LEVEL2_PACKET: 
				result+="LEVEL2"; 
				break;
			case FLAG_LEVEL1_PACKET: 
				result += "LEVEL1";
				break;
			case FLAG_XMLEVENT_PACKET: 
				result += "XMLEVENT";
				break;
			default:
				result += "N/A";
				break;

		}

		result += " data, length="+ CppUtils::long2String(data_m.length());
		result += " \n" + data_m.toString(1024)+"\n";

		return result;
	}

	CppUtils::String dumpToFile()
	{
		long time_event =  CppUtils::getTime();
		CppUtils::String fileName = CppUtils::getStartupDir() + "HtInquotePacket_dump_"+CppUtils::long2String(time_event)+".xml"; 
		CppUtils::saveContentInFile2(fileName, data_m);

		return fileName;
	}
};

// helper function to read data if not provided in a single read function
template<class T>
bool readSmallByteArray(T& socket, CppUtils::Byte*buffer, size_t const buffer_length)
{
	HTASSERT(buffer_length>=0);

	int remained, total_read = 0;

	while(true) {
		remained = buffer_length - total_read;

		int read = socket.Receive(buffer + total_read, remained);
		if(read <= 0) {
			// this is error, just return
			LOG(MSG_ERROR, SOCKETLAYER, "When reading small byte array was not able to complete reading data");
			return false;
		};

		total_read += read;
		if (total_read >= buffer_length)
			break;

	}

	if (total_read != buffer_length)
		return false;

	return true;
}


// helper function to read this packet
template<class T>
bool readFromSocketHtInquotePacket(T& socket, HtInquotePacket& packet) {

	// type flag
	int read = socket.Receive((CppUtils::Byte *)&packet.flag_m, 1);
	if (read  != 1) {
		LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to read packet type flag");
		return false;
	}
	
	// length
	
	CppUtils::Byte packet_size_buf[SOCKET_LENGTH_HEADER_BYTES];
	

	if (!readSmallByteArray(socket, (CppUtils::Byte *)packet_size_buf, SOCKET_LENGTH_HEADER_BYTES)) {
		LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to read packet length");
		return false;
	}

	long packet_size = CppUtils::convert8BytesToLong(packet_size_buf);
	if (packet_size <=0) {
		LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to convert packet length");
		return false;
	}

	static const int MAX_BUF = 16384;
	//

	// packet body
	packet.data_m.length(packet_size);
	int total_read = 0;
	int remained, request_receive_length;
	
	while(true) {

		// the remainder
		remained = packet_size - total_read;
		if (remained > MAX_BUF )
			request_receive_length = MAX_BUF;
		else
			request_receive_length = remained;

		read = socket.Receive(packet.data_m.data() + total_read, request_receive_length);
		if(read <= 0) {
			// this is error, just return
			LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to complete reading data");
			return false;
		};

		total_read += read;
		if (total_read >= packet_size)
			break;

	}
	
	if (total_read != packet_size) {
		LOG(MSG_ERROR, SOCKETLAYER, "After reading packet actual and expected lengths are different");
		return false;
	}

	return true;

}

template<class T>
bool writeToSocketHtInquotePacket(T& socket, HtInquotePacket& packet) {
	
	// send flag
	int send = socket.Send(&packet.flag_m, 1);
	if (send != 1) {
		LOG(MSG_ERROR, SOCKETLAYER, "Cannot write packet type flag");
		return false;
	}

	CppUtils::Byte packet_size_buf[SOCKET_LENGTH_HEADER_BYTES];
	CppUtils::convertLongTo8Bytes(packet.data_m.length(), packet_size_buf);

	send = socket.Send((CppUtils::Byte *)packet_size_buf, SOCKET_LENGTH_HEADER_BYTES);
	if (send != SOCKET_LENGTH_HEADER_BYTES) {
		LOG(MSG_ERROR, SOCKETLAYER, "Cannot write packet length buffer");
		return false;
	}

	// packet body
	send = socket.Send(packet.data_m.data(), packet.data_m.length());
	if (send < packet.data_m.length()) {
		LOG(MSG_ERROR, SOCKETLAYER, "Cannot write packet data");
		return false;
	}

	return true;
}

	
template<class T>
int readFromSocket(T& socket, CppUtils::MemoryChunk& chunk) {
	
	// packet size
	int read;
	CppUtils::Byte packet_size_buf[SOCKET_LENGTH_HEADER_BYTES];

	if (!readSmallByteArray(socket, (CppUtils::Byte *)packet_size_buf, SOCKET_LENGTH_HEADER_BYTES)) {
		LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to read packet length");
		return -1;
	}
	
	long packet_size = CppUtils::convert8BytesToLong(packet_size_buf);
	if (packet_size <=0) {
		LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to convert packet length");
		return -1;
	}

	static const int MAX_BUF = 16384;
	
	// packet body
	chunk.length(packet_size);
	int total_read = 0;
	int remained = 0;
	int request_receive_length = 0;
	while(true) {

		// the remainder
		remained = packet_size - total_read;
		if (remained > MAX_BUF )
			request_receive_length = MAX_BUF;
		else
			request_receive_length = remained;

		read = socket.Receive(chunk.data() + total_read, request_receive_length);
		if(read < 0) {
			// this is error, just return
			LOG(MSG_ERROR, SOCKETLAYER, "When reading packet was not able to complete reading data");
			return -1;
		};

		total_read += read;
		if (total_read >= packet_size)
			break;

	
	}

	return total_read;
}

template<class T>
int writeToSocket(T& socket, CppUtils::MemoryChunk& chunk) {
	
	// packet size
	int send = -1;
	CppUtils::Byte packet_size_buf[SOCKET_LENGTH_HEADER_BYTES];
	CppUtils::convertLongTo8Bytes(chunk.length(), packet_size_buf);

	send = socket.Send((CppUtils::Byte *)packet_size_buf, SOCKET_LENGTH_HEADER_BYTES);
	if (send < 0) return send;

	// packet body
	send = socket.Send(chunk.data(), chunk.length());
	return send;

}

#endif