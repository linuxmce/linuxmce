import QtQuick 1.0


Rectangle {
    id:gridholder
    width: 790
    height: 500
    color: "transparent"
    Image {
        id: name
        source: ""
    }


    GridView {
        id: gridView
        anchors.fill: parent
        x:0
        y: 40
        //z: 2
        width: 790
        height: 460
        anchors.topMargin: 40
        anchors.horizontalCenter: fileviewscreen.horizontalCenter
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
        clip: true
        contentItem.clip: true
        cellWidth: 175
        cellHeight: 109
        keyNavigationWraps: true
        Keys.enabled: true
        Keys.onPressed: console.log(Qt.Key_Right)
       KeyNavigation.right: gridView.moveCurrentIndexRight

    }

    PathView {
        id: path_view1
        x:0
        y: 40
        //z: 1
        width: 790
        height: 460
        visible: false
        opacity: 0
        anchors.topMargin: 40
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

    ListView {
        id: list_view1
        x:0
        y: 40
        //z: 1
        width: 790
        height: 460
        visible: false
        opacity: 0
        model:dataModel
        delegate: contactDelegateList
        highlightFollowsCurrentItem: true
        highlight: appHighlight
        focus: true
        clip: true

    }

    Text {
        id: text1
        x: 15
        y: 10
        text: "Media List"
        font.family: "Droid Sans"
        font.pixelSize: 25
    }

    ButtonSq {
        id: buttonsq1
        x: 389
        y: 1
        width: 41
        height: 37
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
        x: 442
        y: 1
        width: 41
        height: 37
        buttontextitalic: true
        buttontextbold: true
        buttontext: "Cover"
        MouseArea{
            width: 41
            height: 38
            anchors.bottomMargin: 0
            anchors.fill: parent
            onClicked: gridholder.state ="PATHVIEW"
        }
    }

    ButtonSq {
        id: buttonsq3
        x: 495
        y: 1
        width: 41
        height: 37
        buttontextbold: true
        buttontextitalic: true
        buttontext: "List"
        MouseArea{
            width: 42
            height: 41
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.fill: parent
            onClicked: gridholder.state = "LISTVIEW"

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
