import QtQuick 2.2

Item{
    id: optionMenu
    property int localButtonH:style.buttonHeight
    property variant menuItems
    property alias text: menuButton.buttonText
    property alias btHeight: menuButton.height
    property alias btWidth: menuButton.width
    property int menuWidth : scaleX(30)
    property string menuAlign : "left"
    height: manager.appHeight
    width: btWidth

    StyledButton {
        id: menuButton
        buttonText: "supsim"
        width:  scaleX(18)
        height: scaleY(15)
        txtObj.color: "antiquewhite"
        txtObj.isBold: false
        hitArea.onReleased: {
            menuView.visible = !menuView.visible
        }

    }

    Rectangle{
        anchors.fill: menuView
        visible: menuView.visible
        color: "antiquewhite"
        opacity: 0.5
        // used as border between items
    }

    ListView{
        id: menuView
        model: menuItems
        visible: false
        height: localButtonH*menuView.model.count
        width: menuWidth
        spacing: 2
        interactive: false
        anchors.margins: 1
        anchors.top: menuButton.bottom
        anchors.right : menuAlign === "right" ? parent.right : undefined
        anchors.left : menuAlign === "right" ? undefined : parent.left
        delegate: MenuItem {
            height: localButtonH-2
            width: parent.width-2
            buttonText: qsTr(text)
            anchors.margins: 1
            anchors.right : menuAlign === "right" ? parent.right : undefined
            anchors.left : menuAlign === "right" ? undefined : parent.left
            onActivated: {
                menuView.visible = false
                menuView.model.onClick(name)
            }
        }
    }

}
