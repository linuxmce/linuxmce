import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen

    width: appW
    height: appH
    color: "slateblue"

}

    /*
    Component.onCompleted: dcerouter.prepareFileList(manager.i_current_mediaType)
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
            height: 100
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
                    height: 100;
                    width: 156;
                    fillMode: Image.PreserveAspectFit;
                }

                Column {
                    width: background.width - imagerect.width - 20;
                    height: imagerect.height
                    spacing:5
                    Text {
                        text: name;
                        opacity: 1;
                        font.pointSize: 12;
                        color: "black" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //anchors.fill: parent
                        font.bold: true
                    }
                }

            }
        }
    }


    MultiStateFileDisplay{id:grid_view1}



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
      //  AttributeSelector {}
    }
}
*/
