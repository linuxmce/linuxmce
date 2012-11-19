import QtQuick 1.0
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:fileviewscreen
    width: appW
    height: appH
    color: "transparent"
    clip: true
    property int mouselocY: 0
    property int mouselocX: 0
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
    }
    MediaListProgressBar {
        id: progress_bar
        anchors.verticalCenter: parent.verticalCenter
    }



    MediaListGridDelagate {
        id: contactDelegate
    }

    Component
    {
        id: contactDelegateList
        Item {
            width: list_view1.width;
            height: 120

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
                    height: 256
                    width: 256
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


    MultiStateFileDisplay{id:grid_view1; anchors.top: pos_label.bottom}


    ListView{
        id:model_pages
        height: appH
        width: scaleX(10)
        model: dataModel.totalPages
        anchors.left: parent.left
        spacing:scaleY(2)
        delegate: Rectangle{
            height: scaleY(6)
            width: scaleX(6)
            color: "transparent"
            anchors.horizontalCenter: parent.horizontalCenter
            StyledText {
                id:page_label2
                text: index
                font.pixelSize: scaleY(4.5)
                anchors.centerIn: parent
                color: index == manager.media_currentPage ? "green":"slategrey"
                font.bold: true
            }

            MouseArea{
                anchors.fill: parent
                onReleased: {  page_label2.font.italic = true ; manager.requestPage(index);  }
                onPressed: page_label2.font.italic = false
            }

        }

    }
    ListView{
        id:alphalist
        height: grid_view1.height
        width: scaleX(5)
        clip:true
        anchors.left: grid_view1.right
        model:alphabetlist

        delegate:
            Rectangle {
            id:alphabetrect
            height: scaleY(5)
            width: scaleX(4)
            color: "transparent"
            clip:false
            StyledText {
                id: test
                text: name
                font.pixelSize: 18
                anchors.centerIn: parent
                color:"aliceblue"

            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    alphabetrect.scale = 1.5
                }
                onExited: {

                    alphabetrect.scale = 1
                }
                onClicked: manager.setSeekLetter(name)
            }
        }
    }

    Row
    {
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        ButtonSq
        {
            height: scaleY(style.iconHeight)
            width: scaleX(style.iconWidth)
            buttontext: "Go Back"
            buttontextbold: true
            MouseArea
            {
                anchors.fill:parent
                onClicked: manager.goBackGrid()
            }
        }
        AttributeSelector {}
    }
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
}

