/**
 * MAMEMachine - a container for ROM metadata
 *
 * Version: 1.0
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef MAMEMACHINE_H
#define MAMEMACHINE_H

#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std; 

// String trimming functions.
// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

class MAMEDipValue
{
 private:
  string m_sName;
  string m_sValue;
  string m_sDefault;

 public:
  MAMEDipValue(string sName,
	       string sValue,
	       string sDefault)
    {
      m_sName=sName;
      m_sValue=sValue;
      m_sDefault=sDefault;
    }

  string to_json()
  {
    string sRet="";
    sRet+="{";
    sRet+="\"name\":\""+m_sName+"\", ";
    sRet+="\"value\":\""+m_sValue+"\", ";
    sRet+="\"default\":\""+m_sDefault+"\"";
    sRet+="}";
    return sRet;
  }

};

class MAMEDipSwitch
{
 private:
  string m_sName;
  string m_sTag;
  string m_sMask;
  vector <MAMEDipValue *> m_vectDipValues;

 public:
  MAMEDipSwitch(string sName,
		string sTag,
		string sMask)
    {
      m_sName=sName;
      m_sTag=sTag;
      m_sMask=sMask;
    }

  virtual ~MAMEDipSwitch()
    {
      for (vector<MAMEDipValue*>::iterator it=m_vectDipValues.begin();it!=m_vectDipValues.end();++it)
	{
	  delete *it;
	}
    }

  void AddDipValue(MAMEDipValue* dipValue)
  {
    m_vectDipValues.push_back(dipValue);
  }

  string MachineDipValues_get()
  {
    string sRet="";
    for (vector<MAMEDipValue*>::iterator it=m_vectDipValues.begin();it!=m_vectDipValues.end();++it)
      {
	if (it!=m_vectDipValues.begin())
	  sRet+=", ";

	MAMEDipValue* v=*it;
	sRet+=v->to_json();
      }
    return sRet;
  }
  
  string to_json()
  {
    string sRet="";
    sRet+="{";
    sRet+="\"name\":\""+m_sName+"\", ";
    sRet+="\"tag\":\""+m_sTag+"\", ";
    sRet+="\"mask\":\""+m_sMask+"\", ";
    sRet+="\"values\":["+MachineDipValues_get()+"]";
    sRet+="}";
    return sRet;
  }

};

class MAMEControl
{
 private:
  string m_sType;
  string m_sMinimum;
  string m_sMaximum;
  string m_sSensitivity;
  string m_sKeyDelta;
  string m_sReverse;
  string m_sWays;
  string m_sWays2;
  string m_sWays3;
  
 public:
  MAMEControl(string sType,
	      string sMinimum,
	      string sMaximum,
	      string sSensitivity,
	      string sKeyDelta,
	      string sReverse,
	      string sWays,
	      string sWays2,
	      string sWays3)
    {
      m_sType=sType;
      m_sMinimum=sMinimum;
      m_sMaximum=sMaximum;
      m_sSensitivity=sSensitivity;
      m_sKeyDelta=sKeyDelta;
      m_sReverse=sReverse;
      m_sWays=sWays;
      m_sWays2=sWays2;
      m_sWays3=sWays3;
    }

  string to_json()
  {
    string sRet="";
    sRet+="{";
    sRet+="\"type\":\""+m_sType+"\", ";
    sRet+="\"minimum\":\""+m_sMinimum+"\", ";
    sRet+="\"maximum\":\""+m_sMaximum+"\", ";
    sRet+="\"sensitivity\":\""+m_sSensitivity+"\", ";
    sRet+="\"keydelta\":\""+m_sKeyDelta+"\", ";
    sRet+="\"reverse\":\""+m_sReverse+"\",";
    sRet+="\"ways\":\""+m_sWays+"\", ";
    sRet+="\"ways2\":\""+m_sWays2+"\", ";
    sRet+="\"ways3\":\""+m_sWays3+"\"";
    sRet+="}";
    return sRet;
  }
  
};

class MAMEInput
{
 private:
  string m_sService;
  string m_sTilt;
  string m_sPlayers;
  string m_sButtons;
  string m_sCoins;
  vector<MAMEControl*> m_vectMAMEControls;

 public:
  MAMEInput(string sService,
	    string sTilt,
	    string sPlayers,
	    string sButtons,
	    string sCoins)
    {
      m_sService=sService;
      m_sTilt=sTilt;
      m_sButtons=sButtons;
      m_sCoins=sCoins;
    }
  
  virtual ~MAMEInput()
    {
      for (vector<MAMEControl*>::iterator it=m_vectMAMEControls.begin(); it!=m_vectMAMEControls.end(); ++it)
	{
	  delete *it;
	}
    }

  string MachineInputControls_get()
  {
    string sRet;
    for (vector<MAMEControl*>::iterator it=m_vectMAMEControls.begin(); it!=m_vectMAMEControls.end(); ++it)
      {
	MAMEControl* c=*it;
	if (it!=m_vectMAMEControls.begin())
	  {
	    sRet += ", ";
	  }
	sRet+=c->to_json();
      }
    return sRet;
  }

  void MachineInputControls_add(MAMEControl* control)
  {
    m_vectMAMEControls.push_back(control);
  }

  string to_json()
  {
    string sRet="";
    sRet+="{";
    sRet+="\"service\":\""+m_sService+"\", ";
    sRet+="\"tilt\":\""+m_sTilt+"\", ";
    sRet+="\"players\":\""+m_sPlayers+"\", ";
    sRet+="\"buttons\":\""+m_sButtons+"\", ";
    sRet+="\"coins\":\""+m_sCoins+"\", ";
    sRet+="\"controls\":["+MachineInputControls_get()+"]";
    sRet+="}";
    return sRet;
  }

};

/**
 * A single MAME Display entry.
 */
class MAMEDisplay
{
 private:

  string m_sTag;
  string m_sType;
  string m_sRotate;
  string m_sFlipX;
  string m_sWidth;
  string m_sHeight;
  string m_sRefresh;
  string m_sPixClock;
  string m_sHTotal;
  string m_sHBEnd;
  string m_sHBStart;
  string m_sVTotal;
  string m_sVBEnd;
  string m_sVBStart;

 public:

  // ctor
  MAMEDisplay(string sTag,
	      string sType,
	      string sRotate,
	      string sFlipX,
	      string sWidth,
	      string sHeight,
	      string sRefresh,
	      string sPixClock,
	      string sHTotal,
	      string sHBEnd,
	      string sHBStart,
	      string sVTotal,
	      string sVBEnd,
	      string sVBStart)
    {
      m_sTag=sTag;
      m_sType=sType;
      m_sRotate=sRotate;
      m_sFlipX=sFlipX;
      m_sWidth=sWidth;
      m_sHeight=sHeight;
      m_sRefresh=sRefresh;
      m_sPixClock=sPixClock;
      m_sHTotal=sHTotal;
      m_sHBEnd=sHBEnd;
      m_sHBStart=sHBStart;
      m_sVTotal=sVTotal;
      m_sVBEnd=sVBEnd;
      m_sVBStart=sVBStart;
    }

  // to json
  string to_json() 
  {
    string sRet = "";
    sRet = "{";
    sRet+="\"tag\":\""+m_sTag+"\", ";
    sRet+="\"type\":\""+m_sType+"\", ";
    sRet+="\"rotate\":\""+m_sRotate+"\", ";
    sRet+="\"flipx\":\""+m_sFlipX+"\", ";
    sRet+="\"width\":\""+m_sWidth+"\", ";
    sRet+="\"height\":\""+m_sHeight+"\", ";
    sRet+="\"refresh\":\""+m_sRefresh+"\", ";
    sRet+="\"pixclock\":\""+m_sPixClock+"\", ";
    sRet+="\"htotal\":\""+m_sHTotal+"\", ";
    sRet+="\"hbend\":\""+m_sHBEnd+"\", ";
    sRet+="\"hbstart\":\""+m_sHBStart+"\", ";
    sRet+="\"vtotal\":\""+m_sVTotal+"\", ";
    sRet+="\"vbend\":\""+m_sVBEnd+"\", ";
    sRet+="\"vbstart\":\""+m_sVBStart+"\"";
    sRet+="}";
    return sRet;
  }

};

/**
 * A MAME Machine, the central object for parsing
 */
class MAMEMachine
{
 private:

  // Singular Entries

  string m_sMachineName;
  string m_sMachineDescription;
  string m_sMachineManufacturer;
  string m_sMachineYear;
  string m_sMachineCloneOf;
  string m_sMachineRomOf;
  string m_sMachineDriverStatus;
  string m_sMachineDriverEmulationStatus;
  string m_sMachineDriverColorStatus;
  string m_sMachineDriverSoundStatus;
  string m_sMachineDriverGraphicStatus;
  string m_sMachineDriverSaveStateStatus;
  string m_sMachineGenre;
  string m_sMachineRomSHA1;
  string m_sMachinePicturePath;
  string m_sMachineHistory;

  // Primary Keys
  long int m_liPK_NameHash;
  long int m_liPK_Game;
  long int m_liPK_Manufacturer_NameHash;
  long int m_liPK_Manufacturer;
  long int m_liPK_Genre_NameHash;
  long int m_liPK_Genre;
  long int m_liPK_Game_GameSystem;
  long int m_liPK_Rom;
  long int m_liPK_Game_GameSystem_Rom;
  long int m_liPK_Picture;
  long int m_liPK_Game_GameSystem_Picture;
  long int m_liPK_Configuration;
  long int m_liPK_Game_GameSystem_Configuration;

  // vectors
  vector<MAMEDisplay *> m_vectMAMEDisplay; 
  vector<MAMEDipSwitch *> m_vectMAMEDipSwitch;
  
  // objects
  MAMEInput* m_MAMEInput;

 public:
  MAMEMachine() 
    {
      m_sMachineName="";
      m_sMachineDescription="";
      m_sMachineManufacturer="";
      m_sMachineYear="";
      m_sMachineCloneOf="";
      m_sMachineRomOf="";
      m_sMachineDriverStatus="";
      m_sMachineDriverEmulationStatus="";
      m_sMachineDriverColorStatus="";
      m_sMachineDriverSoundStatus="";
      m_sMachineDriverGraphicStatus="";
      m_sMachineDriverSaveStateStatus="";
      m_sMachineGenre="";
      m_sMachineRomSHA1="";
      m_sMachinePicturePath="";
      m_sMachineHistory="";
      m_liPK_NameHash=0;
      m_liPK_Game=0;
      m_liPK_Manufacturer_NameHash=0;
      m_liPK_Manufacturer=0;
      m_liPK_Genre_NameHash=0;
      m_liPK_Genre=0;
      m_liPK_Game_GameSystem=0;
      m_liPK_Rom=0;
      m_liPK_Game_GameSystem_Rom=0;
      m_liPK_Picture=0;
      m_liPK_Game_GameSystem_Picture=0;
      m_liPK_Configuration=0;
      m_liPK_Game_GameSystem_Configuration=0;
      m_MAMEInput=NULL;
    }

  virtual ~MAMEMachine()
    {

      if (m_MAMEInput!=NULL)
	{
	  delete m_MAMEInput;
	}

      for (vector<MAMEDisplay* >::iterator it=m_vectMAMEDisplay.begin(); it!=m_vectMAMEDisplay.end(); ++it)
	{
	  MAMEDisplay* display=*it;
	  delete display;
	}

      for (vector<MAMEDipSwitch* >::iterator it=m_vectMAMEDipSwitch.begin(); it!=m_vectMAMEDipSwitch.end(); ++it)
	{
	  MAMEDipSwitch* dipswitch=*it;
	  delete dipswitch;
	}
    }

  // Special functions for accessors

  string descriptionFor(string sMachineDescription)
  {
    size_t found = sMachineDescription.find_first_of("([{");

    if (found==string::npos)
      {
	return sMachineDescription;
      }
    
    string ret = sMachineDescription.substr(0,found);
    ret = trim(ret);
    return ret;
  }

  string subDescriptionFor(string sMachineDescription)
  {
    size_t found = sMachineDescription.find_first_of("([{");

    if (found==string::npos)
      {
	return "";
      }
    
    return sMachineDescription.substr(found);
  }

  // Get accessors
  string MachineName_get() {return m_sMachineName;}
  string MachineDescription_get() {return descriptionFor(m_sMachineDescription);}
  string MachineSubDescription_get() {return subDescriptionFor(m_sMachineDescription);}
  string MachineManufacturer_get() {return m_sMachineManufacturer;}
  string MachineYear_get() {return m_sMachineYear;}
  string MachineCloneOf_get() {return m_sMachineCloneOf;}
  string MachineRomOf_get() {return m_sMachineRomOf;}
  string MachineDriverStatus_get() {return m_sMachineDriverStatus;}
  string MachineDriverEmulationStatus_get() {return m_sMachineDriverEmulationStatus;}
  string MachineDriverColorStatus_get() {return m_sMachineDriverColorStatus;}
  string MachineDriverSoundStatus_get() {return m_sMachineDriverSoundStatus;}
  string MachineDriverGraphicStatus_get() {return m_sMachineDriverGraphicStatus;}
  string MachineDriverSaveStateStatus_get() {return m_sMachineDriverSaveStateStatus;}
  string MachineGenre_get() {return m_sMachineGenre;}
  string MachineRomSHA1_get() {return m_sMachineRomSHA1;}
  string MachinePicturePath_get() {return m_sMachinePicturePath;}
  string MachineHistory_get() {return m_sMachineHistory;}
  long int liPK_NameHash_get() {return m_liPK_NameHash;}
  long int liPK_Game_get() {return m_liPK_Game;}
  long int liPK_Manufacturer_NameHash_get() {return m_liPK_Manufacturer_NameHash;}
  long int liPK_Manufacturer_get() {return m_liPK_Manufacturer;}
  long int liPK_Genre_NameHash_get() {return m_liPK_Genre_NameHash;}
  long int liPK_Genre_get() {return m_liPK_Genre;}
  long int liPK_Game_GameSystem_get() {return m_liPK_Game_GameSystem;}
  long int liPK_Rom_get() {return m_liPK_Rom;}
  long int liPK_Game_GameSystem_Rom_get() {return m_liPK_Game_GameSystem_Rom;}
  long int liPK_Picture_get() {return m_liPK_Picture;}
  long int liPK_Game_GameSystem_Picture() {return m_liPK_Game_GameSystem_Picture;}
  long int liPK_Configuration_get() {return m_liPK_Configuration;}
  long int liPK_Game_GameSystem_Configuration() {return m_liPK_Game_GameSystem_Configuration;}

  vector<MAMEDisplay *> MachineDisplays_get() {return m_vectMAMEDisplay;}
  MAMEInput* MachineInput_get() {return m_MAMEInput;}
  vector<MAMEDipSwitch*> MachineDipSwitches_get() {return m_vectMAMEDipSwitch;}


  // Set accessors
  void MachineName_set(string sMachineName) {m_sMachineName=sMachineName;}
  void MachineDescription_set(string sMachineDescription) {m_sMachineDescription=sMachineDescription;}
  void MachineManufacturer_set(string sMachineManufacturer) {m_sMachineManufacturer=sMachineManufacturer;}
  void MachineYear_set(string sMachineYear) {m_sMachineYear=sMachineYear;}
  void MachineCloneOf_set(string sMachineCloneOf) {m_sMachineCloneOf=sMachineCloneOf;}
  void MachineRomOf_set(string sMachineRomOf) {m_sMachineRomOf=sMachineRomOf;}
  void MachineDriverStatus_set(string sMachineDriverStatus) {m_sMachineDriverStatus=sMachineDriverStatus;}
  void MachineDriverEmulationStatus_set(string sMachineDriverEmulationStatus) {m_sMachineDriverEmulationStatus=sMachineDriverEmulationStatus;}
  void MachineDriverColorStatus_set(string sMachineDriverColorStatus) {m_sMachineDriverColorStatus=sMachineDriverColorStatus;}
  void MachineDriverSoundStatus_set(string sMachineDriverSoundStatus) {m_sMachineDriverSoundStatus=sMachineDriverSoundStatus;}
  void MachineDriverGraphicStatus_set(string sMachineDriverGraphicStatus) {m_sMachineDriverGraphicStatus=sMachineDriverGraphicStatus;}
  void MachineDriverSaveStateStatus_set(string sMachineDriverSaveStateStatus) {m_sMachineDriverSaveStateStatus=sMachineDriverSaveStateStatus;}
  void MachineGenre_set(string sMachineGenre) {m_sMachineGenre=sMachineGenre;}
  void MachineRomSHA1_set(string sMachineRomSHA1) {m_sMachineRomSHA1=sMachineRomSHA1;}
  void MachinePicturePath_set(string sMachinePicturePath) {m_sMachinePicturePath=sMachinePicturePath;}
  void MachineHistory_set(string sMachineHistory) {m_sMachineHistory=sMachineHistory;}
  void liPK_NameHash_set(long int liPK_NameHash) {m_liPK_NameHash=liPK_NameHash;}
  void liPK_Game_set(long int liPK_Game) {m_liPK_Game=liPK_Game;}
  void liPK_Manufacturer_NameHash_set(long int liPK_Manufacturer_NameHash) {m_liPK_Manufacturer_NameHash=liPK_Manufacturer_NameHash;}
  void liPK_Manufacturer_set(long int liPK_Manufacturer) {m_liPK_Manufacturer=liPK_Manufacturer;}
  void liPK_Genre_NameHash_set(long int liPK_Genre_NameHash) {m_liPK_Genre_NameHash=liPK_Genre_NameHash;}
  void liPK_Genre_set(long int liPK_Genre) {m_liPK_Genre=liPK_Genre;}
  void liPK_Game_GameSystem_set(long int liPK_Game_GameSystem) {m_liPK_Game_GameSystem=liPK_Game_GameSystem;}
  void liPK_Rom_set(long int liPK_Rom) {m_liPK_Rom=liPK_Rom;}
  void liPK_Game_GameSystem_Rom_set(long int liPK_Game_GameSystem_Rom) {m_liPK_Game_GameSystem_Rom=liPK_Game_GameSystem_Rom;}
  void liPK_Picture_set(long int liPK_Picture) {m_liPK_Picture=liPK_Picture;}
  void liPK_Game_GameSystem_Picture_set(long int liPK_Game_GameSystem_Picture) {m_liPK_Game_GameSystem_Picture=liPK_Game_GameSystem_Picture;}
  void liPK_Configuration_set(long int liPK_Configuration) {m_liPK_Configuration=liPK_Configuration;}
  void liPK_Game_GameSystem_Configuration_set(long int liPK_Game_GameSystem_Configuration) {m_liPK_Game_GameSystem_Configuration=liPK_Game_GameSystem_Configuration;}

  void MachineInput_set(MAMEInput* input) 
  {
    m_MAMEInput=input;
  }

  void MachineDriverDipswitchAdd(MAMEDipSwitch* dipswitch) {m_vectMAMEDipSwitch.push_back(dipswitch);}
  void MachineDriverDisplayAdd(MAMEDisplay* display) {m_vectMAMEDisplay.push_back(display);}
  
  string MachineConfigurationDipswitches_get()
  {
    string sRet="";
    for (vector<MAMEDipSwitch*>::iterator it=m_vectMAMEDipSwitch.begin(); it!=m_vectMAMEDipSwitch.end(); ++it)
      {
	MAMEDipSwitch* d = *it;
	if (it!=m_vectMAMEDipSwitch.begin())
	  {
	    sRet += ", ";
	  }
	sRet += d->to_json();
      }
    return sRet;
  }

  /**
   * Calls MAMEDisplay.to_json() on each in vector to output complete displays block
   */
  string MachineConfigurationDisplays_get()
  {
    string sRet="";
    for (vector<MAMEDisplay*>::iterator it=m_vectMAMEDisplay.begin(); it!=m_vectMAMEDisplay.end(); ++it)
      {
	MAMEDisplay* d = *it;
	if (it!=m_vectMAMEDisplay.begin())
	  {
	    sRet += ", ";
	  }
	sRet += d->to_json();
      }
    return sRet;
  }

  string MachineConfiguration_get()
  {
    string sRet="";
    sRet+="{";
    sRet+="\"input\":"+MachineInput_get()->to_json()+", ";
    sRet+="\"displays\":["+MachineConfigurationDisplays_get()+"], ";
    sRet+="\"dipswitches\":["+MachineConfigurationDipswitches_get()+"]";
    sRet+="}";

    return sRet;
  }
  
};

#endif /** MAMEMACHINE_H */
