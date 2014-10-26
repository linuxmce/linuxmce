// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


ListView {
    width: manager.appHeight
    height: scaleY(30)
    orientation: ListView.Horizontal
    spacing: scaleX(1)
    Connections{
            target: dcenowplaying
            onPlayListPositionChanged:{
                positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
            }
        }

}
