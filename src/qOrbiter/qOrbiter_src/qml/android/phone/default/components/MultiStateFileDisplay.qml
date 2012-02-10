import QtQuick 1.0


Rectangle {
    id:gridholder
    width: 320
    height: 480
    clip: true
    anchors.top: parent.top
    anchors.topMargin: scaleY(5)
    anchors.horizontalCenter: parent.horizontalCenter
    color: "transparent"

    ListView {
        id: list_view1
        width: 310
        height: 450
        model:dataModel
        delegate: contactDelegateList
        highlightFollowsCurrentItem: true
        highlight: appHighlight
        focus: true
        clip: true

    }


}
