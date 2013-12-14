import QtQuick 2.0

Item {
    width: manager.appWidth
    height: manager.appHeight

    Keys.onTabPressed: swapFocus()
}
