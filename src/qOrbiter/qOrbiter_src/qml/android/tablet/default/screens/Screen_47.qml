import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../lib/effects"

Item {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
   focus:true
   Component.onCompleted: forceActiveFocus()
   Keys.onMenuPressed: console.log("show subtypes menu!")
   Keys.onPressed: if(event.key===Qt.Key_MediaPrevious){manager.goBackGrid()}
    clip: true
    property int mouselocY: 0
    property int mouselocX: 0
    // Component.onCompleted: manager.requestTypes(manager.i_current_mediaType)

    function runEffects()
    {
        MyJs.createStageComponent("FileDetails"+manager.q_mediaType+".qml" , parent)
    }

    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            runEffects()
        }
    }

    MediaListGridDelagate {
        id: contactDelegate
       visible: false
    }

    Rectangle{
        id: pos_label
        anchors.top: fileviewscreen.top
        anchors.horizontalCenter: fileviewscreen.horizontalCenter
        radius:5
        focus:true
        color:style.darkhighlight
        width: scaleX(75)
        height: scaleY(5)
        Row{
            id:label_row
            width: childrenRect.width
            height: scaleY(5)
            anchors.centerIn: pos_label
            spacing: scaleX(1)
            Text {
                id: grid_position_label
                text: qsTr("You are browsing by:") + manager.i_current_mediaType
            }
            Text {
                id: grid_attritbute_label
                text: qsTr("Attribute Type Sort") + manager.q_attributetype_sort
            }
            Text {
                id: page_label
                text: qsTr("Current Page") + manager.media_currentPage
            }
            TextInput{
                id:seperationSetter
                width: page_label.width
                text: manager.media_pageSeperator

            }
            Rectangle{
                height: parent.height-1
                width: childrenRect.width + 10
                StyledText{
                    text: qsTr("Set")
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        pos_label.forceActiveFocus()
                        if(!seperationSetter.text.match("\d")){
                            manager.setDceGridSep(seperationSetter.text)
                            manager.requestPage(0)
                        }
                        else
                        {
                            seperationSetter.text.color = red
                        }
                    }
                }
            }
        }

    }
    Connections
    {
        target: dataModel
        onProgressChanged:{progress_bar_fill.height = progress_bar.height* ((dataModel.currentCells / dataModel.totalcells)); console.log(dataModel.progress)}
        onReady:progress_bar_fill.height = 0
        onLoadingStatusChanged:progress_bar_fill.color = dataModel.loadingStatus ? "green" : "red"
    }

    Rectangle
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



//    Component
//    {
//        id: contactDelegate
////        DropShadow{
////            sourceItem: imagerect
////            anchors.fill: imagerect
////            color:"black"
////            blur:.75
////        }
//        Rectangle
//        {
//            id:mainItem
//            width: scaleX(25);
//            height: scaleY(25)
//            color: "transparent"
//            clip:true

//            Image
//            {
//                id: imagerect;
//                source: path !=="" ? "http://192.168.80.1/lmce-admin/MediaImage.php?img="+path : ""
//                height: scaleY(12);
//                width: scaleX(18);
//                anchors.centerIn: parent;
//                fillMode: Image.PreserveAspectCrop
//                 smooth: true
//                asynchronous: true
//            }

//            Rectangle{
//                id:textmask
//                color: "grey"
//                anchors.fill:celllabel
//                opacity: .80
//            }

//            Text
//            {
//                id:celllabel
//                text: name;
//                font.pointSize: 14;
//                color: "Black" ;
//                wrapMode: "WrapAtWordBoundaryOrAnywhere"
//                width: imagerect.width
//                font.bold: true
//                anchors.top: imagerect.top
//                anchors.horizontalCenter: imagerect.horizontalCenter
//            }
//            MouseArea
//            {
//                anchors.fill: parent
//                onClicked: {setStringParam(4, id); mouselocX = mouseX; mouselocY = mouseY}
//            }
//        }
//    }

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


    MultiStateFileDisplay{id:grid_view1; anchors.top: pos_label.bottom}


    ListView{
        id:model_pages
        height: manager.appHeight
        width: scaleX(10)
        model: dataModel.totalPages
        anchors.left: parent.left
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
                onReleased: {  page_label.font.italic = true ; manager.requestPage(index);  }
                onPressed: page_label.font.italic = false
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
            Text {
                id: test
                text: name
                font.pixelSize: scaleY(4)
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
                onPressed: { if(dataModel.totalPages===1){
                        grid_view1.maingrid.positionViewAtIndex(dataModel.setSection(name), ListView.Beginning)

                    }else{
                        manager.setSeekLetter(name)
                    }
                }
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
    GenericAttributeSelector{
        id:selector
        activeModel: "NULL"

    }
}

