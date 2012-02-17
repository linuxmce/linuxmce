import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen
    width: appW
    height: appH
    color: "transparent"
    clip: true
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
            height: scaleY(8)
            Rectangle {
                id: background
                color: "transparent"
                border.color: "silver"
                radius: 5
                height: scaleY(8)
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
                    font.pixelSize: scaleY(2);
                    color: "black" ;
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    //anchors.fill: parent
                    width: scaleX(55)
                    font.bold: false
                    anchors.left: imagerect.right
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: setStringParam(4, id)
                    onPressed: parent.color="orange"
                    onReleased: parent.color="transparent"
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
        anchors.horizontalCenter: parent.horizontalCenter
    }


    Row
    {id:bottomrow
        height: childrenRect.height
        anchors.topMargin: scaleY(5)
        width: childrenRect.width
        anchors.top: list_view1.bottom
        anchors.left: list_view1.left
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
        anchors.top: bottomrow.bottom
        anchors.left: parent.left
        color: "silver"

    }
}
