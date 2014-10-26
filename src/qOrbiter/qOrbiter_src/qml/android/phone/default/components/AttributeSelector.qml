import QtQuick 1.0

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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        ButtonSq{
            buttontext: "Play All"
            buttontextfontsize: scaleY(5)
            height: appStyle.stdbuttonh
            width: parent.width
            buttontextbold: false
            imgSource: ""
            color: androidSystem.orangeHighlight
            onActivated: {manager.playMedia("!G"+iPK_Device); attributeSelector.state="hidden"}
        }


        StyledButton{
            buttonText: "Attribute Sort"
            height: appStyle.stdbuttonh
            width: parent.width
            onActivated:{filterTarget.currentModel = attribfilter; attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
             buttontextfontsize: scaleY(5)
            height: skinSyle.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeHighlight
            onActivated:{filterTarget.currentModel = genrefilter; attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
             buttontextfontsize: scaleY(5)
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.greenStandard
            onActivated:{ filterTarget.currentModel = mediasourcelist;attributeSelector.state="hidden"}
        }

        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
             buttontextfontsize: scaleY(5)
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated: {filterTarget.currentModel = fileformatmodel;attributeSelector.state="hidden"}
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
             buttontextfontsize: scaleY(5)
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.orangeStandard
            onActivated: {filterTarget.currentModel = mediatypefilter;attributeSelector.state="hidden"}

        }

        ButtonSq{
            buttontext: "Home"
            buttonsqradius: 7
             buttontextfontsize: scaleY(5)
            height: style.stdbuttonh
            width: parent.width
            imgSource: ""
            color: androidSystem.blueHighlight
            onActivated:{manager.setCurrentScreen("Screen_1.qml");attributeSelector.state="hidden"}

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





