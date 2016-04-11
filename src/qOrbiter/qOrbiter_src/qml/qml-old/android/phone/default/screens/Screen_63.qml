import QtQuick 1.0
import "../components"

StyledScreen {

    id: satcableboxremote
    navigation: "TvMediaControl.qml"

    Item{
        id:container
        anchors.fill: parent

        Component.onCompleted: {
            container.state="INFO"
        }

        NowPlayingBox{
            id:np_box
        }

        TvmetaData {
            id: metadatavideo
            anchors{
                left:parent.right
                right:parent.right
                top:parent.top
                bottom:parent.bottom

            }
        }

        TvbuttonGrid {
            id: remote1
            anchors{
                left:container.right               
                bottom: container.bottom
                top:container.top
            }
        }


        RemoteNumberPad {
            id: remotenumberpad1
            width: container.width
            height: container.height
            anchors{
                left:container.right
                top:container.top
            }
            visible:false
        }

        EPGPlaylist{
            id:playlist;
            anchors{
                left:container.right
                right:container.right
                bottom: container.bottom
                top:container.top
            }
        }
        AdvancedMediaOptions{
            id:advanced_options
            width: parent.width
            anchors{
                left:container.right
                bottom: container.bottom
                top:container.top
            }
        }


        states: [

            State {
                name: "INFO"
                extend: ""
                AnchorChanges{
                    target: metadatavideo
                    anchors{
                        left:container.left
                        right:container.right
                        bottom: container.bottom
                        top:container.top
                    }
                }

            },
            State {
                name: "NUMBERS"
                extend: ""
                PropertyChanges {
                    target: remotenumberpad1
                    visible:true
                }
                AnchorChanges{
                    target: remotenumberpad1
                    anchors{
                        left:container.left
                    }
                }
            },
            State{
                name:"GRID"
                extend:""
                AnchorChanges{
                    target: playlist
                    anchors{
                        left:container.left
                    }
                }
            },
            State{
                name:"REMOTE"
                extend:""
                AnchorChanges{
                    target: remote1
                    anchors{

                        left:container.left
                    }
                }
            }
            ,
            State{
                name:"advanced"
                extend:""
                AnchorChanges{
                    target: advanced_options
                    anchors{

                        left:container.left
                    }
                }
            }

        ]

    }
}
