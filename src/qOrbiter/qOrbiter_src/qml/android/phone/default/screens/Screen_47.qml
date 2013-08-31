import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../lib/handlers"

Item {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
    clip: true
    focus:true
    Component.onCompleted: {
        forceActiveFocus()
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

    Connections
    {
        target: dataModel
        onProgressChanged:progress_bar_fill.width = progress_bar.width* ((dataModel.currentCells / dataModel.totalcells))
        onReady:progress_bar_fill.width = 0
    }

    Rectangle
    {
        id:progress_bar
        height: scaleY(3)
        width: scaleX(50)
        color: "transparent"
        radius:5
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: list_view1.horizontalCenter
        Text {
            id: total_cells
            text: dataModel.totalcells
            color: "yellow"
            font.pixelSize: scaleY(3)
            anchors.left: progress_bar.right
        }

        Text {
            id: current_cells
            text: dataModel.currentCells
            color: "green"
            font.pixelSize: scaleY(3)
            anchors.left: progress_bar_fill.right
        }

        Rectangle{
            id:progress_bar_fill
            height: parent.height
            width: 0
            color: androidSystem.orangeHighlight
            anchors.bottom: parent.bottom
            clip:true
            radius:5
        }
    }

    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            loadComponent("FileDetails.qml")

        }
    }




    Component
    {
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


            MediaListClickHandler{
                id:mouseclick
            }

        }
    }


    ListView {
        id: list_view1
        width: scaleX(95)
        height:scaleY(85)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 15
        anchors.centerIn: parent
        spacing:scaleY(1)

    }
    ListView{
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
                text: index
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

    }
    AttributeSelector {
        id:selector

    }

    GenericAttributeFilter{
        id:filterTarget
        anchors.centerIn: parent
    }
}
