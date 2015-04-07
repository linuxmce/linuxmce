import QtQuick 1.0
Item {
    id: filedetailrect
    height: parent.height
    width: parent.width
    anchors.top: parent.top
  // property int bgImageProp:manager.q_subType ===("1"||"13") ? 43 : manager.q_attributetype_sort===53 ? 43 :36
    MouseArea{
        anchors.fill: filedetailrect
        hoverEnabled: true
    }
    Image {
        id: fdbg
        source: "../img/icons/nowplaying.png"
        anchors.fill: filedetailrect
    }


    Connections{
        target:filedetailsclass
        onImageChanged:filedetailsimage.source = "http://"+m_ipAddress+"/lmce-admin/MediaImage.php?img="+filedetailsclass.screenshot
    }

    Rectangle{
        id:bg_mask
        anchors.fill: parent
        color:"black"
        opacity:.65
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:"darkgreen"
        radius:2.5
        StyledText {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: filedetailsclass.filename
            font.pixelSize: 14
            font.bold: true
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
            width: filedetailsclass.aspect=="wide"? scaleX(30) : scaleX(23)
            height:filedetailsclass.aspect=="wide"?scaleY(40) : scaleY(55)
           source:"http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=imdb&file="+filedetailsclass.file+"&val="+13
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

        width:  parent.width *.40
        height: childrenRect.height
        radius: 2.5
        clip:  true
        color: "transparent"

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
              font.pixelSize: scaleY(4)
                color:"white"
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: rectangle1.width *.95
            }

            StyledText {
                id: programtext
                width: scaleX(35)
                text: qsTr("Album: ") + filedetailsclass.album
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                smooth: true
                font.pixelSize: scaleY(4)
                visible:  filedetailsclass.album =="" ? false: true
            }

            StyledText {
                id: episode
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Track: ") + filedetailsclass.track
                smooth: true
                font.pixelSize: scaleY(4)
                  color:"white"
                visible:  filedetailsclass.track =="" ? false: true
            }

            StyledText {
                id: genre
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"

                smooth: true
                isBold: false
                font.pixelSize: scaleY(4)
                  color:"white"
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
                text: qsTr("Released: ") + dcenowplaying.releasedate

                smooth: true
                font.pixelSize: scaleY(4)
                  color:"white"
                visible:  filedetailsclass.releasedate ==="" ? false: true

            }


            StyledText {
                id: starring
                width: scaleX(35)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: qsTr("Perfomers: ") + filedetailsclass.performerlist

                //  font.bold: true
                isBold: false
                smooth: true
                font.pixelSize: scaleY(4)
                  color:"white"
                elide: "ElideRight"
                visible:  filedetailsclass.performerlist =="" ? false: true

                MouseArea{
                    anchors.fill: starring
                    hoverEnabled: true
                    onEntered: { starring.elide = "ElideNone" ; }
                    onExited: {starring.elide = "ElideRight"; }
                }
            }
        }
    }


    Row{
        id:options
        anchors.bottom: parent.bottom
        height: childrenRect.height
        width: parent.width

        StyledButton{
            buttonText: "Play"
            hitArea.onReleased: manager.playMedia(filedetailsclass.file)
        }

        StyledButton{
            buttonText: "Close"
            hitArea.onReleased: {content.state="viewing"; filedetailsclass.clear()}
        }
    }
}






