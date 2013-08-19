import QtQuick 2.0
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    id: attributeitem
    height: childrenRect.height
    width: childrenRect.width
    color: "transparent"
    Column{
        id:attributerow
        height: childrenRect.height
        width: childrenRect.width
        spacing: 5

        ButtonSq{
            buttontext: "Play All"
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontextbold: true
            MouseArea{
                anchors.fill:parent
                onClicked: manager.playMedia("!G"+iPK_Device)
            }
        }

        ButtonSq
        {
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontext: "Grid Back"
            buttontextbold: true
            MouseArea
            {
                anchors.fill:parent
                onReleased: manager.goBackGrid()
            }
        }
        ButtonSq{
            buttontext: "Attribute Sort"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased:filterTarget.currentModel = attribfilter
            }
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = genrefilter
            }
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw

            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = mediasourcelist
            }
        }




        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = fileformatmodel
            }
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: filterTarget.currentModel = mediatypefilter
            }
        }

        HomeButton{
            id:home
            height: style.stdbuttonh
            width: style.stdbuttonw
        }
    }
}





