import QtQuick 1.0
import "../components"
/*
(17598): "116" :: 159
 "File Server (192.168.80.165) 192.168.80.165(192.168.80.165)" :: 163
"3856" :: 224
 "3" :: 251
 "0" :: 252
"1" :: 253
  */
StyledScreen {
    screen:"Plug n Play Hard Drive Screen"

        HeaderListView{
            height: parent.height
            width: parent.width*.85
            anchors.horizontalCenter: parent.horizontalCenter
            listTitle:"Device Detected: \n"+ screenparams.getParam(163)+"\n Select Type:"
        }

}
