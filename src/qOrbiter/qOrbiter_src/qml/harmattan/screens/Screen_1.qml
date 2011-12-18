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
    Menu {
        id: roomMenu;
        visualParent: pageStack;
        MenuLayout {
            Repeater {
                model: roomList;
                MenuItem {
                    text: title;
                    onClicked: {
                        setActiveRoom(intRoom,entertainArea);
                    }

                }
            }
        }
    }

    ListModel {
        id: pagesModel;
        ListElement {
            visible: false;
            page: "";
            title: "";
        }
        ListElement {
            page: "Screen_2.qml";
            title: "Lights";
        }
        ListElement {
            page: "Screen_3.qml";
            title: "Media";
        }
        ListElement {
            page: "Screen_4.qml";
            title: "Climate";
        }
        ListElement {
            page: "Screen_5.qml";
            title: "Security";
        }
        ListElement {
            page: "Screen_6.qml";
            title: "Telecom";
        }
    }

    WorkerScript
    {
        id: nowPlayingWorker;
        source: "../js/nowPlayingWorker.js";
    }

    Timer
    {
        interval: 1000;
        repeat: true;
        triggeredOnStart: true;

        onTriggered: {
            var msg = {'action':'updateNowPlaying','item':pagesModel,'dcenowplaying':dcenowplaying};
            nowPlayingWorker.sendMessage(msg);
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
