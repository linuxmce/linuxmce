import QtQuick 1.0
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../../lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
    color: "transparent"
    clip: true
    property int mouselocY: 0
    property int mouselocX: 0
    property int mpCount: model_pages.count
    //    Connections{
    //        target:dcerouter
    //        onNewGrid:{manager.requestPage(0); console.log("New Orbiter Grid Ready")}
    //    }

    function runEffects()
    {
        MyJs.createStageComponent("FileDetails"+manager.i_current_mediaType+".qml" , fileviewscreen)
    }

    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            runEffects()

        }
    }
    Rectangle{
        id: pos_label
        anchors.top: fileviewscreen.top
        anchors.horizontalCenter: fileviewscreen.horizontalCenter

        color:style.darkhighlight
        width: grid_view1.width
        height: scaleY(5)
        opacity: .25


    }

    MediaListInfoBar {
        id: label_row
        anchors.centerIn: pos_label
        z:2
    }
    MediaListProgressBar {
        id: progress_bar
        anchors.top: grid_view1.bottom
        anchors.left: grid_view1.left
    }



    MediaListGridDelagate {
        id: contactDelegate
    }


    Rectangle {
        id:grid_view1
        width: scaleX(85)
        height: scaleY(85)
        clip: true
        focus:true
        anchors.horizontalCenter: parent.horizontalCenter
        color: "transparent"

        GridView {
            id: gridView
            //z: 2
            width: scaleX(85)
            height: scaleY(85)
            anchors.centerIn: grid_view1
            model:dataModel
            delegate: contactDelegate
            focus: true
            //clip: true
            //contentItem.clip: true
            cellWidth: scaleX(21)
            cellHeight: scaleY(21)
            flow:GridView.TopToBottom
            opacity:1
            scale:1
            cacheBuffer: 0
            interactive:true
            Keys.onPressed: {
                if(event.key !==Qt.Key_Enter && event.key !== 16777237 && event.key !==16777236 && event.key !==16777234 && event.key !==16777235)
                {
                   gridView.currentIndex = dataModel.setSection(event.key)
                    gridView.positionViewAtIndex(currentIndex,ListView.Beginning)
                }
                console.log(event.key)
            }
            Keys.onEnterPressed: {
                manager.setStringParam(4, dataModel.get(currentIndex, "id"))
            }
            highlightFollowsCurrentItem:true
            Component.onCompleted:forceActiveFocus()

            highlight:Rectangle{
                color:"white"
            }

        }

    }
    Rectangle{
        id:options_rect
        height: scaleY(50)
        width: scaleX(15)
        anchors.left: parent.left
        color:"transparent"
        Column{
            height: parent.height
            width: parent.width
            MoonButton {
                id: pall
                text: "Play All"
                PlayAll{}
            }
            MoonButton {
                id: genre
                text: "Genre"

            }
            MoonButton {
                id: asort
                text: "Attribute \n Sort"
            }
            MoonButton {
                id: sources
                text: "Sources"
            }
            MoonButton {
                id: rez
                text: "Resolution"
            }
            MoonButton {
                id: mTypes
                text: "Media \n Types"
            }
            MoonButton {
                id: goback
                text: "Back"
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
    //            height: scaleY(style.iconHeight)
    //            width: scaleX(style.iconWidth)
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
    }

}

