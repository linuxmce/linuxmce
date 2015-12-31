import QtQuick 2.1
import "../../../skins-common/qt5-lib/handlers"

Item{
    id: contactDelegate
    height: childrenRect.height
    width: childrenRect.width

    Item{
        id:mainItem
        opacity: 0
        scale:0
        rotation: 0
        state:'unsorted'
        //  onStateChanged: console.log("State changed in DG delegate to "+ state)
        MouseArea{
            anchors.fill: mainItem
            hoverEnabled: true
            onEntered: {

                mainItem.scale = 1.25
                mainItem.z = 10
            }
            onExited: {

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



        Rectangle{
            id:frame
            height: parent.height-5
            width: parent.width -5
            anchors.centerIn: mainItem
            clip:true
            color: appStyle.primaryDarkColor
        }

        Image{
            id: imagerect;
            source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
            fillMode: Image.PreserveAspectFit
            smooth: true
            asynchronous: true
            anchors{
                top:parent.top
                left:parent.left
                right: parent.right
                bottom:textmask.top
            }


        }
        GradientFiller{
            fillColor:"black"
            opacity: .65
        }
        Rectangle{
            id:textmask
            color: "grey"
            height: parent.height*.15
            anchors{
                left:frame.left
                right:frame.right
                bottom:parent.bottom
            }

            opacity: .5
        }

        StyledText{
            id:celllabel
            text: name
            font.pointSize: 12;
            color: "white" ;
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            width: imagerect.width
            elide:Text.ElideRight
            font.bold: true
            anchors{
                top:textmask.top
                left:parent.left
                right:parent.right
                bottom: textmask.bottom
            }
        }

        MediaListClickHandler {
            onClicked: {
                if(indexStack.count !==0 && indexStack.get(indexStack.count).idx !==mediaList.currentIndex){
                    indexStack.append({"idx":mediaList.currentIndex})
                }
                console.log("IndexStack length ==>" + indexStack.count)
                depth++

            }
        }


        states: [
            State {
                name: "unsorted"
                when:manager.q_attributetype_sort !== ("52") && manager.q_subType !==("2"||"3") && manager.q_mediaType !== "4"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(33);
                    height: scaleY(33)

                }
            },
            State {
                name: "tv"
                when:manager.q_attributetype_sort === "12"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(21);
                    height: scaleY(21)

                }

            },
            State {
                name: "movies"
                when:manager.q_subType === "2"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(16);
                    height: scaleY(42)

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

                }
            },
            State {
                name: "audio"
                when: manager.q_mediaType === "4"
                PropertyChanges {
                    target: mainItem
                    width: scaleX(19);
                    height: scaleY(30)

                }
            }
        ]
    }
}
