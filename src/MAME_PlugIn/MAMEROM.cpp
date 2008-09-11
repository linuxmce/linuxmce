/****************************************/
/* MAMEROM - Class to return metadata 	*/
/* For a given MAME ROM. 	  	*/
/*				      	*/
/* @author <thom.cherryhomes@gmail.com> */
/****************************************/

#include "MAMEROM.h"

MAMEROM::MAMEROM()
{

}


MAMEROM::~MAMEROM() 
{
}

void MAMEROM::InitDatabase()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MAMEROM::InitDatabase() start");
    	m_pMyDatabase = new Database_lmce_game(LoggerWrapper::GetInstance());
	m_pMyDatabase->Connect("localhost","root","","lmce_game",3306);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MAMEROM::InitDatabase() successful.");
}

string MAMEROM::getTitleForROM(string sROMName) 
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	}

	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_title_CONST) 
		{
			return pRow_RomAttribute->Name_get();
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Title RomAttribute for ROMName: %s",sRomName.c_str());
	return "No Matching Title for ROM: "+sRomName;

}

string MAMEROM::getYearForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_year_CONST) 
		{
			return pRow_RomAttribute->Name_get();
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Year RomAttribute for ROMName: %s",sRomName.c_str());
	return "No Matching Year for ROM: "+sRomName;

}

string MAMEROM::getManufacturerForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_manufacturer_CONST) 
		{
			return pRow_RomAttribute->Name_get();
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Manufacturer RomAttribute for ROMName: %s",sRomName.c_str());
	return "No Matching Manufacturer for ROM: "+sRomName;
}

string MAMEROM::getGenreForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "No Matching Rom for ROM: "+sRomName;
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_genre_CONST) 
		{
			return pRow_RomAttribute->Name_get();
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Genre RomAttribute for ROMName: %s",sRomName.c_str());
	return "No Matching Genre for ROM: "+sRomName;
}
