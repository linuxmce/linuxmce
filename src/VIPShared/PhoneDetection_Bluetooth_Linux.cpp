/*
 PhoneDetection_Bluetooth_Linux
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Bluetooth_Linux.h"
#include "PhoneDevice.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <sys/socket.h>
#include <list>
using namespace std;
using namespace DCE;

void trim(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}

bdaddr_t bdaddr;

static volatile sig_atomic_t __io_canceled = 0;

static void sig_term(int sig)
{
	__io_canceled = 1;
}

PhoneDetection_Bluetooth_Linux::PhoneDetection_Bluetooth_Linux() 
	: PhoneDetectionEngine() 
{
}

bool PhoneDetection_Bluetooth_Linux::ScanningLoop()
{
	if( m_bAbortScanLoop)
		return false;

     if(m_bScanningSuspended)
         return true;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Start of scan loop");
	int num_rsp, length, flags, dev_id = 0;

	length  = 10;	/* ~13 seconds */
	num_rsp = 100;
	flags   = 0;
	const uint8_t *lap=NULL;

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	inquiry_info_with_rssi result, *inq_list;
	inquiry_info *info;
	hci_event_hdr *hdr;
	evt_cmd_status *cs;
	inquiry_cp cp;
	struct hci_filter flt, old_flt;
	struct sigaction sa, sa_term, sa_int;
	struct pollfd p;
	int i, dd, len, err = 0, cancel = 1;

    time_t t_start;
	bool bStartToTimeout = false;
	
	/* Check for unlimited responses */
	if (num_rsp <= 0)
		num_rsp = 255;

	//this code is needed if we wanted to support multiple dongles and specify the correct one by mac address
	string sMacAddress = MacAddress();
	if(!sMacAddress.empty())
	{
		dev_id = hci_devid(sMacAddress.c_str());
		if (dev_id < 0) 
		{
			dev_id = hci_get_route(NULL);
			if (dev_id < 0)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Can't get an ID for bluetooth dongle.");
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Bluetooth dongle %s not found, using hci0.", sMacAddress.c_str());
			}
		}
	}

	/* Open the HCI device */
	dd = hci_open_dev(dev_id);
	if (dd < 0)
	{
		//printf("Error opening bluetooth device");
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error opening bluetooth device");
		Sleep(15000);  // Don't get in a vicous loop, wait 15 seconds
	}

	m_DevInfo.dev_id = dev_id;
	if (ioctl(dd, HCIGETDEVINFO, (void*) &m_DevInfo))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error opening bluetooth device");
		hci_close_dev(dd);
		return false;
	}
	char addr[18];
	ba2str(&m_DevInfo.bdaddr, addr);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Attached to BT adapter: %s\t%s\n", m_DevInfo.name, addr);
	hci_close_dev(dd);
  	
	dd = hci_open_dev(dev_id); // Todo: figure out why I need to keep opening and closing the device
	if (dd < 0) 
	{
		printf("Device open failed");
		return false;
	}

	//printf("opened device\n");

	/* Allocate memory for inquiry results */
	inq_list = (inquiry_info_with_rssi *) malloc(sizeof(inquiry_info_with_rssi) * num_rsp);
	if (!inq_list) {
		err = errno;
		goto close;
	}

	/* Store the current event filter of this HCI socket */
	len = sizeof(old_flt);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &old_flt, (socklen_t *)&(len) ) < 0) {
		err = errno;
		goto close;
	}

	/* Replace the event filter with our needs */
	hci_filter_clear(&flt);
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
	hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
	hci_filter_set_event(EVT_CMD_STATUS, &flt);
	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
		err = errno;
		goto close;
	}

	/* Redirect SIGTERM and SIGINT to our function */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_term;
	sigaction(SIGTERM, &sa, &sa_term);
	sigaction(SIGINT,  &sa, &sa_int);

	/* Send an inquiry command to the device */
	memset(&cp, 0, INQUIRY_CP_SIZE);
	if (lap) {
		memcpy(cp.lap, lap, 3);
	} else {
		cp.lap[0] = 0x33;
		cp.lap[1] = 0x8b;
		cp.lap[2] = 0x9e;
	}

	//printf("# ready to send command\n");

	cp.length = length;
	cp.num_rsp = num_rsp;
	if (hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY, INQUIRY_CP_SIZE, &cp) < 0) {
		err = errno;
		goto done;
	}

	p.fd = dd;
	p.events = POLLIN | POLLERR | POLLHUP;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Inquiry started");	
	
	while (!__io_canceled && cancel && !m_bAbortScanLoop && !m_bScanningSuspended) 
	{
		p.revents = 0;
		int res = poll(&p, 1, 100);

		if(res == 0)
		{
			if(!bStartToTimeout)
			{
				t_start = time(NULL);
				bStartToTimeout = true;
			}

			if(time(NULL) - t_start > 20) //more then 10 sec while the poll keeps timing out
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "poll continued to time out last 20 seconds... reloading Bluetooth_Dongle");
				exit(1);
				return false;
			}
		}
		else if(res < 0)
		{
			switch(errno)
			{
				case EBADF:		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "An invalid file descriptor was given in one of the sets."); break;
				case EFAULT:	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The  array given as argument was not contained in the calling program's address space."); break;
				case EINVAL:	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The nfds value exceeds the RLIMIT_NOFILE value."); break;
				case ENOMEM:	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "There was no space to allocate file descriptor tables."); break;
				case EINTR:     LoggerWrapper::GetInstance()->Write(LV_STATUS, "A signal occurred before any requested event."); break;
				default:		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown error.");
			}

			if(errno != EINTR)
				cancel = true;
		}
		else if(res > 0) 
		{
			bStartToTimeout = false;

			//printf("# in poll\n");
			/* Read the next HCI event (in H4 format) */
			len = read(dd, buf, sizeof(buf));
			printf("# poll - got: %d\n",len);
			if (len < HCI_EVENT_HDR_SIZE + 1)
			{
				printf("#poll - len too small, continuing...\n");
				continue;
			}

			hdr = (hci_event_hdr *) (void *) (buf + 1);
			ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
			len -= (1 + HCI_EVENT_HDR_SIZE);

			//printf("#hrd->evt = %d\n", hdr->evt);

			switch (hdr->evt) 
			{
			case EVT_INQUIRY_RESULT:
				printf("# inquiry result: %d\n",(int) ptr[0]);
				/* Get standard inquiry result, inqmode = 0 */
				for (i = 0; i < ptr[0]; i++) 
				{
					info = (inquiry_info *) ((char *) ptr + (sizeof(*info) * i) + 1);
					bacpy(&result.bdaddr, &info->bdaddr);
					result.pscan_rep_mode    = info->pscan_rep_mode;
					result.pscan_period_mode = info->pscan_period_mode;
					memcpy(result.dev_class, info->dev_class, 3);
					result.clock_offset = info->clock_offset;
					result.rssi = 0;

                    //get the mac address
                    char addr[18];
                    ba2str(&(info+i)->bdaddr, addr);
                    string sMacAddress;

                    //initial name is 'n/a'
                    char name[248];
                    memset(name, 0, sizeof(name));
                    strcpy(name, "N/A");

                    //create the new device
                    PhoneDevice *pDNew = new PhoneDevice(name,addr,result.rssi);
                    bacpy(&pDNew->m_bdaddrDongle, &m_DevInfo.bdaddr);

				    PLUTO_SAFETY_LOCK(vm,m_VariableMutex);
					if(m_mapKnownNames.find(pDNew->m_iMacAddress) != m_mapKnownNames.end())
					{
						string sName = m_mapKnownNames[pDNew->m_iMacAddress];
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found name %s for mac %s in the known names map", 
								sName.c_str(), pDNew->m_sMacAddress.c_str());
						pDNew->m_sID = sName;
					}
					else
					{
						//this is a new one
	                    //use 'hcitool name' to quickly get the name of the device
        	            string sOutputFile("/tmp/hci_remote_name");
            	        system(string("hcitool name " + pDNew->m_sMacAddress + " > " + sOutputFile).c_str());

                	    size_t iSize;
                    	char *pData = FileUtils::ReadFileIntoBuffer(sOutputFile, iSize);
                   		if(pData && iSize > 0)
                        	strcpy(name, pData);
                    	delete pData;

                    	pDNew->m_sID = name;
                    	pDNew->m_sID = StringUtils::Replace(pDNew->m_sID, "\n", "");
                    	pDNew->m_sID = StringUtils::Replace(pDNew->m_sID, "\r", "");
                    	trim(pDNew->m_sID);

						m_mapKnownNames[pDNew->m_iMacAddress] = pDNew->m_sID;
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected first time: name %s for mac %s. Added to known names map", 
								pDNew->m_sID.c_str(), pDNew->m_sMacAddress.c_str());
					}
				    vm.Release();

                    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device %s, %s responded.", pDNew->m_sMacAddress.c_str(), pDNew->m_sID.c_str());
                    AddDeviceToDetectionList(pDNew);
				}
				break;

			case EVT_INQUIRY_RESULT_WITH_RSSI:
				printf("inquiry result with rssi: %d\n",(int) ptr[0]);
				/* Get inquiry result with RSSI, inqmode = 1 */
				for (i = 0; i < ptr[0]; i++) {
					memcpy(&result, ptr + (sizeof(result) * i) + 1, sizeof(result));
//					cur_rsp = add_result(list, cur_rsp, num_rsp, &result);

					printf("got result with rssi\n");
					
					char addr[18];
					ba2str(&result.bdaddr, addr);

					char name[248];
					memset(name, 0, sizeof(name));
					
					// For some reason when I do this, I can only scan once and all future scans fail
	                //if (hci_read_remote_name(dd, &(info+i)->bdaddr, sizeof(name), name, 100000) < 0)
                    
					strcpy(name, "n/a");

					PhoneDevice *pDNew = new PhoneDevice(name,addr,result.rssi);
					bacpy(&pDNew->m_bdaddrDongle, &m_DevInfo.bdaddr);
					
					AddDeviceToDetectionList(pDNew);
				}
				break;

			case EVT_INQUIRY_COMPLETE:
				{
					/* The inquiry ended, because of time or number of responses */
					cancel = 0;
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Inquiry complete\n");
				}
				break;

			case EVT_CMD_STATUS:
				cs = (evt_cmd_status *) ptr;
				//printf("# got a status %d opcode %d ncmd %d\n", cs->status, cs->opcode, cs->ncmd);
				/* This happens when the inquiry command fails */
				if(cs->opcode == cmd_opcode_pack(OGF_LINK_CTL, OCF_INQUIRY) && cs->status != 0) 
				{
					printf("# status error: %d\n", err);
					err = bt_error(cs->status); 
					cancel = 0;
				}
				break;

			default:
				printf("# unknown stage: %d\n", hdr->evt);
				

				break;
			}
		}
	}

	/* If we exit through a signal, cancel our current inquiry */
	if (cancel)
	{
		printf("# sending cancel\n");
		hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY_CANCEL, 0, NULL);
	}

done:
	/* Restore the old event filter */
	setsockopt(dd, SOL_HCI, HCI_FILTER, &old_flt, sizeof(old_flt));

	/* Restore the old signal handlers */
	sigaction(SIGTERM, &sa_term, NULL);
	sigaction(SIGINT,  &sa_int,  NULL);

	//printf("restored signal handlers\n");

close:
	/* Close the HCI device */
	hci_close_dev(dd);

	printf("# closed device\n");

	/* Set the errno value for perror() */
	errno = err;

	/* On error, free the results and exit */
	if (err) {
		if (inq_list) free(inq_list);
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error in bluetooth scan: %d\n", err);
		//cerr << "Error " << err << " in bluetooth scan" << endl;
		Sleep(15000); // No fast looping
		return true;
	}

	/* Return the result pointer and the number of results */
	return true;
}

int PhoneDetection_Bluetooth_Linux::GetLinkQuality(const char *addr)
{
	int dev_id = -1;
	uint16_t *handle;
	struct hci_conn_info_req *cr;
	struct hci_request rq;
	read_link_quality_rp rp;
	bdaddr_t bdaddr;
	int dd;

	//connecting
    int ptype;
    uint8_t role;

    role = 0;
    ptype = HCI_DM1 | HCI_DM3 | HCI_DM5 | HCI_DH1 | HCI_DH3 | HCI_DH5;
	str2ba(addr, &bdaddr);

	
    if(dev_id < 0)
	{
		dev_id = hci_get_route(&bdaddr);
		if(dev_id < 0)
		{
			printf("Cannot connect\n");
			return 0;
	    }
	}
		
    if(hci_create_connection(dd, &bdaddr, htobs(ptype), 0, role, handle,4000) < 0)
    {
//	    printf("Cannot create connection\n");
	//return 0;
	}
	

	//get connection links
	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		printf("HCI device open failed\n");
		return 0;
	}

	cr = (hci_conn_info_req *)malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
	if (!cr)
		return 0;

	bacpy(&cr->bdaddr, &bdaddr);
	cr->type = ACL_LINK;
	if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) {
		printf("Get connection info failed\n");
		return 0;
	}

	memset(&rq, 0, sizeof(rq));
	rq.ogf    = OGF_STATUS_PARAM;
	rq.ocf    = OCF_READ_LINK_QUALITY;
	rq.cparam = &cr->conn_info->handle;
	rq.clen   = 2;
	rq.rparam = &rp;
	rq.rlen   = READ_LINK_QUALITY_RP_SIZE;

	if (hci_send_req(dd, &rq, 100) < 0) {
		printf("HCI get_link_quality request failed\n");
		return 0;
	}

	if (rp.status) {
		printf("HCI get_link_quality cmd failed (0x%2.2X)\n", rp.status);
		return 0;
	}
	
	int link_quality = rp.link_quality;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Link quality for %s: %d", addr, link_quality);
	
	close(dd);
	free(cr);

	return link_quality;
}
