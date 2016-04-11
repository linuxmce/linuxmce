import QtQuick 2.2

Item{
    id:metadataRoot

    Column{
        id:metadata
        spacing: 5
        width: Style.listViewWidth_large
        height: parent.height
        anchors.right: content_item.right
        StyledText{
            width: parent.width
            id:title
            text:qsTr("Title: ")+filedetailsclass.mediatitle
            fontSize: largeFontSize
        }
        StyledText{
            width: parent.width
            id:album
            text:qsTr("Album: ")+filedetailsclass.album
            visible:manager.q_mediaType == "4"
        }
        StyledText{
            width: parent.width
            id:director
            text:qsTr("Director: ")+filedetailsclass.director
        }
        StyledText{
            width: parent.width
            id:compwriter
            text:qsTr("Composer/Writer: ")+filedetailsclass.composerlist
        }
        StyledText{
            width: parent.width
            height:manager.q_mediaType == 5 ? 0 : parent.height *.15
            id:artists
            text:manager.q_mediaType == 5 ? "" : qsTr("Artist: ")+filedetailsclass.performerlist
        }
        StyledText{
            width: parent.width
            id:program
            text:qsTr("Program: ")+filedetailsclass.program
        }
        StyledText{
            width: parent.width
            id:episode
            text:qsTr("Episode: ")+filedetailsclass.episode
        }
        StyledText{
            width: parent.width
            id:description
            text:qsTr("Synopsis:\n")+filedetailsclass.synop

        }
        StyledText{
            width: parent.width
            id:studio
            text:qsTr("Studio: ")+filedetailsclass.studio
        }
        StyledText{
            width: parent.width
            id:release
            text:qsTr("Filename: ")+filedetailsclass.filename
        }

    }


}

