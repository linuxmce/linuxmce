import QtQuick 1.1
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

Item
{
    id:roomselectioncomponent
    //delegate
    height: hideFromOrbiter ? 0 : 90
    width:hideFromOrbiter ? 0 : 160
    Rectangle{
        id:roomdelegate
        height:90
        width: 160
        visible:!hideFromOrbiter
        color: skinStyle.not_color
//        Image {
//            id: roomimage
//            source: room_image
//            height: 90
//            fillMode: Image.PreserveAspectFit
//            width:160
//        }
        Rectangle{
            id:delegateheader
            height: 20
            width: roomdelegate.width
            color: "transparent"
            anchors.top: parent.top
            Text {
                id: celllabel
                height: 25
                width: parent.width
                text: name
                wrapMode: "WrapAnywhere"
                font.bold: true
            }
        }

        ListView{
            id:entertainmentAreaList
            model:ea_list
            width: roomdelegate.width
            height: roomdelegate.height
            anchors.top: parent.bottom
            visible:false
            spacing: 5
            delegate: Rectangle{
                height: 50
                width: 100
                Text{text:ea_list[index].ea_name}
                MouseArea{
                    anchors.fill: parent
                    onClicked: {manager.setActiveRoom(intRoom, ea_list[index].ea_number); componentLoader.source="" }
                }
            }
            onVisibleChanged: console.log("Showing eas"+ea_list)
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                entertainmentAreaList.visible=true
               // currentroom = name

              //  roombutton.buttontext = name
                //roomselectorrect.destroy()
            }
        }
    }
}
