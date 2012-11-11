import QtQuick 2.0

Rectangle {
    id:bookmarks_popup
    width: scaleX(75)
    height: scaleY(65)
    radius: 5
    color: "slategrey"
    anchors.centerIn: parent

    StyledText {
        id: bookmarkslabel
        text: qsTr("Media Bookmarks")
        font.pointSize: 20

    }

    Rectangle{
        id:bookmarksbox
        anchors.centerIn: bookmarks_popup
        color: "aliceblue"
        width: parent.width *.90
        height:parent.height * .75
  //===============
        ListView{
            id:bookmarks_list
            height: bookmarksbox.height *.85
            width: bookmarksbox.width
            clip: true
            anchors.centerIn: bookmarksbox
            model:currentBookmarks
            delegate: Rectangle
            {
            height:scaleY(20)
            width:parent.width
            color:"white"
            Row{
                width: parent.width
                height: parent.height
                spacing: scaleX(1)
                anchors.centerIn: parent
                Rectangle{
                    id:media_bookmark
                    height: scaleY(15)
                    width: parent.width *.30
                    color: "slategrey"
                    border.color: "white"
                    border.width: 1

                    Image {
                        id: media_bookmark_image
                        source: ""
                    }
                    StyledText {
                        id: media_bookmark_name
                        text: qsTr("Media Bookmark")
                        font.pointSize: scaleY(2)
                    }
                }

                Rectangle{
                    id:media_resume
                    height: scaleY(15)
                    width: parent.width *.30
                    color: "slategrey"
                    border.color: "white"
                    border.width: 1
                    Image {
                        id: media_resume_image
                        source: ""
                    }
                    StyledText {
                        id: media_resume_name
                        text: qsTr("Media Resume")
                        font.pointSize: scaleY(2)
                    }
                }

                Rectangle{
                    id:media_label
                    height: scaleY(15)
                    width: parent.width *.30
                    color: "slategrey"
                    border.color: "white"
                    border.width: 1

                    StyledText {
                        id: media_resume_label
                        text:title
                        font.pointSize: scaleY(2)
                    }
                }
            }
        }

    }
  //========================
}

ButtonSq
{
    height: scaleY(5)
    width: scaleX(5)
    buttontext: "Close"
    buttonsqradius: 10
    anchors.top: bookmarksbox.bottom
    MouseArea{
        anchors.fill: parent
        onClicked: {
            bookmarks_popup.destroy()
        }
    }
}
}
