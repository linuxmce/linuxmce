import QtQuick 2.2

Rectangle {
    id:bookmarks
    width: scaleX(75)
    height: scaleY(65)
    radius: 5
    color: "slategrey"

    Text {
        id: bookmarkslabel
        text: qsTr("Media Bookmarks")
        font.pixelSize: 20

    }

    Rectangle{
        id:bookmarksbox
        anchors.centerIn: bookmarks
        color: "aliceblue"
        width: parent.width *.90
        height:parent.height * .75
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
                bookmarks.destroy()
            }
        }
    }
}
