import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs

Rectangle {
            id: attributeitem
            height: childrenRect.height
            width: childrenRect.width
            color: "transparent"
            Row{
                id:attributerow
                height: style.buttonH +10
                width: childrenRect.width
                spacing: 5

                HomeButton{id:home}

                ButtonSq{

                    buttontext: "Attribute Sort"
                    buttonsqradius: 7
                    height: style.buttonH
                    width:  style.buttonW
                         }

                ButtonSq{
                    buttontext: "Genre"
                    buttonsqradius: 7
                    height: style.buttonH
                    width:  style.buttonW
                         }

                ButtonSq{
                    buttontext: "Sources"
                    buttonsqradius: 7
                    height: style.buttonH
                    width:  style.buttonW
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

         }


}
}





