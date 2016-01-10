import QtQuick 1.0


Item {
    id: filedetailrect
    width: scaleX(100)
    height: scaleY(100)
    anchors.centerIn: parent

    Rectangle{
        id:phil
        anchors.fill: parent
        radius: 5
        border.color: style.highlight1
        border.width: 3
        color: "darkred"
    }

    clip: true

    property int bgImageProp:manager.q_subType ==="1" ? 43 : manager.q_attributetype_sort===53 ? 43 : manager.q_subType === "13" ? 13: 36

    Behavior on height{
        PropertyAnimation{
            target: filedetailrect
            property: "height"
            duration: 1000
            easing.type: Easing.OutElastic
            easing.amplitude: .6
        }
    }


    Image{
        id:imdb
        anchors.fill: parent
        source:"http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=atr&file="+filedetailsclass.file+"&val="+bgImageProp
        onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
    }
            Image {
                id: filedetailsimage
                property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
             anchors.fill: parent
                source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
                smooth: true
                visible:imdb.status===Image.Error ? true : false
            }

    //    Connections{
    //        target:filedetailsclass
    //        onImageChanged:filedetailsimage.source = "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?file="+filedetailsclass.file+"&prop="+bgImageProp
    //    }

    Item{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width

        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .65
        }

        StyledText {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Filename")+filedetailsclass.path+ "/"+filedetailsclass.filename
            fontSize: paraText
            font.bold: true
            color: "white"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

    }

//    Item{
//        id:imageholder
//        height:childrenRect.height
//        width:childrenRect.width

//        anchors.verticalCenter: parent.verticalCenter
//        anchors.left: parent.left
//        anchors.leftMargin: scaleX(2)
//        BorderImage {
//            id: borderimg
//            horizontalTileMode: BorderImage.Repeat
//            source: "../img/icons/drpshadow.png"
//            anchors.fill: filedetailsimage
//            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
//            border { left: 10; top: 10; right: 10; bottom: 10 }
//            smooth: true
//        }
//        Image {
//            id: filedetailsimage
//            property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
//            width:profile ? scaleX(25) : scaleX(45)
//            height:profile ? scaleY(65) : scaleY(58)
//            source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?type=img&val="+filedetailsclass.screenshot : ""
//            smooth: true
//        }

//        Image {
//            id: npmask
//            source: "../img/icons/transparencymask.png"
//            anchors.fill: filedetailsimage
//            opacity: .5

//        }
//    }
    Rectangle{
        anchors.fill: rectangle1
        color: "black"
        opacity: .65
    }

    Flickable {
        id: rectangle1
        anchors.verticalCenter: parent.verticalCenter
        anchors.top: titlerect.bottom
        width:  parent.width
        height: scaleY(75)
        anchors.left: parent.left
        anchors.leftMargin: scaleX(1)
        contentHeight: infoCol.height
        contentWidth: width
        clip:true


        Column
        {
        id:infoCol
            spacing:5
            anchors.margins: scaleY(1)
            width: parent.width*.95
            height: childrenRect.height
            StyledText {
                id: fnametext
                text: filedetailsclass.mediatitle
                fontSize: headerText
                color:"white"
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: parent.width
            }



            StyledText {
                id: programtext
                width:parent.width
                text:  filedetailsclass.program
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
               fontSize: headerText
                visible:  filedetailsclass.program =="" ? false: true
                color:"white"
            }

            StyledText {
                id: episode
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Episode: ") + filedetailsclass.episode
                smooth: true
                fontSize: paraText
                visible:  filedetailsclass.episode =="" ? false: true
                color:"white"
            }

            StyledText {
                id: genre
                width:parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Genre(s): ") + filedetailsclass.genre
                //font.bold: true
                smooth: true
               fontSize: paraText
                visible:  filedetailsclass.genre =="" ? false: true
                color:"white"

            }
            StyledText {
                id: released
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Released: ") + dcenowplaying.releasedate
                // font.bold: true
                smooth: true
                fontSize:paraText
                visible:  filedetailsclass.releasedate ==="" ? false: true
                color:"white"
            }


            StyledText {
                id: starring
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Perfomers: ") + filedetailsclass.performerlist
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
                fontSize: paraText
                visible:  filedetailsclass.studio =="" ? false: true
                color:"white"
            }

            StyledText{
                id:rating
                text: filedetailsclass.rating
                visible:  filedetailsclass.rating ==="" ? false: true
                color:"white"
                font.pixelSize:headerText
                font.weight: Font.DemiBold
            }

            StyledText {
                id: synopsis
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text:  filedetailsclass.synop
                smooth: true
                font.pixelSize: paraText
                // elide: "ElideRight"
                visible:  filedetailsclass.synop =="" ? false: true
                color:"white"

            }
            StyledText {
                id: storageDeviceText
                width: scaleX(35)
                text: qsTr("Located on Storage Device: ") + filedetailsclass.qs_storageDevice
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
                fontSize: paraText
                color:"white"
            }
        }
    }


    FileDetailsButtons {
        id: controlrow
    }


}






