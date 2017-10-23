import QtQuick 2.2
import org.linuxmce.enums 1.0
import QtGraphicalEffects 1.0
import "../components"
import "../."

Item{
    signal activated()
    id:back_btn
    scale:ms.pressed ? 1.2 : 1.0
    clip:false
    Glow{
        anchors.fill: back_img
        radius:ms.pressed ? 16 :8
        samples:24
        spread:ms.pressed ? 0.1 :0.0
        color:"black"
        //color:appStyle.apptext_color_active
        source:back_img
    }

    Image {
        id: back_img
        source: "../images/arrow.png"
        anchors.centerIn: parent
        height: parent.height*.75
        width: height

    }
    MouseArea{
        id:ms
        anchors.fill: parent
        onClicked: activated()
    }
}


