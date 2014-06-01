import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item{
    anchors.fill: parent

    Row{
        height:parent.height
        width: parent.width
        spacing: scaleY(2)

        StyledButton{
            buttonText: "Playlist"
            onActivated: pageLoader.item.children[0].state="playlist"
        }
        StyledButton{
            buttonText: "Metadata"
            onActivated: pageLoader.item.children[0].state="metadata"
        }

        StyledButton{
            buttonText: "Advanced"
            onActivated: pageLoader.item.children[0].state="advanced"
        }
    }

}


