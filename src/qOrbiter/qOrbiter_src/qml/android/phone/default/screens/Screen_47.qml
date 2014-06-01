import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/handlers"
StyledScreen{
    id:file_view_screen

    Item {
        id:fileviewscreen
        anchors.fill: parent

        focus:true
        state:"browsing"
        property string currentSeekLetter:"A"
        Component.onCompleted: {
            hdr.state="grid"
            forceActiveFocus()
        }


        ListModel{
            id:alphabetlist

            ListElement{
                name:"0"
            }

            ListElement{
                name:"A"
            }

            ListElement{
                name:"B"
            }

            ListElement{
                name:"C"
            }

            ListElement{
                name:"D"
            }

            ListElement{
                name:"E"
            }

            ListElement{
                name:"F"
            }

            ListElement{
                name:"G"
            }

            ListElement{
                name:"H"
            }

            ListElement{
                name:"I"
            }

            ListElement{
                name:"J"
            }

            ListElement{
                name:"K"
            }

            ListElement{
                name:"L"
            }

            ListElement{
                name:"M"
            }
            ListElement{
                name:"N"
            }

            ListElement{
                name:"O"
            }

            ListElement{
                name:"P"
            }

            ListElement{
                name:"Q"
            }

            ListElement{
                name:"R"
            }

            ListElement{
                name:"S"
            }

            ListElement{
                name:"T"
            }

            ListElement{
                name:"U"
            }

            ListElement{
                name:"V"
            }

            ListElement{
                name:"W"
            }

            ListElement{
                name:"X"
            }

            ListElement{
                name:"Y"
            }

            ListElement{
                name:"Z"
            }
        }

        Keys.onReleased: {

            switch(event.key){
            case Qt.Key_Back:
                manager.goBackGrid();
                 event.accepted=true
                break;
            case Qt.Key_MediaPrevious:
                if (!manager.goBackGrid())
                    event.accepted=false
                else
                     event.accepted=true
         break;
            default:
                console.log(event.key)
                break
            }
        }



        Connections{
            target: filedetailsclass
            onShowDetailsChanged:
            {
                loadComponent("FileDetails.qml")

            }
        }

        Component{
            id: contactDelegateList
            Item {
                width: scaleX(95)
                height: scaleY(15)
                clip:true

                Image  {
                    id: imagerect;
                    fillMode: Image.PreserveAspectCrop
                    source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+path : ""
                    anchors.fill: parent
                }
                Rectangle {
                    id: background
                    color: mouseclick.pressed ? "white":"black"
                    anchors.fill: parent
                    opacity: mouseclick.pressed ? 1 : .65
                    radius: 5
                    border.color: mouseclick.pressed ? "green":"white"
                    Behavior on color {
                        PropertyAnimation{
                            duration:250
                        }
                    }
                }

                StyledText {
                    text: name;
                    font.pixelSize: scaleY(3.5);
                    color: "White" ;
                    isBold: false
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: parent.width
                    height: parent.height
                    font.weight: Font.Light
                }
                Flickable {
                    anchors.fill:parent
                    flickableDirection: Flickable.HorizontalFlick
                    onContentXChanged: alphaList.setDragout(contentX)
                    contentWidth: parent.width + alphaList.width
                    MouseArea{
                        id:mouseclick
                        anchors.fill: parent
                        onClicked: {
                            manager.setStringParam(4, id);
                        }
                    }
                }
            }
        }

        ListView {
            id: list_view1
            model: manager.getDataGridModel("MediaFile", 63)
            delegate: contactDelegateList
            clip: true
            cacheBuffer: 15
            spacing:scaleY(1)
            width: parent.width
            anchors{
                top:parent.top
                left:parent.left
                bottom:parent.bottom
            }

        }
        Connections {
            target: list_view1.model
            onScrollToItem: {console.log("scroll to item : " + item); list_view1.positionViewAtIndex(item, ListView.Beginning); }
        }

        Rectangle {
            id: alphaList
            height: manager.appHeight
            width: scaleX(14)
            anchors{
                top:fileviewscreen.top
                bottom:fileviewscreen.bottom
            }
            color: "black"
            opacity:.75

            x: fileviewscreen.x + fileviewscreen.width

            function setDragout(xLength){
                if (xLength > 30 || xLength < -30) {
                    if (xLength > alphaList.width) {
                        xLength = alphaList.width
                    }
                    alphaList.x = fileviewscreen.x + fileviewscreen.width - xLength
                }
            }

            ListView{
                id:alpha_list
                model: alphabetlist
                anchors.fill:parent

                delegate: Item{
                    height: scaleY(10)
                    width: scaleX(10)

                    StyledText {
                        id:alpha_label
                        text: name
                        anchors.centerIn: parent
                        color: "white"
                        font.bold: true
                        fontSize: 42
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            fileviewscreen.state="browsing"
                            currentSeekLetter = name
                            //			if(dataModel.totalPages==1){
                            //                            list_view1.maingrid.positionViewAtIndex(dataModel.setSection(name), ListView.Beginning)
                            //			}else{
                            //                            manager.setGridStatus(false)
                            manager.seekGrid("MediaFile", name)
                            //			}
                        }
                        onPressAndHold: {
                            fileviewscreen.state="browsing"
                        }
                    }
                }
            }
        }



        //        AttributeSelector {
        //            id:selector
        //        }




        states: [
            State {
                name: "browsing"
                PropertyChanges {
                    target: list_view1
                    width:  scaleX(100)
                }
            },
            State {
                name: "alphabets"
                PropertyChanges {
                    target: list_view1
                    width:scaleX(70)
                }
            }
        ]
    }

}
