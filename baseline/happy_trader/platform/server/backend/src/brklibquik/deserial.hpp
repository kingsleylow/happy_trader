#ifndef _BRKLIBQUIK_DESERIAL_INCLUDED
#define _BRKLIBQUIK_DESERIAL_INCLUDED

#include "brklibquikdefs.hpp"


namespace BrkLib {

// helper describing a field in a QUIK table imported by DDE
struct FieldData
{
	FieldData():
		idx_m(-1)
	{
	}

	FieldData(int const idx, char const* format = NULL):
		idx_m(idx)
	{
		format_m = (format ? format: "");
	}

	//
	int idx_m;

	CppUtils::String format_m;
};

// contains mapping between columns names and the numbers
// initialized only once
class DataRowDescriptor
{
public:
	
	// register simple column
	void registerColumn(CppUtils::String const& columnName, FieldData const& fielddata);

	// returns -1 if not found
	FieldData const& getFieldData(CppUtils::String const& columnName) const;

	inline int size() const
	{
		return  columnMap_m.size();
	}

private:
	map<CppUtils::String, FieldData> columnMap_m;

};

// -------------------------

// helper to implement automatical deserialization
// DO NOT SUPPORT DERIVED CLASSES !!!!


template<class T>
class QuikSerialImpl {
public:
	typedef T TheClass;
	
	// this function is defined in derived classes
	// it initializes an instance of a structure amongst the declared
	// taking the names of fields as column names of decsriptor parameter  
	virtual void initFromRow(CppUtils::StringList const& datarow, DataRowDescriptor const& descriptor) = 0;
		

};

// ------------------------

#define DECLARE_DDE_TOPIC_HELPER( X )	\
	static CppUtils::String const& getDDETopicName()	\
	{	\
		static const CppUtils::String topic( #X );	\
		return topic;	\
	};	

#define DECLARE_QUIK_TABLE_NAME( X )	\
	static CppUtils::String const& getQuikTableName()	\
	{	\
		static const CppUtils::String topic( #X );	\
		return topic;	\
	};	


#define DECLARE_DDE_TOPIC(BOOK, SHEET) \
	DECLARE_DDE_TOPIC_HELPER([##BOOK##]##SHEET)


#define QUIK_BEGIN_SERIALIZE_MAP()	\
	public:	\
		virtual void initFromRow(CppUtils::StringList const& datarow, DataRowDescriptor const& descriptor) {	\
		int idx = -1;	\
		

#define QUIK_STRING_ENTRY(N)	\
		N = "";	\
		idx = descriptor.getFieldData(#N).idx_m;	\
		if (idx < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #N);	\
		\
		if (idx >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#N) + " - " + CppUtils::long2String(idx));	\
		\
		N = datarow[ idx ];	\
	

#define QUIK_DOUBLE_ENTRY(N)	\
		N = -1.0; \
		idx = descriptor.getFieldData(#N).idx_m;	\
		if (idx < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #N);	\
		\
		if (idx >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#N) + " - " + CppUtils::long2String(idx));	\
		\
		N = atof(datarow[ idx ].c_str());	\
	

#define QUIK_LONG_ENTRY(N)	\
		N = -1;	\
		idx = descriptor.getFieldData(#N).idx_m;	\
		if (idx < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #N);	\
		\
		if (idx >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#N) + " - " + CppUtils::long2String(idx));	\
		\
		N = atol(datarow[ idx ].c_str());	\


#define QUIK_INT_ENTRY(N)	\
		N = -1;	\
		idx = descriptor.getFieldData(#N).idx_m;	\
		if (idx < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #N);	\
		\
		if (idx >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#N) + " - " + CppUtils::long2String(idx));	\
		\
		N = atoi(datarow[ idx ].c_str());	\

// translate string values of date/time to double (unix time)
#define QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY(NDESTCOL, NDATECOL, NTIMECOL)	\
	NDESTCOL = -1.0;	\
	FieldData const& date_field = descriptor.getFieldData(#NDATECOL);	\
	if (date_field.idx_m < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #NDATECOL);	\
	\
	FieldData const& time_field = descriptor.getFieldData(#NTIMECOL);	\
	if (time_field.idx_m < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #NTIMECOL);	\
	\
	if (date_field.idx_m >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#NDATECOL) + " - " + CppUtils::long2String(date_field.idx_m));	\
	\
	if (time_field.idx_m >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#NTIMECOL) + " - " + CppUtils::long2String(time_field.idx_m));	\
	\
	NDESTCOL = CppUtils::parseFormattedDateTime(date_field.format_m, NDATECOL ) + CppUtils::parseFormattedDateTime(time_field.format_m, NTIMECOL );	\
	

#define QUIK_TRANSLATE_FROM_DATE_TIME_COLS_ENTRY2(NDESTCOL, NDATECOL, NTIMECOL, FMTDATE, FMTTIME)	\
	NDESTCOL = -1.0; \
	FieldData const& date_field = descriptor.getFieldData(#NDATECOL);	\
	if (date_field.idx_m < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #NDATECOL);	\
	\
	FieldData const& time_field = descriptor.getFieldData(#NTIMECOL);	\
	if (time_field.idx_m < 0)	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableColumnNotRegistered", "ctx_InitFromRow", #NTIMECOL);	\
	\
	if (date_field.idx_m >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#NDATECOL) + " - " + CppUtils::long2String(date_field.idx_m));	\
	\
	if (time_field.idx_m >= datarow.size())	\
			THROW(CppUtils::OperationFailed, "exc_QuikTableRegisteredColumnMoreThanDataVector", "ctx_InitFromRow", CppUtils::String(#NTIMECOL) + " - " + CppUtils::long2String(time_field.idx_m));	\
	\
	NDESTCOL = CppUtils::parseFormattedDateTime(FMTDATE, NDATECOL,0 ) + CppUtils::parseFormattedDateTime(FMTTIME, NTIMECOL,0 ); \


#define	QUIK_END_SERIALIZE_MAP()	\
	};	\
	public:	

}; // end of namespace

#endif