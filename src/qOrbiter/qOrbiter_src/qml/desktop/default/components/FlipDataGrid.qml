// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle
{
    id:mainItem
    width: scaleX(20);
    height: scaleY(20)
    color: "transparent"
    opacity: 0
    scale:0
    rotation: 360

    MouseArea{
        anchors.fill: mainItem
        hoverEnabled: true
        onEntered: {
            mainItem.color = style.darkhighlight
            mainItem.scale = 1.25
            mainItem.z = 10

        }
        onExited: {
            mainItem.color = "transparent"
            mainItem.scale = 1
            mainItem.z = 1
        }
    }
    ParallelAnimation {
        id:fade_and_scale
        running: false
        PropertyAnimation { target: mainItem; property: "opacity"; to: 1; duration: 1000}
        PropertyAnimation { target: mainItem; property: "scale"; to: 1; duration: 500}
        PropertyAnimation { target: mainItem; property: "rotation"; to: 0; duration: 500}

    }

    Component.onCompleted: fade_and_scale.running = true
    Rectangle
    {
        id:frame

        width: scaleX(19);
        height: scaleY(19)
        anchors.centerIn: mainItem
        clip:true
        color: "transparent"

        MouseArea
        {
            anchors.fill: frame
            onClicked: {setStringParam(4, id); mouselocX = mouseX; mouselocY = mouseY}

        }

        BorderImage {
            id: borderimg
            horizontalTileMode: BorderImage.Repeat
            source: "../img/icons/drpshadow.png"
            anchors.fill: imagerect
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            smooth: true
        }

        Image
        {
            id: imagerect;
            source:"image://datagridimg/"+id ;
            height: scaleY(18);
            width: scaleX(18);
            anchors.centerIn: parent;
            fillMode: Image.PreserveAspectCrop
            smooth: true
            asynchronous: true
        }
        Rectangle{
            id:textmask
            color: "grey"
            anchors.fill:celllabel
            opacity: .5
        }

        Text
        {
            id:celllabel
            text: name;
            font.pointSize: 12;
            color: "white" ;
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            width: imagerect.width
            font.bold: true
            anchors.top: imagerect.top
            anchors.horizontalCenter: imagerect.horizontalCenter
        }
    }
}
