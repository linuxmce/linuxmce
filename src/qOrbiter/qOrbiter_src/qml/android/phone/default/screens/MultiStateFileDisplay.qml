import QtQuick 1.0


Rectangle {
    id:gridholder
    width: 310
    height: 450
    color: "transparent"

    Item {
        id: contactDelegateList
        height: childrenRect.height
        width: childrenRect.width
        Rectangle{
            id:delegateItem
            width: 310
            height: 310
            Text {
                id:itemlabel
                text: name
            }

        }
    }


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
