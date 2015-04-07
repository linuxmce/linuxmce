import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import Qt.labs.gestures 1.0
import "../components"
import "../js"
import "../js/timeCodeHelpers.js" as TimeCodeHelpers

Page {
    id: screen_70;
    width: 480
    height: 854

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-home";
            onClicked: setCurrentScreen("Screen_1.qml");
        }
        ToolIcon {
            iconId: "toolbar-volume";
            onClicked: pageStack.pop();
        }
        ToolIcon {
            iconId: "icon-m-camera-high-brightness";
            onClicked: lightTangle.visible = true;
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
        }

    }

    Menu {
        id: playlistDialog;
        visualParent: pageStack;
        MenuLayout {
            Repeater {
                model: manager.getDataGridModel("Playlist", 18);
                MenuItem {
                    text: id;
                    onClicked: changedPlaylistPosition(name);
                }
            }
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
        color: "black";
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
        Rectangle {
            id: lightTangle;
            visible: false;
            width: 480;
            height: 427;
            color: "black";
            opacity: 0.7;
            Text {
                text: "Lights";
                font.pointSize: 48;
                color: "white";
            }
            MouseArea
            {
                id: lightsGestureArea;
                anchors.fill: parent;
                onPressed: {
                    newTimeText.visible = true;
                }
                onPositionChanged: {
                    var y = mouse.y;
                    var iScaleY = 100 / lightsGestureArea.height;
                    var iNewY = Math.floor(y * iScaleY);
                    adjustLighting(""+iNewY);
                }
                onReleased: {
                    var y = mouse.y;
                    var iScaleY = 100 / lightsGestureArea.height;
                    var iNewY = Math.floor(y * iScaleY);
                    adjustLighting(""+iNewY);
                }

            }
        }

        SwipeArea {
            anchors.fill: parent;
            onSwipeRight: manager.changedTrack("-1");
            onSwipeLeft: manager.changedTrack("+1");
        }
        ToolIcon {
            anchors.right: parent.right;
            iconId: "icon-m-content-playlist-inverse";
            onClicked: playlistDialog.open();
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
        height: 20
        Text {
            text: dcenowplaying.qs_subTitle;
            font.pointSize: 16;
            color: "white";
        }
    }

    Rectangle
    {
        color: "black";
        anchors.top: nowplayingsubtitleBox.bottom;
        anchors.topMargin: UiConstants.DefaultMargin;
        anchors.bottomMargin: UiConstants.DefaultMargin;
        id: transportTangle;
        width: 480
        height: 96;
        Button {
            anchors.left: transportTangle.left;
            anchors.verticalCenter: transportTangle.verticalCenter;
            iconSource: "image://theme/icon-m-lockscreen-mediacontrol-previous";
            width: 48;
        }
        Button {
            id: playPause;
            anchors.horizontalCenter: transportTangle.horizontalCenter;
            anchors.verticalCenter: transportTangle.verticalCenter;
            iconSource: "image://theme/icon-m-lockscreen-mediacontrol-"+(dcenowplaying.qs_speed == "0" ? "play": "pause");
            onClicked: pauseMedia();
            width: 48;
        }
        Button {
            anchors.right: transportTangle.right;
            anchors.verticalCenter: transportTangle.verticalCenter;
            iconSource: "image://theme/icon-m-lockscreen-mediacontrol-next";
            width: 48;
        }
    }

    Timer {
        id: singleshot;
        repeat: false;
        interval: 2000;
        triggeredOnStart: false;
        running: true;
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
    }

    Timer {
        id: updatePlayPause
        repeat: true;
        interval: 500;
        triggeredOnStart: true;
        running: true;
        onTriggered: {
            playPause.iconSource = "image://theme/icon-m-lockscreen-mediacontrol-"+(dcenowplaying.qs_speed == "0" ? "play": "pause");
            positionSlider.value = TimeCodeHelpers.timecodeToSeconds(dcenowplaying.timecode);
            positionSlider.minimumValue = 0;
            positionSlider.maximumValue = TimeCodeHelpers.timecodeToSeconds(dcenowplaying.duration);
        }
    }

    Rectangle {
        id: sliderRow;
        color: "black";
        anchors.top: transportTangle.bottom;
        anchors.leftMargin: UiConstants.DefaultMargin;
        anchors.rightMargin: UiConstants.DefaultMargin;
        width: screen_70.width;
        height: 16;
        Text {
            id: newTimeText;
            color: "white";
            anchors.bottom: positionSlider.top;
            text: "1:23:45";
            font.pointSize: 14;
            visible: true;
        }
        ProgressBar {
            anchors.verticalCenter: sliderRow.verticalCenter;
            anchors.topMargin: 48;
            id: positionSlider;
            width: 480;
            value: 0;
            minimumValue: 0;
            maximumValue: 0;
        }
        MouseArea {
            anchors.fill: parent;
            onPressed: {
                newTimeText.visible = true;
            }
            onPositionChanged: {
                var x = mouse.x;
                newTimeText.x = x;
                var iScaleX = parseInt(TimeCodeHelpers.timecodeToSeconds(dcenowplaying.duration)) / positionSlider.width;
                var iNewX = Math.floor(x * iScaleX);
                newTimeText.text = TimeCodeHelpers.secondsToTimecode(iNewX);
            }
            onReleased: {
                newTimeText.visible = false;
                var x = mouse.x;
                var iScaleX = parseInt(TimeCodeHelpers.timecodeToSeconds(dcenowplaying.duration)) / positionSlider.width;
                var iNewX = Math.floor(x * iScaleX);
                jogPosition(""+iNewX);
            }

        }
    }

    Row {
        id: timecodeRow;
        anchors.top: sliderRow.bottom;
        anchors.leftMargin: UiConstants.DefaultMargin;
        anchors.rightMargin: UiConstants.DefaultMargin;
        anchors.topMargin: UiConstants.DefaultMargin;
        width: screen_70.width;
        Text {
            text: dcenowplaying.timecode;
            color: "white";
            font.pointSize: 16;
            width: screen_70.width / 2;
            horizontalAlignment: Text.AlignLeft;
        }
        Text {
            text: dcenowplaying.duration;
            font.pointSize: 16;
            color: "white";
            width: screen_70.width / 2;
            horizontalAlignment: Text.AlignRight;
        }
    }


    Connections {
        target: dcenowplaying;
        onPlayListPositionChanged: { nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp; }
    }

    Component.onCompleted: setNowPlayingData();

}
