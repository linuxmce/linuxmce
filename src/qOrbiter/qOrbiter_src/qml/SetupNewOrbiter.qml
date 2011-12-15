import QtQuick 1.0

Rectangle {
    width: 800
    height: 600
    color:"slategrey"
    Text {
        id: status
        text: qsTr("New Orbiter")
    }

    Rectangle {
          id: connectbutton
          height: 25
          width: 75

          Text {
              id: name
              text: qsTr("Exit")
          }

          radius:  4
          MouseArea{
          ///onClicked: //qmlSetupLmce(devicenumber.text, routerip.text)
          anchors.verticalCenter: parent.verticalCenter
          }

      }

}
