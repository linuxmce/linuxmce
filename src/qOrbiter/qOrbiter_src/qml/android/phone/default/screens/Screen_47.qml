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
        target: filedetailsclass
        onShowDetailsChanged:
        {
            MyJs.createFileDetails()

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
            width: scaleX(80)
            height: scaleY(20)
            Rectangle {
                id: background
                color: mouseclick.pressed ? "orange":"transparent"
                anchors.centerIn: parent
                border.color: "silver"               
                height: scaleY(20)
                width: scaleX(79)

                Image
                {
                    id: imagerect;
                    source:"image://datagridimg/"+id ;
                    height: scaleX(20);
                    width: scaleY(20);
                    fillMode: Image.PreserveAspectFit;
                    anchors.left: background.left
                }

                Text {
                    text: name;                   
                    font.pixelSize: scaleY(3.5);
                    color: "Silver" ;
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    //anchors.fill: parent
                    width: scaleX(55)
                    height: scaleY(20)
                    font.bold: true
                    font.family: "Droid Sans"
                    anchors.left: imagerect.right
                }

                MouseArea {
                    id:mouseclick
                    anchors.fill: parent                   
                    onDoubleClicked: setStringParam(4, id)
                }
            }
        }
    }


    ListView {
        id: list_view1
        width: scaleX(80)
        height:scaleY(95)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        cacheBuffer: 30
        anchors.left: selector.right
        anchors.top: fileviewscreen.top

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
