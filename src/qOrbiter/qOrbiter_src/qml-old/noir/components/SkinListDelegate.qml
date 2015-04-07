import QtQuick 2.0

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

                    setActiveSkin(name)


                }
            }



        StyledText {
            anchors.top: parent.top
            id: nameskin

            text: "Name:" + name + ". Version:" + version
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            font.pointSize: 14

            }
        StyledText {
            anchors.top: nameskin.bottom
            id: skinDescription
            text: description
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            }


            StyledText {
                anchors.top: skinDescription.bottom
                id: skincreator
                text: "Created By:" + creator
                wrapMode: "WrapAnywhere"
            }
            StyledText {
                anchors.top: skincreator.bottom
                id: skinvariant
                text: "Variation for: " + variation
                wrapMode: "WrapAnywhere"
            }

        }

    }
  }
}



