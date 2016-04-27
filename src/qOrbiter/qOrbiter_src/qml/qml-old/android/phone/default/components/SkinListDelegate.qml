import QtQuick 1.0

Component{
        Item {
    id: skindelegate

    height: scaleY(25)
    width: scaleX(85)
    Row{

        height: scaleY(25)
        width: skindelegate.width

        Rectangle{
            id:rectDelegate
            height: scaleY(25)
            width: skindelegate.width

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    console.log("the path:" +path)
                    manager.setActiveSkin(path)

                }
            }



        Text {
            anchors.top: parent.top
            id: nameskin

            text: "Name:" + name + ". Version:" + version
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pixelSize:  scaleY(3)

            }
        Text {
            anchors.top: nameskin.bottom
            id: skinDescription
            text: description
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pixelSize:  scaleY(2)
            }


            Text {
                anchors.top: skinDescription.bottom
                id: skincreator
                text: "Created By:" + creator
                wrapMode: "WrapAnywhere"
                font.pixelSize:  scaleY(2)
            }
            Text {
                anchors.top: skincreator.bottom
                id: skinvariant
                text: "Variation for: " + variation
                wrapMode: "WrapAnywhere"
                font.pixelSize:  scaleY(2)
            }

        }

    }
  }
}



