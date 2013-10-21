import QtQuick 1.1


Item {
    id: filedetailrect
    anchors{
        left:parent.left
        right:parent.right
        top:parent.top
    }

    state: "closed"

    clip: true       
    property int bgImageProp:manager.q_subType ==="1" ? 43 : manager.q_attributetype_sort===53 ? 43 :36

    Timer{
        id:poptimer
        interval: 1500
        onTriggered: filedetailrect.state="open"
        running:true
    }

    Behavior on height{
        PropertyAnimation{
            target: filedetailrect
            property: "height"
            duration: 1000
            easing.type: Easing.OutElastic
            easing.amplitude: .6
        }
    }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

    }

    Image {
        id: fdbg
        source: "../img/icons/nowplaying.png"
        anchors.fill: filedetailrect
        MouseArea{
            anchors.fill: filedetailrect

        }
    }

    Image{
        id:imdb
        anchors.fill: parent
        source:"http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=atr&file="+filedetailsclass.file+"&val="+bgImageProp
       // onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
    }   

    FileDetailsHeader {
        id: titlerect
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
            property bool profile : filedetailsimage.sourceSize.height > filedetailsimage.sourceSize.width ? true : false
            width:profile ? scaleX(25) : scaleX(45)
            height:profile ? scaleY(65) : scaleY(58)
           source:filedetailsclass.screenshot !=="" ? "http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=img&val="+filedetailsclass.screenshot : ""
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
        anchors.top: imageholder.top
        width:  filedetailsimage.profile ? parent.width * .70 :  parent.width *.40
        height: childrenRect.height
        radius: 2.5
        clip:  false
        color:"transparent"
        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(1)

        Rectangle{
        id:fill
        color: "black"
        anchors.fill: parent
        opacity: .65
        }
            Column
            {
                spacing:5
                anchors.margins: scaleY(1)
                width: parent.width
                height: childrenRect.height
                StyledText {
                    id: fnametext
                    text: "Title: " + filedetailsclass.mediatitle
                    font.pixelSize: scaleY(5)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                StyledText {
                    id: programtext
                    width: scaleX(35)
                    text: qsTr("Program: ") + filedetailsclass.program
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.pixelSize: scaleY(4)
                    visible:  filedetailsclass.program =="" ? false: true
                }

                StyledText {
                    id: episode
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Episode: ") + filedetailsclass.episode
                    smooth: true
                    font.pixelSize: scaleY(4)
                    visible:  filedetailsclass.episode =="" ? false: true
                }

                StyledText {
                    id: genre
                    width: parent.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Genre(s): ") + filedetailsclass.genre                   
                    smooth: true
                    font.pixelSize: scaleY(4)
                    visible:  filedetailsclass.genre =="" ? false: true

                }
//                StyledText {
//                    id: released
//                    width: scaleX(35)
//                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
//                    text: qsTr("Released: ") + filedetailsclass.

//                    // font.bold: true
//                    smooth: true
//                    font.pixelSize: scaleY(4)
//                    visible:  filedetailsclass.program ==="" ? false: true
//                }


                StyledText {
                    id: starring
                    width: parent.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Perfomers: ") + filedetailsclass.performerlist
                    smooth: true
                    font.pixelSize: scaleY(4)
                    elide: "ElideRight"
                    visible:  filedetailsclass.performerlist =="" ? false: true

                }
                StyledText {
                    id: studiotext
                    width: parent.width *.95
                    text: qsTr("Program: ") + filedetailsclass.studio
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.pixelSize: scaleY(3)
                    visible:  filedetailsclass.studio =="" ? false: true
                }

                StyledText{
                    id:rating
                    text: filedetailsclass.rating
                    visible:  filedetailsclass.rating ==="" ? false: true
                }

                StyledText {
                    id: synopsis
                    width: parent.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text:  filedetailsclass.synop
                    smooth: true
                    font.pixelSize: scaleY(3)
                   // elide: "ElideRight"
                    visible:  filedetailsclass.synop =="" ? false: true
                }
            }
    }


    FileDetailsActions {
        id: controlrow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: rectangle1.horizontalCenter
        anchors.baselineOffset: scaleY(5)
        width: rectangle1.width
    }

    states: [
        State {
            name: "open"
            PropertyChanges{
                target: filedetailrect
                anchors.top: filedetailrect.parent.top
                anchors.bottom: filedetailrect.parent.bottom
            }
        },
        State {
            name: "closed"
            AnchorChanges{
             target: filedetailrect
             anchors.top: filedetailrect.parent.verticalCenter
             anchors.bottom: filedetailrect.parent.verticaCenter
            }
        }
    ]

}






