/**
 * MAMEXMLParser - A pugixml based parser to extract relevant data
 * from mame's -listxml output.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#include <iostream>

#include "MAMEXMLParser.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

// NOTE TO SELF: KEEP IT ENCAPSULATED, DUMMY!

MAMEXMLParser::MAMEXMLParser(string sMAMEPath, ImportMAME* pImportMAME)
{
  m_sMAMEPath = sMAMEPath;
  m_pImportMAME = pImportMAME;
}

MAMEXMLParser::~MAMEXMLParser()
{
}

bool MAMEXMLParser::mameOutputIsSane()
{
  string sSanityOutput;
  // Sanity checks.
  if (!FileUtils::FileExists("/tmp/mame.tmp"))
    {
      cout << "MAME output at path: /tmp/mame.tmp does not exist!" << endl;
      return false;
    }

  if (FileUtils::FileSize("/tmp/mame.tmp") < 16384)
    {
      cout << "MAME output at path: /tmp/mame.tmp is too small!" << endl << endl;
      cout << "Output:" << endl;
      cout << "-------" << endl;
      FileUtils::ReadTextFile("/tmp/mame.tmp",sSanityOutput);
      cout << sSanityOutput << endl;
      return false;
    }
  return true;
}

bool MAMEXMLParser::getMAMEOutput()
{
  string sMameOutput;
  string sErr;
  const char *args[] ={m_sMAMEPath.c_str(),"-listxml",NULL};
  cout << "Grabbing XML output from MAME, please wait..." << flush;
  
  if (!FileUtils::FileExists(m_sMAMEPath))
    {
      cout << "MAME at path: " << m_sMAMEPath << " not found!" << endl;
      return false;
    }
  
  ProcessUtils::GetCommandOutput(args[0],args,sMameOutput,sErr);
  FileUtils::WriteTextFile("/tmp/mame.tmp",sMameOutput);

  // if (!mameOutputIsSane())
  //   return false;

  cout << "Done!";
  cout << endl;
  return true;
}

bool MAMEXMLParser::parseMAMEOutput()
{
  pugi::xml_document doc;
  pugi::xml_parse_result result; 

  // if (!mameOutputIsSane())
  //   return false;

  cout << "Parsing MAME output. Please wait..." << flush;
  result = doc.load_file("/tmp/mame.tmp"); 

  if (!result)
    {
      cout << "Could not parse MAME output. Error description: " << result.description() << " at offset: " << result.offset << endl;
      return false;
    }

  cout << endl << endl;

  pugi::xml_node xnMAME = doc.child("mame");

  for (pugi::xml_node xnMACHINE: xnMAME.children("machine"))
    {
      MAMEMachine* m=new MAMEMachine();
      string sChecksum;
      pugi::xml_attribute xaNAME=xnMACHINE.attribute("name");
      pugi::xml_node xnDESCRIPTION=xnMACHINE;
      pugi::xml_node xnMANUFACTURER=xnMACHINE;
      pugi::xml_node xnYEAR=xnMACHINE;
      pugi::xml_node xnDISPLAYS=xnMACHINE;
      pugi::xml_node xnINPUT=xnMACHINE;
      pugi::xml_node xnDIPSWITCHES=xnMACHINE;

      pugi::xml_attribute xaCLONEOF=xnMACHINE.attribute("cloneof");
      pugi::xml_attribute xaROMOF=xnMACHINE.attribute("romof");
      pugi::xml_attribute xaDRIVERSTATUS=xnMACHINE.child("driver").attribute("status");
      pugi::xml_attribute xaDRIVEREMULATIONSTATUS=xnMACHINE.child("driver").attribute("emulation");
      pugi::xml_attribute xaDRIVERCOLORSTATUS=xnMACHINE.child("driver").attribute("color");
      pugi::xml_attribute xaDRIVERSOUNDSTATUS=xnMACHINE.child("driver").attribute("sound");
      pugi::xml_attribute xaDRIVERGRAPHICSTATUS=xnMACHINE.child("driver").attribute("graphic");
      pugi::xml_attribute xaDRIVERSAVESTATESTATUS=xnMACHINE.child("driver").attribute("savestate");

      m->MachineName_set(xaNAME.value());
      m->MachineDescription_set(xnDESCRIPTION.child_value("description"));
      m->MachineManufacturer_set(xnMANUFACTURER.child_value("manufacturer"));
      m->MachineYear_set(xnYEAR.child_value("year"));
      m->MachineCloneOf_set(xaCLONEOF.value());
      m->MachineRomOf_set(xaROMOF.value());
      m->MachineDriverStatus_set(xaDRIVERSTATUS.value());
      m->MachineDriverEmulationStatus_set(xaDRIVEREMULATIONSTATUS.value());
      m->MachineDriverColorStatus_set(xaDRIVERCOLORSTATUS.value());
      m->MachineDriverSoundStatus_set(xaDRIVERSOUNDSTATUS.value());
      m->MachineDriverGraphicStatus_set(xaDRIVERGRAPHICSTATUS.value());
      m->MachineDriverSaveStateStatus_set(xaDRIVERSAVESTATESTATUS.value());

      m->MachineInput_set(new MAMEInput(xnINPUT.child("input").attribute("service").value(),
					xnINPUT.child("input").attribute("tilt").value(),
					xnINPUT.child("input").attribute("players").value(),
					xnINPUT.child("input").attribute("buttons").value(),
					xnINPUT.child("input").attribute("coins").value()));

      for (pugi::xml_node xnDISPLAY: xnDISPLAYS.children("display"))
	{
	  m->MachineDriverDisplayAdd(new MAMEDisplay(xnDISPLAY.attribute("tag").value(),
						     xnDISPLAY.attribute("type").value(),
						     xnDISPLAY.attribute("rotate").value(),
						     xnDISPLAY.attribute("flipx").value(),
						     xnDISPLAY.attribute("width").value(),
						     xnDISPLAY.attribute("height").value(),
						     xnDISPLAY.attribute("refresh").value(),
						     xnDISPLAY.attribute("pixclock").value(),
						     xnDISPLAY.attribute("htotal").value(),
						     xnDISPLAY.attribute("hbend").value(),
						     xnDISPLAY.attribute("hbstart").value(),
						     xnDISPLAY.attribute("vtotal").value(),
						     xnDISPLAY.attribute("vbend").value(),
						     xnDISPLAY.attribute("vbstart").value()));
	}

      for (pugi::xml_node xnDIPSWITCH: xnDIPSWITCHES.children("dipswitch"))
	{
	  MAMEDipSwitch* dipswitch = new MAMEDipSwitch(xnDIPSWITCH.attribute("name").value(),
						       xnDIPSWITCH.attribute("tag").value(),
						       xnDIPSWITCH.attribute("mask").value());
	  for (pugi::xml_node xnDIPVALUE: xnDIPSWITCH.children("dipvalue"))
	    {
	      dipswitch->AddDipValue(new MAMEDipValue(xnDIPVALUE.attribute("name").value(),
						      xnDIPVALUE.attribute("value").value(),
						      xnDIPVALUE.attribute("default").value()));
	    }
	  m->MachineDriverDipswitchAdd(dipswitch);
	}
      
      // Commit the changes to the map
      m_pImportMAME->m_mapMachineToMAMEMachine[m->MachineName_get()]=m;

    }

  return true;
}

bool MAMEXMLParser::run()
{
  if (!getMAMEOutput())
    return false;

  if (!parseMAMEOutput())
    return false;

  return true;

}
