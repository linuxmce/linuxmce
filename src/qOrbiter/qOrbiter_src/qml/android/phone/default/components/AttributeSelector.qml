import QtQuick 1.0
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
                onReleased:loadComponent("AttributeTypes.qml")
            }
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: loadComponent("GenreSelector.qml")
            }
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw

            MouseArea{
                anchors.fill:parent
                onReleased: loadComponent("MediaSources.qml")
            }
        }




        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: loadComponent("FileFormat.qml")
            }
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onReleased: loadComponent("MediaTypes.qml")
            }
        }

        HomeButton{
            id:home
            height: style.stdbuttonh
            width: style.stdbuttonw
        }
    }
}





