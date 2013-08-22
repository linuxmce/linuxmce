import QtQuick 2.0
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
        case Qt.Key_Menu:

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
        anchors.top: list_view1.bottom
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

            Rectangle {
                id: background
                color: mouseclick.pressed ? "orange":"black"
                anchors.fill: parent
                opacity: mouseclick.pressed ? 1 : .75
            }

            Image  {
                id: imagerect;
                fillMode: Image.PreserveAspectCrop
                source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?type=img&val="+path : ""
                anchors.fill: parent
            }
            Rectangle{
                id:mask
                anchors.fill: parent
                color:"black"
                opacity:.65
            }

            StyledText {
                text: name;
                font.pixelSize: scaleY(3.5);
                color: "White" ;
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
        height:scaleY(90)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 15
        anchors.left: parent.left
        anchors.leftMargin: scaleX(2)
        anchors.top: fileviewscreen.top

    }
    ListView{
        id:model_pages
        height: manager.appHeight
        width: scaleX(10)
        model: dataModel.totalPages
        anchors.left: list_view1.right
        delegate: Rectangle{
            height: scaleY(10)
            width: scaleX(10)
            color: "transparent"
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

    Text{
        id:spaceholder
        text:manager.dceResponse
        font.pixelSize: scaleY(3)
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: "silver"

    }

    AttributeSelector {
        id:selector

    }

    GenericAttributeFilter{
        id:filterTarget
        anchors.centerIn: parent
    }
}
