import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/effects"

Item {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
    focus:true
    state:"gridbrowsing"
    Component.onCompleted: forceActiveFocus()

    Keys.onReleased:{
        event.accepted = true
        switch(event.key){
        case Qt.Key_Back:
        case Qt.Key_MediaPrevious:
	    if (!manager.goBackGrid())
                event.accepted=false
            break;
        case Qt.Key_Menu:
            if(fileviewscreen.state!=="sorting")
                fileviewscreen.state = "sorting"
            else{
                fileviewscreen.state = "gridbrowsing"
            }
            event.accepted=true
            break;

        default:
            console.log("Key not accepted! ==>" + event.key)

        }
    }
    clip: true
    property int mouselocY: 0
    property int mouselocX: 0
    // Component.onCompleted: manager.requestTypes(manager.i_current_mediaType)

    Connections{
        target: filedetailsclass
        onShowDetailsChanged:{
            fileviewscreen.state = "detail"
        }
    }

    MediaListGridDelagate {
        id: contactDelegate
        visible: false
    }

    MediaPositionHeader {
        id: pos_label
    }

/*    Rectangle
    {
        id:progress_bar
        height: scaleY(65)
        width: scaleX(2)
        color: "transparent"
        border.color: "white"
        border.width: 1
        anchors.verticalCenter: parent.verticalCenter

        Rectangle{
            id:progress_bar_fill
            height: 0
            width: parent.width-1
            color: dataModel.loadingStatus ? "green" : "red"
            anchors.bottom: parent.bottom
            opacity: .25
        }
        StyledText {
            id: current_cells
            text: dataModel.currentCells
            color: "white"
            font.bold: true
            font.pixelSize: scaleY(4)
            anchors.top: progress_bar.bottom
            anchors.left: progress_bar.left
        }

        StyledText {
            id: total_cells
            text: dataModel.totalcells
            color: "white"
            font.bold: true
            font.pixelSize: scaleY(5)
            anchors.bottom: progress_bar.top
            anchors.left: progress_bar.left
        }

    }
*/

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
                    height: 100;
                    width: 156;
                    fillMode: Image.PreserveAspectCrop
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


    MultiStateFileDisplay{
        id:grid_view1; anchors.top: pos_label.bottom
        height: scaleY(100)-pos_label.height-sortingOptions.height
    }


/*    ListView{
        id:model_pages
        height: scaleY(65)
        width: scaleX(10)
        model: dataModel.totalPages
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        delegate: Rectangle{
            height: scaleY(10)
            width: scaleX(10)
            color: "transparent"
            Text {
                id:page_label2
                text: index
                font.pixelSize: scaleY(3.5)
                anchors.centerIn: parent
                color: index == manager.media_currentPage ? "green":"slategrey"
                font.bold: true
            }

            MouseArea{
                anchors.fill: parent
                onReleased: {   manager.requestPage(index);  }

            }

        }

    }*/
    ListView{
        id:alphalist
        height: scaleY(65)
        width: scaleX(5)
        clip:true
        anchors.left: grid_view1.right
        anchors.verticalCenter: parent.verticalCenter
        model:alphabetlist

        delegate:
            Rectangle {
            id:alphabetrect
            height: scaleY(5)
            width: scaleX(4)
            color: "transparent"
            clip:false
            Text {
                id: test
                text: name
                font.pixelSize: scaleY(4)
                anchors.centerIn: parent
                color:"aliceblue"

            }
            MouseArea{
                anchors.fill: parent

                onEntered: {
                    alphabetrect.scale = 1.5
                }
                onExited: {

                    alphabetrect.scale = 1
                }
                onReleased: manager.seekGrid("MediaFile", name)
            }
        }
    }
    Connections { 
	target: manager.getDataGridModel("MediaFile", 63) 
	onScrollToItem: {  grid_view1.maingrid.positionViewAtIndex(item, ListView.Beginning); }
    } 

    Row
    {
        id:sortingOptions
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        z:1

        ButtonSq{
            height: scaleY(style.iconHeight)
            width: scaleX(style.iconWidth)
            buttontext: "Go Back"
            buttontextbold: true
            MouseArea{
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
    GenericAttributeSelector{
        id:selector
        activeModel: "NULL"
    }

    states: [
        State {
            name: "gridbrowsing"
            PropertyChanges {
                target: sortingOptions
                height:0
                visible:false
                z:0
            }
            PropertyChanges {
                target: loadBot
                visible:false
            }
            PropertyChanges {
                target: file_details_loader
                source:""
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
            StateChangeScript{
               // script: {manager.resetModelAttributes; mediatypefilter.resetStates(); attribfilter.resetStates(); }
            }
        },
        State{
            name:"sorting"
            PropertyChanges{
                target:sortingOptions
                height:childrenRect.height
                visible:true
                z:5
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.right
            }
        },
        State{
            name:"menu"
        },
        State {
            name: "detail"
            PropertyChanges {
                target: progress_bar
                visible:false
            }
            PropertyChanges {
                target: sortingOptions
                height:0
                visible:false
                z:0
            }

            PropertyChanges {
                target: loadBot
                visible:false
            }
            AnchorChanges{
                target: file_details_loader
                anchors.left: parent.left
            }
            PropertyChanges {
                target: file_details_loader
                source:"../components/FileDetails_"+manager.i_current_mediaType+".qml"
            }
        }
    ]

    Loader{
        id:file_details_loader
        height: parent.height
        width: parent.width
        anchors.left: files_view_screen.right
        onStatusChanged: {
            console.log("Status in details loader changed")
            if(file_details_loader.status===Loader.Error){
                file_details_loader.source = "../components/GenericFileDetails.qml"
            }
        }
    }
}

