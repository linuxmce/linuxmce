import QtQuick 2.2
import "../components"

Rectangle {
    id:myth_recordings
    width: manager.appWidth
    height: manager.appHeight

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: parent
    }

    HomeButton{
        id:home
        anchors.left: parent.left
    }

    Text {
        id: temp_label
        text: qsTr("Catch All Screen")
        font.pixelSize: scaleY(4)
        anchors.bottom: recordings_control.top
    }
    NowPlayingBox{
        id:myth_now_playing
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle{
        id:buttons_row
        height: scaleY(25)
        width:scaleX(85)
        anchors.top: myth_now_playing.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        color: "transparent"

        Flow{
            id:button_array
            width: parent.width
            height: scaleY(85)

            AvOptionButton{
                buttontext: qsTr("Live")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("livetv")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Schedule")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("schedule")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Recordings")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("recordings")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Music")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("music")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Guide")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("guide")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Menu")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("menu")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Info")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("info")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("exit")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Favorites")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("favorites")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Record")
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.extraButtons("record")
                }
            }
        }



    }

    AudioRemote{
        id:recordings_control
        anchors.bottom: parent.bottom
    }
}
