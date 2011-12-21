import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components"

Page {
    id: screen_1
    tools:
        ToolBarLayout {
           ToolIcon  {
            iconId: "toolbar-view-menu";
        }
    }

    Component.onCompleted: {
        if (dcenowplaying.b_mediaPlaying == true)
        {
            if (pagesModel.get(pagesModel.count-1).title != "Now Playing")
            {
                pagesModel.append({"title":"Now Playing","subtitle":dcenowplaying.maintitle,"page":dcenowplaying.qs_screen});
            }
        }
            else
        {
            if (pagesModel.get(pagesModel.count-1).title == "Now Playing")
            {
                pagesModel.remove(pagesModel.count-1);
            }
        }
    }

    Menu {
        id: roomMenu;
        visualParent: pageStack;
        MenuLayout {
            Repeater {
                model: roomList;
                MenuItem {
                    text: title;
                    onClicked: {
                        setActiveRoom(intRoom,entertain_area);
                    }

                }
            }
        }
    }

    Connections {
        target: dcenowplaying;
        onMediaStatusChanged: {
            if (dcenowplaying.b_mediaPlaying == true)
            {
                if (pagesModel.get(pagesModel.count-1).title != "Now Playing")
                {
                    pagesModel.append({"title":"Now Playing","subtitle":dcenowplaying.maintitle,"page":dcenowplaying.qs_screen});
                }
            }
                else
            {
                if (pagesModel.get(pagesModel.count-1).title == "Now Playing")
                {
                    pagesModel.remove(pagesModel.count-1);
                }
            }
        }
    }

    ListModel {
        id: pagesModel;
        ListElement {
            title: "Lighting";
            page: "Screen_2.qml";
        }
        ListElement {
            title: "Media";
            page: "Screen_3.qml";
        }
        ListElement {
            title: "Climate";
            page: "Screen_4.qml";
        }
        ListElement {
            title: "Telecom";
            page: "Screen_5.qml";
        }
        ListElement {
            title: "Security";
            page: "Screen_6.qml";
        }
    }

    Rectangle {
        id: contentArea;
        width: screen_1.width;
        height: screen_1.height-72;
        anchors.top: headerRect.bottom;
        anchors.leftMargin: UiConstants.DefaultMargin;
        anchors.rightMargin: UiConstants.DefaultMargin;
        ListView {
            id: scenarView;
            anchors.fill: contentArea;
            model: pagesModel;

            delegate: ListDelegate {
                id: listDelegate;
                anchors.leftMargin: UiConstants.DefaultMargin;
                Image {
                    id: listImage;
                    source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "");
                    anchors.right: parent.right;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.leftMargin: UiConstants.DefaultMargin;
                    anchors.rightMargin: UiConstants.DefaultMargin;
                }
                onClicked: appWindow.screenchange(page);
            }

        }

        ScrollDecorator {
            flickableItem: scenarView;
        }

    }

    Rectangle {
        id: headerRect;
        anchors.top: screen_1.top;
        anchors.left: screen_1.left;
        anchors.right: screen_1.right;
        width: screen_1.width;
        height: 72;
        color: "#6300bb";
        Text {
            id: headerText;
            anchors.left: headerRect.left;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.leftMargin: UiConstants.DefaultMargin;
            font.pointSize: 20;
            color: "white";
            text: qsTr("Home");
        }
        Image {
            id: headerMenu;
            anchors.right: headerRect.right;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.rightMargin: UiConstants.DefaultMargin;
            source: "image://theme/icon-m-toolbar-view-menu-white";
        }
        MouseArea {
            anchors.fill: headerRect;
            onClicked: roomMenu.open();
        }


    }

}
