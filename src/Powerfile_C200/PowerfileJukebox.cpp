#include "PowerfileJukebox.h"
#include "Utils.h"

#include "../PlutoUtils/FileUtils.h"
#include "../PlutoUtils/ProcessUtils.h"
#include "../PlutoUtils/StringUtils.h"
using namespace StringUtils;

#include "../DCE/DCEConfig.h"
extern DCEConfig g_DCEConfig;

#include "../pluto_media/Database_pluto_media.h"
#include "../pluto_media/Table_Disc.h"
#include "../pluto_media/Table_DiscLocation.h"
#include "../Media_Plugin/MediaAttributes_LowLevel.h"
#include "Powerfile_C200.h"

#include "../HAL/HalTree.h"

#include "../JobHandler/Job.h"
#include "../JobHandler/Task.h"
#include "../Disk_Drive_Functions/Disk_Drive_Functions.h"
#include "../Disk_Drive_Functions/IdentifyJob.h"
#include "../Disk_Drive_Functions/LoadUnloadJob.h"
#include "../Gen_Devices/AllCommandsRequests.h"
#include "../Gen_Devices/AllScreens.h"

using namespace nsJobHandler;
using namespace nsJukeBox;
using namespace DCE;

/*
* PowerfileJukebox class implementation
*/

PowerfileJukebox::PowerfileJukebox(Powerfile_C200 * pPowerfile)
	: JukeBox(pPowerfile)
{
	m_pPowerfile = pPowerfile;
	m_bDoEject = m_pPowerfile->DATA_Get_Hacks().find('E')!=string::npos;
	m_bStatusCached = false;
	m_sChangerDev = m_pPowerfile->m_pEvent->GetDeviceDataFromDatabase(m_pPowerfile->m_dwPK_Device,DEVICEDATA_Block_Device_CONST);
	m_TransferElement = -1;
}

bool PowerfileJukebox::Get_Jukebox_Status(string * sJukebox_Status, bool bForce)
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	bool bComma = false, bResult = false;
	string sOutput, sStdErr;

	int nDrive = 0; // Drives are counted from 0

	if (true )//! m_bStatusCached || bForce)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting fresh status info (changer:%s) %s", m_sChangerDev.c_str(), "na");//bForce ? " (Forced)" : "");
#ifdef EMULATE_PF
		char * const args[] = {"/bin/cat", "/tmp/samples/mtx-status", NULL};
#else
		const char * const args1[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "nobarcode", "status", NULL};
		const char * const args2[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "altres", "nobarcode", "status", NULL};
		const char * const * args = m_bMtxAltres ? args2 : args1;
#endif
		if (ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr) == 0)
		{
#ifdef WIN32
			size_t size;
			char *pBuffer = FileUtils::ReadFileIntoBuffer("/temp/status",size);
			sOutput = pBuffer;
#endif
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "PowerfileJukebox::Get_Jukebox_Status changer:%s %s", m_sChangerDev.c_str(), sOutput.c_str());
			vector<string> vect_sOutput_Rows;

			Tokenize(sOutput, "\n", vect_sOutput_Rows);
			for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
			{
				string sResult = "";
				vector<string> vsFF;
				string sWhoWhat;
				vector<string> vsC; // C = Components

				ExtractFields(vect_sOutput_Rows[i], vsFF);
				if (vsFF.size() == 0)
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Extract fields failed to get anything from %d: %s", i, vect_sOutput_Rows[i].c_str());
				}
				// Data Transfer Element
				else if (vsFF[0] == "Data" && vsFF.size() >= 4)
				{
					if( m_mapDrive.find(nDrive)==m_mapDrive.end() )
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PowerfileJukebox::Get_Jukebox_Status drive %d not found", nDrive);
						continue;
					}

					int iDiscFrom = 0;
					sWhoWhat = vsFF[3];
					Tokenize(sWhoWhat, ":", vsC); // Unit_number:State

					sResult = string("D") + vsC[0] + "=" + vsC[1];
					if (vsC[1] == "Full")
					{
						sResult += string("-") + vsFF[6];
						sscanf(vsFF[6].c_str(), "%d", &iDiscFrom);

						m_mapDrive[nDrive]->m_mediaInserted = true;
						m_mapDrive[nDrive]->m_iSourceSlot = iDiscFrom;
					}
					else
					{
						m_mapDrive[nDrive]->m_iSourceSlot = 0;
						m_mapDrive[nDrive]->UpdateDiscLocation('E',0); // For now say the drive is empty, when the script starts it will get set again
					}
					m_mapDrive[nDrive]->m_DriveNumber = nDrive;
					nDrive ++;
				}
				// Transfer Element
				else if (vsFF.size() == 4 && vsFF[0] == "Storage" && vsFF[1] == "Element" && StringUtils::StartsWith(vsFF[3],"IMPORT/EXPORT") )
				{
					m_TransferElement = atoi(vsFF[2].c_str());
				}
				// Storage Element
				else if (vsFF.size() == 3 && vsFF[0] == "Storage" && vsFF[1] == "Element")
				{
					sWhoWhat = vsFF[2];
					Tokenize(sWhoWhat, ":", vsC);

					int nSlot = atoi(vsC[0].c_str());
					Slot *pSlot = m_mapSlot[nSlot];
					if( pSlot==NULL )
					{
						pSlot = new Slot(nSlot,Slot::slot_empty,this);
						m_mapSlot[nSlot] = pSlot;
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding slot %d", pSlot->m_SlotNumber);
					}
					pSlot->m_SlotNumber = nSlot;

					sResult = string("S") + vsC[0] + "=" +vsC[1];
					if (vsC[1] == "Empty")
					{
						pSlot->m_eStatus = Slot::slot_empty;
						RemoveDiscFromDb(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber);
					}
					else
					{
						pSlot->m_eStatus = Slot::slot_unknown_medium;
						Row_DiscLocation *pRow_DiscLocation = m_pDatabase_pluto_media->DiscLocation_get()->GetRow(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber);
						if( !pRow_DiscLocation )
						{
							AddDiscToDb(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber,'U');
						}
						else if( pRow_DiscLocation->FK_Disc_isNull()==false )
						{
							pSlot->m_eStatus = Slot::slot_identified_disc;
						}
					}
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Don't know what to do with vsFF[0]=%s and size=%d",
						vsFF[0].c_str(),(int) vsFF.size());
				}
				if (sJukebox_Status && sResult != "")
				{
					if (bComma)
						* sJukebox_Status += ",";
					else
						bComma = true;
					* sJukebox_Status += sResult;
				}
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished getting device status");
			m_bStatusCached = true;
			bResult = true;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get device status %s\n%s",sOutput.c_str(), sStdErr.c_str());
			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_pPowerfile->m_dwPK_Device, DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
				"The Powerfile has locked up.  This is a hardware problem.  You should cycle power on the Powerfile unit.", // Main message
				"", // Command Line
				"generic message", // Description
				"0", // sPromptToResetRouter
				"300", // sTimeout
				"1"); // sCannotGoBack
			m_pPowerfile->SendCommand(SCREEN_PopupMessage);
		}
	}

#ifdef DEBUG
	map_int_Drivep::iterator itDrive;
	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		Drive *pDrive = itDrive->second;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PowerfileJukebox::Get_Jukebox_Status m_DriveNumber: %d inserted %d slot %d m_sSRdev %s drive %s",
			(int) pDrive->m_DriveNumber,
			(int) pDrive->m_mediaInserted,
			(int) pDrive->m_iSourceSlot,
			pDrive->m_sSRdev.c_str(),pDrive->m_sDrive.c_str());
	}

	map_int_Slotp::iterator itSlot;
	for (itSlot = m_mapSlot.begin(); itSlot != m_mapSlot.end(); itSlot++)
	{
		Slot *pSlot = itSlot->second;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PowerfileJukebox::Get_Jukebox_Status m_SlotNumber: %d m_eStatus %d PK_Disc %d",
			(int) pSlot->m_SlotNumber,(int) pSlot->m_eStatus,pSlot->m_pRow_Disc ? pSlot->m_pRow_Disc->PK_Disc_get() : -1);
	}
#endif
	/*
	else // Use cached info
	{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using cached info");
	string sResult, sState;
	bool bComma = false;

	map_int_Slotp::iterator itSlot;
	map_int_Drivep::iterator itDrive;

	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
	Drive * pDrive =  (itDrive->second);
	sState = pDrive->m_iSourceSlot == 0 ? "Empty" : ("Full-" + StringUtils::itos(pDrive->m_iSourceSlot));
	sResult = "D" + StringUtils::itos(pDrive->m_DriveNumber) + "=" + sState;
	if (sJukebox_Status && sResult != "")
	{
	if (bComma)
	* sJukebox_Status += ",";
	else
	bComma = true;
	* sJukebox_Status += sResult;
	}
	}

	for (itSlot = m_mapSlot.begin(); itSlot != m_mapSlot.end(); itSlot++)
	{
	Slot * pSlot = itSlot->second;
	sState = pSlot->m_eStatus == Slot::slot_empty ? "Empty" : "Full"; // XXX: distinguish between varieties of full?
	sResult = "S" + StringUtils::itos(pSlot->m_SlotNumber) + "=" + sState;
	if (sJukebox_Status && sResult != "")
	{
	if (bComma)
	* sJukebox_Status += ",";
	else
	bComma = true;
	* sJukebox_Status += sResult;
	}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished getting device status");
	bResult = true;
	}
	*/
	//return bResult;
	return JukeBox::jukebox_transport_failure;
}

/*virtual*/ bool PowerfileJukebox::Init()
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);
	if( !JukeBox::Init() )
		return false;

	HalTree halTree;
	halTree.Populate();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"PowerfileJukebox::Init populate got %d",halTree.m_mapHalDevice.size());

	string sOutput, sStdErr;
#ifdef EMULATE_PF
	const char * const args[] = {"/bin/cat", "/tmp/samples/lsscsi", NULL};
#else
	const char * const args[] = {"/usr/bin/lsscsi", "-g", NULL};
#endif
	if (ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr) != 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get device names %s\n%s",sOutput.c_str(), sStdErr.c_str());
		return false;
	}

#ifdef WIN32
	size_t size;
	char *pBuffer = FileUtils::ReadFileIntoBuffer("/temp/lsscsi",size);
	sOutput=pBuffer;
#endif
	string sReportingChildDevices;  // To notify gip of the drives in this system

	HalDevice *pHalDevice_Changer = NULL; // We'll fill this in when we find it
	vector<string> vect_sOutput_Rows;
	Tokenize(sOutput, "\n", vect_sOutput_Rows);
	int nDrive = 0;

	// Two passes, so we always get the changer first
	for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
	{
		vector<string> vsFF; // FF = Filtered Fields
		ExtractFields(vect_sOutput_Rows[i], vsFF);

		if (vsFF[1] == "mediumx")
		{
			int iField = 0;
			if (1 == 0
				|| /* Powerfile C200 */     (vsFF[2] == "Escient"  && vsFF[3] == "Powerfile"    && vsFF[4] == "C200" && (iField = 7))
				|| /* Powerfile R200 DLC */ (vsFF[2] == "PowrFile" && vsFF[3] == "C200"                              && (iField = 6))
				|| /* Sony VAIO XL1B2 */    (vsFF[2] == "Sony"     && vsFF[3] == "VAIOChanger1"                      && (iField = 6, m_bMtxAltres = true))
				)
			{
				string sChangerDev = vsFF[iField];
				if( sChangerDev!=m_sChangerDev )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Skipping changer unit: %s looking for %s", vsFF[iField].c_str(), sChangerDev.c_str());
					continue;
				}
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found changer unit: %s", vsFF[iField].c_str());
				pHalDevice_Changer = halTree.GetDeviceWithParm("linux.device_file",m_sChangerDev);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Found an unknown changer unit. Things won't work.");
			}
		}
	}
	for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
	{
		vector<string> vsFF; // FF = Filtered Fields
		ExtractFields(vect_sOutput_Rows[i], vsFF);

		if (vsFF[1] == "cd/dvd" &&
			(
			0 == 1
			|| /* Powerfile C200 */      (vsFF[2] == "TOSHIBA" /* && vsFF[3] == "DVD-ROM" */ && vsFF[4] == "SD-M1212")
			|| /* Powerfile R200 DLC */  (vsFF[2] == "MATSHITA" /* && vsFF[3] == "DVD-RAM" */ && vsFF[4] == "SW-9585S")
			|| /* Sony VAIO XL1B2 */     (vsFF[2] == "MATSHITA" /* && vsFF[3] == "DVD-RAM" */ && vsFF[4] == "SW-9584" )
			)
			)
		{
			// There is a drive.  See if it's one of ours by finding our changer in the tree, then this device, and seeing if they have the same serial number
			HalDevice *pHalDevice_Drive = halTree.GetDeviceWithParm("linux.device_file",vsFF[7]);
			if( !pHalDevice_Drive || !pHalDevice_Changer )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::Init cannot find %s changer (%p)", m_sChangerDev.c_str(),pHalDevice_Changer);
				continue;
			}

			// There should be only 1 difference between the ID's of the changer and the drive
			const char *sz_Changer = pHalDevice_Changer->m_sUdi.c_str();
			const char *sz_Drive = pHalDevice_Drive->m_sUdi.c_str();
			const char *pDiff = NULL;
			bool bDifferentUnits=false; // Will set to true if this drive doesn't belong to the changer
			while( *sz_Drive && *sz_Changer )
			{
				if( *sz_Changer!=*sz_Drive )
				{
					if( pDiff )
					{
						bDifferentUnits=true;  // There's more than one diff
						break;
					}
					pDiff=sz_Changer;
				}
				sz_Changer++;
				sz_Drive++;
			}
			if( bDifferentUnits )
				continue; // This drive doesn't belong to this changer

			m_bMtxAltres = vsFF[4] == "SW-9584";
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found DVD unit %d: %s", nDrive, vsFF[6].c_str());

			// Find the device id for this drive
			DeviceData_Impl *pDevice = m_pPowerfile->m_pData->FindChild(DEVICEDATA_PortChannel_Number_CONST,StringUtils::itos(nDrive));
			if( pDevice )
				m_pPowerfile->SetDeviceDataInDB(pDevice->m_dwPK_Device,DEVICEDATA_Drive_CONST,vsFF[6]);

			Drive * pDrive= new Drive(pDevice ? pDevice->m_dwPK_Device : 0,m_pPowerfile, vsFF[6],
				m_pJobHandler, m_pDatabase_pluto_media, m_pMediaAttributes_LowLevel,nDrive, vsFF[7],this,true);
			m_mapDrive[nDrive] = pDrive;

			/*
			A list like this:
			[internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \t [PK_DeviceData] \t [Value] ... \n
			*/
			sReportingChildDevices += StringUtils::itos(nDrive) + "\t" + "Drive " + StringUtils::itos(nDrive)
				+ "\t\t" + TOSTRING(DEVICETEMPLATE_Disc_Drive_Embedded_CONST) + "\t\t" +
				TOSTRING(DEVICEDATA_Drive_CONST) + "\t" + vsFF[6] + "\n";
			nDrive++;
		}
	}

	if (!Get_Jukebox_Status(NULL,true))
		return false;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished config.  child devices: %s", sReportingChildDevices.c_str());

	string sDefective = m_pPowerfile->DATA_Get_Defective_Units();
	vector<string> vect_sDefective;
	Tokenize(sDefective, ",", vect_sDefective);
	const int iDrive_Number_Max = m_mapDrive.size() - 1;
	for (size_t i = 0; i < vect_sDefective.size(); i++)
	{
		int iDrive_Number = atoi(vect_sDefective[i].c_str());
		if (iDrive_Number < 0 || iDrive_Number > iDrive_Number_Max)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Defective unit %d out of range (0-%d)", iDrive_Number, iDrive_Number_Max);
			continue;
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Marking user-marked-as-defective drive %d as OFFLINE", iDrive_Number);
		( (m_mapDrive[iDrive_Number]))->m_bFunctional = false;
	}

	map_int_Drivep::iterator itDrive;
	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		Drive *pDrive = (itDrive->second);
		if (pDrive->m_iSourceSlot != 0)
		{
			MoveFromDriveToSlot(m_mapSlot[pDrive->m_iSourceSlot], pDrive);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reporting child devices: %s", sReportingChildDevices.c_str());
	Message *pMessage = new Message(m_pPowerfile->m_dwPK_Device,DEVICEID_EVENTMANAGER,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_Reporting_Child_Devices_CONST,1,
		EVENTPARAMETER_Text_CONST,sReportingChildDevices.c_str());
	m_pPowerfile->QueueMessageToRouter(pMessage);

	UpdateDrivesSlotsFromDatabase();

	return true;
}

/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);

	AssertJukeboxIsLocked();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loading disc from slot %d into drive %d", pSlot->m_SlotNumber, pDrive->m_DriveNumber);

	string sSlot = StringUtils::itos(pSlot->m_SlotNumber);
	string sDrive = StringUtils::itos(pDrive->m_DriveNumber);
	const char * const args1[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "nobarcode", "load", sSlot.c_str(), sDrive.c_str(), NULL};
	const char * const args2[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "altres", "nobarcode", "load", sSlot.c_str(), sDrive.c_str(), NULL};
	const char * const* args = m_bMtxAltres ? args2 : args1;

	JukeBox::JukeBoxReturnCode jbRetCode = JukeBox::jukebox_transport_failure;

	if (pDrive->m_mediaInserted)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Disc unit %d full", pDrive->m_DriveNumber);
	}
	else
	{
		dm.Release();
		string sOutput,sStdErr;
		int returncode = ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr);


		if( returncode!=0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromSlotToDrive Loading disc failed once %s\n%s",sOutput.c_str(), sStdErr.c_str());
			if( RetryPowerfileCommand(args[0], args, sOutput, sStdErr,"Full","loaded") ) // It seems that this fails a lot for no reason, even though it really did work
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromSlotToDrive unit recovered %s\n%s",sOutput.c_str(), sStdErr.c_str());
				returncode=0;
			}
		}

		dm.Relock();
		if (returncode == 0)
		{
#ifndef EMULATE_PF
			dm.Release();
			CheckEjectHack(pDrive);
			dm.Relock();
#endif

			pDrive->m_iSourceSlot = pSlot->m_SlotNumber;
			pSlot->m_eStatus = Slot::slot_empty;

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting disc type");
			pDrive->m_mediaDiskStatus = DISCTYPE_NONE;
			pDrive->m_mediaInserted = false;
			pDrive->cdrom_checkdrive(pDrive->m_sDrive.c_str(), &pDrive->m_mediaDiskStatus, false);
			if (pDrive->m_mediaDiskStatus != DISCTYPE_NONE)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loaded disc of type %d", pDrive->m_mediaDiskStatus);
				pDrive->m_mediaInserted = true;
				jbRetCode = JukeBox::jukebox_ok;

				// Update the database
				UpdateDiscLocation(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber,pDrive->m_dwPK_Device_get(),-1);

				if( pDrive->m_mediaDiskStatus==DISCTYPE_DVD_VIDEO )
					pDrive->UpdateDiscLocation('d');
				else if( pDrive->m_mediaDiskStatus==DISCTYPE_CD_AUDIO || pDrive->m_mediaDiskStatus==DISCTYPE_CD || pDrive->m_mediaDiskStatus==DISCTYPE_CD_MIXED )
					pDrive->UpdateDiscLocation('c');
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't get disc type. We're sure there's a disc in there (we just loaded it), assuming defective unit.  drive %d", pDrive->m_DriveNumber);
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromSlotToDrive Loading disc failed: %d %s\n%s", returncode, sOutput.c_str(), sStdErr.c_str());
		}
	}

	return jbRetCode;
}

/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);

	AssertJukeboxIsLocked();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unloading disc from drive %d into slot %d", pDrive->m_DriveNumber, pSlot->m_SlotNumber);

	string sSlot = StringUtils::itos(pSlot->m_SlotNumber);
	string sDrive = StringUtils::itos(pDrive->m_DriveNumber);
	const char * const args1[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "nobarcode", "unload", sSlot.c_str(), sDrive.c_str(), NULL};
	const char * const args2[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "altres", "nobarcode", "unload", sSlot.c_str(), sDrive.c_str(), NULL};
	const char * const* args = m_bMtxAltres ? args2 : args1;

	JukeBox::JukeBoxReturnCode jbRetCode = JukeBox::jukebox_transport_failure;

	if (pDrive->m_mediaInserted==false)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Disc unit %d empty", pDrive->m_DriveNumber);
	}
	else
	{
		dm.Release();
		string sOutput,sStdErr;
		int returncode = ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr);

		if( returncode!=0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromDriveToSlot Unloading disc failed once %s\n%s",sOutput.c_str(), sStdErr.c_str());
			if( RetryPowerfileCommand(args[0], args, sOutput, sStdErr,"is Empty","") ) // It seems that this fails a lot for no reason, even though it really did work
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromDriveToSlot unit recovered %s\n%s",sOutput.c_str(), sStdErr.c_str());
				returncode=0;
			}
		}

		dm.Relock();
		if (returncode == 0)
		{
#ifndef EMULATE_PF
			dm.Release();
			CheckEjectHack(pDrive);
			dm.Relock();
#endif
			pDrive->m_iSourceSlot = -pSlot->m_SlotNumber;
			pSlot->m_eStatus = Slot::slot_unknown_medium; // TODO: distinguish between slot_unknown_medium and slot_identified_disc

			Disk_Drive_Functions * pDDF = pDrive;
			pDDF->m_mediaDiskStatus = DISCTYPE_NONE;
			pDDF->m_mediaInserted = false;

			// Update the database
			UpdateDiscLocation(pDrive->m_dwPK_Device_get(),-1,m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber);

			jbRetCode = JukeBox::jukebox_ok;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::MoveFromDriveToSlot Unloading disc failed %d %s\n%s",returncode, sOutput.c_str(), sStdErr.c_str());
		}
	}

	return jbRetCode;
}

/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Eject(int iSlot_Number,int iPK_Device,int PK_Orbiter)
{
	if( iSlot_Number==0 && iPK_Device==0 )
	{
		LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,LoadUnloadJob::eEjectMultipleDiscs,this,NULL,NULL,PK_Orbiter,m_pCommand_Impl);
		m_pJobHandler->AddJob(pLoadUnloadJob);
		return JukeBox::jukebox_ok;
	}
	if( iPK_Device )
	{
		Drive *pDrive = m_mapDrive_FindByPK_Device(iPK_Device);
		if( !pDrive )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::Eject invalid drive %d",iPK_Device);
			return JukeBox::jukebox_transport_failure;
		}
		return Eject(pDrive,PK_Orbiter);
	}
	
	Slot *pSlot = m_mapSlot_Find(iSlot_Number);
	if( !pSlot )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::Eject invalid slot %d",iSlot_Number);
		return JukeBox::jukebox_transport_failure;
	}

	LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,LoadUnloadJob::eEjectOneDisc,this,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
	m_pJobHandler->AddJob(pLoadUnloadJob);
	return JukeBox::jukebox_ok;
}

/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Eject(Drive *pDrive,int PK_Orbiter)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PowerfileJukebox::Eject Ejecting disc from drive %d", pDrive->m_dwPK_Device_get());
	Slot *pSlot = m_mapSlot_Empty();
	if( !pSlot )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PowerfileJukebox::Eject no empty slot");
		return JukeBox::jukebox_transport_failure;
	}

	LoadUnloadJob *pLoadUnloadJob = new LoadUnloadJob(m_pJobHandler,LoadUnloadJob::eEjectOneDisc,this,pDrive,pSlot,PK_Orbiter,m_pCommand_Impl);
	m_pJobHandler->AddJob(pLoadUnloadJob);
	return JukeBox::jukebox_ok;
}

// Don't call directly.  Call from a job as this may block
/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Eject(Slot *pSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);

	AssertJukeboxIsLocked();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ejecting disc from slot %d", pSlot->m_SlotNumber);

	string sSlot = StringUtils::itos(pSlot->m_SlotNumber);
	string sTxfElement = StringUtils::itos(m_TransferElement);
	const char * const args1[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "nobarcode", "transfer", sSlot.c_str(), sTxfElement.c_str(), NULL};
	const char * const args2[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "altres", "nobarcode", "transfer", sSlot.c_str(), sTxfElement.c_str(), NULL};
	const char * const* args = m_bMtxAltres ? args2 : args1;


	JukeBox::JukeBoxReturnCode jbRetCode = JukeBox::jukebox_transport_failure;

	dm.Release();
	string sOutput,sStdErr;
	int returncode = ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr);
	dm.Relock();
	if (returncode == 0)
	{
		// Update the database
		RemoveDiscFromDb(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber);
		pSlot->m_eStatus = Slot::slot_empty;
		return JukeBox::jukebox_ok;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Ejecting disc failed %s\n%s",sOutput.c_str(), sStdErr.c_str());
		return JukeBox::jukebox_transport_failure;
	}
}

/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Load(Slot *pSlot/*=NULL*/)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);

	AssertJukeboxIsLocked();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loading disc to slot %d", pSlot->m_SlotNumber);
	string sSlot = StringUtils::itos(pSlot->m_SlotNumber);
	string sTransfer = StringUtils::itos(m_TransferElement);
	const char * const args1[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "nobarcode", "transfer", sTransfer.c_str(), sSlot.c_str(), NULL};
	const char * const args2[] = {MTX_CMD, "-f", m_sChangerDev.c_str(), "altres", "nobarcode", "transfer", sTransfer.c_str(), sSlot.c_str(), NULL};
	const char * const* args = m_bMtxAltres ? args2 : args1;

	JukeBox::JukeBoxReturnCode jbRetCode = JukeBox::jukebox_transport_failure;

	dm.Release();
	string sOutput,sStdErr;
	int returncode = ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr);
	dm.Relock();
	if (returncode == 0)
	{
		AddDiscToDb(m_pCommand_Impl->m_dwPK_Device,pSlot->m_SlotNumber,'U');
		pSlot->m_eStatus = Slot::slot_unknown_medium;
		return JukeBox::jukebox_ok;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Load disc failed %s\n%s",sOutput.c_str(), sStdErr.c_str());
		return JukeBox::jukebox_transport_failure;
	}
}

bool PowerfileJukebox::RetryPowerfileCommand(const char * path, const char *const args[], string & sOutput, string & sStdErr, string sSearchToken1,string sSearchToken2)
{
	for(int iRetry=0;iRetry<5;++iRetry)
	{
		Sleep(2000);
		sOutput="";
		sStdErr="";
		int returncode = ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::RetryPowerfileCommand %d %d %s\n%s",iRetry,returncode,sOutput.c_str(), sStdErr.c_str());
		if( returncode==1 && (sSearchToken1.empty() || sStdErr.find(sSearchToken1)!=string::npos  || sOutput.find(sSearchToken1)!=string::npos) && (sSearchToken2.empty() || sStdErr.find(sSearchToken2)!=string::npos  || sOutput.find(sSearchToken2)!=string::npos) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::RetryPowerfileCommand recovered");
			return true;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "PowerfileJukebox::RetryPowerfileCommand never recovered");
	return false;
}
