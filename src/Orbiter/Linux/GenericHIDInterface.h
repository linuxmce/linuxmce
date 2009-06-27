#ifndef GenericHIDInterface_h
#define GenericHIDInterface_h

#include "../Orbiter.h"
#include <vector>

#define HAVE_STDBOOL_H

void *ProcessGenericHIDEvents(void *p);

class GenericHIDInterface
{
 private:
  bool m_bRunning; // Is the thread running?
  pluto_pthread_mutex_t m_GenericHIDMutex; 
  int m_iHoldingDownButton, m_iRepeat, m_iLastButtonPress;
  time_t m_tLastButtonPress;
  Orbiter *m_pOrbiter;
  vector<int> m_vEventFDs;   // Vector of Event File Descriptors

 public:
  GenericHIDInterface(Orbiter *pOrbiter);
  void ProcessGenericHIDEvents();
  void UpdateEventDevices();
  bool DecodeEventInFD(int fd);
  void CloseEventDevices();
};

#endif // GenericHIDInterface_h
