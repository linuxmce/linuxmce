import QtQuick 2.2

Item {
    width: manager.appWidth
    height: manager.appHeight

    Keys.onTabPressed: swapFocus()
}
