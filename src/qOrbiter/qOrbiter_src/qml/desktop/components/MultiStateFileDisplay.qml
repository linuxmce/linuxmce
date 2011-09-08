import QtQuick 1.0


Rectangle {
    id:gridholder
    width: scaleX(90)
    height: scaleY(90)
    clip: true

    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    color: style.lighthighlight
    Image {
        id: name
        source: ""
    }

    GridView {
        id: gridView

        //z: 2
        width: scaleX(85)
        height: scaleY(75)
       anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
       //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)
        keyNavigationWraps: true
        Keys.enabled: true
        Keys.onPressed: console.log(Qt.Key_Right)
        KeyNavigation.right: gridView.moveCurrentIndexRight

    }

    PathView {
        id: path_view1

        //z: 1
        width: scaleX(90)
        height: scaleY(75)
        visible: false
        opacity: 0
        anchors.topMargin: scaleX(15)
        anchors.horizontalCenter: fileviewscreen.horizontalCenter
        model:dataModel
        focus: true
        clip: true
        interactive: true
        Keys.enabled: true
        anchors.fill: parent
        offset: 5
        pathItemCount: 13;
        highlight: appHighlight
        preferredHighlightBegin: 0.35
        preferredHighlightEnd: 0.65
        highlightRangeMode: PathView.StrictlyEnforceRange
        highlightMoveDuration: 100
        dragMargin: parent.height/3
        delegate: appDelegate
        path: Path {
            startX: 10
            startY: path_view1.height-100

            PathPercent { value: 0.0 }
            PathAttribute { name: "iconScaling"; value:  0.6 }
            PathAttribute { name: "iconAngle"; value:  70 }

            PathLine { x: (path_view1.width*0.25); y: path_view1.height-100 }
            PathPercent { value: 0.45 }
            PathAttribute { name: "iconScaling"; value:  0.75 }
            PathAttribute { name: "iconAngle"; value:  70 }

            PathLine { x: path_view1.width/2; y: path_view1.height-100 }
            PathPercent { value: 0.5 }
            PathAttribute { name: "iconScaling"; value:  1.0 }
            PathAttribute { name: "iconAngle"; value:  0 }

            PathLine { x: (path_view1.width*0.75); y: path_view1.height-100 }
            PathPercent { value: 0.55 }
            PathAttribute { name: "iconScaling"; value:  0.75 }
            PathAttribute { name: "iconAngle"; value:  -70 }

            PathLine { x: path_view1.width; y: path_view1.height-100 }
            PathPercent { value: 1.0 }
            PathAttribute { name: "iconScaling"; value:  0.6 }
            PathAttribute { name: "iconAngle"; value:  -70 }

        }
        onCurrentIndexChanged: {
            //console.log("Current index changed: " + currentIndex);
        }
    }

    Component {
        id: appDelegate
        Item {
            id: wrapper
            width: scaleX(75); height: scaleY(75)
            scale:1
            // opacity: path_view1.iconAngle.iconOpacity

            Image {
                id: myIcon
                y: 20; anchors.horizontalCenter: parent.horizontalCenter
                smooth: true
                source:"image://datagridimg/"+id ;
                height: scaleX(25);
                width: scaleY(25);
                fillMode: Image.PreserveAspectFit;
                asynchronous: true
                transform: Rotation {
                    origin.x: wrapper.width/2; origin.y: wrapper.height/2
                    axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
                    //   angle: path_view1.iconAngle
                }

            }

            Text {
                anchors { top: myIcon.bottom; horizontalCenter: parent.horizontalCenter }
                text: name
                smooth: true
                font.pointSize: 12
                //visible: wrapper.PathView.isCurrentItem
            }

            MouseArea {
                anchors.fill: parent
                onClicked: setStringParam(4, id)
            }
        }
    }

    ListView {
        id: list_view1
        x:0
        y: 40
        //z: 1
        width: scaleX(90)
        height: scaleY(75)
        visible: false
        opacity: 0
        model:dataModel
        delegate: contactDelegateList
        highlightFollowsCurrentItem: true
        highlight: appHighlight
        focus: true
        clip: true

    }

    Row{
        id:gridselectorrow
        height: scaleY(5)
        width: scaleX(50)
        ButtonSq {
            id: buttonsq1
            width: scaleX(5)
            height: scaleY(4)
            buttontextitalic: true
            buttontext: "Grid"
            buttontextbold: true
            MouseArea{
                anchors.fill: parent
                onClicked:{
                    gridholder.state =""
                    console.log(gridholder.state)
                    console.log("default clicked")
                }
            }
        }

        ButtonSq {
            id: buttonsq2
            width: scaleX(5)
            height: scaleY(4)
            buttontextitalic: true
            buttontextbold: true
            buttontext: "Cover"
            MouseArea{
                anchors.fill: parent
                anchors.bottomMargin: 0
                onClicked: gridholder.state ="PATHVIEW"
            }
        }

        ButtonSq {
            id: buttonsq3
            width: scaleX(5)
            height: scaleY(4)
            buttontextbold: true
            buttontextitalic: true
            buttontext: "List"
            MouseArea{
                anchors.fill: parent
                onClicked: gridholder.state = "LISTVIEW"
            }
        }
    }
    states: [
        State {
            name: "PATHVIEW"

            PropertyChanges {
                target: gridView
                visible: false
                opacity: 0
                //z: 2
            }

            PropertyChanges {
                target: path_view1
                //x:0
                //y: 40
                //width: 790
                //height: 460
                opacity: 1
                visible: true
                //z: 2
            }

            PropertyChanges {
                target: list_view1
                //x: 398
                //y: 251
                //width: 33
                //height: 31
                visible: false
                opacity: 0
                //z:1
            }
        },
        State {
            name: "LISTVIEW"
            PropertyChanges {
                target: gridView
                visible: false
                opacity: 0
                //z: 1
            }

            PropertyChanges {
                target: path_view1
                //x: 398
                //y: 251
                //width: 33
                //height: 31
                //interactive: false
                visible: false
                opacity: 0
                //z: 1
            }

            PropertyChanges {
                target: list_view1
                //x:0
                //y: 40
                //width: 790
                //height: 460
                visible: true
                opacity: 1
                //z: 2
            }
        }
    ]
}
