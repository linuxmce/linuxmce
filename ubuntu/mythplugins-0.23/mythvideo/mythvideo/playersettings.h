#ifndef PLAYERSETTINGS_H
#define PLAYERSETTINGS_H

#include <uitypes.h>
#include <xmlparse.h>
#include <oldsettings.h>
#include <mythwidgets.h>
#include <mythdialogs.h>

// libmythui
#include <mythuibutton.h>
#include <mythuicheckbox.h>
#include <mythscreentype.h>
#include <mythdialogbox.h>

class PlayerSettings : public MythScreenType
{
  Q_OBJECT

  public:

    PlayerSettings(MythScreenStack *parent, const char *name = 0);
    ~PlayerSettings();

    bool Create(void);
    bool keyPressEvent(QKeyEvent *);

  private:
    MythUITextEdit   *m_defaultPlayerEdit;
    MythUITextEdit   *m_dvdPlayerEdit;
    MythUITextEdit   *m_dvdDriveEdit;
    MythUITextEdit   *m_vcdPlayerEdit;
    MythUITextEdit   *m_vcdDriveEdit;
    MythUITextEdit   *m_altPlayerEdit;

    MythUIText       *m_helpText;

    MythUICheckBox   *m_altCheck;

    MythUIButton     *m_okButton;
    MythUIButton     *m_cancelButton;

  private slots:
    void slotSave(void);
    void slotFocusChanged(void);
    void toggleAlt(void);
};

#endif

