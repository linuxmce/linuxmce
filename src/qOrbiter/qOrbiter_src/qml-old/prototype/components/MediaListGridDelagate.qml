import QtQuick 1.0

import "../components"
import "../../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Item
{
    id: contactDelegate
    height: childrenRect.height
    width: childrenRect.width
    visible: name !=="back (..)"

    Rectangle
    {
        id:mainItem
        opacity: 0
        scale:0
        rotation: 0
        state:'unsorted'
        color:"transparent"
      //  onStateChanged: console.log("State changed in DG delegate to "+ state)


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


            Image
            {
                id: imagerect;
                source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
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
                color: "black"
                anchors.fill:celllabel
                opacity: .8
            }

            StyledText
            {
                id:celllabel
                text: name
               fontSize: paraText
               font.weight: Font.Light
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
                    width: scaleX(35);
                    height: scaleY(35)
                    color: "transparent"
                }
            },
            State {
                name: "tv"
                when:manager.q_attributetype_sort === "12"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(33);
                    height: scaleY(20)
                    color: "transparent"
                }

            },
            State {
                name: "movies"
                when:manager.q_subType === "2"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(20);
                    height: scaleY(45)
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
                    width: scaleX(20);
                    height: scaleY(45)
                    color: "transparent"
                }
            },
            State {
                name: "audio"
                when: manager.q_mediaType === "4"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(25);
                    height: scaleY(45)
                    color: "transparent"
                }
            }
        ]
    }
}
