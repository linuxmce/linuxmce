import QtQuick 1.1

Component{
        Item {
    id: skindelegate

    height: 100
    width: 350
    Row{

        height: 75
        width: skindelegate.width

        Rectangle{
            id:rectDelegate
            height: 75
            width: skindelegate.width

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    console.log("Switching to:" + name)
                    manager.setActiveSkin(path)


                }
            }



        Text {
            anchors.top: parent.top
            id: nameskin

            text: "Name:" + name + ". Version:" + version
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pointSize: 14

            }
        Text {
            anchors.top: nameskin.bottom
            id: skinDescription
            text: description
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }


            Text {
                anchors.top: skinDescription.bottom
                id: skincreator
                text: "Created By:" + creator
                wrapMode: "WrapAnywhere"
            }
            Text {
                anchors.top: skincreator.bottom
                id: skinvariant
                text: "Variation for: " + variation
                wrapMode: "WrapAnywhere"
            }

        }

    }
  }
}



