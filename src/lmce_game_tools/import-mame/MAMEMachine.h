/**
 * MAMEMachine - a container for ROM metadata
 *
 * Version: 1.0
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef MAMEMACHINE_H
#define MAMEMACHINE_H

#include <string>

using namespace std; 

class MAMEMachine
{
 private:
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
  long int m_liPK_NameHash;
  long int m_liPK_Game;
  long int m_liPK_Manufacturer_NameHash;
  long int m_liPK_Manufacturer;
  long int m_liPK_Genre_NameHash;
  long int m_liPK_Genre;
  long int m_liPK_Game_GameSystem;

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
      m_liPK_NameHash=0;
      m_liPK_Game=0;
      m_liPK_Manufacturer_NameHash=0;
      m_liPK_Manufacturer=0;
      m_liPK_Genre_NameHash=0;
      m_liPK_Genre=0;
      m_liPK_Game_GameSystem=0;
    }

  virtual ~MAMEMachine()
    {
      
    }

  // Special functions for accessors
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
  string MachineDescription_get() {return m_sMachineDescription;}
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
  long int liPK_NameHash_get() {return m_liPK_NameHash;}
  long int liPK_Game_get() {return m_liPK_Game;}
  long int liPK_Manufacturer_NameHash_get() {return m_liPK_Manufacturer_NameHash;}
  long int liPK_Manufacturer_get() {return m_liPK_Manufacturer;}
  long int liPK_Genre_NameHash_get() {return m_liPK_Genre_NameHash;}
  long int liPK_Genre_get() {return m_liPK_Genre;}
  long int liPK_Game_GameSystem_get() {return m_liPK_Game_GameSystem;}

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
  void liPK_NameHash_set(long int liPK_NameHash) {m_liPK_NameHash=liPK_NameHash;}
  void liPK_Game_set(long int liPK_Game) {m_liPK_Game=liPK_Game;}
  void liPK_Manufacturer_NameHash_set(long int liPK_Manufacturer_NameHash) {m_liPK_Manufacturer_NameHash=liPK_Manufacturer_NameHash;}
  void liPK_Manufacturer_set(long int liPK_Manufacturer) {m_liPK_Manufacturer=liPK_Manufacturer;}
  void liPK_Genre_NameHash_set(long int liPK_Genre_NameHash) {m_liPK_Genre_NameHash=liPK_Genre_NameHash;}
  void liPK_Genre_set(long int liPK_Genre) {m_liPK_Genre=liPK_Genre;}
  void liPK_Game_GameSystem_set(long int liPK_Game_GameSystem) {m_liPK_Game_GameSystem=liPK_Game_GameSystem;}

};

#endif /** MAMEMACHINE_H */
