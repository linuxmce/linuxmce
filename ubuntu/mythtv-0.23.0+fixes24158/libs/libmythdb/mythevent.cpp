#include "mythevent.h"

QEvent::Type MythEvent::MythEventMessage =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type MythEvent::kExitToMainMenuEventType =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type MythEvent::kMythPostShowEventType =
    (QEvent::Type) QEvent::registerEventType();
QEvent::Type ExternalKeycodeEvent::kEventType =
    (QEvent::Type) QEvent::registerEventType();
