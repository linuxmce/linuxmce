import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    id: attributeitem
    height: childrenRect.height
    width: childrenRect.width
    color: "transparent"


    Row{
        id:attributerow
        height: scaleY(style.buttonH )
        width: childrenRect.width
        spacing: 5

        HomeButton{id:home}

        ButtonSq{
            buttontext: "Play All"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
            MouseArea{
                anchors.fill:parent
                onClicked: manager.playMedia("!G"+iPK_Device)
            }
        }

        ButtonSq{

            buttontext: "Attribute Sort"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
            MouseArea{
                anchors.fill:parent
                onClicked: selector.activeModel = attribfilter
            }
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
            MouseArea{
                anchors.fill:parent
                onClicked: selector.activeModel = genrefilter
            }
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)

            MouseArea{
                anchors.fill:parent
                onClicked: selector.activeModel = mediasourcelist            }
        }




        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
            MouseArea{
                anchors.fill:parent
                onClicked: selector.activeModel = fileformatmodel
            }
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: scaleY(style.buttonH)
            width:  scaleX(style.buttonW)
            MouseArea{
                anchors.fill:parent
                onClicked: selector.activeModel = mediatypefilter
            }
        }
    }

    ListModel{
        id:mediasourcelist

        ListElement{
            name:"Hard Drives"
            pk:"1"
            isSelected: false
        }

        ListElement{
            name:"Discs & Jukeboxes"
            pk:"2"
            isSelected: false
        }

        ListElement{
            name:"Bookmarks"
            pk:"3"
            isSelected: false
        }

        ListElement{
            name:"Downloadable"
            pk:"4"
            isSelected: false
        }
    }
}





