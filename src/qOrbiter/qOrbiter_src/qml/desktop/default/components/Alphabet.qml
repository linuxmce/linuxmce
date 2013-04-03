import QtQuick 1.1
Item
{
    property int listHeight:viewMode ? scaleX(3): appH
    property int listWidth: viewMode ? appW :scaleX(3)
    property string listlabelColor:"aliceblue"
    property int listlabelpixelsize:12
    property string listLabelFont
    property bool viewMode: false //true is landscape

    ListModel{
        id:alphabetlist

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
    Rectangle{
        id:alphaDisplay
        height: childrenRect.height
        width: childrenRect.width
        color: "grey"
        clip:true
        ListView{
            id:alphalist
            height: listHeight
            width: listWidth
            orientation: viewMode ? ListView.Horizontal :ListView.Vertical
            clip:true
            model:alphabetlist
            delegate:
                Rectangle {
                id:alphabetrect
                height: scaleY(5)
                width: scaleX(4)
                color: "transparent"
                clip:false
                StyledText {
                    id: test
                    textLabel: name
                    font.pixelSize: listlabelpixelsize
                    anchors.centerIn: parent
                    color:listlabelColor

                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        alphabetrect.scale = 1.5
                    }
                    onExited: {

                        alphabetrect.scale = 1
                    }
                    onClicked: manager.seekToGridPosition(name)
                }
            }
        }
    }
}
