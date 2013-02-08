import QtQuick 1.0
import com.nokia.android 1.1
import "../../lib/effects"

Rectangle{
    id:templateRow
    height: scaleY(16)
    width: scaleX(100)
    //    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
    clip:true
    color:"transparent"
    radius: 20

    property string bgimgSource
    property int floorplantype
    property variant scenariomodel
    property alias rowFillImage: imgFill.source
    property alias rowFillColor: rectFill


    Rectangle {
        id: templatefloorplan
        height: style.stdbuttonh
        width: style.stdbuttonw
        anchors.top: parent.top
        anchors.topMargin: scaleY(2)
        anchors.left: parent.left
        color:"transparent"
        Image {
            id: onimg
            source: bgimgSource //"../img/ui3/lightingbig.png"
            height: parent.height
            width: parent.width
        }
        MouseArea{
            id: mousearea1
            anchors.fill: parent
            onClicked: {
                manager.showfloorplan(floorplantype)
                manager.setFloorplanType(floorplantype)
            }
        }
    }
    Loader{
        id:popupButton
        Component.onCompleted: {
            if(floorplantype===3)
                source = "NowPlayingButton.qml"
            anchors.left =templatefloorplan.right

        }
    }
    DropShadow{
        sourceItem: rectFill
        anchors.fill: rectFill
        color:"black"
    }

    Image{
        id:imgFill
        anchors.fill: scenarioview
        source:""
        Component.onCompleted: {if(source==="") rectFill.visible = false}
    }

    Rectangle{
        id:rectFill
        anchors.fill: scenarioview
        color: "skyblue"
        opacity:.25
    }

    ListView{
        id: scenarioview
        width: parent.width
        height: parent.height
        model: scenariomodel
        spacing: 5
        orientation:ListView.Horizontal
        anchors.left:popupButton.source !== "" ? popupButton.right: templatefloorplan.right
        clip:true
        delegate: Rectangle{
            height:parent.height
            width:scaleX(20)
            color: "transparent"
            ListItem{
                id:scenariodelegate
                width:scaleX(20)
                height:parent.height
                enabled:true
                clip:true
                DropShadow{
                    sourceItem: sT
                    anchors.fill: sT
                    color: "darkgrey"
                    opacity:.5
                    blur:.75
                    transform: Translate{x:2}

                }
                StyledText{
                    id:sT
                    text:title
                    fontSize: scaleY(4.5)
                    anchors.centerIn: parent
                    isBold: true
                }
                onClicked: manager.execGrp(params)
            }
        }
        interactive: true
    }

}


