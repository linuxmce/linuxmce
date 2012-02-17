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

    Component
    {
        id: contactDelegateList
        Item {
            width: scaleX(75)
            height: scaleY(10)
            Rectangle {
                id: background
                color: mouseclick.pressed ? "orange":"transparent"
                border.color: "silver"
                radius: 5
                height: scaleY(10)
                width: scaleX(75)

                Image
                {
                    id: imagerect;
                    source:"image://datagridimg/"+id ;
                    height: scaleX(8);
                    width: scaleY(8);
                    fillMode: Image.PreserveAspectFit;
                    anchors.left: background.left
                }

                Text {
                    text: name;
                    opacity: 1;
                    font.pixelSize: scaleY(4);
                    color: "Silver" ;
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    //anchors.fill: parent
                    width: scaleX(55)
                    height: scaleY(6)
                    font.bold: false
                    anchors.left: imagerect.right
                }

                MouseArea {
                    id:mouseclick
                    anchors.fill: parent
                    onClicked: setStringParam(4, id)

                }
            }
        }
    }


    ListView {
        id: list_view1
        width: scaleX(75)
        height:scaleY(65)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        spacing: scaleY(2)
        anchors.horizontalCenter: parent.horizontalCenter
    }


    Row
    {id:bottomrow
        height: childrenRect.height
        anchors.topMargin: scaleY(5)
        anchors.top: list_view1.bottom
        width: childrenRect.width
        anchors.horizontalCenter: parent.horizontalCenter
        ButtonSq
        {
            height: style.iconHeight
            width: style.iconWidth
            buttontext: "Go Back"
            buttontextbold: true
            MouseArea
            {
                anchors.fill:parent
                onClicked: goBackGrid()
            }
        }
        AttributeSelector {id:selector}

    }
    Text{
        id:spaceholder
        text:dcemessage
        font.pixelSize: scaleY(3)
        font.family: "Droid Sans"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        color: "silver"

    }
}
