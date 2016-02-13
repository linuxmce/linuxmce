/**
 * MAMEXMLParser - A pugixml based parser to extract relevant data
 * from mame's -listxml output.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#ifndef MAMEXMLPARSER_H
#define MAMEXMLPARSER_H

#include <string>
#include <map>

#include "pugixml.hpp"

using namespace std;

/**
 * A simple container for Roms
 */
class MAMEMachine
{
 private:
  string m_sMachineName;
  string m_sMachineDescription;
  string m_sMachineManufacturer;
  string m_sMachineYear;
  string m_sMachineCloneOf;
  string m_sMachineDriverStatus;
  string m_sMachineDriverEmulationStatus;
  string m_sMachineDriverColorStatus;
  string m_sMachineDriverSoundStatus;
  string m_sMachineDriverGraphicStatus;
  string m_sMachineDriverSaveStateStatus;

 public:
  MAMEMachine() 
    {
      m_sMachineName="";
      m_sMachineDescription="";
      m_sMachineManufacturer="";
      m_sMachineYear="";
      m_sMachineCloneOf="";
      m_sMachineDriverStatus="";
      m_sMachineDriverEmulationStatus="";
      m_sMachineDriverColorStatus="";
      m_sMachineDriverSoundStatus="";
      m_sMachineDriverGraphicStatus="";
      m_sMachineDriverSaveStateStatus="";
    }

  virtual ~MAMEMachine()
    {
      
    }

  // Get accessors
  string MachineName_get() {return m_sMachineName;}
  string MachineDescription_get() {return m_sMachineDescription;}
  string MachineManufacturer_get() {return m_sMachineManufacturer;}
  string MachineYear_get() {return m_sMachineYear;}
  string MachineCloneOf_get() {return m_sMachineCloneOf;}
  string MachineDriverStatus_get() {return m_sMachineDriverStatus;}
  string MachineDriverEmulationStatus_get() {return m_sMachineDriverEmulationStatus;}
  string MachineDriverColorStatus_get() {return m_sMachineDriverColorStatus;}
  string MachineDriverSoundStatus_get() {return m_sMachineDriverSoundStatus;}
  string MachineDriverGraphicStatus_get() {return m_sMachineDriverGraphicStatus;}
  string MachineDriverSaveStateStatus_get() {return m_sMachineDriverSaveStateStatus;}

  // Set accessors
  void MachineName_set(string sMachineName) {m_sMachineName=sMachineName;}
  void MachineDescription_set(string sMachineDescription) {m_sMachineDescription=sMachineDescription;}
  void MachineManufacturer_set(string sMachineManufacturer) {m_sMachineManufacturer=sMachineManufacturer;}
  void MachineYear_set(string sMachineYear) {m_sMachineYear=sMachineYear;}
  void MachineCloneOf_set(string sMachineCloneOf) {m_sMachineCloneOf=sMachineCloneOf;}
  void MachineDriverStatus_set(string sMachineDriverStatus) {m_sMachineDriverStatus=sMachineDriverStatus;}
  void MachineDriverEmulationStatus_set(string sMachineDriverEmulationStatus) {m_sMachineDriverEmulationStatus=sMachineDriverEmulationStatus;}
  void MachineDriverColorStatus_set(string sMachineDriverColorStatus) {m_sMachineDriverColorStatus=sMachineDriverColorStatus;}
  void MachineDriverSoundStatus_set(string sMachineDriverSoundStatus) {m_sMachineDriverSoundStatus=sMachineDriverSoundStatus;}
  void MachineDriverGraphicStatus_set(string sMachineDriverGraphicStatus) {m_sMachineDriverGraphicStatus=sMachineDriverGraphicStatus;}
  void MachineDriverSaveStateStatus_set(string sMachineDriverSaveStateStatus) {m_sMachineDriverSaveStateStatus=sMachineDriverSaveStateStatus;}

};

class MAMEXMLParser
{

 public:
  MAMEXMLParser(string sMAMEPath);
  virtual ~MAMEXMLParser();

  map<string, MAMEMachine *> m_mapRomToMAMEMachine;
  MAMEMachine * m_mapRomToMAMEMachine_Find(string sRomName)
  {
    map<string, MAMEMachine *>::iterator it=m_mapRomToMAMEMachine.find(sRomName);
    return it == m_mapRomToMAMEMachine.end() ? NULL : it->second;
  }

  bool run();

 private:
  string m_sMAMEPath;
  MAMEMachine *m_pMAMEMachine;
  string m_sRomName, m_sDescription, m_sTitleHash, m_sRomTitle, m_sRomSubtitle, m_sRomManufacturer,m_sRomYear,m_sRomStatus,m_sRomCloneOf;

  bool getMAMEOutput();
  bool parseMAMEOutput();
  bool mameOutputIsSane();

};

#endif
