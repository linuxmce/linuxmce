import QtQuick 1.0
import "../components"

StyledScreen {

    id: satcableboxremote
    navigation: "TvMediaControl.qml"

    Item{
        height:parent.height
        width: parent.width

        state:"default"

        NowPlayingBox{
            id:np_box
        }

//        Component.onCompleted: {
//            state="INFO"
//        }

        TvmetaData {
            id: metadatavideo
        }

        TvbuttonGrid {
            id: remote1
        }


        RemoteNumberPad {
            id: remotenumberpad1
        }

        EPGPlaylist{
            id:playlist;

        }


        states: [
            State {
                name: "default"

                AnchorChanges{
                    target: metadatavideo
                    anchors{
                        left:parent.right
                        right:parent.right
                        top:parent.top
                        bottom:parent.bottom

                    }
                }
                AnchorChanges{
                    target: remote1
                    anchors{
                        left:parent.right
                        right:parent.right
                        bottom: parent.bottom
                        top:parent.top
                    }
                }
                AnchorChanges{
                    target: playlist
                    anchors{
                        left:parent.right
                        right:parent.right
                        bottom: parent.bottom
                        top:parent.top
                    }
                }
                AnchorChanges{
                    target: remotenumberpad1
                    anchors{
                        left:parent.right
                        right:parent.right
                        bottom: parent.bottom
                        top:parent.top
                    }
                }

            },
            State {
                name: "INFO"
                extend: "default"
                AnchorChanges{
                    target: metadatavideo
                    anchors{
                        left:parent.left
                        right:parent.right
                        bottom: parent.bottom
                        top:parent.top
                    }
                }
                AnchorChanges{
                    target: remotenumberpad1
                    anchors{
                        left:parent.right
                    }
                }
            },
            State {
                name: "NUMBERS"
                extend: "default"
                AnchorChanges{
                    target: remotenumberpad1
                    anchors{
                        left:parent.left
                    }
                }
            },
            State{
                name:"GRID"
                extend:"default"
                AnchorChanges{
                    target: playlist
                    anchors{
                        left:parent.left
                    }
                }
            },
            State{
                name:"REMOTE"
                extend:"default"
                AnchorChanges{
                    target: remote1
                    anchors{
                        left:parent.left
                    }
                }
            }
            ,
            State{
                name:"advanced"
                extend:"default"
            }

        ]

    }
}
