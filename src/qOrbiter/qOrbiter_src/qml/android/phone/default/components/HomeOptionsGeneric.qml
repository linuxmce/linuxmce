import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(15)
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }

    ListModel {
        id:buttons
	ListElement { text: "Home" }
	function onClick(index) {
	    showOptions = !showOptions;
	    switch (index) {
		case 0:
		manager.gotoQScreen("Screen_1.qml");
		break;
	    }
	}
    }
    OptionMenu {
	menuItems: buttons
    }


}
