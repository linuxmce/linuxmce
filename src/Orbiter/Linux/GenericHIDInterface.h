#ifndef GenericHIDInterface_h
#define GenericHIDInterface_h

#include "../Orbiter.h"
#include "AlarmManager.h"
#include <vector>

#define HAVE_STDBOOL_H
#define ALARM_CHECK_FOR_NEW_DEVICES 1

void *ProcessGenericHIDEvents(void *p);

class GenericHIDInterface : public AlarmEvent
{
 private:
  bool m_bRunning; // Is the thread running?
  pluto_pthread_mutex_t m_GenericHIDMutex; 
  int m_iHoldingDownButton, m_iRepeat, m_iLastButtonPress;
  time_t m_tLastButtonPress;
  Orbiter *m_pOrbiter;
  vector<int> m_vEventFDs;   // Vector of Event File Descriptors
  void LogFDs(vector<int> vectFDs); // Log the current file descriptors.

 public:
  bool m_bDeviceDead;		// We lost a device.
  class AlarmManager *m_pAlarmManager;     // Our Alarm Manager
  GenericHIDInterface(Orbiter *pOrbiter);
  void ProcessGenericHIDEvents();
  void UpdateEventDevices();
  bool DecodeEventInFD(int fd);
  void CloseEventDevices();
  void AlarmCallback(int id, void* param);
};

#endif // GenericHIDInterface_h
