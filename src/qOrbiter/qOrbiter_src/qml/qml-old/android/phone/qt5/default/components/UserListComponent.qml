import QtQuick 2.2


Rectangle {

    id: rectangle1
    height: 45
    width: 45
    clip: true

    transitions: Transition {
        NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
        NumberAnimation { easing.amplitude: 0.5; properties: "height,width"; easing.type: Easing.InOutQuart }
        ColorAnimation { duration: 300 }
        reversible: true
    }

    MouseArea{
        id:basemouse
        anchors.fill:parent
        onClicked: rectangle1.state = "State1"
    }
    Text {
        id: nameLabel
        text: currentuser
        font.bold: true


    }

    Image {
        id: userImage
        source: "../images/personal.png"
        height: 35
        width: 35
        anchors.centerIn: parent
    }



    ListView{
        id: listview1
        width: 0
        height: 0
        visible: false
        model: userList
        anchors.top: text1.bottom
        delegate:  Rectangle
        {
        id: delrect
        height:scaleY(5)
        width: parent.width
        color: style.not_color
        clip:true

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
            onEntered: delrect.color = "grey"
            onExited: delrect.color = style.not_color
            onClicked:
            {
                rectangle1.state = "Default"
                setCurrentUser(index+1)

            }
        }
    }
    interactive: true


}

Text {
    id: text1
    x: 76
    y: 4
    text: currentuser
    visible: false
    font.pixelSize: 12
    opacity: 0
}
states: [
            State {
                name: "State1"

                PropertyChanges {
                    target: rectangle1
                    width: 200
                    height: 175
                    color: "#d9e9ee"
                    radius: 24
                    y:  -75
                }

                PropertyChanges {
                    target: listview1

                    width: 200
                    height: 175
                    clip: true
                    smooth: true
                    visible: true


                }

                PropertyChanges {
                    target: userImage
                    visible: false
                }

                PropertyChanges {
                    target: basemouse
                    visible: false
                }

                PropertyChanges {
                    target: text1
                    x: 92
                    y: 0
                    text: "Select User"
                    smooth: true
                    visible: true
                    font.family: "Nimbus Sans L"
                    font.pointSize: 11
                    verticalAlignment: "AlignTop"
                    horizontalAlignment: "AlignHCenter"
                    opacity: 1
                }

                PropertyChanges {
                    target: nameLabel
                    visible: false
                }
            }
        ]
}
