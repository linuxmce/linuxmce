import QtQuick 1.0


Rectangle {

    id: rectangle1
    width: style.stdbuttonw
    height: style.stdbuttonh

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
        z:5

    }

    Image {
        id: userImage
        source: "../../../img/icons/personal.png"
        height: 60
        width: 60
        anchors.centerIn: parent
    }



    ListView{
        id: listview1
        width: 0
        height: 0
        visible: false
        model: userList


        delegate:  Rectangle
        {
            id: delrect
            height:50
             width: 100


            Text {
                    id: somshit
                    text: username
                    anchors.centerIn: parent
                    font.bold: true
                    z:5

                   }
            MouseArea{
                id:userlistmousearea
                anchors.fill: parent
                onClicked:
                {
                   rectangle1.state = "Default"
                    console.log(index)
                    nameLabel.text = username
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
                width: 400
                height: 200
                color: "#d9e9ee"
                radius: 24

                y:  -150
            }

            PropertyChanges {
                target: listview1

                width: 400
                height: 2000
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
