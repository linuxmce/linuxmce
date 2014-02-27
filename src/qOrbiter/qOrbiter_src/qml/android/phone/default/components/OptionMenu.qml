import QtQuick 1.1
import "../components"

Item{
    id: optionMenu
    property int localButtonH:scaleY(10)
    property variant menuItems

    height: (buttons.count)*localButtonH
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }

    Rectangle{
        anchors.fill: btnDisplay
        color: style.stageBG
    }

    ListView{
        id: menuView
	model: menuItems
        height:advancedrow.height
	delegate:         StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            useBorder: false
            buttonText: text
            onActivated:  menuView.model.onClick(index)
        }

    }



}
