#ifndef PowerfileJukebox_H
#define PowerfileJukebox_H

#include "../Disk_Drive_Functions/JukeBox.h"
#include "../JobHandler/JobHandler.h"

namespace DCE
{
	class Powerfile_C200;
}

#define MTX_CMD "/opt/mtx-pluto/sbin/mtx"

namespace nsJukeBox
{
	class PowerfileJukebox : public JukeBox
	{
		public:
			bool m_bMtxAltres;
			bool m_bDoEject;  // Means as a hack do an eject command after each move, as required by some drives
			DCE::Powerfile_C200 * m_pPowerfile;
			bool m_bStatusCached;
			bool Get_Jukebox_Status(string * sJukebox_Status, bool bForce);
			int m_TransferElement;

		public:
			PowerfileJukebox(class DCE::Powerfile_C200 * pPowerfile);
			virtual bool Init();

			virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode Eject(Slot *pSlot);  // Elect the disc in pSlot
			virtual JukeBoxReturnCode Eject(Drive *pDrive,int PK_Orbiter);  // Elect the disc in pDrive
			virtual JukeBoxReturnCode Eject(int iSlot_Number,int iPK_Device,int PK_Orbiter);
			virtual JukeBoxReturnCode Load(Slot *pSlot=NULL);  // If NULL, just allow bulk inserting into slots.

			void CheckEjectHack(Drive *pDrive)
			{
				if( !m_bDoEject )
					return;
				PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
				string sCmd = MTX_CMD " -f " + pDrive->m_sDrive + (m_bMtxAltres ? " altres" : "") + " nobarcode eject"; // this is a patched version of mtx.  This is just a hack for a bug in the C200.  Don't worry about the result code
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
				system(sCmd.c_str());
			}

			// Commands often fail a lot for no reason.  This function will retry the command up to 5 times with a 2 second delay in between each until sSearchToken is found in the output
			bool RetryPowerfileCommand(const char * path, const char *const args[], string & sOutput, string & sStdErr, string sSearchToken1,string sSearchToken2);
	};
};

#endif /*PowerfileJukebox_H*/
