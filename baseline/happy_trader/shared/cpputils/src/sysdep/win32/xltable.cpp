#include "xltable.hpp"
#include "../../except.hpp"
#include "../../misc_ht.hpp"
#include "../../assertdef.hpp"

namespace CppUtils {

void crackXtableData(vector< vector <String> >& out, CppUtils::Byte const* pData, size_t pDataLength)
{
	out.clear();

	// cells are inside elements, rows are external elements
	
	// begin reading
	int state = 0; // read block type
	// == 1 - read length
	// == 2 - read data itself
	// == 3 - finished

	WORD tdt;            /* the table data type */
	WORD cb;							/* the size (count of bytes) of the data */            

	


	// the size of our transfer
	WORD rowsnum, colsnum;
	CppUtils::String prevValue;

	int currow = 0;
	int curcol = 0;

	
	CppUtils::Byte const* curPtr = pData;
	
	// global cycle
	while(true) {

		// need to exit ??
		size_t read = curPtr - pData;
		if (read >= pDataLength)
			break;

		/*
		// SOME DATA are goinf to be read
		vector<double> floatValArr;
		vector<unsigned short> intValArr;
		vector<CppUtils::String> stringValArr;
		WORD blankCells = 0;
		WORD skipCells = 0;
		vector<bool> boolValueArr;
		vector<int> errorValueArr;
		*/


		cb = 0;
		tdt = 0;

		// read block
		while(true) {

			if (state==0) {
				// read type
				memcpy(&tdt, curPtr, sizeof(WORD));
				curPtr += sizeof(WORD);

				state = 1;

			}
			else if (state==1) {
				memcpy(&cb, curPtr, sizeof(WORD));
				curPtr += sizeof(WORD);

				state = 2;
			}
			else if (state==2) {
				// read data itself
				if (tdt==tdtTable) {
					HTASSERT(cb==4);

					// read 2 WORDS and set up our array
					
					memcpy(&rowsnum, curPtr, sizeof(WORD));
					curPtr += sizeof(WORD);

					memcpy(&colsnum, curPtr, sizeof(WORD));
					curPtr += sizeof(WORD);

					// init out
					for(int i = 0; i < rowsnum; i++) {
						vector<String> row;
						row.resize(colsnum);

						out.push_back( row );
					}

					// reset state
					state=3;

				}
				else if (tdt==tdtFloat) {
					double val;
					int cnt = cb/8;
					//floatValArr.resize(cnt);

					for(int i = 0; i < cnt; i++) {
						memcpy(&val, curPtr, sizeof(double));
						curPtr += sizeof(double);

						//floatValArr[i] = val;
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, CppUtils::double2String(val,8), prevValue);

					}

					// reset state
					state=3;

				}
				else if (tdt==tdtString) {
					CppUtils::String val;
					CppUtils::Byte cch;

					int localcounter = 0;
					while(true) {
						memcpy(&cch, curPtr, sizeof(CppUtils::Byte));
						curPtr += sizeof(CppUtils::Byte);
						localcounter += 1;

						char* buf = (char*)malloc(cch+1);
						memcpy(buf, curPtr, cch);
						curPtr += cch;
						localcounter += cch;

						buf[cch] = '\0';
						val = buf;
						free(buf);

						//stringValArr.push_back(val);
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, val, prevValue);

						// exit if read all
						if (localcounter >=cb)
							break;

					}

					// reset state
					state=3;

				}
				else if (tdt==tdtBool) {
					WORD val;
					int cnt = cb/2;
					//boolValueArr.resize(cnt);

					for(int i = 0; i < cnt; i++) {
						memcpy(&val, curPtr, sizeof(WORD));
						curPtr += sizeof(WORD);

						//boolValueArr[i] = (val==0?false:true);
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, val?"true":"false", prevValue);

					}

					// reset state
					state=3;
				}
				else if (tdt==tdtError) {
					WORD val;
					int cnt = cb/2;
					//errorValueArr.resize(cnt);

					for(int i = 0; i < cnt; i++) {
						memcpy(&val, curPtr, sizeof(WORD));
						curPtr += sizeof(WORD);

						//errorValueArr[i] = val;
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, "CELL ERROR:"+CppUtils::long2String(val), prevValue);

					}

					// reset state
					state=3;
				}
				else if (tdt==tdtBlank) {
					HTASSERT(cb==2);
					WORD blankCells = 0;
		
					memcpy(&blankCells, curPtr, sizeof(WORD));
					curPtr += sizeof(WORD);

					for(int t = 0; t < blankCells; t++)
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, "", prevValue);

					// reset state
					state=3;
					
				}
				else if (tdt==tdtInt) {
					unsigned short val;
					int cnt = cb/2;
					//intValArr.resize(cnt);

					for(int i = 0; i < cnt; i++) {
						memcpy(&val, curPtr, sizeof(unsigned short));
						curPtr += sizeof(unsigned short);

						addConsequentElement(currow, curcol, out, rowsnum, colsnum, CppUtils::long2String(val), prevValue);
						//intValArr[i] = val;

					}

					// reset state
					state=3;
				}
				else if (tdt==tdtSkip) {
					HTASSERT(cb==2);
					WORD skipCells = 0;

					memcpy(&skipCells, curPtr, sizeof(WORD));
					curPtr += sizeof(WORD);

					for(int t = 0; t < skipCells; t++)
						addConsequentElement(currow, curcol, out, rowsnum, colsnum, prevValue, prevValue);

					// reset state
					state=3;
				}
				else
					THROW(CppUtils::OperationFailed, "exc_InvalidXLBlockTableType", "ctx_ParseXLTable", (long)tdt );

			}
			else if (state==3) {
				break;
			}
		} // end of read block

		// reset state
		state = 0;
		// here we finished some block - need to arrange output data

				

		
	} // end of global cycle
};

// --------------------

void addConsequentElement(
		int& currow, 
		int& curcol, 
		vector< vector <String> >& out, 
		int const maxrows, 
		int const maxcols,
		CppUtils::String const& value,
		CppUtils::String& prevVal
)
{
	if (currow >=maxrows)
		THROW(CppUtils::OperationFailed, "exc_MaxNumberOfRowsREached", "ctx_AddConsequentElement", value);

	out[currow][curcol] = value;
	
	curcol++;
	if (curcol >= maxcols) {
		currow++;
		curcol = 0;
	}

	
	prevVal = value;

}

}; // end of namespace