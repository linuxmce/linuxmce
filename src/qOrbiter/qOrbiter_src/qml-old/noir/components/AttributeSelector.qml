import QtQuick 2.0
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
                    height: style.buttonH
                    width:  style.buttonW
                    MouseArea{
                        anchors.fill:parent
                        onClicked: manager.playMedia("!G"+iPK_Device)
                    }
                }

                ButtonSq{

                    buttontext: "Attribute Sort"
                    buttonsqradius: 7
                    height: style.buttonH
                    width:  style.buttonW
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/AttributeTypes.qml")
                        }
                }


                ButtonSq{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: style.buttonH
                    width:  style.buttonW
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/GenreSelector.qml")
                    }
                }


                ButtonSq{
                    buttontext: "Sources"
                    buttonsqradius: 7
                    height:style.buttonH
                    width:style.buttonW

                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/MediaSources.qml")
                    }
                }




            ButtonSq{
                buttontext: "Resolution"
                buttonsqradius: 7
                height: style.buttonH
                width:  style.buttonW
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createFilterObjects("../components/FileFormat.qml")
                }
            }


                ButtonSq{
                 buttontext: "Mediatypes"
                buttonsqradius: 7
                height: style.buttonH
                width:  style.buttonW
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createFilterObjects("../components/MediaTypes.qml")
                }
         }


}
}





