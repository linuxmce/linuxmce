import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(15)
    anchors{
        top:optionLoader.top
        left:optionLoader.left
        right:optionLoader.right
    }

    ListModel {
        id:buttons
        ListElement { text: "Home" }
        function onClick(index) {
            showOptions = !showOptions;
            switch (index) {
            case 0:
                manager.setCurrentScreen("Screen_1.qml");
                break;
            }
        }
    }
    OptionMenu {
        menuItems: buttons
    }


}
