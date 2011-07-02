import QtQuick 1.0


Rectangle {

    id: rectangle1
    width: 65
    height: 50

    MouseArea{
        id:basemouse
        anchors.fill:parent
        onClicked: rectangle1.state = "State1"
    }
    Text {
        id: nameLabel
        text: sDefaultUser
        font.bold:true

    }

    Image {
        id: userImage
        source: "../../../img/icons/personal.png"
        height: 50
        width: 50
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
            height:20
             width: 100

            Text {
                    id: somshit
                    text: username

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
                width: 300
                height: 125
                color: "#d9e9ee"
                radius: 24

                y:  -150
            }

            PropertyChanges {
                target: listview1
                x: 25
                y: 19
                width: 261
                height: 106
                clip: true
                smooth: true
                visible: true
                anchors.rightMargin: 11
                anchors.bottomMargin: 21
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
