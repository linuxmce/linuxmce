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

                ButtonMain{

                    buttontext: "Attribute Sort"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/AttributeTypes.qml")
                        }
                }


                ButtonMain{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/GenreSelector.qml")
                    }
                }


                ButtonMain{
                    buttontext: "Sources"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)

                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/MediaSources.qml")
                    }
                }




            ButtonMain{
                buttontext: "Resolution"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createFilterObjects("../components/FileFormat.qml")
                }
            }


                ButtonMain{
                 buttontext: "Mediatypes"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createFilterObjects("../components/MediaTypes.qml")
                }
         }


}
}





