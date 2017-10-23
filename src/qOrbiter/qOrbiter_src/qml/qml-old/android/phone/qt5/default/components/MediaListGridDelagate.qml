import QtQuick 2.2
import "../effects"
import "../components"
import "../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Item
{
    id: contactDelegate
    height: childrenRect.height
    width: childrenRect.width

    Rectangle
    {
        id:mainItem
        opacity: 0
        scale:0
        rotation: 0
        state:'unsorted'
        color:"transparent"
      //  onStateChanged: console.log("State changed in DG delegate to "+ state)
        MouseArea{
            anchors.fill: mainItem
            hoverEnabled: true
            onEntered: {
                mainItem.color = appstyle.darkhighlight
                mainItem.scale = 1.25
                mainItem.z = 10
            }
            onExited: {
                mainItem.color = "transparent"
                mainItem.scale = 1
                mainItem.z = 1
            }
        }

        ParallelAnimation {
            id:fade_and_scale
            running: false
            PropertyAnimation { target: mainItem; property: "opacity"; to: 1; duration: 1000}
            PropertyAnimation { target: mainItem; property: "scale"; to: 1; duration: 500}
         //   PropertyAnimation { target: mainItem; property: "rotation"; to: 0; duration: 500}

        }

        Component.onCompleted: {fade_and_scale.running = true}


        Rectangle
        {
            id:frame
            height: parent.height-5
            width: parent.width -5
            anchors.centerIn: mainItem
            clip:true
            color: "transparent"

            MediaListClickHandler {
            }

//            BorderImage {
//                id: borderimg
//                horizontalTileMode: BorderImage.Repeat
//                source: "../img/icons/drpshadow.png"
//                anchors.fill: imagerect
//                anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
//                border { left: 10; top: 10; right: 10; bottom: 10 }
//                smooth: true
//            }

            Image
            {
                id: imagerect;
                source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?type=img&val="+path : ""
                anchors.centerIn: parent;
                fillMode: Image.PreserveAspectFit
                smooth: true
                asynchronous: true
                anchors.fill: parent
                sourceSize.width:parent.width
                sourceSize.height:parent.height

            }
            Rectangle{
                id:textmask
                color: "grey"
                anchors.fill:celllabel
                opacity: .5
            }

            StyledText
            {
                id:celllabel
                text: name
                font.pointSize: 12;
                color: "white" ;
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: imagerect.width
                font.bold: true
                anchors.bottom: imagerect.bottom
                anchors.horizontalCenter: imagerect.horizontalCenter
            }
        }

        states: [
            State {
                name: "unsorted"
                when:manager.q_attributetype_sort !== ("52") && manager.q_subType !==("2"||"3") && manager.q_mediaType !== "4"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(21);
                    height: scaleY(21)
                    color: "transparent"
                }
            },
            State {
                name: "tv"
                when:manager.q_attributetype_sort === "12"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(21);
                    height: scaleY(21)
                    color: "transparent"
                }

            },
            State {
                name: "movies"
                when:manager.q_subType === "2"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(16);
                    height: scaleY(42)
                    color: "transparent"
                }
                PropertyChanges {
                    target: celllabel
                    visible:false
                }
                PropertyChanges {
                    target: textmask
                    visible:false
                }

            },
            State {
                name: "seasons"
                when:manager.q_attributetype_sort === "52"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(14);
                    height: scaleY(38)
                    color: "transparent"
                }
            },
            State {
                name: "audio"
                when: manager.q_mediaType === "4"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(17);
                    height: scaleY(28)
                    color: "transparent"
                }
            }
        ]
    }
}
