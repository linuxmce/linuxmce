import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs

Rectangle {
            id: attributeitem
            height: childrenRect.height
            width: childrenRect.width
            color: "transparent"

            function createSelectorComponent(componentName){

            }

            function finishSelectorComponent(componentName){

            }

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
                        onClicked: dcerouter.playMedia("!G"+iPK_Device)
                    }
                }

                ButtonSq{

                    buttontext: "Attribute Sort"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createStageComponent("../components/AttributeTypes.qml", fileviewscreen)
                        }
                }


                ButtonSq{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/GenreSelector.qml")
                    }
                }


                ButtonSq{
                    buttontext: "Sources"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)

                    MouseArea{
                        anchors.fill:parent
                        onClicked: MyJs.createFilterObjects("../components/MediaSources.qml")
                    }
                }




            ButtonSq{
                buttontext: "Resolution"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: MyJs.createFilterObjects("../components/FileFormat.qml")
                }
            }


                ButtonSq{
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





