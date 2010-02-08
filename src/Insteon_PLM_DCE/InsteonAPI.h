#ifndef INSTEONAPI_H_
#define INSTEONAPI_H_


#define IM_CMD_START			0x02

#define COMMAND_GET_IM_INFO 	0x60
#define COMMAND_SEND_SD_ED_MSG	0x62

#define CMD_SD_LIGHT_ON 		0x11
#define CMD_SD_LIGHT_ON_FAST 	0x12
#define CMD_SD_LIGHT_OFF 		0x13
#define CMD_SD_LIGHT_OFF_FAST 	0x14


#include <deque>


namespace InsteonAPI
{
   struct InsteonJob {
	char buffer[512];
	size_t len;
	time_t timeout;
	int sendcount;
    };

	class InsteonAPI 
	{
	private:
		int serialPort;
		
			// queue for sending
	 	std::deque < InsteonJob * > InsteonSendQueue;
		
		void *myInsteon;
		
		//Device ID of PLM
		unsigned char plmIDHiByte;
		unsigned char plmIDMidByte;
		unsigned char plmIDLoByte;
		
		//PLM Data
		unsigned char plmDevCat;
		unsigned char plmDevSubCat;
		unsigned char plmFirmwareRev;
		
		
		// mutex to lock out command queue
		pthread_mutex_t mutexSendQueue;
		
	public:
		
		InsteonAPI(void* myInsteon);
			
		// opens the serial port and starts the initalization of the insteon device
		void *init(const char *device);
		
		// Reads data from the PLM on the serial port
		void receiveFunction();
		
		// Generate an Insteon Job and put it in the send queue
		bool sendFunction(char *buffer, size_t length, int type, bool response);
		
		bool sendSDMsg(char* id,char* cmd);
		bool sendEDMsg(char* buffer,size_t length);
		
		bool setLightOn (char* id, unsigned char level);
		bool setLightOff(char* id);
		
	};




}
#endif /*INSTEONAPI_H_*/