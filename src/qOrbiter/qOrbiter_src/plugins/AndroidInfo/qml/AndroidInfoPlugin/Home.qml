import QtQuick 1.1

Item {
   anchors.fill: parent
   Text {
       text: qsTr("Hello Android \n Tap to exit.")
       anchors.centerIn:parent
       font.bold: true
       color: "white"
       font.pixelSize: 28
       font.weight: Font.Light
       MouseArea {
           anchors.fill: parent
           onClicked: {
               Qt.quit();
           }
       }
   }
}
