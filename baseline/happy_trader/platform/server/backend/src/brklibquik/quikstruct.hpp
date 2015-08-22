#ifndef _BRKLIBQUIK_QUIKSTRUCT_INCLUDED
#define _BRKLIBQUIK_QUIKSTRUCT_INCLUDED

#include "brklibquikdefs.hpp"
#include "deserial.hpp"
#include "quiktablerows.hpp"

namespace BrkLib {



// -------------------------------------

// helper class containing the list of QuikSerialImpl entries
// it contains entries of type T

class QuikPosHolder;

template<class T>
class QuikTable
{
public:
	
	QuikTable():
	posHolder_m( NULL)
	{
	}

	~QuikTable()
	{
		resize(0);
	}

	
	
	void initialize(QuikPosHolder &posHolder, CppUtils::DDEItemList& itemList)
	{
		posHolder_m = &posHolder;
		CppUtils::DDEItem item( getDDETopic(), "R1C1:R1C1" );
		itemList.push_back(item);
	}

	void deinitialize()
	{
		resize(0);
		posHolder_m = NULL;
	}

	DataRowDescriptor& getRowDescriptor()
	{
		return rowDescriptor_m;
	}

	inline void onNewDataArrivedExt(CppUtils::DDEItem const& item, vector< vector <CppUtils::String> > const& data)
	{
		if (item.topic_m == getDDETopic()) {
			// this is our table
			onNewDataArrived( item, data );
		}
	}

	inline int size() const
	{
		return data_m.size();
	}


	inline T& getElement( int const idx)
	{
		return data_m[i];
	}

	inline vector<T> const& getDataVector() const
	{
		return data_m;
	}
	
	// returns DDE topic
	CppUtils::String const& getDDETopic() const
	{
		// just delegate from the base type
		return T::getDDETopicName();
	}

	CppUtils::String const& getQuikTableName() const
	{
		// just delegate from the base type
		return T::getQuikTableName();
	}
	

private:

	// resizes or data - either adding new rows or removing from the end of the list
	void resize(int const ns)
	{
		
		if (ns < 0)
			return;
	
		data_m.resize( ns );
		
	}

	// only forces the array to grow
	void grow(int const ns)
	{
		
		if (ns < 0)
			return;

		int cursize = data_m.size();
		for(int i = cursize; i <= ns; i++) {
			data_m.push_back( T() );
		}
	}


	// this is the main funcction delegated from QuikPosHolder telling that data is arrived
	void onNewDataArrived(CppUtils::DDEItem const& item, vector< vector <CppUtils::String> > const& data)
	{
		int rbeg, cbeg, rend, cend;
		parseExcelItem(item.item_m, cbeg, rbeg, cend, rend );
				
		
		// need to have valid indexes
		if (rbeg <= 0  || cbeg <=0 || rend <=0 || cend <= 0 )
			THROW(CppUtils::OperationFailed, "exc_InvalidDDEItemParse", "ctx_NewDataArrivedFromQuik", item.item_m );

		// get the first row
		if (data.size() > 0) {
			
			if (rowDescriptor_m.size() ==0) {
				// init only the first time
				CppUtils::StringList const& firstRow = data[0];

				// init the row descriptor
				for(int i = 0; i < firstRow.size(); i++) {
					FieldData fdata(i);
					rowDescriptor_m.registerColumn(firstRow[ i ], fdata );
				}
			}

			
			// if we have other data
			grow(rend-1);

			// expect we have our data here
			for(int i = 1; i < data.size(); i++) {
				CppUtils::StringList const& row_i = data[i];
				data_m[ rbeg + i - 2 ].initFromRow(row_i, rowDescriptor_m );

			}

			// notify
			if(posHolder_m) {
				posHolder_m->onQuikTableChanged<T>(*this, rbeg, cbeg, rend, cend);
			}

		}

	}

	

	void parseExcelItem(CppUtils::String const& item, int& cbeg, int& rbeg, int& cend, int& rend) const
	{
		// item == "R1C1:R2C2"
		cbeg = -1;
		rbeg = -1;
		cend = -1;
		rend = -1;

		sscanf( item.c_str(), "R%dC%d:R%dC%d", &rbeg, &cbeg, &rend, &cend);

	};


	// ---------------------------------------------

	DataRowDescriptor rowDescriptor_m;

	vector< T >  data_m;

	QuikPosHolder* posHolder_m;

};



}; // end of namespace

#endif