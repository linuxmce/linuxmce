#ifndef INSTEONAPI_H_
#define INSTEONAPI_H_

// Misc data bytes
#define ACK                             0x06
#define NAK                             0x15
#define REQUEST                         0x0
#define RESPONSE                        0x1
#define IM_02_CMD_START		0x02
#define CFG_40_BIT_MONITOR_MODE	0x40


// Commands received from IM
#define CMD_50_SD_MSG_RECV		0x50
#define CMD_51_ED_MSG_RECV		0x51
#define CMD_52_X10_RECV			0x52
#define CMD_53_ALL_LINK_CMPL		0x53 
#define CMD_54_BUTTON_PRESSED		0x54
#define CMD_55_USER_RESET_DETECT	0x55
#define CMD_56_ALL_LINK_CLEAN_FAIL	0x56
#define CMD_57_GOT_ALL_LINK_RECORD	0x57
#define CMD_58_ALL_LINK_CLEAN_STATUS	0x58

// Commands sent to IM
#define CMD_60_GET_IM_INFO 	0x60
#define CMD_61_SEND_ALL_LINK	0x61
#define CMD_62_SEND_SD_ED_MSG	0x62 
#define CMD_63_SEND_X10		0x63
#define CMD_64_START_ALL_LINK	0x64
#define CMD_65_CANCEL_ALL_LINK	0x65
#define CMD_66_SET_HOST_CAT	0x66
#define CMD_67_RESET_IM		0x67
#define CMD_69_GET_FIRST_ALL_LINK_RECORD	0x69
#define CMD_6A_GET_NEXT_ALL_LINK_RECORD		0x6A
#define CMD_6B_SET_IM_CONFIG	0x6B
#define CMD_73_GET_IM_CONFIG	0x73

// Standard messages
#define CMD_03_SD_PROD_DATA_REQ	0x03
#define CMD_0D_SD_GET_VERSION	0x0D
#define CMD_0F_SD_PING		0x0F
#define CMD_10_SD_ID_REQUEST	0x10
#define CMD_11_SD_LIGHT_ON 	0x11
#define CMD_12_SD_LIGHT_ON_FAST 	0x12
#define CMD_13_SD_LIGHT_OFF 		0x13
#define CMD_14_SD_LIGHT_OFF_FAST 	0x14
#define CMD_15_SD_LIGHT_BRIGHTEN	0x15
#define CMD_16_SD_LIGHT_DIM		0x16
#define CMD_19_SD_LIGHT_GET_STATUS	0x19

// Extended messages (set extended message flag and pad with 0x00)
#define CMD_2F_ED_RW_ALL_LINK_DB	0x2F

// Device categories
#define CAT_01_LIGHT_SWITCH_DIMMABLE	0x01
#define CAT_02_LIGHT_ON_OFF		0x02
#define CAT_03_NEWORK_BRIDGE		0x03
#define CAT_04_IRRIGATION_CONTROL	0x04
#define CAT_05_CLIMATE_CONTROL		0x05
#define CAT_06_POOL_SPA_CONTROL		0x06
#define CAT_07_SENSORS_ACTUATORS	0x07
#define CAT_10_SECURITY_HEALTH_SAFETY	0x10
#define CAT_11_SURVEILLANCE		0x11
#define CAT_12_AUTOMOTIVE		0x12
#define CAT_13_PET_CARE			0x13
#define CAT_14_TOYS			0x14
#define CAT_15_TIMEKEEPING		0x15
#define CAT_16_HOLIDAY			0x16

#include <deque>


namespace InsteonAPI
{
   struct InsteonJob {
	char buffer[512];
	size_t len;
	time_t timeout;
	int sendcount;
	int callbackid;
	int callback_type;
	bool await_response;
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
		
		// counter to get a unique callback id
		int callbackpool;

		// set true when we await an ACK from the dongle, influences state machine
		bool await_ack;
		// same for callback
		int await_callback;
		// callback type temp var for state handling
		int callback_type;

		// Number of consecutive dropped jobs
		int dropped_jobs;
		void dropSendQueueJob();
		void sendNextCommand();

		// polling state
		bool poll_state;

	public:
		
		InsteonAPI(void* myInsteon);
			
		// opens the serial port and starts the initalization of the insteon device
		void *init(const char *device);
		
		// calculate a xor checksum for the zwave frame
		char checksum(char *buf, int len);

		// Initiate a download of the ALL-Link database
		void getDatabase();
		
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
