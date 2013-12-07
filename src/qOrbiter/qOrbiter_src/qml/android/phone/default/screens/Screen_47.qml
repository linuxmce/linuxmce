import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/handlers"

Item {
    id:fileviewscreen
    anchors{
        top:parent.top
        bottom: parent.bottom
        left:parent.left
        right:parent.right
    }

    clip: true
    focus:true
    state:"browsing"
    property string currentSeekLetter:"A"
    Component.onCompleted: {
        hdr.state="grid"
        forceActiveFocus()
    }

    Connections{
        target:manager
        onDceGridSepChanged:manager.requestPage(0)
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
        event.accepted=true
        switch(event.key){
        case Qt.Key_Back:
            manager.goBackGrid()
            break;
        case Qt.Key_MediaPrevious:
            manager.goBackGrid()
            break;
        case Qt.Key_Menu:
            if(selector.state === "hidden")
                selector.state ="atrselect"
            else
                selector.state ="hidden"
            break;
        case Qt.Key_M:
            if(selector.state === "hidden")
                selector.state ="atrselect"
            else
                selector.state ="hidden"
            break;
        default:
            console.log(event.key)
            break
        }
    }

    //    Connections
    //    {
    //        target: dataModel
    //        onProgressChanged:progress_bar_fill.width = progress_bar.width* ((dataModel.currentCells / dataModel.totalcells))
    //        onReady:progress_bar_fill.width = 0
    //    }

    //    Rectangle
    //    {
    //        id:progress_bar
    //        height: scaleY(3)
    //        width: scaleX(50)
    //        color: "transparent"
    //        radius:5
    //        anchors.bottom: parent.bottom
    //        anchors.horizontalCenter: list_view1.horizontalCenter
    //        Text {
    //            id: total_cells
    //            text: dataModel.totalcells
    //            color: "yellow"
    //            font.pixelSize: scaleY(3)
    //            anchors.left: progress_bar.right
    //        }

    //        Text {
    //            id: current_cells
    //            text: dataModel.currentCells
    //            color: "green"
    //            font.pixelSize: scaleY(3)
    //            anchors.left: progress_bar_fill.right
    //        }

    //        Rectangle{
    //            id:progress_bar_fill
    //            height: parent.height
    //            width: 0
    //            color: androidSystem.orangeHighlight
    //            anchors.bottom: parent.bottom
    //            clip:true
    //            radius:5
    //        }
    //    }

    Connections
    {
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


           MouseArea{
               id:mouseclick
               anchors.fill: parent
               onClicked:manager.setStringParam(4, id);
               onPressAndHold: fileviewscreen.state="alphabets"
           }
        }
    }

    Rectangle{
        id:up_arrow
        height: manager.appHeight*.07
        color: "black"
        opacity:.85
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        StyledText{
            anchors.centerIn: parent
            text:"up"
            fontSize: 43
            color:"white"

        }
        MouseArea{
            anchors.fill: parent
            onReleased:{ if(manager.media_currentPage !==0) manager.requestPage( manager.media_currentPage-1) }
        }
    }

    ListView {
        id: list_view1
        model:dataModel
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 15
        spacing:scaleY(1)
        width: parent.width
        anchors{
            top:up_arrow.bottom
            left:parent.left
            bottom:dwn_arrow.top
        }

    }
    Rectangle{
        id:dwn_arrow
        height: manager.appHeight*.07
        color: "black"
        opacity:.85

        anchors{
            left:parent.left
            right:parent.right
            bottom: parent.bottom
        }
        StyledText{
            anchors.centerIn: parent
            text:"Down"
            fontSize: 43
            color:"white"
        }
        MouseArea{
            anchors.fill: parent
            onPressed: manager.requestPage( manager.media_currentPage+1)
        }
    }

    ListView{
        id:alpha_list
        height: manager.appHeight
        width: scaleX(10)
        model: alphabetlist

        anchors{
            top:fileviewscreen.top
            left:list_view1.right
            bottom:fileviewscreen.bottom
            right:fileviewscreen.right
        }

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
                    if(dataModel.totalPages==1){
                        list_view1.maingrid.positionViewAtIndex(dataModel.setSection(name), ListView.Beginning)
                    }else{
                        manager.setGridStatus(false)
                        manager.setSeekLetter(name)

                    }
                }
                onPressAndHold: {
                    fileviewscreen.state="browsing"
                }
            }
        }
    }



    /*  ListView{
        id:model_pages
        height: manager.appHeight
        width: scaleX(10)
        model: dataModel.totalPages
        anchors.left: list_view1.right
        delegate: Item{
            height: scaleY(10)
            width: scaleX(10)

            Rectangle{
                anchors.fill: parent
                radius: 5
                color: "transparent"
            }
            Text {
                id:page_label
                text: name
                font.pixelSize: scaleY(2)
                anchors.centerIn: parent
                color: "orange"
                font.bold: true
            }

            MouseArea{
                anchors.fill: parent
                onReleased: { manager.requestPage(index) ; parent.color = "transparent"}
                onPressed: parent.color = "orange"

            }

        }

    }*/


    AttributeSelector {
        id:selector
    }




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
