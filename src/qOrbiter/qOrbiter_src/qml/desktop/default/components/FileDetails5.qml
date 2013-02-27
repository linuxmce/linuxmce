import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: scaleX(75)
    height: scaleY(75)
    anchors.centerIn: parent
    color:"transparent"
    clip: true    
    border.color: "white"
    border.width: 3
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
        source:"http://"+srouterip+"/lmce-admin/imdbImage.php?imdb="+filedetailsclass.file
    }

    Connections{
        target:filedetailsclass
        onImageChanged:filedetailsimage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
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
            width: filedetailsclass.aspect=="wide"? scaleX(35) : scaleX(25)
            height:filedetailsclass.aspect=="wide"?scaleY(35) : scaleY(58)
            source: ""
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
        width:  parent.width *.40
        height: childrenRect.height
        radius: 2.5
        clip:  false
        color:"transparent"
        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(1)

            Column
            {
                spacing:5
                anchors.margins: scaleY(1)
                width: parent.width
                height: childrenRect.height
                StyledText {
                    id: fnametext
                    text: "Title: " + filedetailsclass.mediatitle
                    font.pixelSize: scaleY(2)
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
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.program =="" ? false: true
                }

                StyledText {
                    id: episode
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Episode: ") + filedetailsclass.episode
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.episode =="" ? false: true
                }

                StyledText {
                    id: genre
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Genre(s): ") + filedetailsclass.genre                   
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.genre =="" ? false: true
                    MouseArea{
                        anchors.fill: genre
                        hoverEnabled: true
                        onEntered: { genre.elide = "ElideNone" ; }
                        onExited: {genre.elide = "ElideRight"; }
                    }
                }
                StyledText {
                    id: released
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Released: ") + filedetailsclass.getProgram()

                    // font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.program ==="" ? false: true
                }


                StyledText {
                    id: starring
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Perfomers: ") + filedetailsclass.performerlist
                    smooth: true
                    font.pixelSize: scaleY(2)
                    elide: "ElideRight"
                    visible:  filedetailsclass.performerlist =="" ? false: true

                    MouseArea{
                        anchors.fill: starring
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
                    }
                }
                StyledText {
                    id: studiotext
                    width: scaleX(35)
                    text: qsTr("Program: ") + filedetailsclass.studio

                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.studio =="" ? false: true
                }

                StyledText{
                    id:rating
                    text: filedetailsclass.rating
                    visible:  filedetailsclass.rating ==="" ? false: true
                }

                StyledText {
                    id: synopsis
                    width: scaleX(40)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text:  filedetailsclass.synop
                    smooth: true
                    font.pixelSize: scaleY(2)
                   // elide: "ElideRight"
                    visible:  filedetailsclass.synop =="" ? false: true

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
                    }
                }
            }
    }


    FileDetailsActions {
        id: controlrow
        anchors.top: rectangle1.bottom
        anchors.topMargin:  scaleY(3)
        anchors.horizontalCenter: rectangle1.horizontalCenter
        width: rectangle1.width
    }


}






