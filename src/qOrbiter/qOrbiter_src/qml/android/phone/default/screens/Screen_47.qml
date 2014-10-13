import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../../skins-common/lib/handlers"
StyledScreen{
    id:file_view_screen
    property int current_view_type:1
    property string currentSeekLetter:""

    function toggleAlphabet(){
        console.log("toggling alphabet")
        if(fileviewscreen.state==="browsing"){
            fileviewscreen.state="alphabets"
        } else {
            fileviewscreen.state="browsing"
        }
    }

    Item {
        id:fileviewscreen
        anchors.fill: parent

        focus:true
        state:"browsing"

        Component.onCompleted: {
            manager.setStringParam(0, manager.q_mediaType)
            hdr.state="grid"
            forceActiveFocus()
        }


        ListModel{
            id:alphabetlist

            ListElement{
                name:"0"
            }

            ListElement{
                name:"A"
            }

            ListElement{
                name:"B"
            }

            ListElement{
                name:"C"
            }

            ListElement{
                name:"D"
            }

            ListElement{
                name:"E"
            }

            ListElement{
                name:"F"
            }

            ListElement{
                name:"G"
            }

            ListElement{
                name:"H"
            }

            ListElement{
                name:"I"
            }

            ListElement{
                name:"J"
            }

            ListElement{
                name:"K"
            }

            ListElement{
                name:"L"
            }

            ListElement{
                name:"M"
            }
            ListElement{
                name:"N"
            }

            ListElement{
                name:"O"
            }

            ListElement{
                name:"P"
            }

            ListElement{
                name:"Q"
            }

            ListElement{
                name:"R"
            }

            ListElement{
                name:"S"
            }

            ListElement{
                name:"T"
            }

            ListElement{
                name:"U"
            }

            ListElement{
                name:"V"
            }

            ListElement{
                name:"W"
            }

            ListElement{
                name:"X"
            }

            ListElement{
                name:"Y"
            }

            ListElement{
                name:"Z"
            }
        }

        Keys.onReleased: {

            switch(event.key){
            case Qt.Key_Back:
                manager.goBackGrid();
                event.accepted=true
                break;
            case Qt.Key_MediaPrevious:
                if (!manager.goBackGrid())
                    event.accepted=false
                else
                    event.accepted=true
                break;
            default:
                console.log(event.key)
                break
            }
        }

        MultiStateFileDisplay{
            id:mediaView
            anchors{
                left:parent.left
                right:alpha_list.left
                top:parent.top
                bottom:parent.bottom
            }
        }


        Connections{
            target: filedetailsclass
            onShowDetailsChanged:{
                loadComponent("FileDetails.qml")
            }
        }



        ListView{
            id:alpha_list
            model: alphabetlist
            width: scaleX(16)
            spacing:scaleX(2)
            anchors{
                top:parent.top
                bottom: parent.bottom
            }

            delegate: StyledButton{
                height: scaleY(10)
                width: scaleX(15)
                buttonText: name
                textSize: 42
                onActivated:   {
                    fileviewscreen.state="browsing"
                    currentSeekLetter = name
                    manager.seekGrid("MediaFile", name)
                }
            }

        }
        states: [
            State {
                name: "browsing"

                AnchorChanges{
                    target:alpha_list
                    anchors{
                        left:parent.right
                    }
                }
            },
            State {
                name: "alphabets"

                AnchorChanges{
                    target:alpha_list
                    anchors{
                        left:undefined
                        right:parent.right
                    }
                }
            }
        ]
    }

}


