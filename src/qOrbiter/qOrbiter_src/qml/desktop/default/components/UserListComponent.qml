import QtQuick 1.1


Rectangle {

    id: userComponent
    height: visible ?parent.height :0
    width:  visible ? childrenRect.width : 0
    clip: false
    color:"transparent"
    state: 'minimized'


    StyledText {
        id: nameLabel
        text: manager.sPK_User
        font.bold: true
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        MouseArea{
            anchors.fill: parent
            onClicked: {console.log("clicked");userComponent.state === "minimized" ? userComponent.state="expanded" : userComponent.state = "minimized"}
        }
    }

    Image {
        id: listviewBg
        source: manager.imagePath+"ui3/grey_popup.png"
        anchors.top: nameLabel.bottom
        ListView{
            id: user_listview
            model: userList
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            delegate:  Item{
                id:userlistComponentDelegate
            height:scaleY(4)
            width:parent.width         
            clip:true
            Image {
                id: bg
                source: manager.imagePath+"ui3/green_button.png"
                anchors.fill: parent
            }
            Text {
                id: usernamelabel
                text: username
                anchors.centerIn: parent
                font.bold: true
            }

            MouseArea{
                id:userlistmousearea
                anchors.fill: parent
                hoverEnabled: true

                onClicked:
                {
                    userComponent.state = "minimized"
                    setCurrentUser(index+1)
                }
            }
        }
        interactive: true
    }
}



states: [
State {
    name: "minimized"


    PropertyChanges{
        target: user_listview
        height:0
        width:0
        visible:false
    }
    PropertyChanges{
        target: listviewBg
        height:0
        width:0
        visible:false

    }
},
State {
    name: "expanded"

    PropertyChanges{
        target: user_listview
        height:scaleY(20)
        width:scaleX(15)
        visible:true

    }
    PropertyChanges{
        target: listviewBg
        height:scaleY(21)
        width:scaleX(15)
        visible:true

    }
}
]


//    transitions: Transition {
//        NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
//        NumberAnimation { easing.amplitude: 0.5; properties: "height,width"; easing.type: Easing.InOutQuart }
//        ColorAnimation { duration: 300 }
//        reversible: true
//    }

//    MouseArea{
//        id:basemouse
//        anchors.fill:parent
//        onClicked: rectangle1.state = "State1"
//    }


//    Image {
//        id: userImage
//        source: "../img/icons/personal.png"
//        height: 60
//        width: 60
//        anchors.centerIn: parent
//    }



//    ListView{
//        id: listview1
//        width: 0
//        height: 0
//        visible: false
//        model: userList
//        anchors.top: text1.bottom
//        delegate:  Rectangle
//        {
//        id: delrect
//        height:scaleY(5)
//        width: parent.width
//        color: style.not_color
//        clip:true

//        Text {
//            id: usernamelabel
//            text: username
//            anchors.centerIn: parent
//            font.bold: true
//        }

//        MouseArea{
//            id:userlistmousearea
//            anchors.fill: parent
//            hoverEnabled: true
//            onEntered: delrect.color = "grey"
//            onExited: delrect.color = style.not_color
//            onClicked:
//            {
//                rectangle1.state = "Default"
//                setCurrentUser(index+1)

//            }
//        }
//    }
//    interactive: true


//}

//Text {
//    id: text1
//    x: 76
//    y: 4
//    text: currentuser
//    visible: false
//    font.pixelSize: 12
//    opacity: 0
//}
//states: [
//            State {
//                name: "State1"

//                PropertyChanges {
//                    target: rectangle1
//                    width: 400
//                    height: 200
//                    color: "#d9e9ee"
//                    radius: 24

//                    y:  -150
//                }

//                PropertyChanges {
//                    target: listview1

//                    width: 400
//                    height: 2000
//                    clip: true
//                    smooth: true
//                    visible: true


//                }

//                PropertyChanges {
//                    target: userImage
//                    visible: false
//                }

//                PropertyChanges {
//                    target: basemouse
//                    visible: false
//                }

//                PropertyChanges {
//                    target: text1
//                    x: 92
//                    y: 0
//                    text: "Select User"
//                    smooth: true
//                    visible: true
//                    font.family: "Nimbus Sans L"
//                    font.pointSize: 11
//                    verticalAlignment: "AlignTop"
//                    horizontalAlignment: "AlignHCenter"
//                    opacity: 1
//                }

//                PropertyChanges {
//                    target: nameLabel
//                    visible: false
//                }
//            }
//        ]
}
