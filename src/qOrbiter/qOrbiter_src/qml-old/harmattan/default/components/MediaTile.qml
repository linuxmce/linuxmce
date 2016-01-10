import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

Image {
    width: 160;
    height: 160;
    source: "image://datagridimg/"+id;
    fillMode: Image.PreserveAspectCrop;
    asynchronous: true;
    MouseArea
    {
        anchors.fill: parent;
        onClicked: setStringParam(4,id);
    }
}

