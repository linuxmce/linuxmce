import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

Page {
    id: screen_47_details
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: {
                dataModel.checkForMore();
                pageStack.pop();
            }
        }
        ToolIcon {
            iconId: "toolbar-mediacontrol-play"
            onClicked: {
                playMedia(filedetailsclass.file);
            }
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            visible: false;
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: fileDetailsMenu.open();
        }
    }

    Connections {
        target: filedetailsclass;
        onImageChanged: detailsImage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
    }

    Timer {
        id: updateScreenie;
        repeat: true;
        interval: 250;
        triggeredOnStart: true;
        running: true;
        onTriggered: detailsImage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
    }

    Image {
        id: detailsImage;
        source: "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
        width: 480;
        height: 427;
        sourceSize.width: 480;
        sourceSize.height: 427;
        fillMode: Image.PreserveAspectCrop;
    }

    Rectangle {
        id: detailsTitle;
        width: 480;
        anchors.top: detailsImage.bottom;
        height: 24;
        anchors.topMargin: UiConstants.DefaultMargin;
        anchors.leftMargin: UiConstants.DefaultMargin;
        color: "black";
        Text {
            text: filedetailsclass.mediatitle;
            font.weight: Font.Bold;
            font.pointSize: 16;
            color: "white";
        }
    }

    Rectangle {
        id: detailsSubtitle;
        width: 480;
        anchors.top: detailsTitle.bottom;
        color: "black";
        height: 20
        Text {
            text: filedetailsclass.episode;
            font.pointSize: 16;
            color: "white";
        }
    }

    Text {
        id: detailsSynopsis;
        width: 480;
        height: 128;
        anchors.top: detailsSubtitle.bottom;
        text: filedetailsclass.synop;
        color: "white";
        font.pointSize: 16;
    }
}

