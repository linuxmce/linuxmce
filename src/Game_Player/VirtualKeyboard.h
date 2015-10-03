/**
 * VirtualKeyboard - a class that creates a uinput virtual keyboard
 * for emulators that can't handle synthetic X events. (e.g. MAME and MESS)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

/** 
 * This class is a singleton, to be shared amongst any of the emulators that
 * need it. 
 */

#include <linux/uinput.h>

namespace DCE
{
  class VirtualKeyboard
  {
  public:
    ~VirtualKeyboard();
    static VirtualKeyboard* GetInstance();
    bool ClickKey(int iKey);

  private:
    struct uinput_user_dev m_uidev;         // uinput device struct describing device on bus.
    int m_iInputfd;                         // uinput file descriptor
    bool m_bOpened;                         // did Open() complete successfully?

    bool Open();                            // Open the Virtual Keyboard.
    void Close();                           // Close the Virtual Keyboard.

    VirtualKeyboard();
    VirtualKeyboard(VirtualKeyboard const&){};
    static VirtualKeyboard* m_pInstance;

  };
}
