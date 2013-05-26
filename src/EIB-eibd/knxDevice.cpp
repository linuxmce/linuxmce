#include "knxDevice.h"

namespace knx
{
	void knxDevice::commandUnhandled(string scommand)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,
					"command [ %s ] not handled by device [ %s ]", scommand.c_str(), stype().c_str());
	}

	knxDevice::knxDevice(DeviceData_Impl *pDeviceData_Impl)
	{
		_device_DCEID=pDeviceData_Impl->m_dwPK_Device;
		string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
		_v_addrlist.clear();
		int bindex = 0, length = 0;
		while(bindex >= 0) {
			length = sChannel.find('|', bindex);
			if(length < 0) {
				_v_addrlist.push_back(Telegram::stringtogaddr(sChannel.substr(bindex, sChannel.length() - bindex)));
				break;
			} else {
				_v_addrlist.push_back(Telegram::stringtogaddr(sChannel.substr(bindex, length - bindex)));
			}
			bindex = length + 1;
		}
	}

	knxDevice *knxDevice::getKnxDevice(DeviceData_Impl *pDevData)
	{
		switch(pDevData->m_dwPK_DeviceTemplate)
		{
			case DEVICETEMPLATE_Standard_Thermostat_CONST: return new Standard_Thermostat(pDevData);break;		// <SET TEMP>|<ACTUEL TEMP>
			case DEVICETEMPLATE_Light_Switch_onoff_CONST: return new LightSwitchOnOff(pDevData);break;			// <ON/OFF>|<ON/OFF STATUS>
			case DEVICETEMPLATE_Wall_Outlet_CONST:return new LightSwitchOnOff(pDevData);break; 
			case DEVICETEMPLATE_Light_Switch_dimmable_CONST: return new LightSwitchdimmable(pDevData);break;	// <ON/OFF>|<DIM VALUE>|<ON/OFF STATUS>|<DIM STATUS>
			case DEVICETEMPLATE_Light_Switch_RGB_CONST: return new LightSwitchRGB(pDevData);break;				// <ON/OFF>|<R VALUE>|<G VALUE>|<B VALUE>|<ON/OFF STATUS>|<R STATUS>|<G STATUS>|<B STATUS>
			case DEVICETEMPLATE_Drapes_Switch_CONST: return new Drapes_Switch(pDevData);break;
			case DEVICETEMPLATE_Blinds_Switch_CONST: return new Blinds_Switch(pDevData);break;					// <LEVEL>|<ANGLE>|<LEVEL STATUS>|<ANGLE STATUS>
			case DEVICETEMPLATE_Brightness_sensor_CONST: return new BrightnessSensor(pDevData);break;			// <BRIGHTNESS>
			case DEVICETEMPLATE_Temperature_sensor_CONST: return new TemperatureSensor(pDevData);break;			// <TEMP>
			case DEVICETEMPLATE_Generic_Input_Ouput_CONST:
			case DEVICETEMPLATE_Air_Quality_Sensor_CONST:
			case DEVICETEMPLATE_Door_Sensor_CONST:
			case DEVICETEMPLATE_Glass_Break_Sensor_CONST:
			case DEVICETEMPLATE_Motion_Detector_CONST:
			case DEVICETEMPLATE_Siren_CONST:
			case DEVICETEMPLATE_Smoke_Detector_CONST:
			case DEVICETEMPLATE_Doorbell_button_CONST:
					return new GenericSensor(pDevData);break;
		}
		return NULL;
	}

	vector<Telegram> knxDevice::ReadVectFromTable(int tablesize, const char *table)
	{
		vector<Telegram> retour;
		for(int i=0;i<tablesize;i++)
		{
			try
			{
				Telegram tl;
				tl.setType(EIBREAD);
				tl.setGroupAddress(_v_addrlist.at(table[i]));
				retour.push_back(tl);
			}catch(out_of_range &e){//we tryed to access a group address that was not referenced
				return retour;
			}
		}
	return retour;
	}

	Telegram *knxDevice::createsTelegramFromAddress(char data, int addressnumber)
	{
		try
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"creating telegram with shortdata %i",data);
			eibaddr_t addr= _v_addrlist.at(addressnumber);
			Telegram *retour=new Telegram();
			retour->setGroupAddress(addr);
			retour->setShortUserData(data);
			return retour;
		}catch(out_of_range &e){
			return NULL;
		}
	}

	Telegram *knxDevice::createcharTelegramFromAddress(char data, int addressnumber)
	{
		try
		{
			eibaddr_t addr= _v_addrlist.at(addressnumber);
			Telegram *retour=new Telegram();
			retour->setGroupAddress(addr);
			retour->setDataFromChar(data);
			return retour;
		}catch(out_of_range &e){
			return NULL;
		}
	}

	Telegram *knxDevice::createfloatTelegramFromAddress(float data, int addressnumber)
	{
		try
		{
			eibaddr_t addr= _v_addrlist.at(addressnumber);
			Telegram *retour=new Telegram();
			retour->setGroupAddress(addr);
			retour->setDataFromFloat(data);
			return retour;
		}catch(out_of_range e){
			return NULL;
		}
	}



// here must be implemented the commands the device must handle: those redeclared in its declaration
// handleTelegram must be re declared too
	Message *LightSwitchOnOff::handleTelegram(const Telegram *tl) // <ON/OFF>|<ON/OFF STATUS>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(1)) // ON / OFF STATUS
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // ON / OFF
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *LightSwitchdimmable::handleTelegram(const Telegram *tl) // <ON/OFF>|<DIM VALUE>|<ON/OFF STATUS>|<DIM STATUS>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(2)) // ON/OFF STATUS
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(3)) // DIM STATUS
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // ON/OFF
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(1)) // DIM VALUE
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}
	
Message *LightSwitchRGB::handleTelegram(const Telegram *tl) // // <ON/OFF>|<R VALUE>|<G VALUE>|<B VALUE>|<ON/OFF STATUS>|<R STATUS>|<G STATUS>|<B STATUS>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(4)) // ON/OFF STATUS
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(5)) // R STATUS
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(6)) // G STATUS
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(7)) // B STATUS
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // ON/OFF
					{
						return createStateChangedEventMessage(tl->getShortUserData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(1)) // R VALUE
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(2)) // G VALUE
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(3)) // B VALUE
					{
						return createLevelChangedEventMessage(tl->getIntData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *Drapes_Switch::handleTelegram(const Telegram *tl) // <LEVEL>|<LEVEL STATUS> (|<OPEN/CLOSE>|<STOP>)
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(1))
					{
						return createLevelChangedEventMessage(tl->getUIntData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0))
					{
						return createLevelChangedEventMessage(tl->getUIntData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *Blinds_Switch::handleTelegram(const Telegram *tl) // <LEVEL>|<ANGLE>|<LEVEL STATUS>|<ANGLE STATUS>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(2))	// Actual level
					{
						return createLevelChangedEventMessage(tl->getUIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(3))	// Actuel angle
					{
						return createAngleChangedEventMessage(tl->getIntData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0))	// Set level
					{
						return createLevelChangedEventMessage(tl->getUIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(1))	// Set angle
					{
						return createAngleChangedEventMessage(tl->getIntData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *Standard_Thermostat::handleTelegram(const Telegram *tl) // <SET TEMP>|<ACTUEL TEMP>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(2)) // Current set temperature
					{
						return createSetPointChangedEventMessage(tl->getFloatData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(3)) // Current set mode
					{
						return createModeChangedEventMessage(tl->getIntData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(4)) // Current temperature
					{
						return createTemperatureChangedEventMessage(tl->getFloatData());
					}
				}break;
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // Set Temperature
					{
						return createSetPointChangedEventMessage(tl->getFloatData());
					}
					if(tl->getGroupAddress()==_v_addrlist.at(1)) // Set Mode
					{
						return createModeChangedEventMessage(tl->getIntData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}
	
	Message *BrightnessSensor::handleTelegram(const Telegram *tl) // <BRIGHTNESS>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // Current brightness
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "  Actual Brightness is %f",tl->getFloatData());
						return createBrightnessChangedEventMessage(tl->getFloatData());
					}
				}break;
							}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *TemperatureSensor::handleTelegram(const Telegram *tl) // <TEMP>
	{
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				case(EIBRESPONSE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0)) // Current temperature
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS, "  Actual temperature is %f",tl->getFloatData());
						return createTemperatureChangedEventMessage(tl->getFloatData());
					}
				}break;
							}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

	Message *GenericSensor::handleTelegram(const Telegram *tl)
	{
		//LoggerWrapper::GetInstance()->Write("LV_STATUS, "genericsensor trying to handle Telegram
		try
		{
			switch(tl->getType())
			{
				case(EIBWRITE):
				{
					if(tl->getGroupAddress()==_v_addrlist.at(0))
					{
						return createSensorTrippedEventMessage(tl->getShortUserData());
					}
				}break;
			}
		}catch(out_of_range e){
			return NULL;
		}
		return NULL;
	}

}

