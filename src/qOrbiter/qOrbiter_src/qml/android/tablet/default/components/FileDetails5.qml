import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: scaleX(100)
    height: 0
    anchors.centerIn: parent
    color: style.highlight2
    clip: true
    radius: 5
    border.color: style.highlight1
    border.width: 3
    property int bgImageProp:manager.q_subType ==="1" ? 43 : manager.q_attributetype_sort===53 ? 43 :36

    Behavior on height{
        PropertyAnimation{
            target: filedetailrect
            property: "height"
            duration: 1000
            easing.type: Easing.OutElastic
            easing.amplitude: .6
        }
    }

    Image {
        id: fdbg
        source: "../img/icons/nowplaying.png"
        anchors.fill: filedetailrect

    }
    Image{
        id:imdb
        anchors.fill: parent
        source:"http://"+m_ipAddress+"/lmce-admin/imdbImage.php?file="+filedetailsclass.file+"&prop="+bgImageProp
        onStatusChanged:  filedetailrect.height = scaleY(100) 
    }

    //    Connections{
    //        target:filedetailsclass
    //        onImageChanged:filedetailsimage.source = "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?file="+filedetailsclass.file+"&prop="+bgImageProp
    //    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:"transparent"
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .65
        }

        radius:2.5
        Text {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Filename")+filedetailsclass.path+ "/"+filedetailsclass.filename
            font.pixelSize: 16
            font.bold: true
            color: "black"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

    }

    Rectangle{
        id:imageholder
        height:childrenRect.height
        width:childrenRect.width
        color: "transparent"

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: scaleX(2)
        BorderImage {
            id: borderimg
            horizontalTileMode: BorderImage.Repeat
            source: "../img/icons/drpshadow.png"
            anchors.fill: filedetailsimage
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            smooth: true
        }
        Image {
            id: filedetailsimage
            property bool poster : sourceSize.height < sourceSize.width ? true : false
            width: poster ? scaleX(35) : scaleX(25)
            height:poster ? scaleY(40) : scaleY(65)
            source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?img="+filedetailsclass.screenshot : ""
            smooth: true
        }

        Image {
            id: npmask
            source: "../img/icons/transparencymask.png"
            anchors.fill: filedetailsimage
            opacity: .5

        }
    }


    Rectangle {
        id: rectangle1
        anchors.verticalCenter: imageholder.verticalCenter

        width:  parent.width  - imageholder.width
        height: childrenRect.height
        radius:2.5
        clip:  true
        color:"transparent"

        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(1)
        Rectangle{
            anchors.fill: parent
            color: "black"
            opacity: .65
        }

        Column
        {
            spacing:5
            anchors.margins: scaleY(1)
            width: parent.width*.95
            height: childrenRect.height
            StyledText {
                id: fnametext
                text: "Title: " + filedetailsclass.mediatitle
                font.pixelSize: scaleY(4)
                color:"white"
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: parent.width
            }

            StyledText {
                id: storageDeviceText
                width: scaleX(35)
                text: qsTr("Located on Storage Device: ") + filedetailsclass.qs_storageDevice
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
                font.pixelSize: scaleY(3)
                color:"white"
            }

            StyledText {
                id: programtext
                width:parent.width
                text: qsTr("Album: ") + filedetailsclass.album
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
                font.pixelSize: scaleY(3)
                visible:  filedetailsclass.album =="" ? false: true
                color:"white"
            }

            StyledText {
                id: episode
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Track: ") + filedetailsclass.track
                smooth: true
                font.pixelSize: scaleY(3)
                visible:  filedetailsclass.track =="" ? false: true
                color:"white"
            }

            StyledText {
                id: genre
                width:parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Genre(s): ") + filedetailsclass.genre
                //font.bold: true
                smooth: true
                font.pixelSize: scaleY(3)
                visible:  filedetailsclass.genre =="" ? false: true
                color:"white"

            }
            StyledText {
                id: released
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Released: ") + dcenowplaying.releasedate
                font.family: "Droid Sans"
                // font.bold: true
                smooth: true
                font.pixelSize: scaleY(3)
                visible:  filedetailsclass.releasedate ==="" ? false: true
                color:"white"
            }


            StyledText {
                id: starring
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Perfomers: ") + filedetailsclass.performerlist

                //  font.bold: true
                smooth: true
                font.pixelSize: scaleY(3)
                elide: "ElideRight"
                visible:  filedetailsclass.performerlist =="" ? false: true
                color:"white"
            }

            StyledText {
                id: studiotext
                width: scaleX(35)
                text: qsTr("Program: ") + filedetailsclass.studio

                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
                font.pixelSize: scaleY(2)
                visible:  filedetailsclass.studio =="" ? false: true
                color:"white"
            }

            StyledText{
                id:rating
                text: filedetailsclass.rating
                visible:  filedetailsclass.rating ==="" ? false: true
                color:"white"
                font.pixelSize: scaleY(3)
            }

            StyledText {
                id: synopsis
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text:  filedetailsclass.synop
                smooth: true
                font.pixelSize: scaleY(3)
               // elide: "ElideRight"
                visible:  filedetailsclass.synop =="" ? false: true
                color:"white"

            }
        }
    }


    FileDetailsButtons {
        id: controlrow
    }


}






