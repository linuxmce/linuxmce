import QtQuick 2.0
import "../js/ComponentLoader.js" as MyJs

Item {
    id: attributeSelector
    height: 0
    width: 0
    state:"hidden"

    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    Behavior on opacity{
        PropertyAnimation{
        duration:350
        }
    }

    Column{
        id:attributerow
        height: parent.height * .95
        width: parent.width*.85
        spacing: 5

        ButtonSq{
            buttontext: "Play All"
            height: style.stdbuttonh /2
            width: parent.width
            buttontextbold: true
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onClicked: manager.playMedia("!G"+iPK_Device)
            }
        }


        ButtonSq{
            buttontext: "Attribute Sort"
            buttonsqradius: 7
            height: style.stdbuttonh /2
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onReleased:filterTarget.currentModel = attribfilter
            }
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: style.stdbuttonh /2
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = genrefilter
            }
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: style.stdbuttonh /2
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = mediasourcelist
            }
        }




        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: style.stdbuttonh /2
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = fileformatmodel
            }
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: style.stdbuttonh /2
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = mediatypefilter
            }
        }

        HomeButton{
            id:home
            height: style.stdbuttonh /2
            width: style.stdbuttonw
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: attributeSelector
                opacity:0
                height:0
                width:0
                visible:false
            }
        },
        State {
            name: "atrselect"
            PropertyChanges {
                target: attributeSelector
                visible:true
                opacity:1
                height:manager.appHeight
                width:manager.appWidth
            }
        }
    ]
}





