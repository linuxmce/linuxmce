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
       KeyNavigation.right: gridView.right

    }

    PathView {
        id: path_view1
        x: 398
        y: 251
        width: 33
        height: 31
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
                 highlight: appHighlight

                 preferredHighlightBegin: 0.5
                 preferredHighlightEnd: 0.5
                 delegate: appDelegate
                 path: Path {
                     startX: 50
                     startY: 200
                     PathLine{x: 200 ; y:250}
                     PathAttribute { name: "iconScale"; value: 0.5 }


                     PathLine{ x: 400; y: 200; }
                     PathAttribute { name: "iconScale"; value: 2.0 }


                     PathLine { x: 750; y: 250; }
                     PathAttribute { name: "iconScale"; value: 0.5}


                 }
    }

    ListView {
    //Rectangle {
        id: list_view1
        x: 398
        y: 251
        width: 33
        height: 31
        //visible: false
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
            }

            PropertyChanges {
                target: path_view1
                x:0
                y: 40
                width: 790
                height: 460
                opacity: 1
                visible: true
            }

            PropertyChanges {
                target: list_view1
                x: 398
                y: 251
                width: 33
                height: 31
                visible: false
            }
        },
        State {
            name: "LISTVIEW"
            PropertyChanges {
                target: gridView
                visible: false
                opacity: 0
            }

            PropertyChanges {
                target: path_view1
                x: 398
                y: 251
                width: 33
                height: 31
                //interactive: false
                visible: false
                opacity: 1
            }

            PropertyChanges {
                target: list_view1
                x:0
                y: 40
                width: 790
                height: 460
                visible: true
                opacity: 1
            }
        }
    ]
}
