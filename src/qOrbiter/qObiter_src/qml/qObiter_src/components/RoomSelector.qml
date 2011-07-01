import QtQuick 1.0

Rectangle {
    id: rooms
    width: 50
    height: 50

    Component {
        id: highlight1
        Rectangle {
            width: delrect.width; height: delrect.height
            color: "lightsteelblue"
            y: list.currentItem.y
            Behavior on y {
                SpringAnimation {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }

    Text {
        id: text1
        x: 43
        y: 5
        text: "text"
        visible: false
        font.pixelSize: 12
        opacity: 0
    }

    ButtonSq {
        id:roombutton
        width: 50
        height: 50
        buttontext:"currentroom"
            }

           ListView {
                id: roomListModel
                visible: false
                model: roomlist
                highlightFollowsCurrentItem:true

                delegate:
                    Rectangle
                 {
                     id: delrect
                     height:20
                      width: 100

                     Text {
                             id: somshit
                             text: Title
                            }
                     MouseArea{
                         id:listMouseArea
                         anchors.fill: parent
                         onClicked:
                         {
                            rooms.state = "Default"
                             console.log(index)
                             roombutton.buttontext = Title
                         }
                        }
                    }
                }


            MouseArea {
                id: mouse_area1
                anchors.fill: parent
                onClicked: rooms.state = "State1"
            }

            states: [
                State {
                    name: "State1"

                    PropertyChanges {
                        target: roombutton
                        visible: false
                    }

                    PropertyChanges {
                        target: rooms
                        width: 100
                        height: 200                       
                        color: "#eae8c0"
                        y: -200
                    }

                    PropertyChanges {
                        target: roomListModel
                        x: 0
                        y: 19
                        width: 100
                        height: 181
                        visible: true

                    }

                    PropertyChanges {
                        target: text1
                        x: 0
                        y: 5
                        text: "Select Location"
                        visible: true
                        z: 16
                        font.pixelSize: 11
                        font.bold: true
                        opacity: 1
                    }

                    PropertyChanges {
                        target: mouse_area1
                        visible: false
                    }

                    PropertyChanges {
                        target: roomListModel
                        width: 100
                        height: childrenRect.height
                        clip:true

                    }
                }
            ]
}
