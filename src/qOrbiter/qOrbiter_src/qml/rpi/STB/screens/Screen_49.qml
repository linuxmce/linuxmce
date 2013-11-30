import QtQuick 1.1

Item {
    width: manager.appWidth
    height: manager.appHeight

    Keys.onTabPressed: swapFocus()
}
