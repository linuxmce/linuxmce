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



        ButtonSq
        {
            height: style.stdbuttonH
            width: style.stdbuttonw
            buttontext: "Grid Back"
            buttontextbold: true
            MouseArea
            {
                anchors.fill:parent
                onClicked: goBackGrid()
            }
        }
        ButtonSq{

            buttontext: "Attribute Sort"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onClicked: MyJs.createFilterObjects("../components/AttributeTypes.qml")
            }
        }


        ButtonSq{
            buttontext: "Genre"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onClicked: MyJs.createFilterObjects("../components/GenreSelector.qml")
            }
        }


        ButtonSq{
            buttontext: "Sources"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw

            MouseArea{
                anchors.fill:parent
                onClicked: MyJs.createFilterObjects("../components/MediaSources.qml")
            }
        }




        ButtonSq{
            buttontext: "Resolution"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onClicked: MyJs.createFilterObjects("../components/FileFormat.qml")
            }
        }


        ButtonSq{
            buttontext: "Mediatypes"
            buttonsqradius: 7
            height: style.stdbuttonh
            width: style.stdbuttonw
            MouseArea{
                anchors.fill:parent
                onClicked: MyJs.createFilterObjects("../components/MediaTypes.qml")
            }
        }

        HomeButton{
            id:home
            height: style.stdbuttonh
            width: style.stdbuttonw
        }


    }
}





