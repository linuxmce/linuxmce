/****************************************/
/* GAMEROM - Class to return metadata 	*/
/* For a given GAME ROM. 	  	*/
/*				      	*/
/* @author <thom.cherryhomes@gmail.com> */
/****************************************/

#include "GAMEROM.h"

GAMEROM::GAMEROM()
{

}


GAMEROM::~GAMEROM() 
{
}

void GAMEROM::InitDatabase()
{
    	m_pMyDatabase = new Database_lmce_game(new NullLogger());
	m_pMyDatabase->Connect("localhost","root","","lmce_game",3306);
}

string GAMEROM::getTitleForROM(string sROMName) 
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"' OR md5 = '"+sROMName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	}

	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_title_CONST) 
		{
			if (pRow_RomAttribute->Name_get().empty())
			{
				return "__BLANK";
			}
			else
			{
				return pRow_RomAttribute->Name_get();
			}
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Title RomAttribute for ROMName: %s",sRomName.c_str());
	return "NOMATCH";

}

string GAMEROM::getSubTitleForROM(string sROMName) 
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"' OR md5 = '"+sROMName+"'";

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	}

	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_subtitle_CONST) 
		{
			if (pRow_RomAttribute->Name_get().empty())
			{
				return "NOMATCH";
			}
			else
			{
				return pRow_RomAttribute->Name_get();
			}
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Title RomAttribute for ROMName: %s",sRomName.c_str());
	return "NOMATCH";

}

string GAMEROM::getYearForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"' OR md5 = '"+sROMName+"'";

	InitDatabase();

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_year_CONST) 
		{
			if (pRow_RomAttribute->Name_get().empty())
			{
				return "__BLANK";
			}
			else
			{
				return pRow_RomAttribute->Name_get();
			}
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Year RomAttribute for ROMName: %s",sRomName.c_str());
	return "NOMATCH";

}

string GAMEROM::getManufacturerForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"' OR md5 = '"+sROMName+"'";

	InitDatabase();

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_manufacturer_CONST) 
		{
			if (pRow_RomAttribute->Name_get().empty())
			{
				return "__BLANK";
			}
			else
			{
				return pRow_RomAttribute->Name_get();
			}
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Manufacturer RomAttribute for ROMName: %s",sRomName.c_str());
	return "NOMATCH";
}

string GAMEROM::getGenreForROM(string sROMName)
{
	Row_Rom 				*pRow_Rom;
	Row_RomAttribute				*pRow_RomAttribute;
	vector<class Row_Rom *> 		v_Rom;	
	vector<class Row_Rom_RomAttribute *>	v_Rom_RomAttributes;
	string 					sRomName = sROMName + ".zip";
	string					s_WhereQuery = "WHERE Romname = '"+sRomName+"' OR md5 = '"+sROMName+"'";

	InitDatabase();

	if (!m_pMyDatabase->Rom_get()->GetRows(s_WhereQuery,&v_Rom)) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	} 

	if (v_Rom.size() == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get Rom matching ROMName: %s",sRomName.c_str());
		return "NOMATCH";
	}
	
	pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(v_Rom[0]->PK_Rom_get());
	pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

	// Iterate through the resulting matching RomAttributes for a Rom
	for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++) 
	{
		pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
		if (pRow_RomAttribute-> FK_RomAttributeType_get() == ROMATTRIBUTETYPE_genre_CONST) 
		{
			if (pRow_RomAttribute->Name_get().empty())
			{
				return "__BLANK";
			}
			else
			{
				return pRow_RomAttribute->Name_get();
			}
		}
	}

	// Nothing was found, return a message.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not Find Genre RomAttribute for ROMName: %s",sRomName.c_str());
	return "NOMATCH";
}
