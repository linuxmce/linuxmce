import QtQuick 1.0
import "../../../../skins-common/lib/effects"

Rectangle{
    id:templateRow
    height: scaleY(16)
    width: scaleX(100)
    //    transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: 15 }
    clip:true
    color:"transparent"
    radius: 20
    visible: scenarioview.count !== 0
    property string bgimgSource
    property int floorplantype
    property variant scenariomodel
    property alias rowFillImage: imgFill.source
    property string rowFillColor: "maroon"


    Rectangle {
        id: templatefloorplan
        height: style.stdbuttonh
        width: style.stdbuttonw
        anchors.top: parent.top
        anchors.topMargin: scaleY(2)
        anchors.left: parent.left
        color:"transparent"

        StyledText{
            text: "F"
            font.bold: true
            fontSize: listViewText
            color:"white"
            anchors.centerIn: parent
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
        anchors.verticalCenter: parent.verticalCenter
        source:floorplantype===5 ? "NowPlayingButton.qml" :""
        anchors.left:templatefloorplan.right
    }

    Image{
        id:imgFill
        anchors.fill: scenarioview
        source:""
        Component.onCompleted: {if(source==="") rectFill.visible = false}
    }

    ListView{
        id: scenarioview
        width: parent.width-popupButton.width
        contentWidth:  (count+1) * scaleX(25)
        height: parent.height
        model: scenariomodel
        spacing: 5
        orientation:ListView.Horizontal
        anchors.left:popupButton.source !== "" ? popupButton.right: templatefloorplan.right
        clip:true
        delegate:
            Item{
                id:scenariodelegate
                width:sT.paintedWidth > scaleX(20) ? sT.paintedWidth+10 : scaleX(20)
                height:parent.height
                enabled:true
                clip:true

                Rectangle{
                    anchors.fill: parent
                    color:ms.pressed ? "white" : rowFillColor
                    opacity: ms.pressed ? 1 : .25
                    border.color: "white"
                    border.width: 2

                    Behavior on color{
                        PropertyAnimation{
                            duration: 100
                        }
                    }

                    Behavior on opacity {
                        PropertyAnimation{
                            duration:250
                        }
                    }
                }

                StyledText{
                    id:sT
                    text:title
                    fontSize: scaleY(4.5)
                    anchors.centerIn: parent
                    isBold: true
                    color:"white"
                }

                MouseArea{
                    id:ms
                    anchors.fill: parent
                    onClicked: manager.execGrp(params)
                }
            }
        interactive: true
    }
}


