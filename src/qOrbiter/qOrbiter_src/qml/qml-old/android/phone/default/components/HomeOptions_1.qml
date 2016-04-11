import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(15)
    height: (buttons.count)*localButtonH
    anchors.fill: parent
    ListModel {
        id:buttons
        ListElement { text: "Exit" }
        ListElement { text: "Sleeping Alarms" }
        // Cannot bind script to property (at least no until qt5), so we fake it till then

        function onClick(index) {
            showOptions = !showOptions;
            switch (index) {
            case 0:
                closeOrbiter();
                break;
            case 1:
                manager.setCurrentScreen("Screen_29.qml");
                break;
            }
        }
    }
    OptionMenu {
        menuItems: buttons
    }
}
