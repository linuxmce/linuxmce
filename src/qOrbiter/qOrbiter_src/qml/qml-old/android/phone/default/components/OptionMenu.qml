import QtQuick 1.1
import "../components"

Item{
    id: optionMenu
    property int localButtonH:scaleY(10)
    property variant menuItems
    height: manager.appHeight
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right

    }

    Rectangle{
        anchors.fill: menuView
        color: appstyle.button_system_color_hover // used as border between items
    }

    ListView{
        id: menuView
        model: menuItems
        height:advancedrow.height
        spacing: 2
        delegate: MenuItem {
            height: localButtonH
            width: advancedrow.width
            buttonText: qsTr(text)
            onActivated:  menuView.model.onClick(index)
        }

    }



}
