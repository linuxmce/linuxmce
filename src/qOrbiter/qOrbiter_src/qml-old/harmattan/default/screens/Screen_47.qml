import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

Page {
    id: screen_47;
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-view-home";
            onClicked: setCurrentScreen("Screen_1.qml");
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            iconId: "toolbar-view-menu";
            onClicked: console.log("Menu clicked.");
        }
    }
    Rectangle {
        id: contentArea;
        width: parent.width;
        height: parent.height-72;
        anchors.top: headerRect.bottom;
        anchors.leftMargin: UiConstants.DefaultMargin;
        anchors.rightMargin: UiConstants.DefaultMargin;
        GridView {
            id: mediaView;
            anchors.fill: parent;
            model: dataModel;
            focus: true;
            cellWidth: 160;
            cellHeight: 160;
            width: parent.width;
            height: parent.height;
            delegate: MediaTile { }
        }

        ScrollDecorator {
            flickableItem: mediaView;
        }

    }


    Rectangle {
        id: headerRect;
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        width: parent.width;
        height: 72;
        color: "black";
        Text {
            id: headerText;
            anchors.left: headerRect.left;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.leftMargin: UiConstants.DefaultMargin;
            font.pointSize: 20;
            color: "white";
            text: "Media List";
        }
        Image {
            id: headerMenu;
            anchors.right: headerRect.right;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.rightMargin: UiConstants.DefaultMargin;
            source: "image://theme/icon-m-toolbar-search-white";
        }
        MouseArea {
            anchors.fill: headerRect;
            onClicked: roomMenu.open();
        }
    }

    Connections {
        target: filedetailsclass;
        onShowDetailsChanged: screenchange("Screen_47_details.qml");
    }

}
