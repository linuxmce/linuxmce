import QtQuick 2.0
import "../components"

//roomsList
// text: title + ":" + intRoom + ", In EA: " + entertain_area
// setActiveRoom(intRoom, entertain_area)
/*
                            setActiveRoom(intRoom, entertain_area)
                            rooms.state = "Default"
                            currentroom = title
                             roombutton.buttontext = title
  */

Rectangle {
    id:roomselectorrect
    height:110
    width: 600
    color: "transparent"
    anchors.centerIn: parent

    Component
    {
        id:roomselectioncomponent


        //delegate
        Rectangle{
            id:roomdelegate
            height:90
            width: 160
            color: style.not_color
            Image {
                id: roomimage
                source: room_image
                height: 90
                fillMode: Image.PreserveAspectFit
                width:160
            }
            Rectangle{
                id:delegateheader
                height: 20
                width: roomdelegate.width
                color: "transparent"
                anchors.top: parent.top
                StyledText {
                    id: celllabel
                    height: 25
                    width: parent.width
                    text: title + ": In EA: " + entertain_area
                    wrapMode: "WrapAnywhere"
                    font.bold: true
                }
            }



            MouseArea{
                anchors.fill: parent
                onClicked: {
                    currentroom = title
                   setActiveRoom(intRoom, entertain_area)
                    roombutton.buttontext = title
                    roomselectorrect.destroy()
                }
            }
        }
    }


    Rectangle{
        id:roomselectorheader
        height: 15
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#99e1f1"
            }

            GradientStop {
                position: 1
                color: "#bbc5db"
            }
        }
        width: roomselectorrect.width
        StyledText {
            id: headertext
            text: "Select Room Please"
        }

    }
    ListView{
        id:roomsource
        anchors.top: roomselectorheader.bottom
        height: 90
        width: 600
        orientation: ListView.Horizontal

        spacing: 5
        clip: true
        model: roomList
        delegate: roomselectioncomponent
        flickableDirection: "HorizontalFlick"


    }

}
