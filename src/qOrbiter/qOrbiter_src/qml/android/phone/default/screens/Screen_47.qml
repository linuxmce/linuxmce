import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen
    width: appW
    height: appH
    color: "transparent"
    clip: true
    border.color: "silver"
    Connections
    {
        target: dataModel
        onProgressChanged:progress_bar_fill.width = ((progress_bar.width) * (dataModel.progress )) /100
        onReady:progress_bar_fill.width = 0
    }

    Rectangle
    {
        id:progress_bar
        height: scaleY(3)
        width: scaleX(50)
        color: "transparent"

        anchors.top: list_view1.bottom
        anchors.horizontalCenter: list_view1.horizontalCenter
        Text {
            id: total_cells
            text: dataModel.totalcells
            color: "grey"
            font.pixelSize: scaleY(3)
            anchors.left: progress_bar.right
        }

        Rectangle{
            id:progress_bar_fill
            height: parent.height
            width: 0
            color: "transparent"
            anchors.bottom: parent.bottom

            Image {
                id: fill_image
                source: "../img/blue.png"
                height: parent.height
                width: parent.width
            }

        }

        Text {
            id: current_cells
            text: dataModel.currentCells
            color: "grey"
            font.pixelSize: scaleY(3)
            anchors.right: progress_bar.left
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

    AttributeSelector {
        id:selector
        anchors.left:fileviewscreen.left
        anchors.verticalCenter: fileviewscreen.verticalCenter
    }

    Component
    {
        id: contactDelegateList
        Item {
            width: scaleX(75)
            height: scaleY(15)
            Rectangle {
                id: background
                color: mouseclick.pressed ? "orange":"transparent"
                anchors.centerIn: parent
                border.color: "silver"               
                height: scaleY(15)
                width: scaleX(75)
                Row{
                    Image
                    {
                        id: imagerect;
                        source:"image://datagridimg/"+id ;
                        height: scaleX(15);
                        width: scaleY(15);
                        fillMode: Image.PreserveAspectFit;

                    }

                    Text {
                        text: name;
                        font.pixelSize: scaleY(2.5);
                        color: "Silver" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //anchors.fill: parent
                        width: scaleX(55)
                        height: scaleY(20)
                        font.family: "Droid Sans"                        
                    }
                }

                MouseArea {
                    id:mouseclick
                    anchors.fill: parent                   
                    onReleased: setStringParam(4, id)
                }
            }
        }
    }


    ListView {
        id: list_view1
        width: scaleX(75)
        height:scaleY(85)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 15
        anchors.left: selector.right
        anchors.top: fileviewscreen.top

    }
    ListView{
        id:model_pages
        height: appH
        width: scaleX(10)
        model: pageList
        anchors.left: list_view1.right
        delegate: Rectangle{
            height: scaleY(10)
            width: scaleX(10)
            color: "transparent"
            Text {
                id:page_label
                text: label
                font.pixelSize: scaleY(2)
                anchors.centerIn: parent
                color: "orange"
                font.bold: true
            }

            MouseArea{
                anchors.fill: parent
                onReleased: { dcerouter.requestPage(index) ; parent.color = "transparent"}
                onPressed: parent.color = "orange"

            }

        }

    }

    Text{
        id:spaceholder
        text:manager.dceResponse
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: "silver"

    }
}
