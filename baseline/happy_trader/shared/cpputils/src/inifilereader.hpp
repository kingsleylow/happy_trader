#ifndef _CPPUTILS_INIFILEREADER_INCLUDED
#define _CPPUTILS_INIFILEREADER_INCLUDED


#include "cpputilsdefs.hpp"

namespace CppUtils {

	class CPPUTILS_EXP CIniReader  
	{
	public:
		// method to set INI file name, if not already specified 
		void setINIFileName(String const& strINIFile);
		
		// methods to return the lists of section data and section names
		void getSectionData(String const& strSection, StringList &sectionDataList);

		void getSectionNames(StringList &sectionNames);
		
		// check if the section exists in the file
		bool sectionExists(String const& strSection);

		// updates the key value, if key already exists, else creates a key-value pair
		long setKey(String const& strValue, String const& strKey, String const& strSection);

		// give the key value for the specified key of a section
		String getKeyValue(String const& strKey,String const& strSection);
		
		// default constructor
		CIniReader();
				
		CIniReader(String const& strFile);
				
		~CIniReader();
		
		
	private:
		
		// ini file name 
		String m_strFileName;
	};


}; // end of namespace 

#endif
