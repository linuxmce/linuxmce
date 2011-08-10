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
    }

    PathView {
        id: path_view1
        x: 218
        y: 100
        width: 15
        height: 15
        visible: false
        opacity: 0
    }

    ListView {
        id: list_view1
        x: 436
        y: 385
        width: 15
        height: 15
        visible: false
        opacity: 0
    }

    Text {
        id: text1
        x: 15
        y: 10
        text: "text"
        font.family: "Droid Sans"
        font.pixelSize: 25
    }

    ButtonSq {
        id: buttonsq1
        x: 382
        y: 2
        width: 42
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
            anchors.fill: parent
            onClicked: state ="PATHVIEW"
        }
    }

    ButtonSq {
        id: buttonsq3
        x: 495
        y: 1
        width: 41
        height: 40
        buttontextbold: true
        buttontextitalic: true
        buttontext: "List"
        MouseArea{
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
            }

            PropertyChanges {
                target: path_view1
                x: 0
                y: 40
                width: 790
                height: 460
                opacity: 1
            }

            PropertyChanges {
                target: list_view1
                x: 436
                y: 385
                width: 15
                height: 15
                visible: false
            }
        },
        State {
            name: "LISTVIEW"
            PropertyChanges {
                target: gridView
                visible: false
            }

            PropertyChanges {
                target: path_view1
                x: 0
                y: 0
                width: 15
                height: 15
                interactive: false
                visible: false
                opacity: 1
            }

            PropertyChanges {
                target: list_view1
                x: 0
                y: 40
                width: 790
                height: 460
                visible: true
                opacity: 1
            }
        }
    ]
}
