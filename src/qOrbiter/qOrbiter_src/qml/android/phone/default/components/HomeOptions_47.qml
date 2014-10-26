import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(10)

    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }

    ListModel {
        id:buttons
	ListElement { text: "Home" }
	ListElement { text: "Play All" }
	ListElement { text: "Attribute\nSort" }
	ListElement { text: "Genre" }
	ListElement { text: "Sources" }
	ListElement { text: "Resolution" }
	ListElement { text: "Mediatypes" }
	ListElement { text: "Users" }

	function onClick(index) {
	    showOptions = !showOptions;
	    switch (index) {
		case 0:
        manager.setCurrentScreen("Screen_1.qml");
		break;
		case 1:
		manager.playMedia("!G"+iPK_Device);
		break;
		case 2:
		filterTarget.currentModel = attribfilter;
		break;
		case 3:
		filterTarget.currentModel = genrefilter;
		break;
		case 4:
		filterTarget.currentModel = mediasourcelist;
		break;
		case 5:
		filterTarget.currentModel = fileformatmodel;
		break;
		case 6:
		filterTarget.currentModel = mediatypefilter;
		break;
		case 7:
		filterTarget.currentModel = mediatypefilter;
		break;
	    }
	}
    }
    OptionMenu {
        anchors.top: parent.top
	menuItems: buttons
    }


}
