import QtQuick 1.1

Rectangle {
   anchors.fill: parent
   color:"black"

   Column{
      anchors.centerIn: parent
       width: childrenRect.width
       height: childrenRect.height

       spacing:10
       Text {
           text:"Status::"+system.statusMessage
           font.pixelSize: 20
           color: "yellow"
       }
       Text {
           text:"Api Level::"+system.apiLevel
           font.pixelSize: 20
           color: "yellow"
       }
       Text {
           text:"Device Name::"+system.deviceName
           font.pixelSize: 20
           color: "yellow"
       }
       Text {
           text:"Device Brand::"+system.deviceBrand
           font.pixelSize: 20
           color: "yellow"
       }
       Text {
           text:"Device Manufacturer::"+system.deviceManufacturer
           font.pixelSize: 20
           color: "yellow"
       }
   }

    Text {
        text: qsTr("Hello Android \n Tap to exit.")
        anchors.top: parent.top

    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
