import QtQuick 1.1
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
                        onClicked: MyJs.createStageComponent("../components/AttributeTypes.qml", attributeitem)
                        }
                }


                ButtonMain{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createStageComponent("../components/GenreSelector.qml", attributeitem)
                    }
                }


                ButtonMain{
                    buttontext: "Sources"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)

                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createStageComponent("../components/MediaSources.qml", attributeitem)
                    }
                }




            ButtonMain{
                buttontext: "Resolution"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createStageComponent("../components/FileFormat.qml", attributeitem)
                }
            }


                ButtonMain{
                 buttontext: "Mediatypes"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createStageComponent("../components/MediaTypes.qml", attributeitem)
                }
         }


}
}





