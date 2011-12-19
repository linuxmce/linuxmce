import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import Qt.labs.gestures 1.0
import "../components"
import "../js"

Page {
    id: screen_70;
    width: 480
    height: 854

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: pageStack.pop();
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
            iconId: "toolbar-menu"
        }

    }

    state: (screen.currentOrientation == Screen.Portrait ? "portrait" : "landscape")
    states: [
        State {
            name: "landscape";
            PropertyChanges {
                target: nowplayingimage;
                width: 427;
                height: 480;
            }

        },
        State {
            name: "portrait";
            PropertyChanges {
                target: nowplayingimage;
                width: 480;
                height: 427;
            }
        }
    ]

    Rectangle {
        id: gestureTangle;
        width: 480;
        height: 427;
        Image {
            id: nowplayingimage
            source: "image://listprovider/updateobject/"+securityvideo.timestamp;
            anchors.fill: parent;
            width: 480;
            height: 427;
            sourceSize.width: 480;
            sourceSize.height: 427;
            fillMode: Image.PreserveAspectCrop;
        }

        SwipeArea {
            anchors.fill: parent;
            onSwipeRight: console.log("Swipe Right");
            onSwipeLeft: console.log("swipe LEFT!!")
        }
    }

    Rectangle {
        id: nowplayingTitle;
        width: 480;
        anchors.top: gestureTangle.bottom;
        height: 24;
        anchors.topMargin: UiConstants.DefaultMargin;
        anchors.leftMargin: UiConstants.DefaultMargin;
        color: "black";
        Text {
            text: dcenowplaying.qs_mainTitle;
            font.weight: Font.Bold;
            font.pointSize: 16;
            color: "white";
        }
    }

    Rectangle {
        id: nowplayingsubtitleBox;
        width: 480;
        anchors.top: nowplayingTitle.bottom;
        color: "black";
        height: 48;
        Text {
            text: dcenowplaying.qs_subTitle;
            font.pointSize: 16;
            color: "white";
        }
    }

    ButtonStyle {
        id: mediaButtonsL;
        background: "";
        checkedBackground: "";
        checkedDisabledBackground: "";
        disabledBackground: "";
        pressedBackground: "";
        position: "horizontal-left";
    }

    ButtonStyle {
        id: mediaButtonsM;
        background: "";
        checkedBackground: "";
        checkedDisabledBackground: "";
        disabledBackground: "";
        pressedBackground: "";
        position: "horizontal-center";
    }

    ButtonStyle {
        id: mediaButtonsR;
        background: "";
        checkedBackground: "";
        checkedDisabledBackground: "";
        disabledBackground: "";
        pressedBackground: "";
        position: "horizontal-right"
    }

    Row {
        anchors.top: nowplayingsubtitleBox.bottom;
        anchors.topMargin: UiConstants.DefaultMargin;
        width: 480;
        height: 30

        Button { width: 480/3; platformStyle: mediaButtonsL; iconSource: "image://theme/icon-m-lockscreen-mediacontrol-previous"; }
        Button { width: 480/3; platformStyle: mediaButtonsM; iconSource: "image://theme/icon-m-lockscreen-mediacontrol-play"; }
        Button { width: 480/3; platformStyle: mediaButtonsR; iconSource: "image://theme/icon-m-lockscreen-mediacontrol-next"; }
    }

    Timer {
        id: singleshot;
        repeat: false;
        interval: 2000;
        triggeredOnStart: false;
        running: true;
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
    }

    Connections {
        target: dcenowplaying;
        onPlayListPositionChanged: { nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp; }
    }

    Component.onCompleted: setNowPlayingData()


}
