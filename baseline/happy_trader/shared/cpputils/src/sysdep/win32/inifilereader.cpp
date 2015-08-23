#include "../../inifilereader.hpp"

namespace CppUtils {

#define MAX_ENTRIES 32000

CIniReader::CIniReader()
{
}

CIniReader::CIniReader(String const& strFile):
m_strFileName(strFile)
{
}

CIniReader::~CIniReader()
{
}

	// Used to retrieve a value give the section and key
String CIniReader::getKeyValue(String const& strKey,String const& strSection)
{
	char ac_Result[255];

	// Get the info from the .ini file	
	long lRetValue = ::GetPrivateProfileString((LPCTSTR)strSection.c_str(),(LPCTSTR)strKey.c_str(), "",ac_Result, 255, (LPCTSTR)m_strFileName.c_str());	
	return String (ac_Result);
}

// Used to add or set a key value pair to a section
long CIniReader::setKey(String const& strValue, String const& strKey, String const& strSection)
{
	long lRetValue = ::WritePrivateProfileString (strSection.c_str(), strKey.c_str(), strValue.c_str(), m_strFileName.c_str());
	return lRetValue;
}

// Used to find out if a given section exists
bool CIniReader::sectionExists(String const& strSection)
{
	char ac_Result[100];	
	String csAux;
	// Get the info from the .ini file	
	long lRetValue = ::GetPrivateProfileString((LPCTSTR)strSection.c_str(),NULL, "",ac_Result, 90, (LPCTSTR)m_strFileName.c_str());
	// Return if we could retrieve any info from that section
	return (lRetValue > 0);
}
  
// Used to retrieve all of the  section names in the ini file
void CIniReader::getSectionNames(StringList &sectionNames)  //returns collection of section names
{
	char ac_Result[MAX_ENTRIES];
	sectionNames.clear();
	
	long lRetValue = ::GetPrivateProfileSectionNames(ac_Result,MAX_ENTRIES,(LPCTSTR)m_strFileName.c_str());
	
	String strSectionName;
	for(int i=0; i < lRetValue; i++)
	{
		if(ac_Result[i] != '\0') {
			strSectionName += ac_Result[i];
		} else {
			if(strSectionName.size() > 0) {
				sectionNames.push_back(strSectionName);
			}
			strSectionName.clear();
		}
	}

}

// Used to retrieve all key/value pairs of a given section.  
void CIniReader::getSectionData(String const& strSection, StringList &sectionDataList)  
{
	char ac_Result[2000];  //change size depending on needs
	sectionDataList.clear();
	long lRetValue = ::GetPrivateProfileSection((LPCTSTR)strSection.c_str(), ac_Result, 2000, (LPCTSTR)m_strFileName.c_str());

	String strSectionData;
	for(int i=0; i < lRetValue; i++)
	{
		if(ac_Result[i] != '\0') {
			strSectionData += ac_Result[i];
		} else {
			if(strSectionData.size() > 0) {
				sectionDataList.push_back(strSectionData);
			}
			strSectionData.clear();
		}
	}

}

void CIniReader::setINIFileName(String const& strINIFile)
{
	m_strFileName = strINIFile;
}


}; //