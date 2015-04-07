import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
Page {

    property alias headerColor: headerRect.color;
    property string pageTitle;
    property alias model: scenarView.model;
    property bool itemsAreScreens;

    id: screenBase;
    tools: ToolBarLayout {
        id: commonTools;
        ToolIcon {
            id: showHome;
            onClicked: (itemsAreScreens == true ? console.log("dimmed home"):setCurrentScreen("Screen_1.qml"));
            iconSource: (itemsAreScreens == true ? "image://theme/icon-m-toolbar-home-dimmed":"image://theme/icon-m-toolbar-home");
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
        }
    }

    Component.onCompleted: {
        if (dcenowplaying.b_mediaPlaying == true)
        {
            if (model.get(model.count-1).title != "Now Playing")
            {
                model.append({"title":"Now Playing","subtitle":dcenowplaying.maintitle,"page":dcenowplaying.qs_screen});
            }
        }
            else
        {
            if (model.get(model.count-1).title == "Now Playing")
            {
                model.remove(model.count-1);
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
                if (scenarView.model.get(scenarView.model.count-1).title != "Now Playing")
                {
                    scenarView.model.append({"title":"Now Playing","subtitle":dcenowplaying.maintitle,"page":dcenowplaying.qs_screen});
                }
            }
                else
            {
                if (scenarView.model.get(scenarView.model.count-1).title == "Now Playing")
                {
                    scenarView.model.remove(scenarView.model.count-1);
                }
            }
        }
    }

    Rectangle {
        id: contentArea;
        width: parent.width;
        height: parent.height-72;
        anchors.top: headerRect.bottom;
        anchors.leftMargin: UiConstants.DefaultMargin;
        anchors.rightMargin: UiConstants.DefaultMargin;
        ListView {
            id: scenarView;
            anchors.fill: contentArea;
            model: pagesModel;

            delegate: ListDelegate {
                id: listDelegate;
                x: 16;
                width: parent.width - 16;
                Image {
                    id: listImage;
                    source: "image://theme/icon-m-common-drilldown-arrow" + (theme.inverted ? "-inverse" : "");
                    anchors.right: parent.right;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.leftMargin: UiConstants.DefaultMargin;
                    anchors.rightMargin: UiConstants.DefaultMargin;
                }
                onClicked: {
                    if (itemsAreScreens == true)
                    {
                        setCurrentScreen(page);
                    }
                    else
                    {
                        execGrp(params);
                    }
                }
            }
        }
        Text {
            id: noScenariosAvailable;
            visible: false;
            anchors.centerIn: parent;
            text: "No Scenarios Available";
            font.pointSize: 48;
            opacity: 0.25;
            width: parent.width;
            wrapMode: Text.WordWrap;
            horizontalAlignment: Text.AlignHCenter;
        }
        ScrollDecorator {
            flickableItem: scenarView;
        }

    }

    Rectangle {
        id: headerRect;
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        width: parent.width;
        height: 72;
        color: "#6300bb";
        Text {
            id: headerText;
            anchors.left: headerRect.left;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.leftMargin: UiConstants.DefaultMargin;
            font.pointSize: 20;
            color: "white";
            text: pageTitle + " ("+currentroom+")";
        }
        Image {
            id: headerMenu;
            anchors.right: headerRect.right;
            anchors.verticalCenter: headerRect.verticalCenter;
            anchors.rightMargin: UiConstants.DefaultMargin;
            source: "image://theme/icon-m-textinput-combobox-arrow";
        }
        MouseArea {
            anchors.fill: headerRect;
            onClicked: roomMenu.open();
        }


    }

}
