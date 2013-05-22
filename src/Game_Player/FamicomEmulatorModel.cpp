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
    // TODO: come back here and figure out a better scheme for inherited
    // configuration writers.

    // ConfigurationWriter config();
    // if (m_mapMedia_Find("default").empty())
    //   {
    //	m_pConfigurationWriter->Add("###ROMPATH###",FileUtils::BasePath(m_mapMedia_Find("flop")));
    //  }
  }

}
