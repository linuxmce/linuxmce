/**
 * FamicomEmulatorModel - Nintendo Family Computer support
 *
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "FamicomEmulatorModel.h"

namespace DCE
{

  FamicomEmulatorModel::FamicomEmulatorModel() : NESEmulatorModel()
  {
    m_sSystemName = "famicom";
  }

  FamicomEmulatorModel::~FamicomEmulatorModel()
  {
  
  }

  void FamicomEmulatorModel::updateTemplateVariables()
  {
    MESSEmulatorModel::updateTemplateVariables();
    if (m_mapConfigTemplateItems["###ROMPATH###"].empty())
      {
	m_mapConfigTemplateItems["###ROMPATH###"] = FileUtils::BasePath(m_mapMedia_Find("flop"));
      }
  }

}
