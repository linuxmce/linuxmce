import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
import "../../../skins-common/lib/handlers"

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
                        onClicked: myFilters.currentFilterModel = mediasourcelist
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
                        myFilters.currentFilterModel = mediatypefilter

                    }
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





