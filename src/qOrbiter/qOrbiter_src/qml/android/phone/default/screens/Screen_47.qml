import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen

    width: appW
    height: appH
    color: "silver"
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
            width: list_view1.width;
            height: scaleY(10)
            Rectangle {
                id: background
                x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
                color: "floralwhite"
                border.color: "black"
                radius: 5
            }
            MouseArea {
                anchors.fill: parent
                onClicked: setStringParam(4, id)
            }
            Row {
                id: topLayout
                x: 10; y: 10; height: imagerect.height; width: parent.width
                spacing: 10



                Image
                {
                    id: imagerect;
                    source:"image://datagridimg/"+id ;
                    height: scaleX(10);
                    width: scaleY(10);
                    fillMode: Image.PreserveAspectFit;
                }

                Column {
                    width: background.width - imagerect.width - 20;
                    height: imagerect.height
                    spacing:5
                    Text {
                        text: name;
                        opacity: 1;
                        font.pointSize: scaleY(3);
                        color: "black" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //anchors.fill: parent
                        font.bold: false
                    }
                }

            }
        }
    }


    ListView {
        id: grid_view1
        width: scaleX(75)
        height: scaleY(75)
        model:dataModel
        delegate: contactDelegateList
        highlightFollowsCurrentItem: true
        highlight: appHighlight
        focus: true
        clip: true

    }



    Component {
        id: appHighlight
        Rectangle { width: 80; height: 80; color: "lightsteelblue" }
    }

    Row
    {
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.left: grid_view1.left

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
        AttributeSelector {}
    }
}
