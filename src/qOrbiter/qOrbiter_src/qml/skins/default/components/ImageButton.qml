import QtQuick 2.2
import "../components"
import "../."
Item{
    id:img_button
    height: manager.isProfile ? appStyle.scaleY(8) : appStyle.scaleY(10)
    width: height
    signal activated()
    property string upImage:""
    property string downImage:""
    property double imgRotation:0
    opacity: mickey.pressed ? 1 : .65
    Behavior on opacity{
        PropertyAnimation{
            duration: 250
        }
    }

    Image {
        id: btnImg
        fillMode: Image.PreserveAspectFit
        source: mickey.pressed ? downImage : upImage
        height: parent.height *.75
        width: height
        anchors.centerIn: parent
        rotation: imgRotation
    }
    MouseArea{
        id:mickey
        anchors.fill: parent
        onPressed: activated()
    }
}
