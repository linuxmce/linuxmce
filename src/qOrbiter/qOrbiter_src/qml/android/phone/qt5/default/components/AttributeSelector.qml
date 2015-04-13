import QtQuick 2.2
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
            height: style.stdbuttonh
            width: parent.width
            buttontextbold: true
            imgSource: ""
            color: "black"
            onActivated: manager.playMedia("!G"+iPK_Device)

        }


        ButtonSq{
            buttontext: "Attribute Sort"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated:{filterTarget.currentModel = attribfilter; attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated:{filterTarget.currentModel = genrefilter; attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated:{ filterTarget.currentModel = mediasourcelist;attributeSelector.state="hidden"}
        }

        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated: {filterTarget.currentModel = fileformatmodel;attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated: {filterTarget.currentModel = mediatypefilter;attributeSelector.state="hidden"}

        }

        ButtonSq{
            buttontext: "Home"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated:{manager.goBacktoQScreen("Screen_1.qml");attributeSelector.state="hidden"}

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





