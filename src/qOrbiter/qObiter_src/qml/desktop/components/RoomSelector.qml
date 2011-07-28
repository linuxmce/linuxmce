import QtQuick 1.0

Rectangle {


    id: rooms
    width: style.stdbuttonw
    height: style.stdbuttonh
    transitions: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
             NumberAnimation { easing.amplitude: 0.5; properties: "height,width"; easing.type: Easing.InOutQuart }
            ColorAnimation { duration: 300 }
        }


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
        width: 60
        height: 60
        buttontext:currentroom
            }

           ListView {
                id: roomListModel
                visible: false
                model: roomList
                highlightFollowsCurrentItem:true
                height: 5
                width: 0

                delegate:
                    Rectangle
                 {
                     id: delrect
                     height:50
                     width: parent.width

                     Text {
                             id: somshit
                             text: Title + ":" + intRoom + ", In EA: " + entertain_area
                             font.pointSize: 12
                            }

                     MouseArea{
                         id:listMouseArea
                         anchors.fill: parent
                         onClicked:
                         {

                            setActiveRoom(intRoom, entertain_area)
                            rooms.state = "Default"

                             roombutton.buttontext = Title
                         }
                        }
                    }
                }


            MouseArea {
                id: mouse_area1
                anchors.fill: parent
                onClicked: rooms.state = "State1"
                z:5
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
                        width: 300
                        height: 200                       
                        color: "#d1d0be"
                        y: -200


                    }

                    PropertyChanges {
                        target: roomListModel
                        x: 0
                        y: 19
                        height: 181
                        width:300
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
                        x: 0
                        y: 0
                        width: 250
                        height: 200
                        anchors.topMargin: 0
                        visible: false
                    }

                    PropertyChanges {
                        target: roomListModel
                        width: 250
                        height: childrenRect.height
                        clip:true

                    }
                }
            ]
}
