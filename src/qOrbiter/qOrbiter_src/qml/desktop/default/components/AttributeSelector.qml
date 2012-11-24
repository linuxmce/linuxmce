import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs
import "../lib/handlers"

Rectangle {
            id: attributeitem
            height: childrenRect.height
            width: childrenRect.width
            color: "transparent"
            Component.onCompleted: {
                manager.requestMediaSubtypes()
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
                   PlayAll{}
                }

                ButtonSq{

                    buttontext: "Attribute Sort"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: {myFilters.currentFilterModel=attribfilter}
                        }
                }


                ButtonSq{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: scaleY(style.buttonH)
                    width:  scaleX(style.buttonW)
                    MouseArea{
                        anchors.fill:parent
                        onClicked: myFilters.currentFilterModel = genrefilter
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
                    onClicked: myFilters.currentFilterModel = fileformatmodel
                }
            }

                ButtonSq{
                 buttontext: "Mediatypes"
                buttonsqradius: 7
                height: scaleY(style.buttonH)
                width:  scaleX(style.buttonW)
                MouseArea{
                    anchors.fill:parent
                    onClicked: {
                MyJs.createFilterObjects("../components/MediaTypes.qml")
                        myFilters.currentFilterModel = mediatypefilter

                    }
                }
         }


}
}





