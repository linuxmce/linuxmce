import QtQuick 2.2
import "../../../skins-common/lib/handlers"
Item{
    id: contactDelegate
    height: childrenRect.height
    width: childrenRect.width

    Item{
        id:mainItem
        state:'unsorted'
        //  onStateChanged: console.log("State changed in DG delegate to "+ state)

        Rectangle{
            id:frame
            height: parent.height-5
            width: parent.width -5
            anchors.centerIn: mainItem
            clip:true
            color: "transparent"
            
            MouseArea{
                anchors.fill: parent

                onClicked: {
                    manager.setStringParam(4, id)
                    if(indexStack.count !==0 && indexStack.get(indexStack.count).idx !==mediaList.currentIndex){
                        indexStack.append({"idx":mediaList.currentIndex})
                    }
                    console.log("IndexStack length ==>" + indexStack.count)
                    depth++

                }
            }

            
            Image{
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
                    width: scaleX(34);
                    height: scaleY(75)

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
                    width: scaleX(17);
                    height: scaleY(28)

                }
            }
        ]
    }
}
