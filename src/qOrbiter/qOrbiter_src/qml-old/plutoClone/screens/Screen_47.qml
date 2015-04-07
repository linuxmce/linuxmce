import QtQuick 2.0
//import Qt.labs.shaders 1.0
//import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"

import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:fileviewscreen
    width: manager.appWidth
    height: manager.appHeight
    color: "transparent"
    clip: true
    property int mouselocY: 0
    property int mouselocX: 0

//    Connections{
//        target: dcerouter
//        onNewGrid:manager.requestPage(0)
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
        radius:5
        color:skinStyle.darkhighlight
        width: grid_view1.width
        height: scaleY(5)
        Row{
            id:label_row
            width: childrenRect.width
            height: scaleY(5)
            anchors.centerIn: pos_label
            spacing: scaleX(1)
            StyledText {
                id: grid_position_label
                text: qsTr("You are browsing by:") + manager.i_current_mediaType
                font.pixelSize: 14

            }
            StyledText {
                id: grid_attritbute_label
                text: qsTr("Sorted by Attribute:  ") + manager.q_attributetype_sort
            }
            StyledText {
                id: page_label
                text: qsTr("Current Page") + manager.media_currentPage
            }

            TextInput{
                id:seperationSetter
                width: page_label.width
                text: manager.media_pageSeperator

                Keys.onEnterPressed: {
                    if (!seperationSetter.text.match("/D"))
                    {
                        manager.setGridSeperator(seperationSetter.text)
                        manager.requestPage(0)
                    }
                }
            }
        }
    }
        Connections
        {
            target: dataModel
            onProgressChanged:{progress_bar_fill.height = ((progress_bar.height) * (dataModel.progress / 100))}
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
                color: height === progress_bar.height ? "green" : "red"
                anchors.bottom: parent.bottom
                opacity: .25
            }

            StyledText {
                id: total_cells
                text: manager.media_pageSeperator
                color: "grey"
                font.bold: false
                font.pixelSize: scaleY(4)
                anchors.bottom: progress_bar.top
            }

        }



        Component
        {
            id: contactDelegate


            Rectangle
            {
                id:mainItem
                width: scaleX(20);
                height: scaleY(20)
                color: "transparent"

                MouseArea{
                    anchors.fill: mainItem
                    hoverEnabled: true
                    onEntered: {
                        mainItem.color = skinStyle.darkhighlight
                        mainItem.scale = 1.25
                        mainItem.z = 10

                    }
                    onExited: {
                        mainItem.color = "transparent"
                        mainItem.scale = 1
                        mainItem.z = 1
                    }
                }

                Rectangle
                {
                    id:frame

                    width: scaleX(19);
                    height: scaleY(19)
                    anchors.centerIn: mainItem
                    clip:true
                    color: "transparent"

                    BorderImage {
                        id: borderimg
                        horizontalTileMode: BorderImage.Repeat
                        source: "../img/icons/drpshadow.png"
                        anchors.fill: imagerect
                        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
                        border { left: 10; top: 10; right: 10; bottom: 10 }
                        smooth: true
                    }

                    Image
                    {
                        id: imagerect;
                    source:path !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?img="+path : ""
                        height: scaleY(18);
                        width: scaleX(18);
                        anchors.centerIn: parent;
                        fillMode: Image.PreserveAspectCrop
                        smooth: true
                        asynchronous: true
                    }



                    Rectangle{
                        id:textmask
                        color: "grey"
                        anchors.fill:celllabel
                        opacity: .5
                    }

                    StyledText
                    {
                        id:celllabel
                        text: name;
                        font.pointSize: 12;
                        color: "white" ;
                        font.family: keyFont.name
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        width: imagerect.width
                        font.bold: true
                        anchors.top: imagerect.top
                        anchors.horizontalCenter: imagerect.horizontalCenter
                    }
                }
                MouseArea
                {
                    anchors.fill: frame
                    onClicked: {manager.setStringParam(4, id);
                        console.log("clicked "+id)
                        // mouselocX = mouseX; mouselocY = mouseY
                    }

                }
            }
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
                        height: 100;
                        width: 156;
                        fillMode: Image.PreserveAspectFit;
                    }

                    Column {
                        width: background.width - imagerect.width - 20;
                        height: imagerect.height
                        spacing:5
                        StyledText {
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
                StyledText {
                    id:page_label2
                    text: index
                    font.pixelSize: scaleY(3.5)
                    anchors.centerIn: parent
                    color:index===manager.media_currentPage ? "green" : "slategrey"
                    font.bold: true
                }

                MouseArea{
                    anchors.fill: parent
                    onReleased: {  page_label.font.italic = true ;
                        manager.requestPage(index);
                    }
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
                height: scaleY(skinStyle.iconHeight)
                width: scaleX(skinStyle.iconWidth)
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

