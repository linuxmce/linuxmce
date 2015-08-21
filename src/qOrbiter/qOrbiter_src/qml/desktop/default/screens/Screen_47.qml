import QtQuick 1.1

import AudioVisual 1.0
import "../components"
import "../animation"
import "../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Item {
    id:fileviewscreen
    anchors.fill: parent

    clip: true
    focus:true
    property int mouselocY: 0
    property int mouselocX: 0

    Connections{
        target:userList
        onVerifyPinForMedia:{
            // show pin box to auth user with userToValidate
            console.log("user to validate::"+userToValidate)
            loadComponent("PinorbiterWindow.qml", {"requestedUser":userToValidate})
        }
    }
    Connections{
        target:manager
        onModelChanged:{
            gridView.positionViewAtIndex(manager.currentIndex, GridView.Beginning)
            manager.currentIndex=-1
        }
    }


    function runEffects(){
        MyJs.createStageComponent("FileDetails"+manager.q_mediaType+".qml" , fileviewscreen)
    }

    Connections{
        target: filedetailsclass
        onShowDetailsChanged:{
            runEffects()
        }
    }

    Rectangle{
        id: pos_label
        anchors.top: fileviewscreen.top
        anchors.horizontalCenter: fileviewscreen.horizontalCenter
        color:skinStyle.darkhighlight
        width: gridView.width
        height: scaleY(5)
        opacity: .25
    }


    MediaListProgressBar {
        id: progress_bar
        anchors.top: gridView.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MediaListGridDelagate {
        id: contactDelegate
        visible: false
    }

    GridView {
        id: gridView
        //z: 2
        width: scaleX(95)
        height: scaleY(85)

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: scaleY(3)
        model: manager.getDataGridModel("MediaFile", 63)
        delegate: MediaListGridDelagate{}
        focus: true
        //clip: true
        //contentItem.clip: true
        cellWidth: contactDelegate.width
        cellHeight: contactDelegate.height
        flow:GridView.TopToBottom
        opacity:1
        scale:1
        cacheBuffer: 15
        interactive:true
        highlightFollowsCurrentItem:true
        Component.onCompleted:{forceActiveFocus(); manager.setStringParam(0, manager.q_mediaType); }
        onModelChanged: {
            currentIndex = manager.currentIndex
            positionViewAtIndex(currentIndex, GridView.Beginning)
            manager.currentIndex=1
        }



        Keys.onPressed: {
            if(event.key !==Qt.Key_Enter && event.key !== 16777237 && event.key !==16777236 && event.key !==16777234 && event.key !==16777235 && event.text != ""){
                manager.seekGrid("MediaFile", event.text)
            }
        }
        Keys.onEnterPressed: {
            manager.setStringParam(4, gridView.model.get(currentIndex, "id"))
            gridView.positionViewAtIndex(0, ListView.Beginning)
        }

        highlight:Rectangle{
            color:"white"
            border.color: "white"
            border.width: 1
            opacity: .35
            BorderImage {
                id: borderimg
                horizontalTileMode: BorderImage.Repeat
                source: "../img/icons/drpshadow.png"
                anchors.fill: parent
                anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                border { left: 10; top: 10; right: 10; bottom: 10 }
                smooth: true
            }
        }
    }
    Connections {
        target: gridView.model
        onScrollToItem: {  gridView.currentIndex = item; gridView.positionViewAtIndex(item, ListView.Beginning); }
    }


    Rectangle{
        id:options_rect
        height: scaleY(50)
        width:childrenRect.width
        anchors.left: parent.left
        color:"transparent"
        Column{
            height: parent.height
            width: childrenRect.width
            MoonButton {
                id: pall
                text: "Play All"
                PlayAll{}
            }
            //            MoonButton {
            //                id: genre
            //                text: "Genre"
            //            }
            MoonButton {
                id: asort
                text: "Attribute \n Sort"
                MouseArea{
                    anchors.fill:parent
                    onClicked: {myFilters.y = asort.y; myFilters.currentFilterModel=attribfilter}
                }
            }
            MoonButton {
                id: sources
                text: "Sources"
            }
            MoonButton {
                id: users
                text: "Users"
                MouseArea{
                    anchors.fill:parent
                    onClicked: {myFilters.y = rez.y; myFilters.selectingUser = true; myFilters.currentFilterModel=userList}
                }
            }
            MoonButton {
                id: rez
                text: "Resolution"
                MouseArea{
                    anchors.fill:parent
                    onClicked: {myFilters.y = rez.y; myFilters.currentFilterModel=fileformatmodel}
                }
            }
            MoonButton {
                id: mTypes
                text: "Media \n Types"

                MouseArea{
                    anchors.fill:parent
                    onClicked: {
                        myFilters.y = mTypes.y
                        myFilters.currentFilterModel = mediatypefilter
                    }
                }
            }
            MoonButton {
                id: goback
                text: "Back"
                MouseArea
                {
                    anchors.fill:parent
                    onClicked:{ manager.goBackGrid()}
                }
            }

            HomeButton{
                height: 50
                width: 50
            }
        }
    }
    //    ListView{
    //        id:model_pages
    //        height: manager.appHeight
    //        width: scaleX(10)
    //        model: dataModel.totalPages
    //        anchors.left: parent.left
    //        spacing:scaleY(2)
    //        delegate: Rectangle{
    //            height: scaleY(6)
    //            width: scaleX(6)
    //            color: "transparent"
    //            anchors.horizontalCenter: parent.horizontalCenter
    //            StyledText {
    //                id:page_label2
    //                text: index
    //                font.pixelSize: scaleY(4.5)
    //                anchors.centerIn: parent
    //                color: index == manager.media_currentPage ? "green":"slategrey"
    //                font.bold: true
    //            }

    //            MouseArea{
    //                anchors.fill: parent
    //                onReleased: {  page_label2.font.italic = true ; manager.requestPage(index);  }
    //                onPressed: page_label2.font.italic = false
    //            }

    //        }

    //    }


    //    Row
    //    {
    //        height: childrenRect.height
    //        anchors.topMargin: 0
    //        width: childrenRect.width
    //        anchors.top: grid_view1.bottom
    //        anchors.horizontalCenter: parent.horizontalCenter

    //        ButtonSq
    //        {
    //            height: scaleY(skinStyle.iconHeight)
    //            width: scaleX(skinStyle.iconWidth)
    //            buttontext: "Go Back"
    //            buttontextbold: true
    //            MouseArea
    //            {
    //                anchors.fill:parent
    //                onClicked: manager.goBackGrid()
    //            }
    //        }
    //        AttributeSelector {}
    //    }
    ListModel{
        id:alphabetlist

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
    FilterView{
        id:myFilters
        anchors.left: options_rect.right
    }

}

